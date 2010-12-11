///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZMeshInstance.h
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

#ifndef ZMESHINSTANCE_H__
#define ZMESHINSTANCE_H__

#include "../librender/ZMaterial.h"
#include "../libworld/ZMesh.h"
#include "../libworld/ZPhysicTriMeshInstance.h"
#include "../libbase/ZLogger.h"

DECLAREZCLASS(ZMeshInstance);
class IPhysicModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZMeshInstance : public ZSpatialEntity
{
public:
    DECLARE_SERIALIZABLE
public:
    ZMeshInstance()
    {
        mMaterial.resize(1);

        mMaterial[0] = (ZMaterial*)_New(ZMaterial);
        //mMaterial[0]->setEffect(IFx::getDefaultFX());
		


        mShadowCaster = false;
        mGui2DMesh = false;
		mTransparent = false;
		//mWndClipper = NULL; FIXME

		mLastFrameNbPixRendered = 0;

		//printf("New MI 0x%x\n", this);

		mPhysicTMInstance = NULL;

    }

    virtual ~ZMeshInstance();

    void SetMesh(ZMesh *pMesh)
    {
        int nbSt = (int)mMaterial.size();
        if (pMesh->GetNbSubMeshes() > mMaterial.size())
        {
            mMaterial.resize(pMesh->GetNbSubMeshes());

            for (uint i = nbSt ; i < pMesh->GetNbSubMeshes() ; i++)
            {
                mMaterial[i] = (ZMaterial*)_New(ZMaterial);
                //mMaterial[i]->setEffect(IFx::getDefaultFX());
            }
        }
        else for (uint i = (uint)mMaterial.size()-1 ; i > pMesh->GetNbSubMeshes() ; i--)
        {
			mMaterial[i]->SubRef();
            mMaterial.resize(pMesh->GetNbSubMeshes());
        }
		
        mMesh.Attach(pMesh);
    }

    ZMesh * GetMesh() const { return mMesh.ptr(); }


    ZMaterial * GetMaterial(int aSubMeshNb) const
    {
        return mMaterial[aSubMeshNb];
    }
	

    tulong GetMemoryUsed() const { return sizeof(ZMeshInstance); }


    void SetShadowCaster(bool aShadow) { mShadowCaster = aShadow; }
    bool IsShadowCaster() const { return mShadowCaster; }
    void SetGui2DMesh(bool aGui2D) { mGui2DMesh = aGui2D; }
    bool IsGui2DMesh() const { return mGui2DMesh; }
	void SetTransparent(bool bTrans ) { mTransparent = bTrans; }
	bool IsTransparent() const { return mTransparent; }

    virtual ZSpatialEntity * Clone();

	/* FIXME
	void SetClipperParent(IGuiWindow *parentClipper) { mWndClipper = parentClipper; }
	IGuiWindow *GetClipperParent() const { return mWndClipper; }
	*/



	void Delete() { delete this; }

	void EnableOcclusionQuery(bool /*bEnable*/)
	{
		/* FIXME
		if ((bEnable)&&(!mOcclusionQuery.ptr()))
		{
			mOcclusionQuery.Attach(GDD->NewOcclusionQuery());
		}
		else
		if ((!bEnable)&&(mOcclusionQuery.ptr()))
		{
			mOcclusionQuery.Detach();
		}
		*/
	}
	IOcclusionQuery* GetOcclusionQuery() const
	{
		return mOcclusionQuery.ptr();
	}
	int GetLastFrameNumberOfPixelsRendered() const { return mLastFrameNbPixRendered; }
	void SetLastFrameNumberOfPixelsRendered(int nbpix)
	{
		mLastFrameNbPixRendered = nbpix;
	}


	void SetPhysicTriMeshInstance (ZPhysicTriMeshInstance *pInst)
	{
		ZASSERT( (!mPhysicTMInstance), "Meshinstance already has physic trimesh instance.");
		mPhysicTMInstance = pInst;
	}

	ZPhysicTriMeshInstance* GetPhysicTriMeshInstance() const
	{
		return mPhysicTMInstance;
	}
	
protected:

	// occlusion
	int mLastFrameNbPixRendered ;
	smartptr<IOcclusionQuery> mOcclusionQuery;
	//
    smartptr<ZMesh> mMesh;

    std::vector<ZMaterial*> mMaterial;

    bool mVisible;
    bool mShadowCaster;
    bool mGui2DMesh;
	bool mTransparent;
	//IGuiWindow * mWndClipper; FIXME
	
	ZPhysicTriMeshInstance *mPhysicTMInstance;
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif

