///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSpatialEntity.cpp
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

#include "ZSpatialEntity.h"
#include "ZScene.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZSpatialEntity)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSpatialEntity::ZSpatialEntity()
{
	//printf("New SE 0x%x\n", this);
	mbVisible = true;
	mWorldMatrix = tmatrix::identity;

	mTransform = (ZTransform*)_New(ZTransform);
	mTransform->AttachSpatialEntity(this);
	mCluster = NULL;
	mbSelectable = true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSpatialEntity::~ZSpatialEntity()
{
	// delete transform, not mesh
	// when subrefing transform, mTransform from mesh must be NULL
	//ZASSERT( (mTransform == NULL) ,"mTransform not null in Spatial Entity destructor");

	// cluster

	if (mCluster)
	{
		ZCluster *tmpClust = mCluster;
		mCluster = NULL;
		tmpClust->RemoveSpatialEntity(this);
	}

	//ZASSERT( (mTransform->GetRefCount() < 2), "~ZSpatialEntity : mTransform still referenced");
}


///////////////////////////////////////////////////////////////////////////////////////////////////

ZSpatialEntity * ZSpatialEntity::GetRootTransformSpatialEntity()
{
	if (!mTransform->GetParent())
		return this;
	if (mTransform->GetParent()->GetSpatialEntity())
		return mTransform->GetParent()->GetSpatialEntity()->GetRootTransformSpatialEntity();
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
