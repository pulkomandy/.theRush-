///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTexture.h
// Creation : 23/06/2007
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
//#include "../libplatform/libplatform.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"


#undef FAR
//#undef INT32
#define XMD_H

#if 0
extern "C"
{
#include "jpeglib.h"
#ifdef USE_PNG
#include "png.h"
#endif
}
#endif

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif


ZTexture* DirectLoadTex(const char *szName)
{
	/*
	ZTexture* nnt = (ZTexture*)FindInstance(ZTexture, szName);
	if (nnt) return nnt;
	*/
	ZBaseClass *pClass = mGLoaderService->CreateInstance(szName);
	if (pClass)
		return (ZTexture*)pClass->QueryInstance();
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_SERIALIZATION(ZTexture)
END_SERIALIZATION()



///////////////////////////////////////////////////////////////////////////////////////////////////
    ZTexture *ZTexture::mFrameTexturesToUpdate[2048];
    int ZTexture::mFrameTexturesToUpdateIndex = 0;

void ClearUpdateTexturesList()
{
	ZTexture::mFrameTexturesToUpdateIndex = 0;
}

void UpdateAllNeededTextures()
{
	PROFILER_START(UpdateVideoTextures);
    for (int i=0;i<ZTexture::mFrameTexturesToUpdateIndex;i++)
    {
		if (ZTexture::mFrameTexturesToUpdate[i])
		{
			if (ZTexture::mFrameTexturesToUpdate[i]->CanDecode())
			{
				float alstfrmt = ZTexture::mFrameTexturesToUpdate[i]->GetLastFrameTime();
				if ( alstfrmt > 0.04f)
				{
					ZTexture::mFrameTexturesToUpdate[i]->Update();
					ZTexture::mFrameTexturesToUpdate[i]->SetLastFrameTime(alstfrmt - 0.04f);
				}
				else
				{
					/*
					if (GClientGame)
						ZTexture::mFrameTexturesToUpdate[i]->SetLastFrameTime(alstfrmt + gTimer.GetEllapsed() * GClientGame->GetGameSpeed());
					else
					*/
						ZTexture::mFrameTexturesToUpdate[i]->SetLastFrameTime(alstfrmt + gTimer.GetEllapsed());
				}
			}
			ZTexture::mFrameTexturesToUpdate[i]->SetFrameUpdate(false);
		}
    }

    ZTexture::mFrameTexturesToUpdateIndex = 0;
	PROFILER_END();
}
