///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxNavigator.cpp
// Creation : 08/11/2007
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
//#include "wxStdAfx.h"
#include "PSM_ZenCameraView.h"
//#include "wxMainFrame.h"
#include "../libbase/LibBase.h"
#include "../libworld/libworld.h"
//#include "../libplatform/libplatform.h"
#include "../libplatform/IInput.h"
/*
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
float interv = 0.5f;

PSM_ZenCameraView::PSM_ZenCameraView()
{
    mCurrentCameraManipulation = CAMERA_NONE;
    mbMouseCaptured = false;

    mFPSMode_KEY_LEFT     = TKEY_LEFT;
    mFPSMode_KEY_RIGHT    = TKEY_RIGHT;
    mFPSMode_KEY_UP       = TKEY_UP;
    mFPSMode_KEY_DOWN     = TKEY_DOWN;
    mFPSMode_KEY_SPACE    = TKEY_SPACE;
    mFPSMode_KEY_CONTROL  = TKEY_CONTROL;
//    mRotateAroundNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void PSM_ZenCameraView::Tick()
{
    if (mCurrentCameraManipulation == CAMERA_FPS)
    {
        float aStrafe = 0;
        float aWalk = 0;
        float aUp = 0;

ZCamera *pCam = (ZCamera*)FirstInstanceOf(ZCamera);
pCam->UpdateFPSMoves();
pCam->Update();


/*
        if (PSM_InputDevice::GetInputDevice()->KeyDown(mFPSMode_KEY_LEFT))
            aStrafe = 1;
        if (PSM_InputDevice::GetInputDevice()->KeyDown(mFPSMode_KEY_RIGHT))
            aStrafe = -1;

        if (PSM_InputDevice::GetInputDevice()->KeyDown(TKEY_CONTROL))
            aUp = -1;
        if (PSM_InputDevice::GetInputDevice()->KeyDown(TKEY_SPACE))
            aUp = 1;

        if (PSM_InputDevice::GetInputDevice()->KeyDown(TKEY_DOWN))
            aWalk = -1;
        if (PSM_InputDevice::GetInputDevice()->KeyDown(TKEY_UP))
            aWalk = 1;

        float elapsed = psm->timer->GetElapsed();
        if (aUp != 0)
        gCurrentWorld->GetCameraManager().GetCurrent()->Translate(PSM_NODE_BASE_WORLD,
            tvector3(0,1,0) * elapsed * aUp );

        if ((aWalk != 0) || (aStrafe != 0))
        gCurrentWorld->GetCameraManager().GetCurrent()->Translate(PSM_NODE_BASE_LOCAL,
            ((tvector3(0,-1,0)* aWalk) + (tvector3(1,0,0)* aStrafe) ) * elapsed);
*/
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool PSM_ZenCameraView::LButtonDown(int mx, int my, bool aCtrlDown, bool aShiftDown, bool aAltDown)
{
    mPointerX = mx;
    mPointerY = my;

    if (aCtrlDown)
    {
        //CaptureMouse(); //
        mCurrentCameraManipulation = CAMERA_STRAFE;
        mbMouseCaptured = true;
    }
    else
    if (aAltDown)
    {
        /*
        if (mRotateAroundNode)
        {
            mCurrentCameraManipulation = CAMERA_ROTATE_AROUND;
            mbMouseCaptured = true;
        }
        */
    }
    return mbMouseCaptured;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool PSM_ZenCameraView::StopCameraManipulation()
{
    mCurrentCameraManipulation = CAMERA_NONE;
    bool bRet = mbMouseCaptured;

    mbMouseCaptured = false;
    return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool PSM_ZenCameraView::RButtonDown(int mx, int my, bool aCtrlDown, bool aShiftDown, bool aAltDown)
{
    mPointerX = mx;
    mPointerY = my;

    //CaptureMouse(); //
    mCurrentCameraManipulation = CAMERA_FPS;
    mbMouseCaptured = true;
    return mbMouseCaptured;
}

bool PSM_ZenCameraView::MButtonDown(int mx, int my, bool aCtrlDown, bool aShiftDown, bool aAltDown)
{
    mPointerX = mx;
    mPointerY = my;

    //CaptureMouse(); //
    mCurrentCameraManipulation = CAMERA_STRAFE_UPDOWN_RIGHTLEFT;
    mbMouseCaptured = true;
    return mbMouseCaptured;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool PSM_ZenCameraView::OnMouseMove(int mx, int my)
{
    ZCamera *pCam = (ZCamera*)FirstInstanceOf(ZCamera);
    switch (mCurrentCameraManipulation)
    {
    case CAMERA_NONE:
        break;
    case CAMERA_STRAFE:
        /*
        gCurrentWorld->GetCameraManager().GetCurrent()->Translate(PSM_NODE_BASE_LOCAL,
            tvector3(mx - mPointerX, 0, 0) * 0.1f);
        gCurrentWorld->GetCameraManager().GetCurrent()->Translate(PSM_NODE_BASE_WORLD,
            tvector3(0, my - mPointerY, 0) * 0.1f);

        mPointerX = mx;
        mPointerY = my;
        */
        break;
    case CAMERA_FPS:
{


        //if (GetInputDevice()->MouseButDown(1))
        {
            float nXDiff = (mx-mPointerX);//*0.1f;//GetInputDevice()->MouseDeltaX();
            float nYDiff = (my-mPointerY);//*0.1f;//GetInputDevice()->MouseDeltaY();
            pCam->ComputeFPSView(nXDiff, nYDiff);
            mPointerX = mx;
            mPointerY = my;
        }

        pCam->Update();
}
        break;
	case CAMERA_STRAFE_UPDOWN_RIGHTLEFT:
		{
            float nXDiff = (mx-mPointerX);//*0.1f;//GetInputDevice()->MouseDeltaX();
            float nYDiff = (my-mPointerY);//*0.1f;//GetInputDevice()->MouseDeltaY();
            //pCam->ComputeFPSView(nXDiff, nYDiff);
			const tmatrix& cwm = pCam->GetTransform()->GetWorldMatrix();

			pCam->GetTransform()->PreTranslate(-nXDiff*cwm.V4.right);
            pCam->GetTransform()->PreTranslate(nYDiff*cwm.V4.up);


            mPointerX = mx;
            mPointerY = my;

			pCam->Update();
		}
		break;
    case CAMERA_ROTATE_AROUND:
        {
            /*
            if (!mRotateAroundNode) return true;

            PSM_Camera *pCam = gCurrentWorld->GetCameraManager().GetCurrent();

            tmatrix rt1, rt2, res, t1, t2;

            tvector3 tcamPos(pCam->finalTM.position.x,pCam->finalTM.position.y,pCam->finalTM.position.z);

            tvector3 tobjPos(mRotateAroundNode->finalTM.position.x,mRotateAroundNode->finalTM.position.y,mRotateAroundNode->finalTM.position.z);


            t1.Translation(-tobjPos);
            t2.Translation(tobjPos);

            rt1.RotationAxis(tvector3::up,-(mx - mPointerX)*0.005f);
            rt2.RotationAxis(pCam->right,(my - mPointerY)*0.005f);


            pCam->baseTM = pCam->baseTM * t1 * rt2 * rt1 * t2;


            mPointerX = mx;
            mPointerY = my;
            */
        }
        break;
    }


    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void PSM_ZenCameraView::OnMouseWheel(int wheelDirection)
{
	ZCamera *pCam = GetActiveCamera();//(ZCamera*)FirstInstanceOf(ZCamera);
	pCam->GetTransform()->PostTranslate( tvector3( 0, 0, 1.f )*wheelDirection/*event.GetWheelRotation()*/*0.4f);
	pCam->Update();
    /*
    if (wheelDirection<0)
    {
        gCurrentWorld->GetCameraManager().GetCurrent()->Translate(PSM_NODE_BASE_LOCAL, tvector3(0,10,0));
    }
    else
    {
        gCurrentWorld->GetCameraManager().GetCurrent()->Translate(PSM_NODE_BASE_LOCAL, tvector3(0,-10,0));
    }
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////
