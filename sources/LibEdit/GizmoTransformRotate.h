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

#ifndef GIZMOTRANSFORMROTATE_H__
#define GIZMOTRANSFORMROTATE_H__

#include "GizmoTransform.h"

class CGizmoTransformRotate : public CGizmoTransform  
{
public:
    DECLARE_SERIALIZABLE
public:
	CGizmoTransformRotate();
	virtual ~CGizmoTransformRotate();

	// return true if gizmo transform capture mouse
	virtual bool OnMouseDown(unsigned int x, unsigned int y);
	virtual void OnMouseMove(unsigned int x, unsigned int y);
	virtual void OnMouseUp(unsigned int x, unsigned int y);

	virtual void Draw();

    virtual void SetSnap(const float snap) {m_AngleSnap = snap; }
    virtual void SetSnap(const tvector3& snap) {}
    /*
	void SetAngleSnap(float snap)
	{
		m_AngleSnap = snap;
	}
    */

	float GetAngleSnap()
	{
		return m_AngleSnap;
	}

	virtual void ApplyTransform(tvector3& trans, bool bAbsolute);

protected:
	enum ROTATETYPE
	{
		ROTATE_NONE,
		ROTATE_X,
		ROTATE_Y,
		ROTATE_Z,
		ROTATE_SCREEN,
		ROTATE_TWIN
	};
	ROTATETYPE m_RotateType,m_RotateTypePredict;
	tplane m_plan;
	tvector3 m_LockVertex,m_LockVertex2;
	float m_Ng2;
	tvector3 m_Vtx,m_Vty,m_Vtz;
	tvector3 m_Axis,m_Axis2;
	tmatrix m_OrigScale,m_InvOrigScale;
	float m_AngleSnap;


	bool GetOpType(ROTATETYPE &type, unsigned int x, unsigned int y);
	bool CheckRotatePlan(tvector3 &vNorm, float factor, const tvector3 &rayOrig,const tvector3 &rayDir,int id);
	void Rotate1Axe(const tvector3& rayOrigin,const tvector3& rayDir);

};

#endif // !defined(AFX_GIZMOTRANSFORMROTATE_H__F92EF632_4BAE_49CE_B7B8_213704C82589__INCLUDED_)
