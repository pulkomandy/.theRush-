///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZFx.h
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
#include "FFxImport.h"
/*
#ifdef MAC_OS
#import <OpenGL/OpenGL.h>
#else
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <GL/gl.h>
#endif
*/
#include "IDisplayDevice.h"
#include "libbase/ZLogger.h"
#include "libbase/ZTimer.h"
#include "libworld/ZCamera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
CGcontext gDefaultCGcontext;
tmatrix mShadowMap;
float ZFx::mCameraInfos[2];

///////////////////////////////////////////////////////////////////////////////////////////////////

float			ZFx::mShaderTime = 0;
extern const char* COMPILE_ARGS[];

/*
ZFx* ZFx::getDefaultFX()
{
	if (GDD->GetClassID() == ClassIDZDisplayDeviceNULL)
	{
		static ZFx *genNULLFx = NULL;
		if (!genNULLFx)
		{
			genNULLFx = (ZFx*)_New(ZFx);
			genNULLFx->AddRef();
			genNULLFx->SetName(".\\ZenithDatas\\DefaultShader.fx");
		}
		return genNULLFx;
	}
	else if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL)
	{
		if (!((ZDisplayDeviceOGL*)GDD)->gDefaultShader.ptr())
		{
			ZFx* nnfx = DirectLoad(".\\ZenithDatas\\DefaultShader.fx");
			((ZDisplayDeviceOGL*)GDD)->gDefaultShader.Attach(nnfx);
		}
		return ((ZDisplayDeviceOGL*)GDD)->gDefaultShader.ptr();
	}
#ifdef WIN32
	else
	if (!((ZDisplayDeviceDX9*)GDD)->gDefaultShader.ptr())
    {
        ZFx* nnfx = DirectLoad(".\\ZenithDatas\\DefaultShader.fx");
		((ZDisplayDeviceDX9*)GDD)->gDefaultShader.Attach(nnfx);
    }
    return ((ZDisplayDeviceDX9*)GDD)->gDefaultShader.ptr();
#else
	return NULL;
#endif
}
*/
ZFx *DirectLoad(const char *filename)
{
	tstring tmpf = filename ;
	tmpf.SimplifyPath();


	ZFx* nnfx = (ZFx*)FindInstance(ZFx, tmpf.c_str());
	if (nnfx) 
	{
		nnfx->AddRef();// never free shader
		return nnfx;
	}

    nnfx = (ZFx*)_New(ZFx);

    FFxImport fxImport;
	if (GDD->GetClassID() != ClassIDZDisplayDeviceNULL)
	{
		fxImport.importCgfx(nnfx, gDefaultCGcontext, tmpf.c_str(), COMPILE_ARGS);
		//gDefaultShader.Attach(nnfx);
		nnfx->init(gDefaultCGcontext, true);
	}
	else
	{
		nnfx->AddRef();
	}

	nnfx->SetName(tmpf);
	nnfx->AddRef(); // never free shader
    return nnfx;
}

bool DirectLoad(ZFx *nnfx, const char *filename)
{

    FFxImport fxImport;
	if (GDD->GetClassID() != ClassIDZDisplayDeviceNULL)
	{
		fxImport.importCgfx(nnfx, gDefaultCGcontext, filename, COMPILE_ARGS);
		//gDefaultShader.Attach(nnfx);
		nnfx->init(gDefaultCGcontext, true);
	}

	nnfx->SetName(filename);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_SERIALIZATION(ZFx)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////

ZFx::ZFx() :
mCurTechnique(0),mDoesBlend(0),mbNeedMiniFrame(false),mbHasZWrite(true),mbNeedZBuffer(false),mbHWIEnable(false)
{
}

ZFx::~ZFx()
{
	clearAll();
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZFx::clearAll()
{
	// delete techniques
	foreach (tech, mTechniques, FFxTechnique*)
		delete (*tech);
	mTechniques.clear();
	mCurTechnique = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZFx::setEffectFileName(const char* aFileName)
{
	mSourceFileName = aFileName;
}

void ZFx::addTechnique(FFxTechnique* aTechnique)
{
	mTechniques.push_back(aTechnique);
	if (mCurTechnique == 0)
		mCurTechnique = aTechnique;
}

void ZFx::init(CGcontext aContext, bool bAddNewParams)
{
	mCGcontext = aContext;

	foreach (param, mParams, FFxParam*)
		(*param)->init(aContext);

	foreach (tech, mTechniques, FFxTechnique*)
	{
		(*tech)->init(aContext);
		if ((*tech)->mHWITechnic)
			(*tech)->mHWITechnic->init(aContext);
	}

	connectProgramParameters(bAddNewParams);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// runtime rendering


FFxTechnique* ZFx::selectTechniqueByIndex(uint32_t aNum)
{
	if (aNum < mTechniques.size())
		mCurTechnique = mTechniques[aNum];
	else
		LOG("Invalid technique num %d", aNum);
	return mCurTechnique;
}

FFxTechnique* ZFx::selectTechniqueByIndex(uint32_t aNum, ZMaterial *pMat)
{
	if (aNum < mTechniques.size())
	{
		mCurTechnique = mTechniques[aNum];
		mCurTechnique->mCurMaterial = pMat;
	}
	else
		LOG("Invalid technique num %d", aNum);
	return mCurTechnique;
}

int ZFx::getNumPasses() const
{
	if (mCurTechnique == 0)
		return 0;
	return (int)mCurTechnique->mPasses.size();
}

bool ZFx::beginMaterial(ZMaterial* aMaterial)
{
	const ZCamera *pCam = GetActiveCamera();
	mCameraInfos[0] = pCam->GetNearPlane();
	mCameraInfos[1] = pCam->GetFarPlane() * 0.5773502f;// 1.f/sqrt( 1≤+1≤+1≤) -> set cube corner as far as that

	if (mCurTechnique == 0)
		return false;
	mCurTechnique->beginMaterial(aMaterial);

	// all effects parameter should not be set
	// we must move to the pass the parameters thar are meaningful for the programs
	foreach (param, mParams, FFxParam*)
		(*param)->apply();


	return true;
}

void ZFx::setViewDependentMatrices()
{
	//mShaderTime = ;

/*
	if (GClientGame)
	{
		if (GClientGame->GetGameSpeed() >0.01f)
		{
			mShaderTime = LERP(mShaderTime, fabs(gTimer.GetTime()), GClientGame->GetGameSpeed());
		}
	}
	else
		*/
		mShaderTime = gTimer.GetTime();


	foreach (param, mMatrixParams, FFxParam*)
		(*param)->apply();

}

void ZFx::setPassState(int aPass)
{
	if (mCurTechnique)
		mCurTechnique->setPassState(aPass);
}

void ZFx::setPassState(int aPass, RenderingState_t rs)
{
	if (mCurTechnique)
		mCurTechnique->setPassState(aPass, rs);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// This table contains meaningful info attached to parameter semantics

struct FFxSemanticInfo
{
	enum
	{
		IDENTITY,
		GL_MATRIX,
		TIME,
		SUNVECTOR,
		SUNCOLOR,
		SUNAMBIENT,
		SHADOWFILTERSIZE,
		CAMERAINFOS,
	};

	const char* mName;
	uint32_t	mType;
	uint32_t	mGLMatrixID;
	uint32_t	mGLMatrixOp;
};



static FFxSemanticInfo SemanticInfo[] =
{
	{"worldviewprojection", FFxSemanticInfo::GL_MATRIX, ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"worldviewprojectiontranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},
	{"worldviewprojectioninverse", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_INVERSE},
	{"worldviewprojectioninversetranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_INVERSE_TRANSPOSE},

	{"modelviewprojection", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"modelviewprojectiontranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},
	{"modelviewprojectioninverse", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_INVERSE},
	{"modelviewprojectioninversetranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_INVERSE_TRANSPOSE},

	{"modelview", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"modelviewtranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},
	{"modelviewinverse", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_INVERSE},
	{"modelviewinversetranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_INVERSE_TRANSPOSE},

	{"worldview", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"worldviewtranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},
	{"worldviewinverse", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_INVERSE},
	{"worldviewinversetranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_MODELVIEW_MATRIX, ZCG_GL_MATRIX_INVERSE_TRANSPOSE},

	{"view", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_VIEW_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"viewtranspose", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_VIEW_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},
	{"viewinverse", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_VIEW_MATRIX, ZCG_GL_MATRIX_INVERSE},
	{"viewinversetranspose", FFxSemanticInfo::GL_MATRIX, ZCG_GL_VIEW_MATRIX, ZCG_GL_MATRIX_INVERSE_TRANSPOSE},

	{"viewprojection", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_VIEW_PROJECTION_MATRIX, ZCG_GL_MATRIX_IDENTITY},

	{"world", FFxSemanticInfo::GL_MATRIX,  ZCG_GL_WORLD_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"worldtranspose", FFxSemanticInfo::GL_MATRIX, ZCG_GL_WORLD_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},
	{"worldinverse", FFxSemanticInfo::GL_MATRIX, ZCG_GL_WORLD_MATRIX, ZCG_GL_MATRIX_INVERSE},
	{"worldinversetranspose", FFxSemanticInfo::GL_MATRIX, ZCG_GL_WORLD_MATRIX, ZCG_GL_MATRIX_INVERSE_TRANSPOSE},

	{"projection", FFxSemanticInfo::GL_MATRIX, ZCG_GL_PROJECTION_MATRIX, ZCG_GL_MATRIX_IDENTITY},
	{"projectiontranspose", FFxSemanticInfo::GL_MATRIX, ZCG_GL_PROJECTION_MATRIX, ZCG_GL_MATRIX_TRANSPOSE},

    {"shadowmatrix", FFxSemanticInfo::GL_MATRIX, ZCG_FRESH_SHADOW_MATRIX, ZCG_GL_MATRIX_IDENTITY},
    {"TextureTransform", FFxSemanticInfo::GL_MATRIX, ZCG_TEXTURE_TRANSFORM_MATRIX, ZCG_GL_MATRIX_IDENTITY},
    {"PreviousWorld", FFxSemanticInfo::GL_MATRIX, ZCG_PREVIOUS_WORLD_MATRIX, ZCG_GL_MATRIX_IDENTITY},

	{"time", FFxSemanticInfo::TIME, 0, 0},
	{"SunColor", FFxSemanticInfo::SUNCOLOR, 0, 0},
	{"SunAmbient", FFxSemanticInfo::SUNAMBIENT, 0, 0},
	{"SunVector", FFxSemanticInfo::SUNVECTOR, 0, 0},
	{"ShadowFilterSize", FFxSemanticInfo::SHADOWFILTERSIZE, 0, 0},
	{"CameraInfos", FFxSemanticInfo::CAMERAINFOS, 0, 0},
	{0, 0, 0},
};

FFxSemanticInfo* getSemanticInfo(const char* semantic)
{

	FFxSemanticInfo*	info = SemanticInfo;
	while (info->mName)
	{
		if (stricmp(semantic, info->mName) == 0)
			return info;
		info++;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// This function create an effect param with the same type and the same intial value
// than the specified CG parameter

FFxParam* ZFx::createEffectParam(CGparameter parameter, bool& aMatrixParam)
{
	FFxData*	data = 0;
	const char* name = cgGetParameterName(parameter);
	//CGtype  type = cgGetParameterType(parameter);
	const char* semantic = cgGetParameterSemantic(parameter);
	aMatrixParam = false;

	if (semantic == 0)
		semantic = "";
	else
	{
		// handle semantic
		FFxSemanticInfo* semanticInfo = getSemanticInfo(semantic);
		if (semanticInfo)
		{
			switch (semanticInfo->mType)
			{
			case FFxSemanticInfo::GL_MATRIX:
			{
				data = new FFxDataGlMatrix(semanticInfo->mGLMatrixID, semanticInfo->mGLMatrixOp);
				aMatrixParam = true;
			}
			break;
			case FFxSemanticInfo::IDENTITY:
			{
				data = FFxData::create(CG_FLOAT4x4, &tmatrix::identity.m16[0]/*right.x*/, 16);
			}
			break;
			/* FIXME
			case FFxSemanticInfo::SUNVECTOR:
				data = FFxData::create(CG_FLOAT3, (float*)&GetSky()->GetSunVector(), 3, true);
				break;
			case FFxSemanticInfo::SUNCOLOR:
				data = FFxData::create(CG_FLOAT4, (float*)&GetSky()->GetSunColor(), 4, true);
				break;
			case FFxSemanticInfo::TIME:
				data = FFxData::create(CG_FLOAT, (float*)&ZFx::mShaderTime, 1, true);
				break;
			case FFxSemanticInfo::SUNAMBIENT:
				data = FFxData::create(CG_FLOAT4, (float*)&GetSky()->GetSunAmbient(), 4, true);
				break;
			case FFxSemanticInfo::SHADOWFILTERSIZE:
				data = FFxData::create(CG_FLOAT, (float*)&GetSky()->GetShadowFilterSizeDiv1024(), 1, true);
				break;
			case FFxSemanticInfo::CAMERAINFOS:
				data = FFxData::create(CG_FLOAT2, (float*)&mCameraInfos, 2, true);
				break;
				*/
			default:
				data = 0;
				break;
			}
		}
		else
		{
			//LOG("can't find semantic '%s'\n", semantic);
		}
	}
	if (data == 0)
		data = FFxData::create(parameter);

	// create and connectparameter
	if (data)
	{
		FFxEffectParam* param = new FFxEffectParam(name, semantic, data);
		param->init(mCGcontext);
		return param;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// This function find a global/effect parameter by name and connect it to a program parameter

void ZFx::connectParameter(const tstring& aEffectParamName, CGparameter parameter, bool bAddNewParams)
{
	// connect the effect parameter to the program parameter
	FFxParam*	fxParam = FFxParamable::getNamedParam(aEffectParamName);
	if (fxParam == 0 && bAddNewParams)
	{
		bool bMatrixParam;

		fxParam = createEffectParam(parameter, bMatrixParam);

		if (fxParam)
		{
			if (bMatrixParam)
				FFxParamable::addMatrixParam(fxParam);
			else
				FFxParamable::addParam(fxParam);
		}
	}
	if (fxParam)
	{
		/*
		LOG ("connecting (%x)%s to (%x)%s'\n",
			fxParam->mCGparam,
			fxParam->mName.c_str(),
			parameter,
			cgGetParameterName(parameter)
		);
		*/
		cgConnectParameter(fxParam->mCGparam, parameter);
	}
	else
	{
		//LOG ("can't find effect parameter '%s'\n", aEffectParamName.c_str());
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// This function connect the parameters of the programs to the effect
// The function must be called when all the programed have been loaded and all the
// technique validated


void dumpCgSamplerStateAssignment(CGparameter param, CGstateassignment sa, CGbool unabridged);

void ZFx::connectTechnique(FFxTechnique* technique, bool bAddNewParams)
{
	// traverse all passes
	for (unsigned int j=0; j<technique->mPasses.size(); j++)
	{
		FFxPass* pass = technique->mPasses[j];
		// traverse all programs
		for (unsigned int k=0; k<pass->mPrograms.size(); k++)
		{
			FFxProgram* program = pass->mPrograms[k];

			// each program hold an array of connection pairs generated at export time
			// this allow to reconnect the arguments parameters of the program
			foreach (connection, program->mConnections, FFxParamConnection)
			{
				CGparameter parameter = cgGetNamedParameter(program->mProgram, connection->mTo.c_str());
				if (parameter)
				{
					connectParameter(connection->mFrom, parameter, bAddNewParams);
				}
			}
			// traverse all uniform parameters and connect remaining unconnected parameters that share
			// the same name than a global/effect parameter
			// GLOBAL DOMAIN

			for (
				CGparameter	parameter = cgGetFirstLeafParameter(program->mProgram, CG_GLOBAL);
				parameter;
			parameter = cgGetNextParameter(parameter)
				)
			{
				if (
					cgGetParameterVariability(parameter) == CG_UNIFORM &&
					cgIsParameterUsed(parameter, program->mProgram) &&
					cgGetConnectedParameter(parameter) == 0
					)
				{
					if (cgGetTypeClass(cgGetParameterType(parameter)) != CG_PARAMETERCLASS_SAMPLER)
						connectParameter(cgGetParameterName(parameter), parameter, bAddNewParams);
					else
					{
						//LOG("connecting texture sampler %s: resindex=%d\n", cgGetParameterName(parameter), cgGetParameterResourceIndex(parameter));
						int smpIdx = this->GetSamplerByName(cgGetParameterName(parameter));
						FFxSamplerParam tmp(parameter, cgGetParameterResourceIndex(parameter), smpIdx, NULL);
						pass->addSamplerParam(this, &tmp);


						// --
					}
				}
			}

			// PROGRAM DOMAIN

			for (
				CGparameter	parameter = cgGetFirstLeafParameter(program->mProgram, CG_PROGRAM);
				parameter;
			parameter = cgGetNextParameter(parameter)
				)
			{
				if (
					cgGetParameterVariability(parameter) == CG_UNIFORM &&
					cgIsParameterUsed(parameter, program->mProgram) &&
					cgGetConnectedParameter(parameter) == 0
					)
				{
					if (cgGetTypeClass(cgGetParameterType(parameter)) != CG_PARAMETERCLASS_SAMPLER)
						connectParameter(cgGetParameterName(parameter), parameter, bAddNewParams);
					else
					{
						// LOG("connecting texture sampler %s: resindex=%d\n", cgGetParameterName(parameter), cgGetParameterResourceIndex(parameter));
						FFxSamplerParam tmp(parameter, cgGetParameterResourceIndex(parameter), 0, NULL);
						pass->addSamplerParam(this, &tmp);
					}
				}
			}
		}
	}
}
void ZFx::connectProgramParameters(bool bAddNewParams)
{
	// traverse all techniques
	for (unsigned int i=0; i<mTechniques.size(); i++)
	{
		FFxTechnique* technique = mTechniques[i];
		connectTechnique(technique, bAddNewParams);
		if (technique->mHWITechnic)
			connectTechnique(technique->mHWITechnic, bAddNewParams);

	}
}





///////////////////////////////////////////////////////////////////////////////////////////////////
// This function build the effect from a cgfx file.
// CAUTION: this is a MINIMAL import of CG not full CGFX.
// To import a cgfx, you must use the FFxImport class which can be run only from the PC.
//
// This function allows only to extract the vertex/fragment shader from a cgfx file.
// Effect parameters, techniques, passes and state change will NOT be imported.
//

bool ZFx::importProgramFromCgfx(const char* aCgfxFileName, const char* aVertexEntry, const char* aFragmentEntry)
{
	FFxTechnique* technique = new FFxTechnique();
	FFxPass* pass = new FFxPass();
	FFxProgram* vertexProgram = new FFxProgram(CG_VERTEX_DOMAIN, aCgfxFileName, aVertexEntry, 0);
	FFxProgram* fragmentProgram = new FFxProgram(CG_FRAGMENT_DOMAIN, aCgfxFileName, aFragmentEntry, 0);

	pass->addProgram(vertexProgram);
	pass->addProgram(fragmentProgram);
	technique->addPass(pass);
	addTechnique(technique);

	init(gDefaultCGcontext, true);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// This function build the effect from 2 .cg files
// CAUTION: this is a MINIMAL import of CG not full CGFX.
// To import a cgfx, you must use the FFxImport class which can be run only from the PC.
//
// This function allows only to extract the vertex/fragment shader from a cgfx file.
// Effect parameters, techniques, passes and state change will not be imported.
//

bool ZFx::importProgramFromCg(const char* aVertexShader, const char* aFragmentShader)
{
	FFxTechnique* technique = new FFxTechnique();
	FFxPass* pass = new FFxPass();
	FFxProgram* vertexProgram = new FFxProgram(CG_VERTEX_DOMAIN, aVertexShader, "main", 0);
	FFxProgram* fragmentProgram = new FFxProgram(CG_FRAGMENT_DOMAIN, aFragmentShader, "main", 0);

	pass->addProgram(vertexProgram);
	pass->addProgram(fragmentProgram);
	technique->addPass(pass);
	addTechnique(technique);

	init(gDefaultCGcontext, true);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Use the programs of this FX by default

void ZFx::setAsDefault()
{
	if (!mTechniques.empty() && !mTechniques[0]->mPasses.empty() && mTechniques[0]->mPasses[0]->mPrograms.size() >= 2)
	{
		FFxProgram::sDefaultVertexProgram = mTechniques[0]->mPasses[0]->mPrograms[0]->mProgram;
		FFxProgram::sDefaultFragmentProgram = mTechniques[0]->mPasses[0]->mPrograms[1]->mProgram;
	}
}


void D3D9SetPrograms(ZFx* pAEffect)
{
	ZFx *pEffect = (ZFx *)pAEffect;
	FFxPass* pPass;
	if (pEffect->mbHWIEnable)
	{
		ZASSERT(pEffect->mCurTechnique->mHWITechnic, "Shader set using HWI but no good technic found.");
		pPass = pEffect->mCurTechnique->mHWITechnic->mPasses[0];
	}
	else
		pPass = pEffect->mCurTechnique->mPasses[0];

	for (unsigned int i=0;i<pPass->mPrograms.size();i++)
	{
		pPass->mPrograms[i]->bind();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
