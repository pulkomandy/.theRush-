///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWGizmosNode.h
// Creation : 02/06/2009
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

#ifndef ZENFWGIZMOSNODE_H__
#define ZENFWGIZMOSNODE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../LibPlatform/libPlatform.h"

class Zen3DEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenFWGizmosNode : public ZenFWNode
{
public:
	ZenFWGizmosNode(Zen3DEdit *p3DEdit) : ZenFWNode("GizmosNode")
	{
		mbIsActive = false;
		mSelectedSE = NULL;
		mbDrawSEBVolume = false;
		mbDrawClusters = false;
		mbDrawLights = mbDrawOccluders = mbDrawSound = true;
		m3DEdit = p3DEdit;
	}
	virtual ~ZenFWGizmosNode();

	virtual int GetPreferredLine() const { return 0; }

	virtual void Run();
	virtual unsigned int GetPriority() const { return 2; }
	void Init();

	void SetSelection(ZSpatialEntity *pse);

	bool mbDrawSEBVolume, mbDrawClusters, mbDrawLights, mbDrawOccluders, mbDrawSound;

protected:
	void DrawGizmo(ZCamera *pCam);
	void RenderChildClusters(ZCluster *pClust);
	void RenderSEBVolumes(ZCluster *pClust);

	smartptr<ZMeshInstance> mWireSelectionBox;
	smartptr<ZMesh> wDummysph;
	smartptr<ZMesh> wDummyCone;
	smartptr<ZMesh> wirebox;

	//IMeshInstance *miDummySph;
	smartptr<ZTexture> mSelectionTexture;
	ZSpatialEntity * mSelectedSE;


	Zen3DEdit *m3DEdit;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
