///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFxProgram.cpp
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
#include "libbase/ZLogger.h"
#include "libbase/ZFile.h"
#include <cassert>

#include <Cg/cg.h>
#ifdef WIN32
#include <Cg/cgD3D9.h>
#endif
#include <Cg/cgGL.h>

static void checkForCgError(const char *situation)
{
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR) {
		printf("CGERR: %s: %s\n",
				situation, string);
		if (error == CG_COMPILER_ERROR) {
//			printf("%s\n", cgGetLastListing(prof));
			printf("compile error\n");
		}
	}
}

//---------------------------------------------------------------------------------
// Static data FFxProgram
//---------------------------------------------------------------------------------

extern CGprofile gCGVertexProfile;
extern CGprofile gCGPixelProfile;

CGprogram FFxProgram::sDefaultVertexProgram = 0;
CGprogram FFxProgram::sDefaultFragmentProgram = 0;

const char* COMPILE_ARGS[] =
{
	"-DCM=100.0",
	0
};


//---------------------------------------------------------------------------------
// FFxProgram
//---------------------------------------------------------------------------------
FFxProgram::FFxProgram() :
mProgramFlags(0),
mProgram(0),
mSourceBuffer(0),
mProfileDomain (CG_VERTEX_DOMAIN),
mProfile((CGprofile) 0)
{
}

FFxProgram::FFxProgram(CGdomain aProfileDomain, const char* aFileName, const char* aEntry, const char** aArgs) :
mProgramFlags(0),
mProgram(0),
mSourceBuffer(0),
mProfileDomain(aProfileDomain)
{
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
		mProfile = (aProfileDomain == CG_VERTEX_DOMAIN) ?cgD3D9GetLatestVertexProfile():cgD3D9GetLatestPixelProfile();
	else
#endif
	{
		mProfile = (aProfileDomain == CG_VERTEX_DOMAIN) ?cgGLGetLatestProfile(CG_GL_VERTEX):cgGLGetLatestProfile(CG_GL_FRAGMENT );

		cgGLSetOptimalOptions(mProfile);
		checkForCgError("setprofile");
	}

	mFileName = aFileName;
	mEntry = aEntry;
    // FIXME mArgs
	//mArgs = aArgs;
}

//---------------------------------------------------------------------------------
// ~FFxProgram
//---------------------------------------------------------------------------------
FFxProgram::~FFxProgram()
{
	if (mProgram /*&& !mProgramFlags.flags(FFX_PROGRAM_ERROR)*/)
	{
		cgDestroyProgram(mProgram);
		/*
		if (mSourceBuffer)
			delete [] mSourceBuffer;
			*/
	}
}

//---------------------------------------------------------------------------------
// addParamConnection
//---------------------------------------------------------------------------------

void FFxProgram::addParamConnection(const tstring& aFrom, const tstring& aTo)
{
	mConnections.push_back(FFxParamConnection(aFrom, aTo));
}


//---------------------------------------------------------------------------------
// setDefaultProgram
//---------------------------------------------------------------------------------
void FFxProgram::useDefaultProgram(CGcontext aContext)
{
//	mProgramFlags.setf(FFX_PROGRAM_ERROR);
	if (mProfileDomain == CG_VERTEX_DOMAIN)
	{
		mProgram = sDefaultVertexProgram;
	}
	else
	{
		mProgram = sDefaultFragmentProgram;
	}
}

//---------------------------------------------------------------------------------
// resolve includes
// to avoid accessing the run time compiler calling filesystem functions (not supported on debug)
// we must manually replace the includes directives by the file referenced.
// this is weird but I can't see other alternative except to load precompile the shaders offline.
//---------------------------------------------------------------------------------
char* resolveIncludes(char * src)
{
	const char* pInclude;
	int srcSize = (int)strlen(src);
	int offset = 0;
	while ((pInclude = strstr(src+offset, "#include")))
	{
		char* fstart;
		char* fend;
		if ((fstart = strchr((char*)pInclude, '\"')) && (fend = strchr(fstart+1, '\"')) )
		{
			chdir("ZenithDatas");
			int includePos = int(pInclude - src);
			tstring	fname(fstart+1, (fend-fstart-1));
			ZFile file;
            if (!file.Open(fname.c_str()))
			{
				LOG("can't open include file '%s'\n", fname.c_str());
				break;
			}
            int includeSize = file.GetSize();
			int newSize = srcSize + includeSize - (fend - pInclude);

			char* newSrc = (char*) new char [newSize];
			memcpy(newSrc, src, includePos);
			file.Read(newSrc + includePos, includeSize);
			memcpy(newSrc + includePos + includeSize, fend+1, newSize - (includePos + includeSize));
			newSrc[newSize-1] = 0;
			delete [] src;

			src = newSrc;
			srcSize = newSize;
			offset = includePos+1;
			chdir("..");
		}

	}
	return src;
}

//---------------------------------------------------------------------------------
// init
// this function load and compile a vertex/fragment program on the specified context
// if the source file is not found, it assigns a default GREEN color shader
// if the compile failed, it assigns a default RED color shader
//---------------------------------------------------------------------------------
bool FFxProgram::init(CGcontext aContext)
{
	CGenum	fileType;

	// remove existing program (in case a program is already loaded)

	if (mProgram)
	{
		cgDestroyProgram(mProgram);
		mProgram = 0;
	}
	fileType = CG_SOURCE;

#ifdef WIN32
	HRESULT hr;

	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
	{
		const char** profileOpts;
		profileOpts = cgD3D9GetOptimalOptions(mProfile);
		const char *nOpts[16];
		nOpts[0] = COMPILE_ARGS[0];
		int idx =1;
		while ( (idx<15) && profileOpts[idx-1])
		{
			nOpts[idx] = profileOpts[idx-1];
			idx++;
		}
		nOpts[idx] = 0;

		const char *szDXprofile = NULL;

		if (mProfile == CG_PROFILE_VS_2_0)
		{
			szDXprofile = D3DXGetVertexShaderProfile(GDD->GetD3D9Device());
		}
		else
		{
			szDXprofile = D3DXGetPixelShaderProfile(GDD->GetD3D9Device());
		}
/*

		//LPD3DXBUFFER mDXShader;
		D3DXCompileShader(
			mSourceBuffer,
			strlen(mSourceBuffer),
			NULL,
			NULL,
			mEntry.c_str(),
			szDXprofile,
			0,
			&mDXShader,
			NULL,
			NULL
			);
		*/



		mProgram = cgCreateProgram(aContext, fileType, mSourceBuffer, mProfile, mEntry.c_str(), nOpts);
		hr = cgD3D9LoadProgram(mProgram, true, 0);
	}
	else
#endif
	{
		// opengl
		//mProgram = cgCreateProgram(aContext, fileType, mSourceBuffer, mProfile, mEntry.c_str(), COMPILE_ARGS);
		mProfile = (mProfileDomain == CG_VERTEX_DOMAIN) ?cgGLGetLatestProfile(CG_GL_VERTEX):cgGLGetLatestProfile(CG_GL_FRAGMENT );
		assert(cgGLIsProfileSupported(mProfile));
		mProgram = cgCreateProgramFromFile(aContext, fileType, mFileName, mProfile, mEntry.c_str(), COMPILE_ARGS);
		if (mProgram == NULL) {
			CGerror error;
			const char* errTxt = cgGetLastErrorString(&error);
			LOG ("ERROR %x: can't compile %s : %s\n", error, mFileName.c_str(), errTxt);
			useDefaultProgram(aContext);
			assert(false);
			return false;
		}
		cgGLLoadProgram(mProgram);
		checkForCgError("loadprog");
	}
	if (mProgram == 0)
	{
		CGerror error = cgGetError();
		LOG ("ERROR %x: can't load or compile %s : %s\n", error, mFileName.c_str(), cgGetLastErrorString(&error));
		useDefaultProgram(aContext);
		assert(false);
		return false;
	}
	return true;
}



//---------------------------------------------------------------------------------
// bind vertex/fragment program
// PSGL specific
//---------------------------------------------------------------------------------
void FFxProgram::bind()
{
	if (!mProgram)
		return;
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
	{
		HRESULT hr = cgD3D9BindProgram(mProgram);
		/*
		if (mProfile == CG_PROFILE_VS_2_0)
		{
			HRESULT hr = GDD->GetD3D9Device()->SetVertexShader((IDirect3DVertexShader9*)mDXShader->GetBufferPointer());
			int a = 1;
		}
		else
		{
			GDD->GetD3D9Device()->SetPixelShader((IDirect3DPixelShader9*)mDXShader->GetBufferPointer());
		}
		*/
	}
	else
#endif
	{
		cgGLEnableProfile(mProfile);
		cgGLBindProgram(mProgram);
	}
}

//---------------------------------------------------------------------------------
// unbind vertex/fragment program and restore OpenGL/ES fixed pipeline
// PSGL specific
//---------------------------------------------------------------------------------

void FFxProgram::unbind()
{
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
	{
	}
	else
#endif
	{
	cgGLDisableProfile(mProfile);
	cgGLUnbindProgram(mProfile);
	}
	/*
#ifdef WIN32
    extern LPDIRECT3DDEVICE9	gD3DDevice;
    gD3DDevice->SetVertexShader(NULL);
    gD3DDevice->SetPixelShader(NULL);
#else
	cgGLDisableProfile(mProfile);
	cgGLUnbindProgram(mProfile);
#endif
	*/
}
