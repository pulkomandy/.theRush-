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

#include "stdafx.h"
#include "../LibBase/LibBase.h"
#include "../LibBase/ZBaseMaths.h"
#include "ZRenderTargetDX9.h"

extern D3DPRESENT_PARAMETERS	d3dpp; 
extern D3DFORMAT				mBackBufferFormatDX9, mBackBufferZStencilFormatDX9;

void ZRenderTargetDX9::Init(int sizeX, int sizeY, RT_FORMAT format, HWND aAdditionalSwapchain)
{
	extern D3DFORMAT mBackBufferFormatDX9, mBackBufferZStencilFormatDX9;
	mASwapChain = aAdditionalSwapchain;
	mSizeX = sizeX;
	mSizeY = sizeY;

	if (aAdditionalSwapchain)
	{
#if 0
		extern D3DPRESENT_PARAMETERS d3dpp; 

		d3dpp.BackBufferWidth                   = sizeX;
		d3dpp.BackBufferHeight                  = sizeY;
		d3dpp.hDeviceWindow = aAdditionalSwapchain;
		d3dpp.PresentationInterval				= 0;
		d3dpp.BackBufferCount = 1;
		d3dpp.SwapEffect  = D3DSWAPEFFECT_COPY;

		GD3D9Device->CreateAdditionalSwapChain( &d3dpp, &mpSwpChain);
#endif
	}
	switch (format)
	{
	case RT_RGBA8:
		{
			GD3D9Device->CreateTexture(sizeX, sizeY, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mTexture, NULL );
		}
		break;
	case RT_R16G16B16A16F:
		GD3D9Device->CreateTexture(sizeX, sizeY, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mTexture, NULL );
		break;
	case RT_R32F:
		GD3D9Device->CreateTexture(sizeX, sizeY, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &mTexture, NULL );
		break;
	case RT_ZBUFFER:
		if (mZBuffer)
			mZBuffer->Release();
		GD3D9Device->CreateDepthStencilSurface( sizeX, sizeY, mBackBufferZStencilFormatDX9, D3DMULTISAMPLE_NONE, 0, false, &mZBuffer, NULL);
		break;

	case RT_SHADOWMAP:
		if( GDD->SupportsDF24() )
		{
			// Here we create the Depth Stencil Texture (DST). We will bind the surface of this texture
			// to the IDirect3DDevice9's Depth/Stencil Surface for rendering our scene's depth
			// information. Afterwards, we bind this texture (shadow map) to a sampler for looking up
			// depth info in a subsequent pass.
			GD3D9Device->CreateTexture( sizeX, sizeY, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)(MAKEFOURCC('D','F','2','4')), D3DPOOL_DEFAULT, &mZTexture, NULL );
			mZTexture->GetSurfaceLevel( 0, &mZBuffer );

			// Now we create an offscreen render target that matches the multisample type of our DST.
			// Here we create a really small color buffer to conserve space. If the debug runtime is
			// detected we create a full size color buffer though to be friendly with its validation
			// code.
			// Since our backbuffer is 4xAA by default (framework settings), we must create a non-MSAA
			// color buffer to cobind with our DST.
			GD3D9Device->CreateRenderTarget( 1, 1, D3DFMT_R5G6B5, D3DMULTISAMPLE_NONE, 0, FALSE, &mSurface, NULL );
		}
		else
		{
			GD3D9Device->CreateTexture( sizeX, sizeY, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &mTexture, NULL );
			GD3D9Device->CreateTexture( sizeX, sizeY, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24S8, D3DPOOL_DEFAULT, &mZTexture, NULL );
			mZTexture->GetSurfaceLevel(0, &mZBuffer);
		}
		break;
	}

}


void ZRenderTargetDX9::DrawFSQuad()
{
	SCREEN_VERTEX mVertex[4];
	static const DWORD SCREEN_VERTEX_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;


	FLOAT fWidth5 = (FLOAT)mSizeX - 0.5f;
	FLOAT fHeight5 = (FLOAT)mSizeY - 0.5f;

	FLOAT fTexWidth1 = (FLOAT)1;
	FLOAT fTexHeight1 = (FLOAT)1;

	// Fill in the vertex values
	mVertex[0].pos = D3DXVECTOR4(fWidth5, -0.5f, 0.0f, 1.0f);
	mVertex[0].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[0].tex1 = D3DXVECTOR2(fTexWidth1, 0.0f);

	mVertex[1].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.0f, 1.0f);
	mVertex[1].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[1].tex1 = D3DXVECTOR2(fTexWidth1, fTexHeight1);

	mVertex[2].pos = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	mVertex[2].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[2].tex1 = D3DXVECTOR2(0.0f, 0.0f);

	mVertex[3].pos = D3DXVECTOR4(-0.5f, fHeight5, 0.0f, 1.0f);
	mVertex[3].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[3].tex1 = D3DXVECTOR2(0.0f, fTexHeight1);


	GD3D9Device->SetFVF(SCREEN_VERTEX_FVF) ;
	GD3D9Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertex, sizeof(SCREEN_VERTEX)) ;
}

void ZRenderTargetDX9::DrawFSQuad(int texWidth, int texHeight, float tu, float tv)
{
	SCREEN_VERTEX mVertex[4];
	static const DWORD SCREEN_VERTEX_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;


	FLOAT fWidth5 = (FLOAT)texWidth - 0.5f;
	FLOAT fHeight5 = (FLOAT)texHeight - 0.5f;

	FLOAT fTexWidth1 = (FLOAT)1;
	FLOAT fTexHeight1 = (FLOAT)1;

	// Fill in the vertex values
	mVertex[0].pos = D3DXVECTOR4(fWidth5, -0.5f, 0.0f, 1.0f);
	mVertex[0].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[0].tex1 = D3DXVECTOR2(tu, 0.0f);

	mVertex[1].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.0f, 1.0f);
	mVertex[1].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[1].tex1 = D3DXVECTOR2(tu, tv);

	mVertex[2].pos = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	mVertex[2].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[2].tex1 = D3DXVECTOR2(0.0f, 0.0f);

	mVertex[3].pos = D3DXVECTOR4(-0.5f, fHeight5, 0.0f, 1.0f);
	mVertex[3].clr = 0xFFFFFFFF;//D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
	mVertex[3].tex1 = D3DXVECTOR2(0.0f, tv);


	GD3D9Device->SetFVF(SCREEN_VERTEX_FVF) ;
	GD3D9Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertex, sizeof(SCREEN_VERTEX)) ;
}

void ZRenderTargetDX9::DrawFSQuadAO()
{
	SCREEN_VERTEXAO mVertex[4];
	static const DWORD SCREEN_VERTEX_AO_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEX2 ;


	tvector4 screenDirs[5] = {
		/*tvector3( 0, 0, 1),*/
		vector4(-1,-1, 1, 0),
		vector4(-1, 1, 1, 0),
		vector4( 1,-1, 1, 0),
		vector4( 1, 1, 1, 0) };
		tmatrix invProj;
		invProj.Inverse(GDD->GetProjectionMatrix());
		for(int dash = 0; dash<4; dash++)
		{
			screenDirs[dash].Transform(invProj);
			//screenDirs[dash].Normalize();
		}


		FLOAT fWidth5 = (FLOAT)mSizeX - 0.5f;
		FLOAT fHeight5 = (FLOAT)mSizeY - 0.5f;

		FLOAT fTexWidth1 = (FLOAT)1;
		FLOAT fTexHeight1 = (FLOAT)1;

		// Fill in the vertex values
		mVertex[0].pos = D3DXVECTOR4(fWidth5, -0.5f, 0.0f, 1.0f);
		mVertex[0].screenvxy = D3DXVECTOR2(1, 0);
		mVertex[0].screenvzw = D3DXVECTOR2(40, 0);


		mVertex[1].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.0f, 1.0f);
		mVertex[1].screenvxy = D3DXVECTOR2(1, 1);
		mVertex[1].screenvzw = D3DXVECTOR2(40, 40);


		mVertex[2].pos = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
		mVertex[2].screenvxy = D3DXVECTOR2(0, 0);
		mVertex[2].screenvzw = D3DXVECTOR2(0, 0);


		mVertex[3].pos = D3DXVECTOR4(-0.5f, fHeight5, 0.0f, 1.0f);
		mVertex[3].screenvxy = D3DXVECTOR2(0, 1);
		mVertex[3].screenvzw = D3DXVECTOR2(0, 40);



		GD3D9Device->SetFVF(SCREEN_VERTEX_AO_FVF) ;
		GD3D9Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertex, sizeof(SCREEN_VERTEXAO)) ;
}



void ZRenderTargetDX9::GetDominantColor(tcolor& dstColor)
{
	if (mTexture)
	{
		IDirect3DTexture9 *pDominantTex = NULL;
		GD3D9Device->CreateTexture(mSizeX, mSizeY,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM , &pDominantTex, NULL);

		IDirect3DSurface9 *mSurfaceSrc, *mSurfaceDst;
		mTexture->GetSurfaceLevel(0, &mSurfaceSrc);
		pDominantTex->GetSurfaceLevel(0, &mSurfaceDst);
		/*
		HRESULT hr = GD3D9Device->StretchRect(mSurfaceSrc, 
		NULL, 
		mSurfaceDst, 
		NULL, 
		D3DTEXF_LINEAR);
		*/
		HRESULT hr = GD3D9Device->GetRenderTargetData(
			mSurfaceSrc,
			mSurfaceDst
			);
		mSurfaceSrc->Release();
		mSurfaceDst->Release();


		D3DLOCKED_RECT LockedRect;
		hr = pDominantTex->LockRect(0, &LockedRect, NULL, D3DLOCK_READONLY);

		unsigned int red = 0,green = 0, blue = 0;
		unsigned char *ptr = (unsigned char *)LockedRect.pBits;
		for (int y=0;y<mSizeY;y++)
		{
			for (int x=0;x<mSizeX;x++)
			{
				red += ptr[2];
				green += ptr[1];
				blue += ptr[0];

				ptr += 4;

			}
		}
		int count = mSizeY*mSizeX;
		red /= count;
		green /= count;
		blue /= count;

		dstColor.a = 1;
		dstColor.r = red*(1.f/255.f);
		dstColor.g = green*(1.f/255.f);
		dstColor.b = blue*(1.f/255.f);



		pDominantTex->UnlockRect(0);
		pDominantTex->Release();


	}
}


BEGIN_SERIALIZATION(ZRenderTargetDX9)
END_SERIALIZATION()
