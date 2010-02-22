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




#ifndef GIZMOTRANSFORMMOVE_H__
#define GIZMOTRANSFORMMOVE_H__


#include "GizmoTransform.h"

class CGizmoTransformMove : public CGizmoTransform  
{
public:
    DECLARE_SERIALIZABLE
public:
	CGizmoTransformMove();
	virtual ~CGizmoTransformMove();

	// return true if gizmo transform capture mouse
	virtual bool OnMouseDown(unsigned int x, unsigned int y);
	virtual void OnMouseMove(unsigned int x, unsigned int y);
	virtual void OnMouseUp(unsigned int x, unsigned int y);

	virtual void Draw();
	// snap

	virtual void SetSnap(const tvector3& snap)
	{
		m_MoveSnap = snap;
	}
    virtual void SetSnap(const float snap) {}

	tvector3 GetMoveSnap()
	{
		return m_MoveSnap;
	}

	virtual void ApplyTransform(tvector3& trans, bool bAbsolute);

protected:
	enum MOVETYPE
	{
		MOVE_NONE,
		MOVE_X,
		MOVE_Y,
		MOVE_Z,
		MOVE_XY,
		MOVE_XZ,
		MOVE_YZ,
		MOVE_XYZ
	};
	MOVETYPE m_MoveType,m_MoveTypePredict;
	//tplane m_plan;
	//tvector3 m_LockVertex;
	tvector3 m_MoveSnap;

	bool GetOpType(MOVETYPE &type, unsigned int x, unsigned int y);
	tvector3 RayTrace(tvector3& rayOrigin, tvector3& rayDir, tvector3& norm);
};

#endif // !defined(AFX_GIZMOTRANSFORMMOVE_H__8276C568_C663_463C_AE7F_B913E2A712A4__INCLUDED_)
