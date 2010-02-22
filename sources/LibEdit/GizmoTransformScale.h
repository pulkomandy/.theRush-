///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : GizmoTransformScale.h
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

#ifndef GIZMOTRANSFORMSCALE_H__
#define GIZMOTRANSFORMSCALE_H__

#include "GizmoTransform.h"

class CGizmoTransformScale : public CGizmoTransform  
{
public:
    DECLARE_SERIALIZABLE
public:
	CGizmoTransformScale();
	virtual ~CGizmoTransformScale();

	// return true if gizmo transform capture mouse
	virtual bool OnMouseDown(unsigned int x, unsigned int y);
	virtual void OnMouseMove(unsigned int x, unsigned int y);
	virtual void OnMouseUp(unsigned int x, unsigned int y);

	virtual void Draw();

    /*
	void SetScaleSnap(float snap)
	{
		m_ScaleSnap = snap;
	}
    */
    virtual void SetSnap(const float snap) {m_ScaleSnap = snap; }
    virtual void SetSnap(const tvector3& snap) {}

	float GetScaleSnap()
	{
		return m_ScaleSnap;
	}

	virtual void ApplyTransform(tvector3& trans, bool bAbsolute);

protected:
	enum SCALETYPE
	{
		SCALE_NONE,
		SCALE_X,
		SCALE_Y,
		SCALE_Z,
		SCALE_XY,
		SCALE_XZ,
		SCALE_YZ,
		SCALE_XYZ
	};
	SCALETYPE m_ScaleType,m_ScaleTypePredict;


	unsigned int m_LockX;
	float m_ScaleSnap;

	bool GetOpType(SCALETYPE &type, unsigned int x, unsigned int y);
	//tvector3 RayTrace(tvector3& rayOrigin, tvector3& rayDir, tvector3& norm, tmatrix& mt, tvector3 trss);
	void SnapScale(float &val);

};

#endif // !defined(AFX_GIZMOTRANSFORMSCALE_H__85E46839_15B0_4CE4_A85A_547015AF853A__INCLUDED_)
