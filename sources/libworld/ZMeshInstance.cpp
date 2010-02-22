///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZMeshInstance.cpp
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


BEGIN_SERIALIZATION(ZMeshInstance)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////


ZMeshInstance::~ZMeshInstance()
{
#ifdef _DEBUG
	LOG("Destroying Mesh Instance 0x%x \"%s\"\n", this, this->GetName().c_str());
#endif

	//ZASSERT( (mMesh->GetRefCount() < 2) , "~ZMeshInstance : Too much references on ZMesh");
/*
	if (mPhysicModel&&mPhysicModel->mPhysicWorld)
		mPhysicModel->mPhysicWorld->RemoveCollisionMesh((ZPhysicCollisionMeshBullet*)mPhysicModel);
*/
    for (uint i=0;i<mMaterial.size();i++)
        delete mMaterial[i];
	mMaterial.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSpatialEntity * ZMeshInstance::Clone()
{
	ZMeshInstance *nInst = (ZMeshInstance*)_New(ZMeshInstance);
	nInst->SetMesh(GetMesh());
	nInst->SetVisible(IsVisible());
	nInst->SetShadowCaster(this->IsShadowCaster());
	// occlusion
	if (GetOcclusionQuery())
		nInst->EnableOcclusionQuery(true);

	/*
	nInst->mTransform.Attach(mTransform->Clone());
	nInst->mTransform->SetMeshInstance(nInst);
	*/


	for (uint i=0;i<nInst->mMaterial.size();i++)
		delete nInst->mMaterial[i];

	for (uint i=0;i<mMaterial.size();i++)
	{
		nInst->mMaterial[i] = mMaterial[i]->Clone();
	}

	nInst->SetTransparent(IsTransparent());

	tstring clonename = "CloneOf";
	clonename += ((ZSpatialEntity*)this)->GetName();
	((ZSpatialEntity*)nInst)->SetName(clonename);

	
	//nInst->SetPhysicTriMesh(GetPhysicTriMesh);
	if (mPhysicTMInstance)
	{
		ZPhysicWorldBullet *aWorld = mPhysicTMInstance->GetWorld();
		nInst->SetPhysicTriMeshInstance(aWorld->AddPhysicModelToWorld(mPhysicTMInstance->GetTriMesh(), aWorld));

	}
/*
	if (mPhysicModel)
	{

		ZMesh *pmodmesh = mPhysicModel->GetTrimesh();
		ZMesh *pmodmeshDst = (ZMesh*)_New(ZMesh);

		pmodmeshDst->SetIndexBuffer(pmodmesh->GetIndexBuffer());
		pmodmeshDst->SetVertexBuffer(pmodmesh->GetVertexBuffer());


		ZPhysicCollisionMeshBullet *newcol = ZPhysicWorldBullet::CreateCollisionMesh(pmodmeshDst);
		nInst->mPhysicModel = newcol;

	}
	*/
	return nInst;
}

///////////////////////////////////////////////////////////////////////////////////////////////////