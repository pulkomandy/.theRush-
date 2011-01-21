///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFXImport.h
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
#include "FFxImport.h"
#include "ZFx.h"
#include "libbase/ZLogger.h"
#include "libbase/ZFile.h"
#ifdef WIN32
#else
#define _stricmp strcasecmp
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

FFxImport::FFxImport()
{
}

FFxImport::~FFxImport()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

CGerror checkCgError()
{
	CGerror error = cgGetError();

	if (error != CG_NO_ERROR)
	{
		const char* report = cgGetErrorString(error);
		ERR("CG Error Detected:  %s\n", report);
		//FASSERT(0 && report);
	}

	return error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// importCgStateAssignment

FFxProgram* FFxImport::importCgProgram(const char* aFileName, CGprogram program)
{
	//CHECKTHREAD;

	// import program source information
	FFxProgram*	fxProgram = new FFxProgram();
	fxProgram->mProfile = cgGetProgramProfile(program);
	fxProgram->mProfileDomain = cgGetProfileDomain(fxProgram->mProfile);
	fxProgram->mFileName = aFileName;
	fxProgram->mSourceBuffer = cgGetProgramString(program, CG_PROGRAM_SOURCE );
	fxProgram->mEntry = cgGetProgramString(program, CG_PROGRAM_ENTRY);

	// import connected parameters
	CGparameter	parameter;
	for (parameter = cgGetFirstParameter(program, CG_PROGRAM);
		parameter;
		parameter = cgGetNextParameter(parameter))
	{
		if (cgGetParameterVariability(parameter) == CG_UNIFORM
			&& cgIsParameterReferenced(parameter))
		{
			CGparameter from = cgGetConnectedParameter(parameter);
			if (from)
			{
				LOG("param (%x)%s is connected from program param (%x)%s \n",
					parameter,
					cgGetParameterName(parameter),
					from,
					cgGetParameterName(from));

				// add parameter connection
				fxProgram->addParamConnection(cgGetParameterName(from), cgGetParameterName(parameter));
			}
			else
			{
				LOG("param (%x)%s is not connected ! \n",
					parameter,
					cgGetParameterName(parameter));
			}
		}
	}
	return fxProgram;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// importCgStateAssignment

void FFxImport::importCgStateAssignment(ZFx* fx, FFxPass *pPass, CGstateassignment sa, CGbool unabridged)
{

	CGstate state = cgGetStateAssignmentState(sa);
	//const float *fvalues;
	const int *ivalues;
	const CGbool *bvalues;
	const char *szName = cgGetStateName(state);
	int numValues;
	//FFxState*	setting = NULL;

	if (!_stricmp(szName,"cullmode"))
	{
		ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
		pPass->mRenderingState.mCulling = ivalues[0];
	}
	else if (!_stricmp(szName,"BlendEquation"))
	{
		ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
		pPass->mRenderingState.mBlending = ivalues[0];
		if (pPass->mRenderingState.mBlending)
			fx->doesBlend(pPass->mRenderingState.mBlending);

	}
	else if (!_stricmp(szName,"DepthMask"))
	{
		bvalues = cgGetBoolStateAssignmentValues(sa, &numValues);
		pPass->mRenderingState.mZWriteEnable = bvalues[0]?1:0;
		fx->SetZWriteEnable(pPass->mRenderingState.mZWriteEnable);
	}
	else if (!_stricmp(szName,"ZEnable"))
	{
		bvalues = cgGetBoolStateAssignmentValues(sa, &numValues);
		pPass->mRenderingState.mZTestType = bvalues[0]?1:0;
	}
	else if (!_stricmp(szName,"alphatestenable"))
	{
		bvalues = cgGetBoolStateAssignmentValues(sa, &numValues);
		pPass->mRenderingState.mAlphaTestEnable = bvalues[0]?1:0;
	}
	else if (!_stricmp(szName, "FillMode"))
	{
		ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
		pPass->mRenderingState.mFillMode = ivalues[0];

	}
/*
    	cullmode =none;

		BlendEnable = true;
		BlendEquation = int(2);
		DepthMask = false;  //z write
		ZEnable = true;
*/

	return;
#if 0
	CGtype type = cgGetStateType(state);
	switch (type)
	{
	case CG_FLOAT:
	case CG_FLOAT1:
	case CG_FLOAT2:
	case CG_FLOAT3:
	case CG_FLOAT4:
		//fvalues = cgGetFloatStateAssignmentValues(sa, &numValues);
		break;
	case CG_INT:
	case CG_INT1:
	case CG_INT2:
	case CG_INT3:
	case CG_INT4:

		ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
//		setting = new FFxGLEnableState(fx, cgGetStateName(state), 0, (int*)ivalues);//bvalues[0]);

		break;
	case CG_BOOL:
	case CG_BOOL1:
	case CG_BOOL2:
	case CG_BOOL3:
	case CG_BOOL4:

        {
		    bvalues = cgGetBoolStateAssignmentValues(sa, &numValues);
			//FFxGLEnableState *rndState = new FFxGLEnableState(fx, cgGetStateName(state), 0, bvalues[0]?true:false);
            //setting = rndState;
        }

		break;
	case CG_STRING:
		LOG("          State Assignment Value: %s\n", cgGetStringStateAssignmentValue(sa));
		break;
	case CG_TEXTURE:
		//LOG("          State Assignment Value: 0x%p\n", cgGetTextureStateAssignmentValue(sa));
		break;
	case CG_SAMPLER1D:
	case CG_SAMPLER2D:
	case CG_SAMPLER3D:
	case CG_SAMPLERCUBE:
	case CG_SAMPLERRECT:
		//LOG("          State Assignment Value: 0x%p\n", cgGetSamplerStateAssignmentValue(sa));
		break;
	case CG_PROGRAM_TYPE:
	default:
		//LOG("UNEXPECTED CASE: 0x%x (%d)\n", type, type);
		break;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// importCgFx
// this function creates a ZFx from a cgfx file.
// it must be run on the PC platform only for 2 reasons :
// - loading of cgfx file is not supported on PS3
// - we don't want to rely on PSGL on the runtime, so we don't refer to any function
// from PSGL FX library. This bring more work but will allow us to use cgfx with GCM.

char* resolveIncludes(char * src);
bool FFxImport::importCgfx(ZFx* fx, CGcontext aContext, const char* aFileName, const char** args)
{
	CGeffect effect;

	// load a cgfx file in memory
	ZFile effectFile;
	if (effectFile.Open(aFileName))
	{
		int sz = effectFile.GetSize();
		char *buf = new char [sz+1];
		effectFile.Read(buf, sz);
		buf[sz] = 0;
		buf = resolveIncludes(buf);
#ifdef WIN32
		if (GDD->GetClassID()  == ClassIDZDisplayDeviceDX9)
		{
			effect = cgCreateEffect(aContext, buf, args);
		}
		else
#endif			
		{
			tstring nBuf = buf;
			//nBuf.Replace("ps_2_0","glslf");
			//nBuf.Replace("vs_2_0","glslv");
			nBuf.Replace("ps_2_0","arbfp1");
			nBuf.Replace("vs_2_0","arbvp1");

			//effect = cgCreateEffect(aContext, nBuf.c_str(), args);
			effect = cgCreateEffectFromFile(aContext, aFileName, args);
		}
		

		
 		if (!effect)//checkCgError())
		{
			LOG ("importCgfx ERROR: can't load or compile %s : %s\n", aFileName, cgGetLastListing(aContext));

			delete [] buf;
			// display last listing
			return false;
		}
		delete [] buf;
		return importEffect(fx, effect, aFileName);
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool FFxImport::importEffect(ZFx* fx, CGeffect effect, const char* aFileName)
{
	CGtechnique technique;
	CGpass pass;
	CGstateassignment stateassign ;
	CGparameter parameter;
	const char* name;
	bool	unabridged = true;
	if (!fx) return false;

	fx->setEffectFileName(aFileName);
	FFxTechnique* mLastfxTechnique = NULL;
	// import techniques
	for (technique = cgGetFirstTechnique(effect);
		technique;
		technique = cgGetNextTechnique(technique))
	{
		name = cgGetTechniqueName(technique);
		FFxTechnique* fxTechnique = new FFxTechnique();
		fxTechnique->mName = name;
		if (strstr(name,"HWI_"))
		{
			mLastfxTechnique->mHWITechnic = fxTechnique;
		}
		else
		{
			fx->addTechnique(fxTechnique);
		}

		// import passes
		for (pass = cgGetFirstPass(technique);
			pass;
			pass = cgGetNextPass(pass))
		{
			name = cgGetPassName(pass);
			if (name == 0) name = "";


			FFxPass* fxPass = new FFxPass();
			fxPass->mName = name;

				fxTechnique->addPass(fxPass);

			// import state assignments
			for (stateassign = cgGetFirstStateAssignment(pass);
				stateassign;
				stateassign =cgGetNextStateAssignment(stateassign))
			{
				if (cgGetStateType(cgGetStateAssignmentState(stateassign)) == CG_PROGRAM_TYPE)
				{
					// import vertex/fragment program info
					CGprogram	program = cgGetProgramStateAssignmentValue(stateassign);
					FFxProgram*	fxProgram = importCgProgram(aFileName, program);
					if (fxProgram)
						fxPass->addProgram(fxProgram);
				}
				else
				{
					// import GL setting
					importCgStateAssignment(fx, fxPass, stateassign, unabridged);
				}
			}
		}
		mLastfxTechnique = fxTechnique;
	}

	// import effect parameters
	for (parameter = cgGetFirstEffectParameter(effect);
		parameter;
		parameter = cgGetNextParameter(parameter))
	{
		// skip parameters which are not connected at all
		if (cgGetNumConnectedToParameters(parameter) == 0)
			continue;

		const char *name = cgGetParameterName(parameter);
		const char *semantic = cgGetParameterSemantic(parameter);
		//const char *typestring = cgGetTypeString(cgGetParameterNamedType(parameter));
		CGtype type = cgGetParameterType(parameter);
		//int resindex = cgGetParameterResourceIndex(parameter);

		//LOG ("effect parameter %x: %s:%s type=%s(%d) resindex=%d\n", parameter, name, semantic, typestring, type, resindex);

		switch (cgGetParameterClass(parameter))
		{
		case CG_PARAMETERCLASS_SAMPLER:
			{

				CGstateassignment sa;
				samplerparam_t smpprm;
				memset(&smpprm, 0, sizeof(samplerparam_t));
				smpprm.szSamplerName = cgGetParameterName(parameter);
				for (sa = cgGetFirstSamplerStateAssignment(parameter); sa; sa = cgGetNextStateAssignment(sa))
				{
					CGstate state = cgGetSamplerStateAssignmentState(sa);
					CGtype type = cgGetStateType(state);

					const char *stName = cgGetStateName(state);
					//const char *stType = cgGetTypeString(type);

					if ( (type == CG_INT) ||
						(type == CG_INT1) ||
						(type == CG_INT2) ||
						(type == CG_INT3) ||
						(type == CG_INT4) )
					{
						const int *ivalues = NULL;
						int numValues;
						ivalues = cgGetIntStateAssignmentValues(sa, &numValues);
						if (!stricmp(stName,"MinFilter"))
						{
							smpprm.minFilter = ivalues[0];
						}
						else if (!stricmp(stName,"MipFilter"))
						{
							smpprm.mipFilter = ivalues[0];
						}
						else if (!stricmp(stName,"MagFilter"))
						{
							smpprm.magFilter = ivalues[0];
						}
						else if (!stricmp(stName,"AddressU"))
						{
							smpprm.AddressU = ivalues[0];
						}
						else if (!stricmp(stName,"AddressV"))
						{
							smpprm.AddressV = ivalues[0];
						}
						else if (!stricmp(stName,"AddressW"))
						{
							smpprm.AddressW = ivalues[0];
						}
					}
				}
				fx->AddSamplerParameter(smpprm);

				//LOG ("WARNING: sampler ignored\n");
			}
			break;

		case CG_PARAMETERCLASS_SCALAR:
		case CG_PARAMETERCLASS_VECTOR:
		case CG_PARAMETERCLASS_MATRIX:
			{
				// read values from parameters and convert to float
				int nvalues = 0;
				float fvalues[16];
				int i = 0;
				const double* dvalues = cgGetParameterValues(parameter, CG_CURRENT, &nvalues);
				for (i=0; i<nvalues; i++)
				{
					fvalues[i] = (float) dvalues[i];
				}
				for (; i<16; i++)
				{
					fvalues[i] = (float) 0.0f;
				}

				// create new parameter
				// right now we export only data with no semantic
				if (semantic == 0 || *semantic == 0 || _stricmp(semantic, "diffuse") == 0 || _stricmp(semantic, "specular") == 0 || _stricmp(semantic, "position") == 0)
				{
					FFxData* data = FFxData::create(type, fvalues, nvalues);
					FFxParam*	param = new FFxEffectParam(name, semantic, data);
					fx->addParam(param);
				}
			}
			break;
		case CG_PARAMETERCLASS_ARRAY:
			//LOG ("ERROR: array not handled\n");
			break;
		case CG_PARAMETERCLASS_STRUCT:
			//LOG ("ERROR: struct not handled\n");
			break;
		case CG_PARAMETERCLASS_OBJECT:
			//LOG ("ERROR: object not handled\n");
			break;
		default:
			//LOG ("ERROR: unknown class\n");
			break;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////




