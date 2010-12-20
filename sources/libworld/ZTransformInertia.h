///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTransformInertia.h
// Creation : 23/06/2007
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

#ifndef ZTRANSFORMINERTIA_H__
#define ZTRANSFORMINERTIA_H__

#include "ZTransform.h"

class ZScene;

DECLAREZCLASS(ZTransformInertia);

class ZTransformInertia : public ZTransform
{
public:
    DECLARE_SERIALIZABLE
public:
    ZTransformInertia()
    {
		mTimeFactor = 1.f;
		mMaximumDistance = 100000.f;
    }
    virtual ~ZTransformInertia()
    {
	}


	void SetTimeFactor(float aTime) { mTimeFactor = aTime; }
	void SetMaximumDistance(float aDist) { mMaximumDistance = aDist; }

	float GetTimeFactor() const { return mTimeFactor; }
	float GetMaximumDistance() const { return mMaximumDistance; }
protected:
	
	virtual void ComputeWorldMatrix();

	float mTimeFactor;
	float mMaximumDistance;

};

#endif
