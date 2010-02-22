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

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_SERIALIZATION(ZTransform)
SERIALIZE_MATRIX(mLocalMatrix)
SERIALIZE_MATRIX(mWorldMatrix)
SERIALIZE_MATRIX(mPivot)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTransform::Update()
{


	this->ComputeWorldMatrix();
	// --


	// compute world bounding sphere

	if (mSpatialEntity.ptr())
	{
		// bounds computation
		ZBoundingVolume tmpb; // mesh
		//tmpb.SetBoxMinMax(tvector3(-1,-1,-1), tvector3(1,1,1));
		if (mSpatialEntity->GetClassID() == ClassIDZMeshInstance)
		{
			ZMeshInstance *pmi = (ZMeshInstance*)mSpatialEntity->QueryInstance();
			ZMesh *pMesh = pmi->GetMesh();
			tmpb = pMesh->GetBVolume();
			if (pmi->GetPhysicTriMeshInstance())
			{
				pmi->GetPhysicTriMeshInstance()->SetMatrix(mWorldMatrix);
			}
		}
		else if (mSpatialEntity->GetClassID() == ClassIDZTrigger)
		{
			tmpb = ((ZTrigger*)mSpatialEntity->QueryInstance())->GetOriginVolume();
		}
#if 0
		else if (mSpatialEntity->GetClassID() == ClassIDZOccluderBox)
		{
			tmpb.SetBoxMinMax(tvector3(-1,-1,-1), tvector3(1,1,1));
			tmpb.ComputeBSphereFromBoxMinMax();
		}
		else if (mSpatialEntity->GetClassID() == ClassIDZLight)
		{
			ILight *pLight = (ILight*)mSpatialEntity->QueryInstance();
			if ( pLight->GetLightType() == LT_POINTLIGHT)
			{
				tmpb.SetBoxMinMax(tvector3(-0.5f,-0.5f,-0.5f)*pLight->GetRadius(), tvector3(0.5f,0.5f,0.5f)*pLight->GetRadius());
				tmpb.mBSphere = vector4(0,0,0, pLight->GetRadius());
			}
			else
			{
				float valsp1 = pLight->GetLength()*pLight->GetSpotAngle2();
				//float spng = pLight->GetSpotAngle2();
				tmpb.SetBoxMinMax(tvector3(-0.5f,-0.5f,0)*valsp1, tvector3(0.5f*valsp1,0.5f*valsp1,pLight->GetLength()));
				tmpb.ComputeBSphereFromBoxMinMax();
			}
		}
#endif
		tvector3 mBounds[8];
		tmpb.Build8CornersBox(mBounds);
		for (int i=0;i<8;i++)
			mBounds[i].TransformPoint(mWorldMatrix);

		tmpb.SetMinMaxFromCorners((unsigned char*)mBounds, sizeof(tvector3), 8);
		//tmpb.ComputeBSphereFromBoxMinMax();
		tmpb.TransformBSphere(tmpb.GetBSphere(), mWorldMatrix);
		mSpatialEntity->GetBVolume() = tmpb;

		//mSpatialEntity->SetWorldBSphere(tvector4(mWorldMatrix.position, 1));

	}
	// update childs
	if (mSpatialEntity.ptr())
	{
		if (mScene)
		{
			if (!mScene->ClearAsked())
				mScene->AddSEJobChangeMatrix(mSpatialEntity, mWorldMatrix);
		}
		else
		{
			if (!GScene->ClearAsked())
				GScene->AddSEJobChangeMatrixNoSceneInsertion(mSpatialEntity, mWorldMatrix);
		}
	}

	// update childs
	std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
	for (; iter != mChilds.end(); ++iter)
	{
		(*iter)->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTransform::FindByName(const char *szName, std::vector<ZTransform*>& arrayDest)
{
	if (mSpatialEntity.ptr())
	{
		const char *seSEName = mSpatialEntity->GetName().c_str();
		if (seSEName)
			if (strstr(seSEName, szName))
				arrayDest.push_back(this);
	}

	std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
	for (; iter != mChilds.end(); ++iter)
	{
		(*iter)->FindByName(szName, arrayDest);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZTransform* ZTransform::GetFirstWithSpatialEntity()
{
	if (mSpatialEntity.ptr())
		return this;

	std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
	for (; iter != mChilds.end(); ++iter)
	{
		ZTransform *ptr = (*iter)->GetFirstWithSpatialEntity();
		if (ptr)
			return ptr;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTransform::AddToScene(ZScene *pScene)
{
	ZASSERT( (!mParent), "Can't add a child transform to scene. Unlink it or add its parent");
	ZASSERT( (!mScene), "Transform already attached to a scene" );

	AddTransformToScene(pScene);
	pScene->AddRootTransform(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTransform::RemoveFromScene()
{
	if (!mScene) 
		return;

	//ZASSERT( (mScene), "Transform not assigned to a scene");
	ZASSERT( (!mParent), "Can't unassign child transform from scene. Unlink it before");

	mScene->RemoveRootTransform(this);
	RemoveTransformFromScene(mScene);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTransform::RemoveTransformFromScene(ZScene *pScene)
{
	mScene = NULL;

	if (mSpatialEntity.ptr())
		pScene->AddSEJobRemove(mSpatialEntity); // simple add

	std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
	for (; iter != mChilds.end(); ++iter)
	{
		(*iter)->RemoveTransformFromScene(pScene);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTransform::AddTransformToScene(ZScene *pScene)
{
	mScene = pScene;

	if (mSpatialEntity.ptr())
		pScene->AddSEJobChangeMatrix(mSpatialEntity, mSpatialEntity->GetWorldMatrix()); // simple add

	std::list<smartptr<ZTransform> >::iterator iter = mChilds.begin();
	for (; iter != mChilds.end(); ++iter)
	{
		(*iter)->AddTransformToScene(pScene);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
