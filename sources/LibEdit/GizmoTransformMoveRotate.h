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


#ifndef GIZMOTRANSFORMMOVEROTATE_H__
#define GIZMOTRANSFORMMOVEROTATE_H__


#include "GizmoTransformMove.h"
#include "GizmoTransformRotate.h"

class CGizmoTransformMoveRotate : 
	public CGizmoTransformMove
{
public:
	CGizmoTransformMoveRotate();
	virtual ~CGizmoTransformMoveRotate();

	virtual void SetScreenSize(int x, int y)
	{
		mTransMove.SetScreenSize(x,y);
		mTransRotate.SetScreenSize(x,y);
	}

	virtual void SetEditMatrix(tmatrix *pMatrix)
	{
		mTransMove.SetEditMatrix(pMatrix);
		mTransRotate.SetEditMatrix(pMatrix);
	}

	virtual void SetCameraMatrix(const tmatrix &Model, const tmatrix &Proj)
	{
		mTransMove.SetCameraMatrix(Model,Proj);
		mTransRotate.SetCameraMatrix(Model,Proj);
	}

	virtual void SetCam(PSM_Camera *pCam)
	{
		mTransMove.SetCam(pCam);
		mTransRotate.SetCam(pCam);
	}

	virtual bool OnMouseDown(unsigned int x, unsigned int y);
	virtual void OnMouseMove(unsigned int x, unsigned int y);
	virtual void OnMouseUp(unsigned int x, unsigned int y);

	virtual void Draw();

	// snap
	void UseSnap(bool bUseSnap)
	{
		m_bUseSnap = bUseSnap;
		mTransMove.UseSnap(bUseSnap);
		mTransRotate.UseSnap(bUseSnap);
	}

	virtual void ApplyTransform(tvector3& trans, bool bAbsolute) {}

private:
	CGizmoTransformMove mTransMove;
	CGizmoTransformRotate mTransRotate;
};

#endif // !defined(AFX_GIZMOTRANSFORMMOVEROTATE_H__F639C7CE_B711_4B98_B4EB_9F3AF57C8605__INCLUDED_)
