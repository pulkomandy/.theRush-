///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFxPass.cpp
// Creation : 28/06/2007
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

#include "FFx.h"
#include "ZFx.h"
#include "ZMaterial.h"

#include <Cg/cg.h>
#ifdef WIN32
#include <Cg/cgD3D9.h>
#else
#define _stricmp strcasecmp
#endif
#include <Cg/cgGL.h>

extern ZTexture *gCurrentFrameBufferTexture;
extern ZTexture *gCurrentMiniFrameBufferTexture;
extern ZTexture *gCurrentZBufferTexture;
extern ZTexture *m_pJitterTexture;
extern ZTexture *GpSMZTexture;

extern RenderingState_t GRenderingState;
//---------------------------------------------------------------------------------
// FFxPass building
//---------------------------------------------------------------------------------

FFxPass::FFxPass()
{
	// delete programs
	foreach (program, mPrograms, FFxProgram*)
		delete (*program);

    mZBufferTexture = mMiniFrameBufferTexture = mJitterSamplerIndex = mFrameBufferTexture = mRenderTargetSampler = /*mCurVelSampler = mLastVelSampler =*/ mShadowSamplerIndex = -1;


}


FFxPass::~FFxPass()
{
	unsigned int i;
	for (i = 0;i<mPrograms.size();i++)
		delete mPrograms[i];
}

void FFxPass::init(CGcontext aContext)
{
	foreach (program, mPrograms, FFxProgram*)
		(*program)->init(aContext);
}

void FFxPass::addProgram(FFxProgram* aProgram)
{
	mPrograms.push_back(aProgram);
}

void FFxPass::addSamplerParam(ZFx *pFx, FFxSamplerParam* aSamplerParam)
{
  const char *szsampler = cgGetParameterName(aSamplerParam->mCGparam);

  aSamplerParam->mTexture = NULL;

  if( (!_stricmp(szsampler,"ShadowMapSampler")) || (!_stricmp(szsampler,"ShadowMapSampler2")) )
  {
    aSamplerParam->mTexture = GpSMZTexture;
  }
  else if( !_stricmp(szsampler,"RenderTargetSampler") )
  {
    mRenderTargetSampler = int(mSamplerParams.size())-1;
  }
  else if( !_stricmp(szsampler,"FrameBuffer") )
  {
    aSamplerParam->mTexture = gCurrentFrameBufferTexture;
  }
  else if( !_stricmp(szsampler,"MiniFrameBuffer") )
	{
    aSamplerParam->mTexture = gCurrentMiniFrameBufferTexture;
		pFx->SetNeedsMiniFrame(true);
	}
  else if( !_stricmp(szsampler,"ZBuffer") )
	{
    aSamplerParam->mTexture = gCurrentZBufferTexture;
		pFx->SetNeedsZBuffer(true);
	}
  else if( !_stricmp(szsampler,"JitterSampler") )
  {
    aSamplerParam->mTexture = m_pJitterTexture;
  }

	mSamplerParams.push_back(*aSamplerParam);
}


//---------------------------------------------------------------------------------
// run time rendering
//---------------------------------------------------------------------------------

//CGparameter	aGGCGparam;

int GA1 = 1, GA2 = 1;

void SetAB(int a1, int a2)
{
	GA1 = a1;
	GA2 = a2;
}
float fMipMapLODBias = -1.0f;

void STLB(float v)
{
	fMipMapLODBias = v;
}

typedef void (*cgSetTextureParameterFunc)( CGparameter param, void* tex );
FFxPass *gLastPass = NULL;
void FFxPass::setPassState(ZMaterial* aMaterial)
{
	gLastPass = this;
	cgSetTextureParameterFunc cgSetTextureParameter;
#ifdef WIN32
	if( GDD->GetClassID() == ClassIDZDisplayDeviceDX9 )
		cgSetTextureParameter = (cgSetTextureParameterFunc) cgD3D9SetTextureParameter;
	else
#endif
	if( GDD->GetClassID() == ClassIDZDisplayDeviceOGL )
		cgSetTextureParameter = (cgSetTextureParameterFunc) cgGLSetTextureParameter;
	else
		return;

	// apply rendering state change for this pass
/*
	unsigned mBlending: 4; // 0 = none, 1 = (one, one), 2 = (SRCALPHA, INVSRCALPHA)
						// default = 0
	unsigned mZWriteEnable : 1; // default = 1
	unsigned mZTestType : 3; // 0 = none, 1=LessEgal,
					  // default = 1
	unsigned mCulling : 2; // 0 = none, 1=CCW, 2=CW
					// default = 1
*/
	// Apply Rendering States
	GDD->SetRenderingState(mRenderingState);
	

	// set sampler state
	ZFx *pfx = aMaterial->getEffect();

	int textureIndex = 0;
	int textureIndexAv = 0;
	ZTexture* texture;
	foreach(sampler, mSamplerParams, FFxSamplerParam)
	{
		// Set incremental sampling state
		const FFxSamplerParam& smpp = (*sampler);

		CGparameter aGGCGparam = smpp.mCGparam;

		// Check if we have already a texture linked to this pass when shader were built
		texture = smpp.mTexture;

		// If it's not the case, take the texture from material
		if( !texture && aMaterial )
		{
			const std::vector<smartptr<ZTexture> >& textures = aMaterial->getTextureArray();
			// the material should provide a texture at the specified index location
			if (textureIndexAv < (int)textures.size() && (texture = textures[textureIndexAv]) )
			{
				// frame update
				texture->SetFrameUpdate(texture->NeedUpdate());
				textureIndexAv++;
			}
		}

		// bind texture object to this sampler
		if( texture != NULL && smpp.mTextureIndex != -1 )
		{
			const samplerparam_t& smpt = pfx->GetSamplerParam(smpp.mSamplerIndex);

			GDD->SetSamplerState(smpp.mTextureIndex, smpt, texture);

			
			cgSetTextureParameter(aGGCGparam, texture->GetPlatformHandle());
		}

		textureIndex ++;
	}

	// bind vertex/fragment program for this pass
	foreach(program, mPrograms, FFxProgram*)
		(*program)->bind();
}
void FFxPass::UnbindPrograms()
{
	foreach(program, mPrograms, FFxProgram*)
		(*program)->unbind();

}

void CleanPasses()
{
	if (gLastPass)
		gLastPass->UnbindPrograms();
}
