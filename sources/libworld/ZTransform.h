///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTransform.h
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

#ifndef ZTRANSFORM_H__
#define ZTRANSFORM_H__

#include "ZSpatialEntity.h"
#include "../libbase/ZSmartPtr.h"

class ZScene;

DECLAREZCLASS(ZTransform);

class ZTransform : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
    ZTransform()
    {
        mParent = NULL;
        mLocalMatrix = tmatrix::identity;
        mWorldMatrix = tmatrix::identity;
		mBaseScale = tvector3::one;
		mBaseRotation = tquaternion::identity;
		mBaseTranslation = tvector3::zero;
		mPivot = tmatrix::identity;
		mScene = NULL; 
		/*
#ifdef _DEBUG
		printf("New Transform 0x%x\n", this);
#endif
		*/

    }
    virtual ~ZTransform()
    {
#ifdef _DEBUG
		LOG("Destroying Transform 0x%x\n", this);
#endif
		
		std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
		for (;iter != mChilds.end() ; ++iter)
		{
			(*iter)->mParent = NULL;
		}
    }


    virtual void  SetLocalMatrix(const tmatrix &amat) { mLocalMatrix = amat; }
    virtual const tmatrix& GetLocalMatrix() const { return mLocalMatrix; }

    virtual const tmatrix& GetPreviousWorldMatrix()
	{
		return mPreviousWorldMatrix;
	}
    virtual void  SetPreviousWorldMatrix(const tmatrix &amat)
	{
		mPreviousWorldMatrix = amat;
	}


    virtual void  SetWorldMatrix(const tmatrix &amat) { mWorldMatrix = amat; }
    virtual const tmatrix& GetWorldMatrix() const { return mWorldMatrix; }

    virtual void Update();
    virtual void PreTranslate(const tvector3& trans)
    {
        tmatrix mt;
        mt.Translation(trans);
        mLocalMatrix *= mt;
        /*
        mt *= mLocalMatrix;
        mLocalMatrix = mt;
        */
    }
    virtual void PostTranslate(const tvector3& trans)
    {
        tmatrix mt;
        mt.Translation(trans);
        //mLocalMatrix *= mt;
        mt *= mLocalMatrix;
        mLocalMatrix = mt;
    }

    virtual void PreScale(const tvector3& sc)
	{
        tmatrix mt;
        mt.Scaling(sc);
        mLocalMatrix *= mt;
	}
    virtual void PostScale(const tvector3& sc)
	{
        tmatrix mt;
        mt.Scaling(sc);
        //mLocalMatrix *= mt;
        mt *= mLocalMatrix;
        mLocalMatrix = mt;
	}


    virtual void PreRotate(const tvector4& rotate)
    {
        tmatrix mt;
        mt.RotationAxis((tvector3&)rotate, rotate.w);
        mLocalMatrix *= mt;
        /*
        mt *= mLocalMatrix;
        mLocalMatrix = mt;
        */
    }

    virtual void PostRotate(const tvector4& rotate)
    {
        tmatrix mt;
        mt.RotationAxis((tvector3&)rotate, rotate.w);
        mt *= mLocalMatrix;
        mLocalMatrix = mt;
    }


    virtual const tvector4& WorldPosition() const
    {
        return /*(const tvector3& )*/mWorldMatrix.V4.position;
    }

    virtual void SetWorldPosition(const tvector3 &pos)
    {
        mLocalMatrix.V4.position = pos;
    }

    virtual const tvector4& LocalPosition() const
    {
        return /*(const tvector3& )*/mLocalMatrix.V4.position;
    }
    virtual void SetLocalPosition(const tvector3 &pos)
    {
        mLocalMatrix.V4.position = pos;
		mLocalMatrix.V4.position.w = 1.f;
    }

	/* FIXME
	virtual IAnimController * GetAnimCtrl()
	{
		if (!mAnimCtrl.ptr())
		{
			mAnimCtrl.Attach((IAnimController*)_New(ZAnimController));
			mAnimCtrl->SetOwnerTransform(this);
		}
		return mAnimCtrl.ptr();
	}
	*/

	virtual void  SetPivot(const tmatrix &amat) { mPivot = amat; }
	virtual const tmatrix& GetPivot() const { return mPivot; }


	virtual void SetBaseScale(const tvector3& vt) { mBaseScale = vt; }
	virtual const tvector3& GetBaseScale() const { return mBaseScale; }
	virtual void SetBaseTranslation(const tvector3& vt) { mBaseTranslation = vt; }
	virtual const tvector3& GetBaseTranslation() const { return mBaseTranslation; }
	virtual void SetBaseRotation(const tquaternion& vt) { mBaseRotation = vt; }
	virtual const tquaternion& GetBaseRotation() const { return mBaseRotation; }

	virtual tulong GetMemoryUsed() const
	{
		return (sizeof(ZTransform)+mChilds.size()*sizeof(smartptr<ZTransform>));
	}


	// Spatial Entity Management
	virtual void AttachSpatialEntity(ZSpatialEntity *pSE)
	{
		if (mSpatialEntity.ptr())
			mSpatialEntity->SetTransform(NULL);

		mSpatialEntity.Attach(pSE);
		// can assign NULL spatial entity
		if (pSE)
			pSE->SetTransform(this);
	}

	ZSpatialEntity *GetSpatialEntity() const { return mSpatialEntity.ptr(); }
	
	// Hierarchy

	void AddChild(ZTransform *pTrans)
	{
		pTrans->SetParent(this);
	}

	void RemoveChild( ZTransform *pTrans)
	{
		pTrans->SetParent(NULL);
	}

	virtual void SetParent(ZTransform* pParent)
	{
		if (pParent)
			((ZTransform*)pParent)->mChilds.push_back(this);
		if (mParent)
		{
			ZTransform * tmpParent = mParent;
			mParent = NULL;
			if (GetRefCount()>1)
				tmpParent->mChilds.remove(this);
			else
			{
				tmpParent->mChilds.remove(this);
				return;
			}
		}
		mParent = (ZTransform*)pParent;
	}

	ZTransform* GetParent() const { return mParent; }

	virtual ZTransform *GetRootTransform()
	{
		if (!mParent)
			return this;
		
		return mParent->GetRootTransform();
	}


	virtual ZTransform* Clone()
	{
		ZTransform *nTrans;

		if (mSpatialEntity.ptr())
		{
			nTrans = (ZTransform*)mSpatialEntity->Clone()->GetTransform();
		}
		else
		{
			nTrans = (ZTransform*)_New(ZTransform);//(ZTransform*)NewClass(ZTransform);
		}
		// copy shits from ztransform
		nTrans->Copy(this);
		return nTrans;
	}

	virtual void Copy(ZTransform *pTrans)
	{
		ZTransform *ptr = (ZTransform*)pTrans;

		// copy properties
		SetName(ptr->GetName());
		mLocalMatrix = ptr->mLocalMatrix;
		mWorldMatrix = ptr->mWorldMatrix;
		mPreviousWorldMatrix = ptr->mPreviousWorldMatrix;
		mPivot = ptr->mPivot;

		mBaseScale = ptr->mBaseScale;
		mBaseTranslation = ptr->mBaseTranslation;
		mBaseRotation = ptr->mBaseRotation;

		mExternRef = ptr->mExternRef;

		/* FIXME
		if (ptr->mAnimCtrl.ptr())
		{
			mAnimCtrl.Attach(ptr->mAnimCtrl->Clone());
			mAnimCtrl->SetOwnerTransform(this);
		}
		*/

		// copy childs

		std::list<smartptr<ZTransform> >::const_iterator iter = ptr->mChilds.begin();
		for (;iter != ptr->mChilds.end() ; ++iter)
		{
			AddChild( (*iter)->Clone() );
		}

	}

	// Update


	void AddToScene(ZScene *pScene);

	void RemoveFromScene();

	virtual ZScene *GetScene() { return mScene; }


	virtual void SetVisible(bool bVis) 
	{
		if (mSpatialEntity.ptr())
			mSpatialEntity->SetVisible(bVis);

		std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
		for (; iter != mChilds.end(); ++iter)
		{
			(*iter)->SetVisible(bVis);
		}
	}

	virtual void FindByName(const char *szName, std::vector<ZTransform*>& arrayDest);
	virtual ZTransform* GetFirstWithSpatialEntity();

	virtual void SetExternalRef(const char *szExternalName) { mExternRef = szExternalName; }
	virtual const tstring& GetExternalRef() const { return mExternRef; }

	ZTransform* GetFirstChild() const
	{
		if (mChilds.empty())
			return NULL;
		else
			return (*mChilds.begin());
	}
public:
	tstring mExternRef;

	// Matrices
    tmatrix mLocalMatrix;
    tmatrix mWorldMatrix;
    tmatrix mPreviousWorldMatrix;
	tmatrix mPivot;

	tvector3 mBaseScale, mBaseTranslation;
	tquaternion mBaseRotation;

	// Animation
	//smartptr<IAnimController> mAnimCtrl; FIXME

	// Hierarchy
	std::list<smartptr<ZTransform> > mChilds;
	smartptr<ZSpatialEntity> mSpatialEntity; 
	ZScene *mScene; 
	ZTransform *mParent;
protected:
	
	void RemoveTransformFromScene(ZScene *pScene);
	void AddTransformToScene(ZScene *pScene);

	virtual void ComputeWorldMatrix()
	{
		// build base Matrix
		tmatrix basesc, basetr, basert;
		basert.RotationQuaternion(mBaseRotation);
		basetr.Translation(mBaseTranslation);
		basesc.Scaling(mBaseScale);

		tmatrix mBaseMatrix;

		mBaseMatrix = (basesc * basert * basetr );

		if (mParent)
		{
			mWorldMatrix = mPivot * mLocalMatrix * mBaseMatrix * mParent->GetWorldMatrix();
		}
		else
		{
			mWorldMatrix = mPivot * mLocalMatrix * mBaseMatrix ;
		}
	}
};

#endif
