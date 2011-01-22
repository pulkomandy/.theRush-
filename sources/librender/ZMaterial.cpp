///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZMaterial.cpp
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

#include <vector>

#include "FFx.h"
#include "ZFx.h"
#include "ZMaterial.h"
#include "libbase/ZLogger.h"

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZMaterial)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMaterial::ZMaterial() :
mEffect(0),mDisabled(0),mTexIndex(0),mTechnic(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMaterial::~ZMaterial()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMaterial *ZMaterial::Clone()
{
	ZMaterial *pMat = (ZMaterial*)_New(ZMaterial);
	pMat->Clone(this);
	return pMat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMaterial::Clone(ZMaterial *pMatToClone)
{
	ZMaterial *pMat = (ZMaterial*)(void*)pMatToClone;//NewClass(ZMaterial);

	unsigned int i;
	for (i=0;i<pMat->mParams.size();i++)
	{
		mParams.push_back(pMat->mParams[i]);
	}

	mDisabled = pMat->mDisabled;

	mPhysicsMaterial = pMat->mPhysicsMaterial;
	mTexIndex = pMat->mTexIndex;

	mTextures = pMat->mTextures;
	mEffectFileName = pMat->mEffectFileName;
	mEffect.Attach(pMat->mEffect.ptr());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int	ZMaterial::getNumParams() const
{
	return int(mParams.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

FFxSetParam* ZMaterial::getParam(int aParamIndex) const
{
	if (aParamIndex < int(mParams.size()))
		return mParams[aParamIndex];
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

FFxSetParam* ZMaterial::getNamedParam(const char* aName) const
{
	foreach_const (param, mParams, FFxSetParam*)
	{
		if ((*param)->mName == aName)
		{
			return *param;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZFx* ZMaterial::getEffect() const
{
	return mEffect;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMaterial::setEffectFileName(const char * aFileName)
{
	mEffectFileName = aFileName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

const char *	ZMaterial::getEffectFileName() const
{
    return mEffectFileName.c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMaterial::setEffect(ZFx* aEffect)
{
	// FX not supported yet for OGL Renderer
	//if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL) return;
	mEffect.Attach(aEffect);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMaterial::addParam(FFxSetParam* aParam)
{
	mParams.push_back(aParam);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMaterial::addTexture(ZTexture* aTexture)
{
	if (!aTexture) return;
	mTextures.push_back(aTexture);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// This function connect the specified effect to the material.
// It tries to connect the exposed parameters of the effect to the material.
// An option allows to create a new parameter when it is not found in the material (false)
// An option enable the removing of material parameters that are not beeing declared
// in the effect (default is false). The latter option is used when re-connected an effect
// at runtime
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FFxSemanticInfo;
FFxSemanticInfo* getSemanticInfo(const char* semantic);
void ZMaterial::connectEffect(bool aAddNewParams,  bool aRemoveOldParams)
{
	if (!mEffect.ptr())
	{
		if (!mEffectFileName.Empty())
		{
			tstring	effectName = mEffectFileName.GetBaseName();
			ZFx * 	fx = (ZFx *)FindInstance(ZFx, effectName.c_str());//_factory("ZFx")->getObjectFromMemory(effectName.c_str());
			if (fx)
			{
				mEffect.Attach(fx);
				LOG ("connecting effect %s to material %s\n", effectName.c_str(), GetName().c_str());
			}
		}
		if (!mEffect.ptr())
			return;
	}
    foreach_const (itEffect, mEffect->getParams(), FFxParam*)
	{
		FFxParam*	effectParam = *itEffect;
		// search the effect parameter in the material array
		FFxSetParam* materialParam =(FFxSetParam*) getNamedParam(effectParam->mName);
		const char* semantic = cgGetParameterSemantic(effectParam->mCGparam);
		FFxSemanticInfo* semanticInfo = getSemanticInfo(semantic);
		if (semanticInfo)
		{
			//int a = 1;
		}
		else
		if (materialParam)
		{
			// connect material param to effect param
			materialParam->mCGparam = effectParam->mCGparam;
		}
	    else if (cgGetTypeClass(cgGetParameterType(effectParam->mCGparam)) != CG_PARAMETERCLASS_MATRIX)
		{
			// the effect parameter has not been found in the material
			// this is the case when the material is created by the runtime or
			// when the effect is changed by the runtime. In such case, the
			// material parameters are not supposed to match the effect.
			// In this situation there's 2 options : keep this effect parameter unconnected
			// (default behavior) or add a new parameter in the material with the current
			// data value
			if (aAddNewParams)
			{
				LOG ("adding new parameter '%s' to material '%s'\n", effectParam->mName.c_str(), GetName().c_str());
				FFxData* data = FFxData::create(effectParam->mCGparam);
				if (data)
				{
					FFxSetParam* materialParam = new FFxSetParam(effectParam->mName, data);
					materialParam->mCGparam = effectParam->mCGparam;
					addParam (materialParam);
				}
			}
			else
			{
				WARN ("can't connect effect parameter '%s' to material '%s'\n", effectParam->mName.c_str(), GetName().c_str());
			}
		}
	}

	// if the user asked, remove unconnected parameter
	if (aRemoveOldParams)
	{
		std::vector<FFxSetParam*>::iterator it = mParams.begin();
	    while (it != mParams.end())
		{
			FFxSetParam*	materialParam = *it;
			if (materialParam->mCGparam == 0)
			{
				// connect material param to effect param
				LOG ("removing old parameter '%s' from material '%s'", materialParam->mName.c_str(), GetName().c_str());
				it = mParams.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// runtime rendering functions
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZMaterial::open(bool bAlreadyOpen)
{
	if (!mEffect.ptr() || mDisabled)
		return false;
	if (!bAlreadyOpen)
	{
		mEffect->beginMaterial(this);

		// apply material parameters, this will update current effect parameters
		// which are themself connected to the vertex/fragment program parameters
		foreach (param, mParams, FFxSetParam*)
		{
			(*param)->apply();
		}
	}
	mEffect->setViewDependentMatrices();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZMaterial::open()
{
	if (!mEffect.ptr() || mDisabled)
		return false;

	mEffect->beginMaterial(this);

	// apply material parameters, this will update current effect parameters
	// which are themself connected to the vertex/fragment program parameters
	foreach (param, mParams, FFxSetParam*)
	{
		(*param)->apply();
	}
	mEffect->setViewDependentMatrices();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ZMaterial::getNumPasses() const
{
	if (!mEffect.ptr())
		return 0;
	else
		return mEffect->getNumPasses();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZMaterial::setPass(int32 aPassNum)
{
	mEffect->setPassState(aPassNum);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZMaterial::IsEsqualTo(ZMaterial *pMat)
{
	ZMaterial *pm = (ZMaterial*)pMat;
	if (pm->mEffect != mEffect)
		return false;
	if (pm->GetTechnic() != GetTechnic())
		return false;
	if (pm->mParams.size() != mParams.size())
		return false;
	if (pm->mTextures.size() != mTextures.size())
		return false;
	for (unsigned int i=0;i<mTextures.size();i++)
	{
		if (mTextures[i] != pm->mTextures[i])
			return false;
	}


	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
