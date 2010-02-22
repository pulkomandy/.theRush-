///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZScene.cpp
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

#include "stdafx.h"
#include "MiniBall/Sphere.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
ZScene *GScene;
BEGIN_SERIALIZATION(ZScene)
END_SERIALIZATION()

ZScene *GetScene()
{
	return (ZScene*)FirstInstanceOf(ZScene);
}
void ClearWorld()
{
}
IWorld * GetWorld()
{
	return NULL;
}

void ResetParticles()
{
}
void ClearParticles()
{
}


void ZCluster::RecomputeBounds()
{
	int idx = 0;
	tvector3 *pv = (tvector3*)new tvector3 [ (mChildClusters.size()+mEntities.size())*8];

	// clusters
	std::list<ZCluster >::iterator iterc = mChildClusters.begin();
	for (;iterc != mChildClusters.end(); ++iterc)
	{
		(*iterc).RecomputeBounds();
		ZBoundingVolume& bvol = (*iterc).GetBVolume();
		bvol.Build8CornersBox(&pv[idx]);
		idx +=8;
	}
	// SE
	std::list<smartptr<ZSpatialEntity> >::const_iterator iterm = mEntities.begin();
	for (;iterm != mEntities.end(); ++iterm)
	{
		ZBoundingVolume& bvol = (*iterm)->GetBVolume();
		bvol.Build8CornersBox(&pv[idx]);
		idx +=8;
	}
	// Set Min/max & miniball
	mBVolume.SetMinMaxFromCorners((unsigned char *)pv, sizeof(tvector3), idx);

	// sphere

	MiniBall::Sphere mbs = MiniBall::Sphere::miniBall((unsigned char*)pv,
		sizeof(tvector3), idx);
	tvector4 mBSphere = vector4(mbs.center.getX(), mbs.center.getY(), mbs.center.getZ(), mbs.radius);

	// set sphere values
	mBVolume.SetBSphere(mBSphere);

	delete [] pv;
}

void ZCluster::Split()
{
#define NBMAXMESHPERCLUSTER 8
	if (mEntities.size() <= NBMAXMESHPERCLUSTER)
			return;

	std::vector<ZSpatialEntity*> mDispatch[8];
	unsigned int i;
	float hr = mBVolume.mBSphere.w*0.5f; // halfradius

	std::list<smartptr<ZSpatialEntity> >::iterator iterm = mEntities.begin();
	for (;iterm != mEntities.end() ; ++iterm)
	{
		const tvector4& mis = (*iterm)->GetBVolume().GetBSphere();


		//if (mis.w <= hr)
		{
			int idxdisp = 0;
			if (mis.z>mBVolume.mBSphere.z)
			{
				idxdisp +=4;
			}
			if (mis.y>mBVolume.mBSphere.y)
			{
				idxdisp +=2;
			}
			if (mis.x>mBVolume.mBSphere.x)
			{
				idxdisp +=1;
			}
			mDispatch[idxdisp].push_back((*iterm));
		}
	}
	// check splitting works
	for (i = 0 ; i < 8 ; i++)
	{
		if (mDispatch[i].size() == mEntities.size())
			return;
	}

	// build cluster when having at least 1 meshinstance

	static const tvector3 decals[8]={
		tvector3(-1,-1,-1),
		tvector3(1,-1,-1),
		tvector3(-1,1,-1),
		tvector3(1,1,-1),
		tvector3(-1,-1,1),
		tvector3(1,-1,1),
		tvector3(-1,1,1),
		tvector3(1,1,1) };

	for (i = 0 ; i < 8 ; i++)
	{
		if (!mDispatch[i].empty())
		{
			//ZCluster *pclust = new ZCluster(mScene);//AddChildCluster();
			mChildClusters.push_front(ZCluster(mScene, this));
			ZCluster *pclust = &(*mChildClusters.begin());

			pclust->mBVolume.mBSphere = vector4(mBVolume.mBSphere.x + decals[i].x*hr*0.5f,
				mBVolume.mBSphere.y + decals[i].y*hr*0.5f,
				mBVolume.mBSphere.z + decals[i].z*hr*0.5f,
				hr);
			for (uint j=0;j<mDispatch[i].size();j++)
			{
				ZSpatialEntity *pEnt = mDispatch[i][j];

this->RemoveSpatialEntity(pEnt);
				pEnt->SetCluster(pclust);//>SetParentCluster(pclust);//>GetParentCluster()->RemoveMesh(
				pclust->AddSpatialEntity(pEnt);
				
			}
		}
	}
	// re split child clusters

	std::list<ZCluster>::iterator iterc = mChildClusters.begin();
	for (;iterc != mChildClusters.end() ; ++iterc)
	{
		(*iterc).Split();
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZCluster::Reset()
{
	ZScene *pScene = (ZScene*)mScene;
	if (&pScene->mRootCluster != this)
	{
		std::list<smartptr<ZSpatialEntity> >::const_iterator iti = mEntities.begin();
		for (;iti != mEntities.end(); ++iti)
		{
			ZSpatialEntity *pse = (*iti);
			
			pse->SetCluster(&pScene->mRootCluster);
			pScene->mRootCluster.AddSpatialEntity( (*iti));
		}
		mEntities.clear();
	}
	std::list<ZCluster>::iterator iter = mChildClusters.begin();
	for (;iter != mChildClusters.end(); ++iter)
	{
		(*iter).Reset();
	}
	
	if (&pScene->mRootCluster == this)
	{
		mChildClusters.clear();
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::Tick()
{
	PROFILER_START(ZScene::Tick);

	std::vector<ZSceneTrigger::TriggerCheckPeer_t> mTriggerPeers;
	// apply modification job list
	ZSpatialEntity* mEnts[1024];
	SEJob_t mJobs[1024];
	int nbEnts = mSpatialEntitiesUpdates.ListToArray((ZBaseClass**)mEnts, mJobs, 1024);

	for (int i = 0;i<nbEnts;i++)
	{
		ZSpatialEntity* pSE = mEnts[i];
		switch (mJobs[i].mJobType)
		{
		case SEJOB_NEWMATRIX:
			{
#ifdef _DEBUG
				const char *szEntityName = pSE->GetName().c_str();
#endif
				pSE->SetWorldMatrix(mJobs[i].mWorldMat);

				if (!mJobs[i].mbSceneInsertion)
				{
					pSE->SubRef();
					continue;
				}
				if (pSE->GetCluster())
				{

					// assign a better cluster
					ZCluster *pNewClust = FindBestClusterForWSphere( pSE->GetBVolume().mBSphere, pSE->GetCluster());
					if (pNewClust != pSE->GetCluster())
					{
						pNewClust->AddSpatialEntity( pSE );
						pSE->GetCluster()->RemoveSpatialEntity( pSE );
						pSE->SetCluster( pNewClust );
					}
				}
				else
				{
					// Add To scene
					ZCluster *pNewClust = FindBestClusterForWSphere( pSE->GetBVolume().mBSphere, NULL);
					pNewClust->AddSpatialEntity( pSE );
					pSE->SetCluster( pNewClust );
				}
				// trigger 
				if (pSE->GetClassID() == ClassIDZTrigger)
				{
					ZTrigger *pTrig = (ZTrigger*)pSE->QueryInstance();
					mSceneTriggers.UpdateTriggerDependanciesList(pTrig, &mTriggerPeers);
				}
			}
			break;
		case SEJOB_REMOVE:
			//mRootCluster.RemoveSpatialEntity(pSE);
			if (pSE->GetCluster())
			{
				pSE->GetCluster()->RemoveSpatialEntity(pSE);
				pSE->SetCluster(NULL);
			}
			break;
		case SEJOB_CLUSTERIZE:
			// PSE is the scene
			RecomputeClusters();
			break;
		case SEJOB_CLEAR:
			//pSE->AddRef();
			DoClear();
			break;
		}
		pSE->SubRef();
	}

	if (!mTriggerPeers.empty())
		mSceneTriggers.CheckTriggers(&mTriggerPeers);

	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void ZScene::RecomputeClusters()
{
	mRootCluster.Reset();

	// get world Bounding sphere used to split world efficiently

	mRootCluster.RecomputeBounds();

	// At this point all mesh instances are in root cluster

	mRootCluster.Split(); // recurse split instances

	// recompute clusters size

	mRootCluster.RecomputeBounds();

	//mRootCluster.LogIt(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZCluster * ZScene::FindBestClusterForWSphere(const tvector4& wSphere, ZCluster *pCurrentCluster)
{
	ZCluster *pwkc;

	//if (!pCurrentCluster) 
	{
		pwkc = pCurrentCluster = &mRootCluster;
	}
	/*
	else
	{
		if (wSphere.CanFitIn(mRootCluster.GetBVolume().mBSphere)) 
			return pCurrentCluster;
	
		// get parent cluster big enough to support sphere
		pwkc = pCurrentCluster->mParentCluster;
		while (pwkc)
		{
			if (wSphere.CanFitIn(pwkc->GetBVolume().mBSphere))
				break;
			pwkc = pwkc->mParentCluster;
		}
		// got root?
		if (!pwkc)
			return &mRootCluster;
	}
	*/
	// try to find a child cluster big enough
	std::list<ZCluster>::iterator iter = pwkc->mChildClusters.begin();
	for (; iter != pwkc->mChildClusters.end(); )
	{
		if (wSphere.CanFitIn( (*iter).GetBVolume().mBSphere ) )
		{
			pwkc = &(*iter);
			iter = pwkc->mChildClusters.begin();
			continue;
		}
		++iter;
	}
	// return best one
	return pwkc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::BuildRenderingQueue(ZRenderQueue *pCtx, ZCamera *pCam)
{
	PROFILER_START(BuildRenderingQueue);
	
	// do job queue
	Tick(); 

	pCtx->Begin();

	static int fnb = -1;

	extern int frameNb;

	fnb = frameNb;
	//LOG("Build %d\n", frameNb);
/*
	// 2D Mesh
	ZMeshInstance *mgMesh = (ZMeshInstance*)FirstInstanceOf(ZMeshInstance);
	while (mgMesh)
	{
		if (mgMesh->IsGui2DMesh()&&mgMesh->IsVisible())
		{
			pCtx->Add2D(mgMesh, 0);
		}

		mgMesh = (ZMeshInstance*)NI(mgMesh);//>mNextInstance->QueryInstance();
	}


	if (GDD->Is3DLayerEnable()) 
	{
			// set view infos in context
		*/
		pCtx->SetViewProjection(pCam->GetTransform()->GetWorldMatrix(), pCam->GetProjectionMatrix());
		pCtx->SetFrustum(*pCam->GetFrustum());

		// Get all occluder boxes
		
		tmatrix invcam;
		invcam.Inverse(pCam->GetTransform()->GetWorldMatrix());
		PreprocessOccluders(invcam);
		
/*
		// water visible/Reflexion layer

		PROFILER_START(Water);
		pCtx->DrawWater(bWaterVis);
		if (bWaterVis)
		{
			tmatrix transworld;
			transworld.Scaling(1,-1,1);
			transworld*=pCam->GetTransform()->GetWorldMatrix();
			pCtx->SetReflexionMatrices(transworld, pCam->GetProjectionMatrix());
			ZFrustum mRefFrus;
			mRefFrus.Update(transworld, pCam->GetProjectionMatrix());

			RecurseDispatchForReflexionInContextWithVisibilityTest(pCtx, &mRootCluster, mRefFrus);
		}
		PROFILER_END();

*/
		
		// normal view
		PROFILER_START(SolidView);
		RecurseDispatchInContextWithVisibilityTest(pCtx, &mRootCluster);
		//RecurseDispatchInContext(pCtx, &mRootCluster);
		PROFILER_END();

/*
		if (GDD->GetRenderProfile() != RENDER_PROFILE_NETBOOK)
		{
		// build shadow matrices & frustum
			PROFILER_START(CascadedShadows);
			ComputeCascadedShadowInfos(pCam, pCtx->GetShadowMapBuffer());
			PROFILER_END();
			

			// build shadows caster mesh list for each split
			
			PROFILER_START(DispatchSolid);
			RecurseDispatchForShadowMapWithVisibilityTest(pCtx, &mRootCluster, 3);
			PROFILER_END();
		}
*/
		
	
	// Done
	pCtx->End();


	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Add a light to render in rendering context. Pretransformed things
#if 0
void ZScene::AddRenderLight(IRenderingContext *pCtx, ILight *pl) const
{
	const tcolor& lightCol = pl->GetColor();
/*
	char tmps[512];
	sprintf(tmps,"In Context 0x%x\n",this);
	OutputDebugStringA(tmps);
	sprintf(tmps, "Add Light 0x%d\n",pl);
	OutputDebugStringA(tmps);
*/
	if (((lightCol.r>0.01f)||(lightCol.g>0.01f)||(lightCol.b>0.01f)) )
	{
		VISIBLELIGHTS_t* le = pCtx->AddLightEntry();

		const tmatrix & lightMat = pl->GetTransform()->GetWorldMatrix();
		const tmatrix & vmt = pCtx->GetViewMatrix();
		tvector3 lightwpos;
		lightwpos = lightMat.position;

		// transform light in viewspace

		// inv transform?
		tmatrix viewMat;
		viewMat.Inverse(vmt);
		tvector3 lightdist = lightwpos - viewMat.position; // get vector distance

		lightwpos.TransformPoint(vmt);

		float lightRadius = pl->GetRadius();
		switch (pl->GetLightType())
		{
		case LT_SPOT:
			le->mbInsideLight = IsPointInCone(viewMat.position, lightMat.position, lightMat.dir, pl->GetLength(), pl->GetSpotAngle2()*pl->GetLength()*0.5f);
			break;
		case LT_POINTLIGHT:
			le->mbInsideLight = (lightdist.lengthSquared() <= (lightRadius*lightRadius));
			break;
		}
		

		// safety check

		le->mTransformedLight = vector4(lightwpos, lightRadius);
		le->mLight = pl;
		le->mLightColor = lightCol;
		le->mLightColor.r *= lightCol.a;
		le->mLightColor.g *= lightCol.a;
		le->mLightColor.b *= lightCol.a;
		le->mTransformedDir.TransformVector(lightMat.dir, vmt);
		le->mTransformedDir.Normalize();
		le->mLightMatrix = lightMat;

		pl->AddRef();
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::AddSEToContext(ZRenderQueue *pCtx, ZSpatialEntity *pse) const
{
	TClassID clsID = pse->GetClassID();
	if (clsID == ClassIDZMeshInstance)
	{
		ZMeshInstance *pmi = (ZMeshInstance*)pse->QueryInstance();
		if (pmi->IsTransparent())
		{
			//pCtx->AddTransparent(pmi);
		}
		else 
		{
			pCtx->AddSolid(pmi);
		}
	}
#if 0
	else if (clsID == ClassIDZLight)
	{
		ILight *pml = (ILight*)pse->QueryInstance();
		AddRenderLight(pCtx, pml);
	}
#endif
}
#if 0
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::RecurseDispatchForReflexionInContextWithVisibilityTest(IRenderingContext *pCtx, ZCluster *pClust, ZFrustum& mRefFrus) const
{
	int vis = mRefFrus.SphereInFrustumVis(pClust->GetBVolume().GetBSphere());
	if (vis)
	{
		// do occlusion test
	}
	switch(vis)
	{
	case 0: // totaly out of frustum
		return;
		break;
	case 1: // partially in
		{
			std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
			for (;iteres != pClust->mEntities.end(); ++iteres)
			{
				ZSpatialEntity *pse = (*iteres);
				const tvector4& mws = pse->GetBVolume().GetBSphere();
				// should add transparent mesh
				if ( pse->IsVisible() && mRefFrus.SphereInFrustum(mws) )
				{
					if (pse->GetClassID() == ClassIDZMeshInstance)
						pCtx->AddReflexionWater( (ZMeshInstance*)pse->QueryInstance() );
				}
			}


			// iterate on child
			std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
			for (;iterc != pClust->mChildClusters.end(); ++iterc)
			{
				RecurseDispatchForReflexionInContextWithVisibilityTest(pCtx, &(*iterc), mRefFrus);
			}
		}
		// check each mesh and call RecurseDispatchInContext on each child cluster
		break;
	case 2: // totaly in
		// append to the context every mesh in this cluster and in all child clusters recursively
		RecurseDispatchForReflexionInContext(pCtx, pClust);
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::RecurseDispatchForReflexionInContext(IRenderingContext *pCtx, ZCluster *pClust) const 
{
	// append meshes

	std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
	for (;iteres != pClust->mEntities.end(); ++iteres)
	{
		ZSpatialEntity *pse = (*iteres);
		//const tvector4& mws = pse->GetBVolume().GetBSphere();
		// should add transparent mesh
		if ( pse->IsVisible() )
		{
			if (pse->GetClassID() == ClassIDZMeshInstance)
				pCtx->AddReflexionWater( (ZMeshInstance*)pse->QueryInstance() );
		}
	}

	// go for child clusters
	std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
	for (;iterc != pClust->mChildClusters.end(); ++iterc)
	{
		RecurseDispatchForReflexionInContext(pCtx, &(*iterc));
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
void ZScene::RecurseDispatchInContextWithVisibilityTest(ZRenderQueue *pCtx, ZCluster *pClust) const
{
	// cluster visibility
	const ZFrustum & fru = pCtx->GetFrustum();
	const tvector4& clustSphere = pClust->GetBVolume().GetBSphere();
	int vis = fru.SphereInFrustumVis(clustSphere);
	if (vis)
	{
		// do occlusion test
		if (IsSphereOccluded(clustSphere))
			vis = 0;
	}
	switch(vis)
	{
	case 0: // totaly out of frustum
		return;
		break;
	case 1: // partially in
		{
			std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
			for (;iteres != pClust->mEntities.end(); ++iteres)
			{
				ZSpatialEntity *pse = (*iteres);
				const tvector4& mws = pse->GetBVolume().GetBSphere();
				if ( pse->IsVisible() && fru.SphereInFrustum(mws) && (!IsSphereOccluded(mws)) )
				{
					AddSEToContext(pCtx, pse);
				}
			}

			// iterate on child
			std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
			for (;iterc != pClust->mChildClusters.end(); ++iterc)
			{
				RecurseDispatchInContextWithVisibilityTest(pCtx, &(*iterc));
			}
		}
		// check each mesh and call RecurseDispatchInContext on each child cluster
		break;
		
	case 2: // totaly in
		// append to the context every mesh in this cluster and in all child clusters recursively
		RecurseDispatchInContext(pCtx, pClust);
		break;
		
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
// draw every thing!
void ZScene::RecurseDispatchInContext(ZRenderQueue *pCtx, ZCluster *pClust) const
{

	std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
	for (;iteres != pClust->mEntities.end(); ++iteres)
	{
		ZSpatialEntity *pse = (*iteres);
		const tvector4& mws = pse->GetBVolume().GetBSphere();
		if ( pse->IsVisible() && (!IsSphereOccluded(mws)) )
		{
			AddSEToContext(pCtx, pse);
		}
	}
	// iterate on child
	std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
	for (;iterc != pClust->mChildClusters.end(); ++iterc)
	{
		const tvector4& mws = (*iterc).GetBVolume().GetBSphere();
		if (!IsSphereOccluded(mws))
			RecurseDispatchInContext(pCtx, &(*iterc));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::RecurseDispatchForShadowMapWithVisibilityTest(ZRenderQueue *pCtx, ZCluster *pClust, int iSplit) const
{
	for (int i = iSplit ; i >= 0 ; i--)
	{
		shadowmap_t & psm = pCtx->GetShadowMapBuffer()[i];

		// cluster visibility
		const ZFrustum & fru = psm.mShadowFrustum;
		int vis = fru.SphereInFrustumVis(pClust->GetBVolume().GetBSphere());
		if (vis)
		{
			// do occlusion test
		}
		switch(vis)
		{
		case 0: // totaly out of frustum
			// So no need to check for smaller shadow split -> return.
			return;
			break;
		case 1: // partially in
			{
				std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
				for (;iteres != pClust->mEntities.end(); ++iteres)
				{
					ZSpatialEntity *pse = (*iteres);
					if (fru.SphereInFrustum(pse->GetBVolume().GetBSphere()))
					{
						if ( (pse->GetClassID() == ClassIDZMeshInstance) && (pse->IsVisible()) )
						{
							ZMeshInstance * pmi = (ZMeshInstance*)pse->QueryInstance();
							if (pmi->IsShadowCaster())
							{
								pmi->AddRef();
								psm.mCasterMeshes[psm.mNbCasterMeshes] = pmi;
								psm.mCasterMeshesMatrix[psm.mNbCasterMeshes] = pmi->GetWorldMatrix();
								psm.mNbCasterMeshes++;
							}
						}
					}
				}


				// iterate on child
				std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
				for (;iterc != pClust->mChildClusters.end(); ++iterc)
				{
					RecurseDispatchForShadowMapWithVisibilityTest(pCtx, &(*iterc), i);
				}
			}
			// check each mesh and call RecurseDispatchInContext on each child cluster
			break;
		case 2: // totaly in
			// append to the context every mesh in this cluster and in all child clusters recursively
			RecurseDispatchForShadowMapInContext(pCtx, pClust, i);
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::RecurseDispatchForShadowMapInContext(ZRenderQueue *pCtx, ZCluster *pClust, int iSplit) const 
{
	shadowmap_t & psm = pCtx->GetShadowMapBuffer()[iSplit];
	
	// append meshes
	std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
	for (;iteres != pClust->mEntities.end(); ++iteres)
	{
		ZSpatialEntity *pse = (*iteres);
		if ( (pse->GetClassID() == ClassIDZMeshInstance) && (pse->IsVisible()) )
		{
			ZMeshInstance * pmi = (ZMeshInstance*)pse->QueryInstance();
			if (pmi->IsShadowCaster())
			{
				pmi->AddRef();
				psm.mCasterMeshes[psm.mNbCasterMeshes] = pmi;
				psm.mCasterMeshesMatrix[psm.mNbCasterMeshes] = pmi->GetWorldMatrix();
				psm.mNbCasterMeshes++;
			}
		}
	}

	// go for child clusters
	std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
	for (;iterc != pClust->mChildClusters.end(); ++iterc)
	{
		RecurseDispatchForShadowMapInContext(pCtx, &(*iterc), iSplit);
	}
}




///////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void ZScene::UpdateAnimations(float timeEllapsed)
{
	IAnimController *mgCtrl;

	// AnimationsUpdate
	PROFILER_START(AnimationsUpdate);

	// lock objects
	LockCriticalSection(GInstancesCriticalSection);

    mgCtrl = (IAnimController*)FirstInstanceOf(ZAnimController);
    while (mgCtrl)
    {
		mgCtrl->AddRef();
        mgCtrl=(IAnimController *)NI(mgCtrl);
    }

	UnLockCriticalSection(GInstancesCriticalSection);

	// update animation
    mgCtrl = (IAnimController*)FirstInstanceOf(ZAnimController);
    while (mgCtrl)
    {
		mgCtrl->Tick(timeEllapsed);
        mgCtrl=(IAnimController *)NI(mgCtrl);
    }

	// unlock
    mgCtrl = (IAnimController*)FirstInstanceOf(ZAnimController);
    while (mgCtrl)
    {
		mgCtrl->SubRef();
        mgCtrl=(IAnimController *)NI(mgCtrl);
    }

	PROFILER_END();
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

ZSpatialEntity *ZScene::RecursePick(ZCluster* pClust,const tvector3 & origin, const tvector3 &dir, tvector3& normal, 
	float &t, unsigned int* pTriNb)
{
	float curT = t;

	const tvector4& aClustWS = pClust->GetBVolume().GetBSphere();
	float aInter = IntersectRaySphere(origin, dir*curT, aClustWS, aClustWS.w);
	if ( aInter != -1.f)
	{
		ZSpatialEntity * curSE = NULL;
		std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
		for (;iteres != pClust->mEntities.end(); ++iteres)
		{
			ZSpatialEntity *pse = (*iteres);
			if ((!pse->IsSelectable()) || (!pse->IsVisible()) )
				continue;

			//const tvector4& mws = pse->GetBVolume().GetBSphere();
			//float seInter = IntersectRaySphere(origin, dir*curT, mws, mws.w);
			bool intersect = isRayAABBoxIntersect(pse->GetBVolume().GetBoxMin(), 
				pse->GetBVolume().GetBoxMax(), origin, dir*curT);

			if ( intersect ) 
			{
				//curT *= seInter;
				TClassID clsID = pse->GetClassID();
				if (clsID == ClassIDZLight)
				{
					curSE = pse;
				}
				else if (clsID == ClassIDZMeshInstance)
				{
					//if (curT< (SquaredDistance(origin, mws)+(mws.w*mws.w)))
					{

						tmatrix invMat;
						invMat.Inverse(pse->GetWorldMatrix());
						tvector3 norig, ndir;
						norig.TransformPoint(origin, invMat);
						ndir.TransformVector(dir, invMat);
						ndir.Normalize();
						//ndir *= t;
						ndir *= curT;

						tvector3 gnorm;
						float newT;

						if ( ((ZMeshInstance*)pse->QueryInstance())->GetMesh()->PickRay(norig, ndir, gnorm, newT, pTriNb))
							curSE = pse;

						if (newT<curT)
							curT = newT;
					}
				}
				t = curT;
				
			}
		}

		std::list<ZCluster >::iterator iterc= pClust->mChildClusters.begin();
		for (;iterc != pClust->mChildClusters.end(); ++iterc)
		{
			ZSpatialEntity *pse = RecursePick(&(*iterc), origin, dir, normal, curT, pTriNb);
			if (pse)
				curSE = pse;
		}

		return curSE;
	}
	return NULL;
}

ZSpatialEntity *ZScene::Pick(const tvector3 & origin, const tvector3 &dir, tvector3& normal, 
	float &t, unsigned int* pTriNb)
{
	//bool bFound = false;
	float curT = t;

	ZCluster* pClust = &mRootCluster;
	return RecursePick(pClust, origin, dir, normal, curT, pTriNb);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::DoClear()
{
	// unlink roottransforms
#ifdef _DEBUG
	LOG("Beginning of ZScene::DoClear()\n");
#endif

	std::list<ZTransform*>::iterator ittr = mRootTransforms.begin();
	
	while (!mRootTransforms.empty())
	{
		ZTransform *ptr = (*ittr);
		ptr->RemoveFromScene();
		if ((ptr->GetRefCount() == 0)&&(!ptr->GetParent()))
			ptr->SubRef();
		ittr = mRootTransforms.begin();
	}
	Tick();
	
	


		

	// destroy clusters
	mRootCluster.Clear();

	mbClearAsked = false;
#ifdef _DEBUG
	LOG("END of ZScene::DoClear()\n");
#endif


}

///////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void ZScene::LoadLights(ZFile *pFile)
{
	unsigned long nbLights;
	ZFile& mFile = (*pFile);
	mFile >> nbLights;
	for (unsigned int i =0;i<nbLights;i++)
	{
		ILight *pLight = (ILight*)_New(ZLight);
		pLight->GetTransform()->AddToScene(this);
		pLight->LoadLight(pFile);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::SaveLights(ZFile *pFile)
{
	ZFile& mFile = (*pFile);

	unsigned long nbLights = 0;
	ILight* pLight = (ILight*)FirstInstanceOf(ZLight);
	while (pLight)
	{
		if (pLight->GetTransform()->GetScene())
			nbLights++;
		pLight = (ILight*)NI(pLight);
	}
	mFile << nbLights;
	// save them
	pLight = (ILight*)FirstInstanceOf(ZLight);
	while (pLight)
	{
		if (pLight->GetTransform()->GetScene())
			pLight->SaveLight(pFile);

		pLight = (ILight*)NI(pLight);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::LoadOccluders(ZFile *pFile)
{
	unsigned long nbOcc;
	ZFile& mFile = (*pFile);
	mFile >> nbOcc;
	for (unsigned int i =0;i<nbOcc;i++)
	{
		IOccluderBox *pOcc = (IOccluderBox*)_New(ZOccluderBox);
		tmatrix mt;
		mFile >> mt;
		bool bActive;
		mFile >> bActive;
		pOcc->SetVisible(bActive);

		pOcc->GetTransform()->SetLocalMatrix(mt);
		pOcc->GetTransform()->AddToScene(GetScene());
		pOcc->GetTransform()->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::SaveOccluders(ZFile *pFile)
{
	ZFile& mFile = (*pFile);

	unsigned long nbOcc = 0;
	IOccluderBox* pOcc = (IOccluderBox*)FirstInstanceOf(ZOccluderBox);
	while (pOcc)
	{
		nbOcc++;
		pOcc = (IOccluderBox*)NI(pOcc);
	}
	mFile << nbOcc;
	// save them
	pOcc = (IOccluderBox*)FirstInstanceOf(ZOccluderBox);
	while (pOcc)
	{
		
		tmatrix mt = pOcc->GetTransform()->GetLocalMatrix();
		mFile << mt;
		bool bActive = pOcc->IsVisible();
		mFile << bActive;

		pOcc = (IOccluderBox*)NI(pOcc);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::Load3DSounds(ZFile *pFile)
{
	ZFile& mFile = (*pFile);
	unsigned long nb3DSnd = 0;
	mFile >> nb3DSnd;
	for (uint i=0;i<nb3DSnd;i++)
	{
		ISoundInstance *pInst = (ISoundInstance*)_New(ZSoundInstance);
		pInst->LoadInstance(pFile);
		mDeferred3DSoundsLoading.push_back(pInst);
	}
	// deferred playback
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::Save3DSounds(ZFile *pFile)
{
	ZFile& mFile = (*pFile);

	unsigned long nb3DSnd = 0;
	ISoundInstance* pSnd = (ISoundInstance*)FirstInstanceOf(ZSoundInstance);
	while (pSnd)
	{
		if (pSnd->getNode())
			nb3DSnd++;
		pSnd = (ISoundInstance*)pSnd->mNextInstance;
	}
	mFile << nb3DSnd;
	// save them
	pSnd = (ISoundInstance*)FirstInstanceOf(ZSoundInstance);
	while (pSnd)
	{
		if (pSnd->getNode())
			pSnd->SaveInstance(pFile);
		pSnd = (ISoundInstance*)pSnd->mNextInstance;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::UpdateLoaded3DSounds()
{
	for (uint i=0;i<mDeferred3DSoundsLoading.size();i++)
	{
		ISoundInstance *pInst = mDeferred3DSoundsLoading[i];
		ISoundSample *pSamp = pInst->GetSample();
		
		pSamp->setLoop(true);
		// parameters
		pSamp->setInnerAttenuationRadius(pInst->getInnerAttenuationRadius());
		pSamp->setOuterAttenuationRadius(pInst->getOuterAttenuationRadius());
		pSamp->setAttenuationType(pInst->getAttenuationType());
		pSamp->setNode(pInst->getNode());

		mDeferred3DSoundsLoading[i]->SetSample(true, pSamp);//((ZSoundInstance*)mDeferred3DSoundsLoading[i])->mSample);
		mDeferred3DSoundsLoading[i]->play();

	}
	mDeferred3DSoundsLoading.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::LoadParticlesInstances(ZFile *pFile)
{
	ZFile& mFile = (*pFile);
	unsigned long nbParts = 0;
	mFile >> nbParts;
	for (uint i=0;i<nbParts;i++)
	{
		IMetaEmitter *pEmit = (IMetaEmitter*)_New(ZMetaEmitter);
		pEmit->Load(pFile);
		tmatrix mt;
		mFile >> mt;
		pEmit->SetGenerationMatrix(mt);
		tstring pname;
		mFile.Read(pname);
		pEmit->SetName(pname);
		pEmit->SetSceneEmitter(true);

		pEmit->SetActive(true);
		pEmit->Generate();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::SaveParticlesInstances(ZFile *pFile)
{
	ZFile& mFile = (*pFile);

	unsigned long nbParts = 0;
	IMetaEmitter* pEmit = (IMetaEmitter*)FirstInstanceOf(ZMetaEmitter);
	while (pEmit)
	{
		if (pEmit->IsSceneEmitter())
			nbParts++;
		pEmit = (IMetaEmitter*)pEmit->mNextInstance;
	}
	mFile << nbParts;
	// save them
	pEmit = (IMetaEmitter*)FirstInstanceOf(ZMetaEmitter);
	while (pEmit)
	{
		if (pEmit->IsSceneEmitter())
		{
			pEmit->Save(pFile);
			tmatrix mt = pEmit->GetGenerationMatrix();
			mFile << mt;
			mFile.Write(pEmit->GetName());

		}
		
		//
		pEmit = (IMetaEmitter*)pEmit->mNextInstance;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZScene::UpdateEmitters(float timeEllapsed)
{
	PROFILER_START(MetaEmitterUpdate);
    IMetaEmitter *mgME = (IMetaEmitter*)FirstInstanceOf(ZMetaEmitter);
    while (mgME)
    {
		IMetaEmitter *mgME2 = mgME;
		mgME2 = (IMetaEmitter *)mgME2->mNextInstance;

		mgME->Tick(timeEllapsed);
        if (mgME->CanDelete())
		{
			delete mgME;		
		}
		mgME = mgME2;
    }

	PROFILER_END(); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif

void ZScene::CleanJobListReferences()
{
	Tick();
}


///////////////////////////////////////////////////////////////////////////////////////////////////