///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZCamera.h
// Creation : 15/06/2007
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

#include "stdafx.h"
#include "../libplatform/IInput.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

ZCamera *mActiveCamera = NULL;
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZCamera)
/*SERIALIZE_AGREGAT(mTransform)*/
SERIALIZE_3FLOAT(mNearPlane, mFarPlane, mVFov)
SERIALIZE_BOOL(mbIsOrtho)
SERIALIZE_MATRIX(mProjection)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZCamera::BuildRay(int x, int y, int frameX, int frameY, tvector3 &rayOrigin, tvector3 &rayDir)
{
    tvector3 screen_space;
    y=frameY-y;
    x=frameX-x;
    // device space to normalized screen space
    screen_space.x = ( ( (2.f * (float)x) / frameX ) - 1 ) / mProjection.m[0][0];//.right.x;
    screen_space.y = -( ( (2.f * (float)y) / frameY ) - 1 ) / mProjection.m[1][1];
    screen_space.z = -1.f;


    // screen space to world space

    //rayDir = -rayDir;
    const tmatrix camMat = GetTransform()->GetWorldMatrix();
	/*
    tmatrix oo;
    oo.Transpose(GetTransform()->GetWorldMatrix());
    rayOrigin.TransformPoint(tvector3(-oldm.V4.position.x, 
		-oldm.V4.position.y, 
		-oldm.V4.position.z), oo);
		*/
	rayOrigin = camMat.V4.position;

    rayDir.TransformVector(screen_space, camMat);
    rayDir.Normalize();
    rayDir=-rayDir;


    //rayOrigin.TransformPoint(tvector3::zero, mTransform.GetWorldMatrix());// = tvector3(mTransform.GetWorldMatrix().m[3][0], mTransform.GetWorldMatrix().m[3][1], mTransform.GetWorldMatrix().m[3][2] );

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZCamera::ComputeFPSView(float nXDiff, float nYDiff)
{
    if ((nXDiff != 0.0f)||(nYDiff != 0.0f))
    {
        tvector3	g_vEye;
        const tmatrix oldm = GetTransform()->GetWorldMatrix();
		/*
        tmatrix oo;
        oo.Transpose(GetTransform()->GetWorldMatrix());
        g_vEye.TransformPoint(tvector3(-oldm.V4.position.x, -oldm.V4.position.y, -oldm.V4.position.z), oo);
		*/
		g_vEye = oldm.V4.position;

/*
        tvector3	g_vLook(oo.V4.dir.x, oo.V4.dir.y, oo.V4.dir.z);  // Look Vector
        tvector3	g_vUp(oo.V4.up.x, oo.V4.up.y, oo.V4.up.z);      // Up Vector
        tvector3	g_vRight(oo.V4.right.x, oo.V4.right.y, oo.V4.right.z);   // Right Vector
		*/
		tvector3	g_vLook = oldm.V4.dir;
		tvector3	g_vUp = oldm.V4.up;
		tvector3	g_vRight = oldm.V4.right;


        // make rotation matrices
        tmatrix matRotation;


        matRotation.RotationAxis(g_vRight, nYDiff*0.01f);
		g_vLook.TransformVector(matRotation);
        g_vUp.TransformVector(matRotation);


        matRotation.RotationAxis(tvector3::YAxis, nXDiff*0.01f);
        g_vLook.TransformVector(matRotation);
        g_vUp.TransformVector(matRotation);

        // make view matrix

        g_vLook.Normalize();
        g_vRight.Cross(g_vUp, g_vLook);
        g_vRight.Normalize();
        g_vUp.Cross(g_vLook, g_vRight);
        g_vUp.Normalize();

        tmatrix view(
            g_vRight.x,
            g_vRight.y,
            g_vRight.z,
            0.0f,

            g_vUp.x,
            g_vUp.y,
            g_vUp.z,
            0.0f,

            g_vLook.x,
            g_vLook.y,
            g_vLook.z,
            0.0f,
/*
            -D3DXVec3Dot( (D3DXVECTOR3 *)&g_vEye, (D3DXVECTOR3 *)&g_vRight ),
            -D3DXVec3Dot( (D3DXVECTOR3 *)&g_vEye, (D3DXVECTOR3 *)&g_vUp ),
            -D3DXVec3Dot( (D3DXVECTOR3 *)&g_vEye, (D3DXVECTOR3 *)&g_vLook ) ,
            */
            g_vEye.x, //-g_vEye.Dot(g_vRight),
            g_vEye.y, //-g_vEye.Dot(g_vUp),
            g_vEye.z, //-g_vEye.Dot(g_vLook),
            1);

		//view.OrthoNormalize();
        GetTransform()->SetLocalMatrix(view);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZCamera::UpdateFPSMoves()
{
	float interv = mInterv;

	if (GetInputDevice()->KeyDown(TKEY_a))
	{
		GetTransform()->PostTranslate(tvector3( -interv, 0, 0 ));
	}
	if (GetInputDevice()->KeyDown(TKEY_d))
	{
		GetTransform()->PostTranslate(tvector3( interv, 0, 0 ));
	}

	if (GetInputDevice()->KeyDown(TKEY_w))
	{
		GetTransform()->PostTranslate(tvector3( 0, 0, interv ));
	}
	if (GetInputDevice()->KeyDown(TKEY_s))
	{
		GetTransform()->PostTranslate(tvector3( 0, 0, -interv ));
	}
	/*
	if (GetInputDevice()->KeyDown(TKEY_P))
	{
	GDD->CaptureZBuffer("c:\\temp\\ZBuffer1.dds");
	}

	if (GetInputDevice()->KeyDown(TKEY_O))
	{
	GDD->CaptureZBuffer("c:\\temp\\ColorBuffer1.dds");
	}
	*/
	/*
	static float ttoa = 0.1f;
	tmatrix tto;
	tto.RotationZ(ttoa);
	GetTransform()->SetLocalMatrix(GetTransform()->GetLocalMatrix() * tto);
	*/
	//ttoa += 0.01f;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

