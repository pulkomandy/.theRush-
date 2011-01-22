///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : IGizmo.h
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

#ifndef IGIZMO_H__
#define IGIZMO_H__

DECLAREZCLASS(CGizmoTransformMove);
DECLAREZCLASS(CGizmoTransformRotate);
DECLAREZCLASS(CGizmoTransformScale);

class IGizmo : public ZBaseClass
{
public:
    enum LOCATION
    {
        LOCATE_VIEW,
        LOCATE_WORLD,
        LOCATE_LOCAL,
    };

	enum ROTATE_AXIS
	{
		AXIS_X = 1,
		AXIS_Y = 2,
		AXIS_Z = 4,
		AXIS_TRACKBALL = 8,
		AXIS_SCREEN = 16,
		AXIS_ALL = 31

	};


    virtual void SetCam(ZCamera *pCam) = 0;
    virtual void SetScreenSize(int x, int y) = 0;
	virtual void SetEditMatrix(tmatrix *pMatrix) = 0;
    virtual void SetEditTransform(ZTransform *pTransform) = 0;
    virtual void SetEditVQS(tvector3* aPosition, tquaternion *aQT, tvector3 *aScale) = 0;
	virtual void SetCameraMatrix(const tmatrix &Model, const tmatrix &Proj) = 0;

    // return true if gizmo transform capture mouse
	virtual bool OnMouseDown(unsigned int x, unsigned int y) = 0;
	virtual void OnMouseMove(unsigned int x, unsigned int y) = 0;
	virtual void OnMouseUp(unsigned int x, unsigned int y) = 0;

    virtual void UseSnap(bool bUseSnap) = 0;
	virtual bool IsUsingSnap() = 0;
    virtual void SetSnap(const tvector3& snap) = 0;
    virtual void SetSnap(const float snap) = 0;


    virtual void SetLocation(LOCATION aLocation)  = 0;
    virtual LOCATION GetLocation() = 0;
	virtual void SetAxisMask(unsigned int mask) = 0;




	virtual void Draw() = 0;
};

#endif
