///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZRenderTargetOGL.h
// Creation : 24/09/2007
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

#ifndef ZRENDERTARGETOGL_H__
#define ZRENDERTARGETOGL_H__

#include "ZTextureOGL.h"

class ZRenderTargetOGL :  public virtual ZTextureOGL , public IRenderTarget
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRenderTargetOGL() 
	{
		mSizeX = mSizeY = 0;
	}

	virtual ~ZRenderTargetOGL() 
	{
	}

    virtual void BindAsTarget(int level)
	{
		
	}
	virtual void UnBindAsTarget(int level)
	{

	}

    //virtual void BindAsTexture(int level, bool wantZTexture = false)
    virtual void Bind(uint8 aLevel) 
	{
	}


	virtual void Save(const char *szFile)
	{
	}

	virtual void* GetAPITexture(bool wantZTexture = false) { return 0; }

	virtual void Resize(int sizex, int sizey)
	{
	}

	virtual void Init(int sizeX, int sizeY, RT_FORMAT format, WindowHandle_t aAdditionalSwapchain) {}

	virtual void Present(RECT *pSrcRect = NULL)
	{
	}

	virtual void DrawFSQuad()
	{
	}

	virtual void DrawFSQuad(int texWidth, int texHeight, float tu, float tv)
	{
	}

	virtual void DrawFSQuadAO()
	{
	}

	virtual ZTexture * GetTexture()
	{
	    return NULL;
	}
	virtual uint32 GetWidth() { return mSizeX; }
	virtual uint32 GetHeight() { return mSizeY; }

	virtual void StretchCopyTo(IRenderTarget *pDest, RECT *pSrcRect = NULL, RECT *pDstRect = NULL)
	{
	}

	virtual void GetDominantColor(tcolor& dstColor) { }

	virtual ZTexture *BuildCopy()
	{
			return NULL;
	}
    virtual tulong GetMemoryUsed() const { return sizeof(ZRenderTargetOGL);}
	virtual void Reset() 
	{
	}
protected:
	int mSizeX, mSizeY;

};

#endif
