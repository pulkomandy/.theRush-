///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSpatialEntity.h
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

#ifndef ZSPATIALENTITY_H__
#define ZSPATIALENTITY_H__

#include "..\libbase\ZBaseClass.h"
#include "..\libbase\ZSerializator.h"
#include "..\libbase\ZBaseMaths.h"
#include "..\libbase\ZBVolume.h"

class ZCluster;
class ZTransform;

class ZSpatialEntity : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZSpatialEntity();

	~ZSpatialEntity();

	virtual ZSpatialEntity* Clone() { return NULL; }


	void SetWorldMatrix(const tmatrix & worldMatrix)
	{
		mWorldMatrix = worldMatrix;
	}
	const tmatrix& GetWorldMatrix() const
	{
		return mWorldMatrix;
	}

	// Bounding Volume
	ZBoundingVolume& GetBVolume() { return mBVolume; }


	void SetTransform(ZTransform *pTrans) { mTransform = pTrans; }

	ZTransform* GetTransform() const
	{
		return mTransform;
	}

	// only called by scene
	void SetCluster(ZCluster* pCluster)
	{
		mCluster = pCluster;
	}
	ZCluster *GetCluster() const
	{
		return mCluster;
	}

	// visibility
	bool IsVisible() const { return mbVisible; }
	void SetVisible(bool bVis) { mbVisible = bVis; }

	void SetSelectable(bool bSelectable) { mbSelectable = bSelectable; }
	bool IsSelectable() const { return mbSelectable; }

	ZSpatialEntity * GetRootTransformSpatialEntity();
protected:
	ZTransform *mTransform;
	ZCluster *mCluster;
	// only called by ZTransform
	friend class ZTransform;
	friend class ZCluster;
	friend class ZScene;




	tmatrix mWorldMatrix;
	ZBoundingVolume mBVolume;

	bool mbVisible;
	bool mbSelectable;

};


#endif
