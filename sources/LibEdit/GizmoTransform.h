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

#ifndef GIZMOTRANSFORM_H__
#define GIZMOTRANSFORM_H__

#include "libworld/ZTransform.h"
#include "libworld/ZCamera.h"
#include "IGizmo.h"
#include "GizmoTransformRender.h"

class CGizmoTransform : public IGizmo , protected CGizmoTransformRender
{
public:



	CGizmoTransform()
	{
		m_pMatrix = NULL;
		m_bUseSnap = false;
        mCamera = NULL;
        mTransform = NULL;
        mEditPos =  mEditScale = NULL;
        mEditQT = NULL;
		mMask = AXIS_ALL;
	}

	virtual ~CGizmoTransform()
	{
	}

	virtual void SetScreenSize(int x, int y)
	{
		m_X = x;
		m_Y = y;
	}

	virtual void SetEditMatrix(tmatrix *pMatrix)
	{
		m_pMatrix = pMatrix;
        mTransform = NULL;

        mEditPos = mEditScale = NULL;
        mEditQT = NULL;
	}
    virtual void SetEditTransform(ZTransform *pTransform)
    {
        mTransform = pTransform;
		if(!pTransform)
			m_pMatrix = NULL;
		else
			m_pMatrix = (tmatrix*)&mTransform->GetLocalMatrix();
        mEditPos = mEditScale = NULL;
        mEditQT = NULL;
    }

    virtual void SetEditVQS(tvector3* aPosition, tquaternion *aQT, tvector3 *aScale)
    {
        mEditPos = aPosition;
        mEditQT = aQT;
        mEditScale = aScale;

        mTransform = NULL;
        m_pMatrix = NULL;

        tmatrix p1,p2,p3;
        p1.Identity();
        p2.Identity();
        p3.Identity();
        if (aPosition)
            p1.Translation(*aPosition);
        if (aQT)
            p2.RotationQuaternion(*aQT);
        if (aScale)
            p3.Scaling(*aScale);
        mWorkingMatrix = p3 * p2 * p1;
        m_pMatrix = &mWorkingMatrix;
    }

	virtual void SetCameraMatrix(const tmatrix &Model, const tmatrix &Proj)
	{
		m_Model = Model;
		m_Proj = Proj;

		m_invmodel=m_Model;
		m_invmodel.Inverse();

		m_invproj=m_Proj;
		m_invproj.Inverse();
	}



	// tools

	void BuildRay(int x, int y, tvector3 &rayOrigin, tvector3 &rayDir)
	{
        if (mCamera)
            mCamera->BuildRay(x, y, m_X, m_Y, rayOrigin, rayDir);
	}

	virtual void SetCam(ZCamera *pCam)
	{
        mCamera = pCam;	

        /*
        m_CamSrc = tvector3(mCamera->GetTransform()->GetWorldMatrix().m[3][0], mCamera->GetTransform()->GetWorldMatrix().m[3][1], mCamera->GetTransform()->GetWorldMatrix().m[3][2] );
		m_CamDir = tvector3(mCamera->GetTransform()->GetWorldMatrix().m[2][0], mCamera->GetTransform()->GetWorldMatrix().m[2][1], mCamera->GetTransform()->GetWorldMatrix().m[2][2] );
		m_CamUp = tvector3(mCamera->GetTransform()->GetWorldMatrix().m[1][0], mCamera->GetTransform()->GetWorldMatrix().m[1][1], mCamera->GetTransform()->GetWorldMatrix().m[1][2] );
*/

        const tmatrix oldm = mCamera->GetTransform()->GetWorldMatrix();
        tmatrix oo;
        oo.Transpose(mCamera->GetTransform()->GetWorldMatrix());
        m_CamSrc.TransformPoint(tvector3(-oldm.V4.position.x, -oldm.V4.position.y, -oldm.V4.position.z), oo);

        m_CamDir.TransformVector(tvector3::ZAxis, oo);
        //m_CamDir=-m_CamDir;

        m_CamUp.TransformVector(tvector3::YAxis, oo);
        //m_CamUp=-m_CamDir;

		m_CamDir.Normalize();
        m_CamUp.Normalize();

	}

	tvector3 GetVector(int vtID)
	{
		switch (vtID)
		{
		case 0: return tvector3(1,0,0);
		case 1: return tvector3(0,1,0);
		case 2: return tvector3(0,0,1);
		}
		return tvector3(0,0,0);
	}

	tvector3 GetTransformedVector(int vtID)
	{
		tvector3 vt;
		switch (vtID)
		{
		case 0: vt = tvector3(1,0,0); break;
		case 1: vt = tvector3(0,1,0); break;
		case 2: vt = tvector3(0,0,1); break;
		}
        if (mLocation == LOCATE_LOCAL)
        {
            vt.TransformVector(*m_pMatrix);
		    vt.Normalize();
        }
		return vt;
	}
	virtual void SetAxisMask(unsigned int mask)
	{
		mMask = mask;
	}
/*
	tvector3 GetTransformedVector(tvector3& vt)
	{
		tmatrix mt = *m_pMatrix;
		mt.NoTrans();
		vt.TransformPoint(mt);
		return vt;
	}
*/
	void ComputeScreenFactor()
	{
		if (m_pMatrix)
		{
			tvector3 sz(1,1,1);
			sz.TransformPoint(m_invproj);
			sz+=m_pMatrix->GetTranslation();
			sz.TransformPoint(m_invmodel);
			m_ScreenFactor=sz.Length()/4.5f;
		}
		else
			m_ScreenFactor = 1;
	}

	tplane m_plan;
	tvector3 m_LockVertex;
	float m_Lng;

    tvector3 RayTrace2(const tvector3& rayOrigin, const tvector3& rayDir, const tvector3& norm, const tmatrix& mt, tvector3 trss, bool lockVTNorm = true)
    {
        extern tvector3 ptd;

	    tvector3 df,inters,df2;

	    m_plan=vector4(m_pMatrix->GetTranslation(), norm);
	    m_plan.RayInter(inters,rayOrigin,rayDir);
	    ptd = inters;
	    df = inters - m_pMatrix->GetTranslation();
	    df /=GetScreenFactor();
	    df2 = df;

	    df2.TransformPoint(mt);
	    df2 *= trss;

	    m_LockVertex = df;
        if (lockVTNorm)
        {
	        m_LockVertex.Normalize();
        }
        else
        {
            m_LockVertex = inters;
        }
	    m_Lng = df.Length();

	    return df2;
    }

	float GetScreenFactor()
	{
		return m_ScreenFactor;
	}


	// snap
	virtual void UseSnap(bool bUseSnap)
	{
		m_bUseSnap = bUseSnap;
	}

	virtual bool IsUsingSnap()
	{
		return m_bUseSnap;
	}
	//transform
	virtual void ApplyTransform(tvector3& trans, bool bAbsolute) = 0;

    void SetLocation(LOCATION aLocation) { mLocation = aLocation; }
    LOCATION GetLocation() { return mLocation; }


protected:
	tmatrix *m_pMatrix;
	tmatrix m_Model,m_Proj;
	tmatrix m_invmodel,m_invproj;
	tvector3 m_CamSrc,m_CamDir,m_CamUp;
	int m_X, m_Y;
	tmatrix m_svgMatrix;
	float m_ScreenFactor;
	bool m_bUseSnap;
    ZCamera *mCamera;
	
    LOCATION mLocation;

    tmatrix mWorkingMatrix; // for dissociated components
    tvector3 *mEditPos, *mEditScale ;
    tquaternion *mEditQT;
	//draw helpers

	unsigned int mMask;
	void SnapIt(float &pos, float &snap)
	{
		float sn = (float)fmod(pos,snap);
		if (fabs(sn)< (snap*0.25f) ) pos-=sn;
		if (fabs(sn)> (snap*0.75f) ) pos=( (pos-sn) + ((sn>0)?snap:-snap) );
	}

    ZTransform *mTransform;
};

#endif // !defined(AFX_GIZMOTRANSFORM_H__913D353E_E420_4B1C_95F3_5A0258161651__INCLUDED_)
