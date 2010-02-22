///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFxTechnique.cpp
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

#include "stdafx.h"
#include "FFx.h"
#include "../libbase/LibBase.h"


///////////////////////////////////////////////////////////////////////////////////////////////////

FFxTechnique::FFxTechnique() :
mCurMaterial(0),mHWITechnic(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

FFxTechnique::~FFxTechnique()
{
	// delete passes
	foreach (pass, mPasses, FFxPass*)
		delete (*pass);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FFxTechnique::init(CGcontext aContext)
{
	for (std::vector<FFxPass*>::iterator it =mPasses.begin();
		it != mPasses.end();
		it++)
	{
		(*it)->init(aContext);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// add pass

void FFxTechnique::addPass(FFxPass* aPass)
{
	mPasses.push_back(aPass);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// this function set the standards and user uniform parameters used by the vertex/fragment shaders

void FFxTechnique::beginMaterial(ZMaterial* aMaterial)
{
	mCurMaterial = aMaterial;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FFxTechnique::setPassState(uint aPass)
{
	if (aPass >= mPasses.size())
	{
		LOG("Invalid pass state num %d", aPass);
		return;
	}
	mPasses[aPass]->setPassState(mCurMaterial);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FFxTechnique::setPassState(int aPass, RenderingState_t rs)
{
	mPasses[aPass]->mRenderingState = rs;
	mPasses[aPass]->setPassState(mCurMaterial);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
