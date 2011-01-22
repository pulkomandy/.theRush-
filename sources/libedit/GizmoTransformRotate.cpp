///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : GizmoTransform.h
// Creation : 12/07/2007
// Author : Cedric Guillemet
// Description :
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GizmoTransformRotate.h"
#include "librender/IDisplayDevice.h"
#ifdef MAC_OS
#import <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern tvector3 ptd;

BEGIN_SERIALIZATION(CGizmoTransformRotate)
END_SERIALIZATION()

CGizmoTransformRotate::CGizmoTransformRotate() : CGizmoTransform()
{
	m_RotateType = ROTATE_NONE;
}

CGizmoTransformRotate::~CGizmoTransformRotate()
{

}

bool CGizmoTransformRotate::CheckRotatePlan(tvector3 &vNorm, float factor,
											const tvector3 &rayOrig,const tvector3 &rayDir,int id)
{
	tvector3 df, inters;
	m_Axis2 = vNorm;
	m_plan=vector4(m_pMatrix->GetTranslation(), vNorm);
	m_plan.RayInter(inters,rayOrig,rayDir);
	ptd = inters;
	df = inters - m_pMatrix->GetTranslation();
	df/=GetScreenFactor();

	if ( ((df.Length()/factor) >0.9f) && ( (df.Length()/factor) < 1.1f) )
	{
		m_svgMatrix = *m_pMatrix;

		m_LockVertex = df;
		m_LockVertex.Normalize();

		m_Vtx = m_LockVertex;
		m_Vty.Cross(m_LockVertex,vNorm);
		m_Vty.Normalize();
		m_Vtx *= factor;
		m_Vty *= factor;
		m_Vtz.Cross(m_Vtx,m_Vty);
		m_Ng2 = 0;
		if (id!=-1)
			m_Axis = GetVector(id);

		m_OrigScale.Scaling(GetTransformedVector(0).Length(),
		GetTransformedVector(1).Length(),
		GetTransformedVector(2).Length());

		m_InvOrigScale.Inverse(m_OrigScale);

		return true;
	}
	return false;
}

bool CGizmoTransformRotate::GetOpType(ROTATETYPE &type, unsigned int x, unsigned int y)
{
	tvector3 rayOrigin,rayDir, axis;
	tvector3 dir = m_pMatrix->GetTranslation()-m_CamSrc;
	dir.Normalize();

	BuildRay(x, y, rayOrigin, rayDir);

	if (mMask&AXIS_TRACKBALL)
	if (CheckRotatePlan(dir,1.0f,rayOrigin,rayDir,-1))
	{
		tmatrix mt = *m_pMatrix;
		mt.NoTrans();
		//mt.Inverse();
		//m_Axis = m_Axis2 = dir;
		m_Axis.TransformPoint(mt);
/*
		m_Axis *=tvector3(GetTransformedVector(0).Length(),
		GetTransformedVector(1).Length(),
		GetTransformedVector(2).Length());
*/
		type = ROTATE_TWIN;
		return true;
	}

	// plan 1 : X/Z
	m_Axis = GetTransformedVector(0);
	if (mMask&AXIS_X)
	if (CheckRotatePlan(m_Axis,1.0f,rayOrigin,rayDir,0)) { type = ROTATE_X; return true; }
	m_Axis = GetTransformedVector(1);
	if (mMask&AXIS_Y)
	if (CheckRotatePlan(m_Axis,1.0f,rayOrigin,rayDir,1)) { type = ROTATE_Y; return true; }
	m_Axis = GetTransformedVector(2);
	if (mMask&AXIS_Z)
	if (CheckRotatePlan(m_Axis,1.0f,rayOrigin,rayDir,2)) { type = ROTATE_Z; return true; }

	//m_Axis = GetTransformedVector(dir);
	if (mMask&AXIS_SCREEN)
	if (CheckRotatePlan(dir,1.2f,rayOrigin,rayDir,-1))
	{
		tmatrix mt = *m_pMatrix;
		mt.NoTrans();
		mt.Inverse();
		m_Axis = m_Axis2 = dir;
		m_Axis.TransformPoint(mt);

		m_Axis *=tvector3(GetTransformedVector(0).Length(),
		GetTransformedVector(1).Length(),
		GetTransformedVector(2).Length());

		type = ROTATE_SCREEN;
		return true;
	}

	type = ROTATE_NONE;

	return false;
}

bool CGizmoTransformRotate::OnMouseDown(unsigned int x, unsigned int y)
{
	if (m_pMatrix)
	{
		return GetOpType(m_RotateType, x, y);
	}

	m_RotateType = ROTATE_NONE;
	return false;
}


void CGizmoTransformRotate::Rotate1Axe(const tvector3& rayOrigin,const tvector3& rayDir)
{
	tvector3 inters;
	m_plan=vector4(m_pMatrix->GetTranslation(), m_Axis2);
	m_plan.RayInter(inters,rayOrigin,rayDir);
	ptd = inters;

	tvector3 df = inters - m_pMatrix->GetTranslation();

	df.Normalize();
	m_LockVertex2 = df;

	float acosng = df.Dot(m_LockVertex);
	m_Ng2 = (float)acos(acosng);

	if (df.Dot(m_Vty)>0)
		m_Ng2 = -m_Ng2;

	tmatrix mt,mt2;

	if (m_bUseSnap)
	{
		m_Ng2*=(360.0f/ZPI);
		SnapIt(m_Ng2,m_AngleSnap);
		m_Ng2/=(360.0f/ZPI);
	}

	mt.RotationAxis(m_Axis,m_Ng2);
	mt.Multiply(m_InvOrigScale);
	mt.Multiply(m_svgMatrix);
	mt2 = m_OrigScale;
	mt2.Multiply(mt);
	*m_pMatrix=mt2;

		if (m_Axis == tvector3::ZAxis)
		{
			if (mEditQT)
			{
				/*
				Dans le cadre du jeu, et vu les pb avec les quaternions,
				le 1er float du quaternion en stockage est l'angle en radian.
				le stockage reste un quaternion.
				il y a des pbs de conversion quaternion/matrix
				*/
#if USE_QUATERNION
				tquaternion gth(*m_pMatrix);

				gth.Normalize();
				gth.UnitInverse();

				tquaternion qtg;
				qtg.RotationAxis(m_Axis,m_Ng2);
				*mEditQT = gth;//gth+qtg;//tquaternion(mt2);
				mEditQT->Normalize();
#else
				mEditQT->z = m_Ng2;
#endif
			}
		}
}

void CGizmoTransformRotate::OnMouseMove(unsigned int x, unsigned int y)
{
	tvector3 rayOrigin, rayDir, axis;

	BuildRay(x, y, rayOrigin, rayDir);

	if (m_RotateType != ROTATE_NONE)
	{
		if (m_RotateType == ROTATE_TWIN)
		{
			tvector3 inters;
			tvector3 dir = m_pMatrix->GetTranslation()-m_CamSrc;
			dir.Normalize();

			m_plan=vector4(m_pMatrix->GetTranslation(), dir);
			m_plan.RayInter(inters,rayOrigin,rayDir);
			ptd = inters;
			tvector3 df = inters - m_pMatrix->GetTranslation();
			df/=GetScreenFactor();
			float lng1 = df.Length();
			if (lng1 >= 1.0f) lng1 = 0.9f;

			float height = (float)sin(acos(lng1));
			tvector3 m_CamRealUp,camRight;
			camRight.Cross(m_Axis,m_CamUp);
			m_CamRealUp.Cross(camRight,dir);

			tvector3 idt = height*dir;
			idt+=df;

			//ptd = idt;
			idt= m_LockVertex-idt;

			tmatrix mt,mt2;

			mt.LookAtLH(tvector3(0,0,0),idt,m_CamRealUp);
			mt.Multiply(m_InvOrigScale);
			mt.Multiply(m_svgMatrix);
			mt2 = m_OrigScale;
			mt2.Multiply(mt);
			*m_pMatrix=mt2;
/*
			if (mEditQT)
			{
				*mEditQT = tquaternion(mt2);
			}
			*/
		}
		else
		{
			Rotate1Axe(rayOrigin, rayDir);

		}

        if (mTransform) mTransform->Update();
	}
	else
	{
		// predict move
		if (m_pMatrix)
		{
			GetOpType(m_RotateTypePredict, x, y);
		}
	}
}

void CGizmoTransformRotate::OnMouseUp(unsigned int x, unsigned int y)
{
	m_RotateType = ROTATE_NONE;
}

void CGizmoTransformRotate::Draw()
{
	if (m_pMatrix)
	{
		ComputeScreenFactor();

		tvector3 right,up,frnt,dir;

		//glDisable(GL_DEPTH_TEST);
		tvector3 orig(m_pMatrix->GetTranslation());
		tvector3 plnorm(m_CamSrc-orig);
		plnorm.Normalize();

		tplane plCam = vector4(plnorm,0);


		dir = orig-m_CamSrc;
		dir.Normalize();

		right.Cross(dir,GetTransformedVector(1));
		right.Normalize();

		up.Cross(dir,right);
		up.Normalize();

		right.Cross(dir,up);
		right.Normalize();

		tvector3 axeX(1,0,0),axeY(0,1,0),axeZ(0,0,1);


        if (mLocation == LOCATE_LOCAL)
        {
            axeX.TransformVector(*m_pMatrix);
		    axeY.TransformVector(*m_pMatrix);
		    axeZ.TransformVector(*m_pMatrix);
		    axeX.Normalize();
		    axeY.Normalize();
		    axeZ.Normalize();
        }

		// Twin
		if (mMask&AXIS_TRACKBALL)
		{
		if (m_RotateTypePredict != ROTATE_TWIN)
			DrawCircle(orig, 0.2f,0.2f,0.2f,right*GetScreenFactor(),up*GetScreenFactor());
		else
			DrawCircle(orig, 1,1,1,right*GetScreenFactor(),up*GetScreenFactor());
		}

		// Screen
		if (mMask&AXIS_SCREEN)
		{
		if (m_RotateTypePredict != ROTATE_SCREEN)
			DrawCircle(orig, 1.0f,0.3f,1.0f,up*1.2f*GetScreenFactor(),right*1.2f*GetScreenFactor());
		else
			DrawCircle(orig, 1,1,1,up*1.2f*GetScreenFactor(),right*1.2f*GetScreenFactor());
		}

		// X
		right.Cross(dir, axeX);
		right.Normalize();
		frnt.Cross(right, axeX);
		frnt.Normalize();

		if (mMask&AXIS_X)
		{
		if (m_RotateTypePredict != ROTATE_X)
			DrawCircleHalf(orig, 1,0,0,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
		else
			DrawCircleHalf(orig, 1,1,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
		}

		// Y

		right.Cross(dir, axeY);
		right.Normalize();
		frnt.Cross(right, axeY);
		frnt.Normalize();

		if (mMask&AXIS_Y)
		{
		if (m_RotateTypePredict != ROTATE_Y)
			DrawCircleHalf(orig, 0,1,0,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
		else
			DrawCircleHalf(orig, 1,1,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
		}

		// Z
		right.Cross(dir, axeZ);
		right.Normalize();
		frnt.Cross(right, axeZ);
		frnt.Normalize();

		if (mMask&AXIS_Z)
		{
		if (m_RotateTypePredict != ROTATE_Z)
			DrawCircleHalf(orig, 0,0,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
		else
			DrawCircleHalf(orig, 1,1,1,right*GetScreenFactor(),frnt*GetScreenFactor(),plCam);
		}
		// camembert
		if ( (m_RotateType != ROTATE_NONE) && (m_RotateType != ROTATE_TWIN ) )
			DrawCamem(orig,m_Vtx*GetScreenFactor(),m_Vty*GetScreenFactor(),-m_Ng2);
/*
		// debug
		glPointSize(20);
		glBegin(GL_POINTS);
		glVertex3fv(&ptd.x);
		glEnd();

		glEnable(GL_DEPTH_TEST);
        */
#ifdef WIN32
    GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    GDD->GetD3D9Device()->SetRenderState(D3DRS_CULLMODE , D3DCULL_NONE );
    GDD->GetD3D9Device()->SetRenderState(D3DRS_ZENABLE , D3DZB_TRUE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHATESTENABLE , FALSE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE , TRUE);
#endif
	extern RenderingState_t GRenderingState;
	GRenderingState.mAlphaTestEnable = 0;
	GRenderingState.mZWriteEnable = 1;
	GRenderingState.mBlending = 0;
	GRenderingState.mCulling = 0;
	GRenderingState.mZTestType = 1;
	}


}

void CGizmoTransformRotate::ApplyTransform(tvector3& trans, bool bAbsolute)
{
	tmatrix mt;
	m_OrigScale.Scaling(GetTransformedVector(0).Length(),
			GetTransformedVector(1).Length(),
			GetTransformedVector(2).Length());

	if (bAbsolute)
	{
		tvector3 translation = m_pMatrix->GetTranslation();

		//X
		mt.RotationAxis(GetVector(0),((trans.x/360)*ZPI));
		mt.Multiply(m_OrigScale);
		*m_pMatrix=mt;
		//Y
		mt.RotationAxis(GetVector(1),((trans.y/360)*ZPI));
		mt.Multiply(m_OrigScale);
		*m_pMatrix=mt;
		//Z
		mt.RotationAxis(GetVector(2),((trans.z/360)*ZPI));
		mt.Multiply(m_OrigScale);
		*m_pMatrix=mt;

		//translate
		m_pMatrix->m16[12] = translation.x;
		m_pMatrix->m16[13] = translation.y;
		m_pMatrix->m16[14] = translation.z;
	}
	else
	{
		tmatrix mt2;
		m_InvOrigScale.Inverse(m_OrigScale);

		if (trans.x!=0)
		{
			m_svgMatrix = *m_pMatrix;
			mt.RotationAxis(GetVector(0),((trans.x/360)*ZPI));
			mt.Multiply(m_InvOrigScale);
			mt.Multiply(m_svgMatrix);
			mt2 = m_OrigScale;
			mt2.Multiply(mt);
			*m_pMatrix=mt2;
		}
		if (trans.y!=0)
		{
			m_svgMatrix = *m_pMatrix;
			mt.RotationAxis(GetVector(1),((trans.y/360)*ZPI));
			mt.Multiply(m_InvOrigScale);
			mt.Multiply(m_svgMatrix);
			mt2 = m_OrigScale;
			mt2.Multiply(mt);
			*m_pMatrix=mt2;
		}
		if (trans.z!=0)
		{
			m_svgMatrix = *m_pMatrix;
			mt.RotationAxis(GetVector(2),((trans.z/360)*ZPI));
			mt.Multiply(m_InvOrigScale);
			mt.Multiply(m_svgMatrix);
			mt2 = m_OrigScale;
			mt2.Multiply(mt);
			*m_pMatrix=mt2;
		}
	}
}
