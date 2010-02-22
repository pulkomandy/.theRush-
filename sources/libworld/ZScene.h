///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZScene.h
// Creation : 10/05/2009
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

#ifndef ZSCENE_H__
#define ZSCENE_H__


///////////////////////////////////////////////////////////////////////////////////////////////////
class ZScene;
class ZTrigger;
DECLAREZCLASS(ZScene);
class ZTriggerCallback
{
public:
	virtual void CallBack(ZTrigger* trig1, ZTrigger* trig2) = 0;
};

class ZCluster
{
public:
	ZCluster(ZScene *aScene, ZCluster *aParentCluster)
	{
		mScene = aScene;
		mParentCluster = aParentCluster;
	}
	~ZCluster()
	{
		Clear();
	}

	ZScene *GetScene() const { return mScene; }
	void AddSpatialEntity(ZSpatialEntity *pSE)
	{
		mEntities.push_back(pSE);
	}
	void RemoveSpatialEntity(ZSpatialEntity *pSE)
	{
		pSE->SetCluster(NULL);
		mEntities.remove(pSE);
	}

	void Clear()
	{
		mChildClusters.clear();
		std::list<smartptr<ZSpatialEntity> >::iterator iti = mEntities.begin();
		for (; iti != mEntities.end(); ++iti)
		{
			(*iti)->mCluster = NULL;
		}
		mEntities.clear();
	}


	void RecomputeBounds();
	void Split();

	// set every spatial entity attached to world inside the root cluster
	void Reset();
	ZBoundingVolume& GetBVolume() { return mBVolume; }

#ifdef _DEBUG

	void LogIt(int aLevel) const
	{
		std::list<smartptr<ZSpatialEntity> >::const_iterator iti = mEntities.begin();
		/*
		for (;iti != mEntities.end(); ++iti)
		{
			for (int i=0;i<aLevel;i++)
			{
				LOG("  ");
			}
			LOG("SE 0x%x\n", (*iti).ptr());
		}
		*/
		{
			for (int i=0;i<aLevel;i++)
			{
				LOG("  ");
			}
			LOG("# %d SE \n", mEntities.size());
		}

		std::list<ZCluster >::const_iterator iter = mChildClusters.begin();
		for (;iter != mChildClusters.end(); ++iter)
		{
			for (int i=0;i<aLevel;i++)
			{
				LOG("  ");
			}
			(*iter).LogIt(aLevel+1);
		}

	}
#endif

	std::list<ZCluster > mChildClusters;
	std::list<smartptr<ZSpatialEntity> > mEntities;

	ZScene *mScene;
	ZCluster *mParentCluster;
	ZBoundingVolume mBVolume;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZSceneTrigger.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

extern ZScene *GScene;

class ZScene : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZScene() : mRootCluster(this, NULL), mbClearAsked(false)
	{
		GScene = this;
	}
	virtual ~ZScene()
	{
		CleanJobListReferences();
		GScene = NULL;
		DoClear();
	}

	void AddSEJobChangeMatrix(ZSpatialEntity *pSE, const tmatrix& aNewMatrix)
	{
		pSE->AddRef();
		mSpatialEntitiesUpdates.SingleToList(pSE, SEJob_t(aNewMatrix));
	}

	void AddSEJobChangeMatrixNoSceneInsertion(ZSpatialEntity *pSE, const tmatrix& aNewMatrix)
	{
		pSE->AddRef();
		mSpatialEntitiesUpdates.SingleToList(pSE, SEJob_t(aNewMatrix, false));
	}

	void AddSEJobRemove(ZSpatialEntity *pSE)
	{
		pSE->AddRef();
		mSpatialEntitiesUpdates.SingleToList(pSE, SEJob_t(SEJOB_REMOVE));
	}
	void AddCluterizeJob()
	{
		this->AddRef();
		mSpatialEntitiesUpdates.SingleToList(this, SEJob_t(SEJOB_CLUSTERIZE));
	}
	void Clear()
	{
		AddRef();
		mbClearAsked = true;
		mSpatialEntitiesUpdates.SingleToList(this, SEJob_t(SEJOB_CLEAR));
	}
	bool ClearAsked() const { return mbClearAsked; }

	void Tick();

	void BuildRenderingQueue(ZRenderQueue *pCtx, ZCamera *pCam);

	const ZBoundingVolume& GetBVolume() { return mRootCluster.GetBVolume(); }
	void UpdateAnimations(float timeEllapsed);
	void UpdateEmitters(float timeEllapsed);

	ZSpatialEntity *Pick(const tvector3 & origin, const tvector3 &dir, tvector3& normal,
		float &t, unsigned int* pTriNb = NULL);
#ifdef _DEBUG

	void LogIt()
	{
		mRootCluster.LogIt(0);
	}
#endif

	ZCluster * GetRootCluster() { return &mRootCluster; }

	void AddRootTransform(ZTransform *pTrans)
	{
		mRootTransforms.push_back(pTrans);
	}
	void RemoveRootTransform(ZTransform *pTrans)
	{
		mRootTransforms.remove(pTrans);
	}
	// Register callback
	void RegisterTriggerCallback(unsigned int aGroup1, unsigned int aGroup2, ZTriggerCallback* pcb)
	{
		mSceneTriggers.RegisterTriggerCallback(aGroup1, aGroup2, pcb);
	}
	// lights
	void LoadLights(ZFile *pFile);
	void SaveLights(ZFile *pFile);
	// Occluders
	void LoadOccluders(ZFile *pFile);
	void SaveOccluders(ZFile *pFile);
	// sounds
	void Load3DSounds(ZFile *pFile);
	void Save3DSounds(ZFile *pFile);
	void UpdateLoaded3DSounds();
	// particles
	void LoadParticlesInstances(ZFile *pFile);
	void SaveParticlesInstances(ZFile *pFile);

protected:
	ZCluster mRootCluster;
	std::list<ZTransform*> mRootTransforms;
	//std::vector<ISoundInstance*> mDeferred3DSoundsLoading; FIXME

	enum SEJobTypes
	{
		SEJOB_NEWMATRIX,
		SEJOB_REMOVE,
		SEJOB_CLUSTERIZE,
		SEJOB_CLEAR
	};

	typedef struct SEJob_t
	{
		SEJob_t() {}
		SEJob_t(SEJobTypes aJobType)
		{
			mJobType = aJobType;
		}
		SEJob_t(const tmatrix& mt, bool bSceneInsertion = true)
		{
			mJobType = SEJOB_NEWMATRIX;
			mWorldMat = mt;
			mbSceneInsertion = bSceneInsertion;
		}
		tmatrix mWorldMat;
		SEJobTypes mJobType;
		bool mbSceneInsertion;

	} SEJob_t;
	ZDataResource<SEJob_t> mSpatialEntitiesUpdates;

	void RecomputeClusters();
	ZCluster * FindBestClusterForWSphere(const tvector4& wSphere, ZCluster *pCurrentCluster);

	friend class ZCluster;

	// Building Context

	inline void AddSEToContext(ZRenderQueue *pCtx, ZSpatialEntity *pse) const;
	void RecurseDispatchInContextWithVisibilityTest(ZRenderQueue *pCtx, ZCluster *pClust) const;
	void RecurseDispatchInContext(ZRenderQueue *pCtx, ZCluster *pClust) const;
#if 0	
	void RecurseDispatchForReflexionInContextWithVisibilityTest(IRenderingContext *pCtx, ZCluster *pClust, ZFrustum& refFrus) const;
	void RecurseDispatchForReflexionInContext(IRenderingContext *pCtx, ZCluster *pClust) const ;
	void AddRenderLight(IRenderingContext *pCtx, ILight *pl) const;
#endif

	void RecurseDispatchForShadowMapWithVisibilityTest(ZRenderQueue *pCtx, ZCluster *pClust, int iSplit) const;
	void RecurseDispatchForShadowMapInContext(ZRenderQueue *pCtx, ZCluster *pClust, int iSplit) const ;

	// picking
	ZSpatialEntity *RecursePick(ZCluster* pClust,const tvector3 & origin, const tvector3 &dir, tvector3& normal,
		float &t, unsigned int* pTriNb);

	// clear
	bool mbClearAsked;
	void DoClear();

	void CleanJobListReferences();
	// triggers
	ZSceneTrigger mSceneTriggers;

	
};



///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
