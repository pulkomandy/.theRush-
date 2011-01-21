///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZCamera.h
// Creation : 15/06/2007
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

#ifndef ZCAMERA_H__
#define ZCAMERA_H__

#include "../libbase/ZFrustum.h"
#include "../libworld/ZTransform.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

DECLAREZCLASS(ZCamera);
class ZCamera;

extern ZCamera *mActiveCamera;
///////////////////////////////////////////////////////////////////////////////////////////////////

class ZCamera : public ZBaseClass
{

public:
    DECLARE_SERIALIZABLE

public:

    ZCamera()
    {
		mInterv = 2.f;
        mNearPlane = 0.1f;
        mFarPlane = 1000.0f;
        mVFov = DEGTORAD * 70;
        mRatio = 1;
        mbIsOrtho = false;
		mTransform.Attach( (ZTransform*)_New(ZTransform));
		mRoll = 0;
    }

    virtual ~ZCamera()
    {
    }

    virtual void SetVFov(float aVFov) { mVFov = aVFov; UpdateProjection(); }
    virtual float GetVFov() { return mVFov; }
    virtual tulong GetMemoryUsed() const { return sizeof(ZCamera); }

    virtual void SetPlanes(float aNear, float aFar) { mNearPlane = aNear; mFarPlane = aFar; UpdateProjection(); }
    virtual float GetFarPlane() const { return mFarPlane; }
    virtual float GetNearPlane() const { return mNearPlane; }
	virtual tvector3		GetWorldPosition()
	{
		return GetTransform()->GetWorldMatrix().V4.position;
	}

    virtual void SetRatio(float ratio) { mRatio = ratio; UpdateProjection(); }
	virtual float			GetRatio() { return mRatio; }
    virtual void Update()
    {
		mTransform->Update();
        UpdateFrustum();
    }

    virtual const tmatrix& GetProjectionMatrix() const
    {
        return mProjection;
    }

    virtual ZTransform*     GetTransform() const
    {
		return mTransform.ptr();//static_cast<ZTransform&>(*this);
    }

    virtual void BuildRay(int x, int y, int frameX, int frameY, tvector3 &rayOrigin, tvector3 &rayDir);
    virtual void ComputeFPSView(float nXDiff, float nYDiff);

    virtual void            SetProjectionMatrix(const tmatrix &mat)
    {
        mProjection = mat;
    }

    virtual const ZFrustum* GetFrustum()
    {
        return &mFrustum;
    }

	virtual void			SetActive() { mActiveCamera = this; }

	virtual void			SetFPSMoveSpeed(float ainterv)
	{
		mInterv = ainterv;
	}

	virtual void			UpdateFPSMoves();

	virtual void			SetRoll(float aRoll) { mRoll = aRoll; }
	virtual float			GetRoll() { return mRoll; }
public:

	float		mRoll;
    tmatrix     mProjection;

    float      mNearPlane;
    float      mFarPlane;

    float      mVFov;
    bool       mbIsOrtho;
    float      mRatio;
	float		mInterv;

    smartptr<ZTransform>  mTransform;
    ZFrustum    mFrustum;
    void UpdateProjection()
    {
        mProjection.PerspectiveFovLH2(mVFov, mRatio, mNearPlane, mFarPlane);
        UpdateFrustum();
    }
    void UpdateFrustum()
    {
		tmatrix mt;
		mt.Inverse( mTransform->GetWorldMatrix() );
        mFrustum.Update( mt, mProjection );
    }

};




inline ZCamera *GetActiveCamera()
{
	return mActiveCamera;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
