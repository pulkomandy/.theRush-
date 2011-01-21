///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZLight.h
// Creation : 26/11/2008
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

#ifndef ZLIGHT_H__
#define ZLIGHT_H__

#include "../libbase/ZSerializator.h"
#include "ZSpatialEntity.h"
#include "ZTransform.h"


DECLAREZCLASS(ZLight);
class ZFile;

enum LIGHT_TYPES
{
	LT_POINTLIGHT,
	LT_SPOT,
};
///////////////////////////////////////////////////////////////////////////////////////////////////

class ZLight : public ZSpatialEntity
{
public:
    DECLARE_SERIALIZABLE
public:
	ZLight() : ZSpatialEntity()
	{
		mColor = tcolor(1,1,1,1);
		mbActive = true;
		mLength = 1;
		mAttenNear = 0;
		mLightType = LT_POINTLIGHT;
		mRadius = 10;
		mAngle1 = 0.5f;
		mAngle2 = 1.f;
		mName.Printf("Light%03d",GetInstancesCount(ZLight));
	}
	virtual ~ZLight()
	{
#ifdef _DEBUG
#ifdef WIN32
	char tmps[512];
	sprintf_s(tmps, 512, "Destroying Light 0x%x \"%s\"\n", this, GetName().c_str());
	OutputDebugStringA(tmps);
#endif
#endif
	}

	virtual void SetActive(bool bActive )
	{
		mbActive = bActive;
	}
	virtual bool IsActive() const { return mbActive; }

	virtual void SetColor(const tcolor& color)
	{
		mColor.r = color.r;
		mColor.g = color.g;
		mColor.b = color.b;
	}
	virtual const tcolor& GetColor() const { return mColor; }

	virtual float GetRadius() const { return mRadius; }
	virtual void SetRadius(float aRadius) { mRadius = aRadius; ComputeBoxSize(); }

	virtual void SetPower(float aPower) { mColor.a = aPower; }
	virtual float GetPower() const { return mColor.a; }

	virtual ZSpatialEntity* Clone();

	virtual float GetLength() const { return mLength; }
	virtual void SetLength(float aLen) { mLength = aLen; ComputeBoxSize(); }

	virtual LIGHT_TYPES GetLightType() { return mLightType; }
	virtual void SetLightType(LIGHT_TYPES aType) { mLightType = aType; ComputeBoxSize(); }

	virtual void LoadLight(ZFile *pFile);
	virtual void SaveLight(ZFile *pFile);

	virtual float GetAttenNear() const { return mAttenNear; }
	virtual void SetAttenNear(float aten) { mAttenNear = aten; }
	virtual float GetSpotAngle1() const  { return mAngle1; }
	virtual void SetSpotAngle1(float ng) { mAngle1 = ng; ComputeBoxSize(); }
	virtual float GetSpotAngle2() const { return mAngle2; }
	virtual void SetSpotAngle2(float ng) { mAngle2 = ng; ComputeBoxSize(); }


	// Cluster

	void ComputeBoxSize()
	{
		switch (mLightType)
		{
		case LT_POINTLIGHT:
			mBoxCenter = tvector3::zero;
			mBoxSize = tvector3(mRadius, mRadius, mRadius);
			break;
		case LT_SPOT:
			mBoxCenter = mTransform->GetWorldMatrix().V4.position;
			mBoxCenter += mTransform->GetWorldMatrix().V4.dir * (mLength *0.5f);

			mBoxSize = tvector3(mLength*mAngle2*0.5f,
				mLength*mAngle2*0.5f,
				mLength * 0.5f );

			break;
		}
	}
protected:
	float mAngle1, mAngle2;
	float mAttenNear;
	bool mbActive;
	float mRadius;
	tcolor mColor;
	float mLength;
	LIGHT_TYPES mLightType;
	tvector3 mBoxSize, mBoxCenter;
};
#endif
