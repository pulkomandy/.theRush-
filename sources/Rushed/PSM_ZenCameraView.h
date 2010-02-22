///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxNavigator.h
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

#ifndef PSM_ZENCAMERAVIEW_H__
#define PSM_ZENCAMERAVIEW_H__

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

class PSM_ZenCameraView
{
public:

    PSM_ZenCameraView();

    virtual ~PSM_ZenCameraView() {}


    void Tick(); // in frame loop
    // those 3 functions returns true if they need a mouse capture
    bool LButtonDown(int mx, int my, bool aCtrlDown, bool aShiftDown, bool aAltDown);
    bool RButtonDown(int mx, int my, bool aCtrlDown, bool aShiftDown, bool aAltDown);
	bool MButtonDown(int mx, int my, bool aCtrlDown, bool aShiftDown, bool aAltDown);
    bool OnMouseMove(int mx, int my);
    void OnMouseWheel(int wheelDirection);
    bool StopCameraManipulation();
//    void SetSelectedNode(PSM_Node *pNode) { mRotateAroundNode = pNode; }

private:

    enum CAMERA_MANIPULATION
    {
        CAMERA_NONE,
        CAMERA_STRAFE,
        CAMERA_FPS,
        CAMERA_ROTATE_AROUND,
		CAMERA_STRAFE_UPDOWN_RIGHTLEFT,
    };

    CAMERA_MANIPULATION mCurrentCameraManipulation;
    int mPointerX, mPointerY;
    bool mbMouseCaptured;
//    PSM_Node *mRotateAroundNode;
    float mRotateAroundDistance;

    unsigned char mFPSMode_KEY_LEFT;
    unsigned char mFPSMode_KEY_RIGHT;
    unsigned char mFPSMode_KEY_UP;
    unsigned char mFPSMode_KEY_DOWN;
    unsigned char mFPSMode_KEY_SPACE;
    unsigned char mFPSMode_KEY_CONTROL;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
