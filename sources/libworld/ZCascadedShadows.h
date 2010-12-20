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
#ifndef CASCADED_SHADOWS_H
#define CASCADED_SHADOWS_H

#include "..\libbase\ZFrustum.h"
#include "..\libworld\ZCamera.h"

class ZMeshInstance;
class ZTexture;


typedef struct shadowmap_t
{
	// shadow map rendering
	tmatrix mShdView, mShdProj, mShdMat;
	ZFrustum mShadowFrustum;

	// solid objects rendering
	float mNearPlane, mFarPlane;
	ZFrustum mViewFrustum;

	// meshes for shadowmaps
	ZMeshInstance * mCasterMeshes[1024];
	tmatrix mCasterMeshesMatrix[1024];
	unsigned int mNbCasterMeshes;

	// Solid casted meshes viewed
	ZMeshInstance * mCastedMeshes[1024];
	tmatrix mCastedMeshesMatrix[1024];
	unsigned int mNbCastedMeshes;

	// texture
	// IDIRECT3DTEXTURE9 *m_pSMZTexture;
	ZTexture* m_pSMZTexture;

} shadowmap_t;


//void RenderSolids(IMeshInstance **meshes, int nbMeshes);
#ifdef WIN32
void BuildJitterShadowTexture();
#endif
void RenderShadowMaps();
void RenderShadowCasted();

void DisplayShadowMaps();


// V2
void InitShadowSystem();
void UninitShadowSystem();
void ComputeCascadedShadowInfos(ZCamera *pCam, shadowmap_t* aShdmi);
void RenderCascadedShadowMaps(shadowmap_t * aShdmi);

#endif
