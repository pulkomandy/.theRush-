///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTrails.cpp
// Creation : 02/08/2007
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

#ifndef ZTRAILS_H__
#define ZTRAILS_H__



///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct trailVT
{
    tvector3 pos;// x,y,z;
    tvector3 dir;//float dirx, diry, dirz;
    tvector2 uv;//float u,v;
    float width,alpha;
} trailVT;

class ZTrails : public ZBaseClass
{

public:
    DECLARE_SERIALIZABLE

public:

    ZTrails()
    {
		mTrailVT = NULL;

        mIndex = 0;
        mbSecondHalf = false;
		mDeltaTime = 0;
		mAlphaAttenSpeed = 0.f;
    }

    virtual ~ZTrails()
    {
		if (mMesh)
		{
			mMesh->GetTransform()->RemoveFromScene();
		}

		if (mTrailVT)
			delete [] mTrailVT;

    }

	void SetAlphaAttenSpeed(float aAtten)
	{
		mAlphaAttenSpeed = aAtten;
	}

	void SetTrailColor(const tcolor& color)
	{
		mTrailColor->setVector((const tvector4&)color);
		mTrailColor->Apply();
	}


    void Init(uint32_t aNbPairs, ZFx *afx, ZTexture *ptex);

	void SetLastPair(const tvector3 &vt, float aAlpha, float aWidth, float timeEllapsed);

	void UpdateAlphaAndBB( float timeEllapsed )
	{
		// -- dec alpha
		float decAlpha = timeEllapsed * mAlphaAttenSpeed;

		tvector3 amin(9999999,9999999,9999999), amax(-9999999,-9999999,-9999999);
		trailVT *avt = (trailVT*)mTrailVT;
		for(unsigned int dca = 0;dca<mMaxIndex;dca++)
		{
			amin.Minimize(avt->pos);
			amax.Maximize(avt->pos);
			avt->alpha -=decAlpha;
			avt++;
		}
		mMesh->GetMesh()->GetBVolume().SetBoxMinMax(amin, amax);
		tvector3 bcent = (amin+amax)*0.5f;
		float rad = (amax-amin).Length()*0.5f;
		mMesh->GetMesh()->GetBVolume().SetBSphere(vector4(bcent, rad));
		mMesh->GetTransform()->Update();

		// --
	}

    void AddPair(const tvector3 &vt, float aAlpha, float aWidth, float timeEllapsed);

	void SetMarkerPosition(const tvector3& position, float aAlpha, float aWidth, float timeEllapsed)
	{
		mDeltaTime += timeEllapsed;//GetTimer()->GetEllapsed();
		static const float dtuptr = 0.1f;
		if (mDeltaTime > dtuptr)
		{
			AddPair(position, aAlpha, aWidth, timeEllapsed);
			mDeltaTime -= dtuptr;
		}
		else
		{
			SetLastPair(position, aAlpha, aWidth, timeEllapsed);
		}

	}

	virtual ZMeshInstance* GetTrailMesh() { return mMesh; }

protected:
    smartptr<ZMeshInstance> mMesh;
    uint32_t mMaxIndex;
    uint32_t mIndex;
    tvector3 mLastPair;
    tvector3 mLastDir;
    bool mbSecondHalf;
	trailVT *mTrailVT;
	float mDeltaTime;
	FFxSetParam *mTrailColor;
	float mAlphaAttenSpeed;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
