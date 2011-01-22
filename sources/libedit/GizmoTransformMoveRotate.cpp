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

#include "GizmoTransformMoveRotate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGizmoTransformMoveRotate::CGizmoTransformMoveRotate()
{

}

CGizmoTransformMoveRotate::~CGizmoTransformMoveRotate()
{

}


bool CGizmoTransformMoveRotate::OnMouseDown(unsigned int x, unsigned int y)
{
	if (mTransMove.OnMouseDown(x,y)) return true;
	if (mTransRotate.OnMouseDown(x,y)) return true;
	return false;
}

void CGizmoTransformMoveRotate::OnMouseMove(unsigned int x, unsigned int y)
{
	mTransMove.OnMouseMove(x,y);
	mTransRotate.OnMouseMove(x,y);
}

void CGizmoTransformMoveRotate::OnMouseUp(unsigned int x, unsigned int y)
{
	mTransMove.OnMouseUp(x,y);
	mTransRotate.OnMouseUp(x,y);
}

void CGizmoTransformMoveRotate::Draw()
{
	mTransMove.Draw();
	mTransRotate.Draw();
}
