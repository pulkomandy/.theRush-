///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZBVolume.h
// Creation : 11/05/2009
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

#ifndef ZBVOLUME_H__
#define ZBVOLUME_H__

#include "../libbase/ZBaseMaths.h"

typedef struct ZBoundingVolume
{
public:
    void SetBoxMinMax(const tvector3 &aMin, const tvector3 &aMax)
    {
        mBoxMin = aMin;
        mBoxMax = aMax;
    }
	bool PointInVolume(const tvector3 &point) const
	{
		return ( (point.x >= mBoxMin.x)&&(point.x <= mBoxMax.x) &&
			(point.y >= mBoxMin.y)&&(point.x <= mBoxMax.y) &&
			(point.z >= mBoxMin.z)&&(point.x <= mBoxMax.z) );
	}

    const tvector3& GetBoxMin() const { return mBoxMin; }
    const tvector3& GetBoxMax() const { return mBoxMax; }

	void SetBSphere(const tvector4& bSphere) { mBSphere = bSphere; }
	const tvector4& GetBSphere() const { return mBSphere; }

	void Build8CornersBox(tvector3 *pVerts)
	{
		pVerts[0] = tvector3(mBoxMin.x, mBoxMin.y, mBoxMin.z);
		pVerts[1] = tvector3(mBoxMax.x, mBoxMin.y, mBoxMin.z);
		pVerts[2] = tvector3(mBoxMin.x, mBoxMax.y, mBoxMin.z);
		pVerts[3] = tvector3(mBoxMax.x, mBoxMax.y, mBoxMin.z);
		pVerts[4] = tvector3(mBoxMin.x, mBoxMin.y, mBoxMax.z);
		pVerts[5] = tvector3(mBoxMax.x, mBoxMin.y, mBoxMax.z);
		pVerts[6] = tvector3(mBoxMin.x, mBoxMax.y, mBoxMax.z);
		pVerts[7] = tvector3(mBoxMax.x, mBoxMax.y, mBoxMax.z);
	}

	void SetMinMaxFromCorners(unsigned char *pv, int sizeOfVertex, int vertexCount)
	{
		unsigned char *pv2 = pv;
		tvector3 vtMin(99999999.f, 99999999.f, 99999999.f), vtMax(-99999999.f, -99999999.f, -99999999.f);
		for (int i=0;i<vertexCount;i++)
		{
			vtMin.Minimize(*((tvector3*)pv2));
			vtMax.Maximize(*((tvector3*)pv2));
			pv2 += sizeOfVertex;
		}
		mBoxMin = vtMin;
		mBoxMax = vtMax;
	}

	void ComputeBSphereFromBoxMinMax()
	{
		tvector3 mid = (mBoxMin+mBoxMax)*0.5f;
		mBSphere = vector4(mid, Distance(mid, mBoxMin));
	}

	void TransformBSphere(const tvector4& sphere, const tmatrix& mat)
	{
		tvector3 radius(sphere.w,0,0);
		mBSphere.Transform(vector4(sphere,1), mat);
		radius.TransformVector(mat);
		mBSphere.w = radius.Length();
	}

    tvector3 mBoxMin,mBoxMax;
	tvector4 mBSphere;

} ZBoundingVolume;

#endif
