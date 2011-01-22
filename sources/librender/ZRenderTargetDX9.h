///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZRenderTargetDX9.h
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

#ifndef ZRENDERTARGETDX9_H__
#define ZRENDERTARGETDX9_H__

#include "ZTextureDX9.h"

extern IDirect3DDevice9 *GD3D9Device;

class ZRenderTargetDX9 :  public virtual ZTextureDX9 , public IRenderTarget
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRenderTargetDX9() 
	{
		mpSwpChain = NULL;
		mSurface = NULL;
		mZBuffer = NULL;
		mTexture = NULL;
		mZTexture = NULL;
		mOldZ = NULL;
		mOldDB = NULL;

		mASwapChain = NULL;
		mSizeX = mSizeY = 0;
		mZTex = NULL;
	}

	virtual ~ZRenderTargetDX9() 
	{
		if (mpSwpChain) mpSwpChain->Release();
		if (mSurface) mSurface->Release();
		if (mTexture) mTexture->Release();
		if (mZBuffer) mZBuffer->Release();
		if (mZTexture) mZTexture->Release();
	}

    virtual void BindAsTarget(int level)
	{
		IDirect3DDevice9 * pdev = GD3D9Device;
		LPDIRECT3DSURFACE9 pSurf;

		pSurf = mSurface;

		if( mSurface == NULL )
		{
			if( mTexture )
			{
				mTexture->GetSurfaceLevel(0, &pSurf);
			}

			if( mpSwpChain )
			{
				mpSwpChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
			}
		}

		pdev->SetRenderTarget(level, pSurf);

		if( mSurface != NULL )
		{
			pSurf->Release();
		}

        if( mZBuffer )
        {	
            pdev->SetDepthStencilSurface(mZBuffer);
			//mZBuffer->Release();
        }
		
		D3DVIEWPORT9 newViewport;
		newViewport.X = 0;
		newViewport.Y = 0;
		newViewport.Width  = mSizeX;
		newViewport.Height = mSizeY;
		newViewport.MinZ = 0.0f;
		newViewport.MaxZ = 1.0f;
		pdev->SetViewport(&newViewport);
		
	}
	virtual void UnBindAsTarget(int level)
	{
		GD3D9Device->SetRenderTarget(level, NULL);

	}

    //virtual void BindAsTexture(int level, bool wantZTexture = false)
    virtual void Bind(uint8_t aLevel) 
	{
		GD3D9Device->SetTexture(aLevel, /*wantZTexture ? mZTexture :*/ mTexture);
		//D3DXSaveTextureToFile("c:\\temp\\frnt1.dds", D3DXIFF_DDS , mTexture, 0);
	}


	virtual void Save(const char *szFile)
	{
		if (mTexture)
			D3DXSaveTextureToFileA(szFile, D3DXIFF_DDS , mTexture, 0);
		if (mSurface)
			D3DXSaveSurfaceToFileA(szFile, D3DXIFF_DDS , mSurface, NULL, 0);
	}

	virtual void* GetAPITexture(bool wantZTexture = false) { return wantZTexture ? mZTexture : mTexture; }

	virtual void Resize(int sizex, int sizey)
	{
		if (mpSwpChain) { mpSwpChain->Release(); mpSwpChain = NULL; }
		if (mTexture) { mTexture->Release(); mTexture = NULL; }
		//if (mSurface) { mSurface->Release(); mSurface = NULL; }
		if (mZBuffer) { mZBuffer->Release(); mZBuffer = NULL; }
	
		Init(sizex, sizey, mFormat, mASwapChain);
	}

	virtual void Init(int sizeX, int sizeY, RT_FORMAT format, WindowHandle_t aAdditionalSwapchain);

	virtual void Present(RECT *pSrcRect = NULL)
	{
		if (mpSwpChain)
		{
			mpSwpChain->Present(pSrcRect, NULL, mASwapChain, NULL, 0 );
		}
	}

	virtual void DrawFSQuad();

	virtual void DrawFSQuad(int texWidth, int texHeight, float tu, float tv);

	virtual void DrawFSQuadAO();

	virtual ZTexture * GetTexture()
	{
		if (!mZTex)
		{
			mZTex.Attach( GDD->NewTexture());
			mZTex->ForceTextureHandle(mTexture);
			mZTex->SetName("RTT texture");
		}

		return mZTex;
	}
	virtual uint32_t GetWidth() { return mSizeX; }
	virtual uint32_t GetHeight() { return mSizeY; }

	virtual void StretchCopyTo(IRenderTarget *pDest, RECT *pSrcRect = NULL, RECT *pDstRect = NULL)
	{
		if (mTexture)
		{
			IDirect3DSurface9 *mSurfaceSrc, *mSurfaceDst;
			mTexture->GetSurfaceLevel(0, &mSurfaceSrc);
			((ZRenderTargetDX9*)pDest)->mTexture->GetSurfaceLevel(0, &mSurfaceDst);
			HRESULT hr = GD3D9Device->StretchRect(mSurfaceSrc, 
				NULL, 
				mSurfaceDst, 
				NULL, 
				D3DTEXF_LINEAR);
			mSurfaceSrc->Release();
			mSurfaceDst->Release();

		}
	}

	virtual void GetDominantColor(tcolor& dstColor);

	virtual ZTexture *BuildCopy()
	{
		
		{

			return NULL;
		}
	}
	IDirect3DSwapChain9 *mpSwpChain;
	IDirect3DTexture9 * mTexture;
	IDirect3DTexture9 * mZTexture;
	IDirect3DSurface9 *mZBuffer;
	IDirect3DSurface9 *mSurface;

	IDirect3DSurface9 *mOldZ;
	IDirect3DSurface9 *mOldDB;

	HWND mASwapChain;
	RT_FORMAT mFormat;

    virtual tulong GetMemoryUsed() const { return sizeof(ZRenderTargetDX9);}
	virtual void Reset() 
	{
	}
protected:
	int mSizeX, mSizeY;

	struct SCREEN_VERTEX 
	{
		D3DXVECTOR4 pos;
		DWORD       clr;
		D3DXVECTOR2 tex1;
	};

	struct SCREEN_VERTEXAO
	{
		D3DXVECTOR4 pos;
		D3DXVECTOR2 screenvxy;
		D3DXVECTOR2 screenvzw;
	};
	smartptr<ZTexture> mZTex;

};
#endif
