///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZDisplayDeviceDX9.cpp
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
#include "../libgame/libgame.h"
#include "ZD3D9Enum.h"
#include "ddsDX9.h"
#include "DxErr.h"
#include <Cg/cg.h>
#include <Cg/cgD3D9.h>
#include "../libworld/libworld.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
extern IDisplayDevice * GDD;
extern CGcontext gDefaultCGcontext;
void CGerrorCallback();

IDirect3DDevice9 *GD3D9Device;
DWORD GDX9ThreadId;
D3DPRESENT_PARAMETERS	d3dpp; 
D3DFORMAT				mBackBufferFormatDX9, mBackBufferZStencilFormatDX9;
HWND GEditorWindowHandle = 0;

extern "C" {
const char*  WINAPI DXGetErrorString9A(__in HRESULT hr)
{
	return DXGetErrorStringA(hr);
}
}

extern RenderingState_t GRenderingState;
extern samplerparam_t GSamplerState[8];

extern ZTexture *gCurrentFrameBufferTexture      ;
extern ZTexture *gCurrentMiniFrameBufferTexture  ;
extern ZTexture *gCurrentZBufferTexture          ;
extern ZTexture* m_pJitterTexture                ;
extern ZTexture* GpSMZTexture                    ;
/*
void DisplayDeviceInitBase(IDisplayDevice *device)
{
  gCurrentFrameBufferTexture = device->NewTexture();
  gCurrentMiniFrameBufferTexture = device->NewTexture();
  gCurrentZBufferTexture = device->NewTexture();
  m_pJitterTexture = device->NewTexture();
  GpSMZTexture = device->NewTexture();

	IRenderingContext* rc = device->GetContextToBuild();
  for(int i = 0; i < 8; i++ )
  {
    rc->GetShadowMapBuffer()[i].m_pSMZTexture = device->NewTexture();
  }

	rc = device->GetContextToRasterize();

  for(int i = 0; i < 8; i++ )
  {
    rc->GetShadowMapBuffer()[i].m_pSMZTexture = device->NewTexture();
  }
}
*/

#include "ZTextureDX9.h"

#include "ZRenderTargetDX9.h"
class IRenderTarget;

class ZDisplayDeviceDX9 : public IDisplayDevice
{
public:
    DECLARE_SERIALIZABLE
public:
    ZDisplayDeviceDX9()
    {
        GDD = this;
		mbSketchRendering = false;
		mQueueSwap = 0;
		mBeginSceneDone = false;
    }

    virtual ~ZDisplayDeviceDX9()
    {
		UninitShadowSystem();
		mD3DDevice->Release();
		mD3D->Release();
    }

	virtual void EnableSketchRendering(bool bEnable) { }
	virtual bool& IsSketchRenderingEnabled() { return mbSketchRendering; }
    virtual IVertexArray *  NewVertexArray() { return (IVertexArray*)_New(ZVertexArrayDX9); }
    virtual IIndexArray *   NewIndexArray() { return (IIndexArray*)_New(ZIndexArrayDX9); }
    virtual ZTexture *      NewTexture() 
	{ 
		ZTextureDX9 *nt = new ZTextureDX9;
		_AddInstance(nt);
		return nt; 
	}
	virtual IRenderTarget* NewRenderTarget()
	{
		IRenderTarget *nrtt = (IRenderTarget*)_New(ZRenderTargetDX9);

		return nrtt;

	}
	virtual IOcclusionQuery * NewOcclusionQuery() { return NULL; }
    virtual bool BeginScene();

    virtual void EndScene() 
	{
		

		if (mBeginSceneDone)
			mD3DDevice->EndScene();
		mBeginSceneDone = false;
	}

    virtual void Present(RECT *pSrcRect = NULL) 
	{
		PROFILER_START(DX9Present);
#if 0
		if (mDefaultRenderTarget)
			mDefaultRenderTarget->Present(pSrcRect);
		else
		{
#endif
			mD3DDevice->Present( NULL/*pSrcRect*/, NULL, NULL, NULL );
#if 0
		}
#endif
		for (int i=0;i<8;i++)
			mD3DDevice->SetTexture(i, NULL);

		mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		mD3DDevice->SetRenderState(D3DRS_ZENABLE , TRUE);
		mD3DDevice->SetRenderState(D3DRS_CULLMODE , D3DCULL_CCW);
		mD3DDevice->SetVertexShader(NULL);
		mD3DDevice->SetPixelShader(NULL);

		if (!mColorFrameBufferFileName.Empty())
		{
			IDirect3DSurface9 *mSurfaceSrc;
			GetD3D9Device()->GetRenderTarget( 0, &mSurfaceSrc );

			tstring fileName = mColorFrameBufferFileName;
			fileName += ".jpg";
			D3DXSaveSurfaceToFileA(fileName.c_str(),
				D3DXIFF_JPG,
				mSurfaceSrc,
				NULL,
				NULL
				);
			mSurfaceSrc->Release();
			LOG("Screen Shot saved in %s.\n", mColorFrameBufferFileName.c_str());
			mColorFrameBufferFileName = "";
		}

		PROFILER_END();
	}

	virtual void SetViewport(RECT *pRC) { }
    virtual void SetRenderingState(RenderingState_t& renderingState);
    virtual void SetSamplerState( int index, const samplerparam_t& samplerParam, ZTexture* texture );

    virtual void SetProjectionMatrix(const tmatrix & mat)
    {
        mProjectionMatrix = mat;
		mD3DDevice->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX*)mat.m16);
    }

    virtual void SetViewMatrix(const tmatrix & mat)
    {
        mViewMatrix = mat;
		mViewMatrix.Inverse();
		mD3DDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)mViewMatrix.m16);
    }

    virtual void SetWorldMatrix(const tmatrix & mat)
    {
        mWorldMatrix = mat;
		mD3DDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)mat.m16);
    }



	virtual void SetTextureTransformMatrix(const tmatrix & mat) {}
    virtual void SetPreviousWorldMatrix(const tmatrix & mat) {}
    virtual void SetPreviousViewMatrix(const tmatrix & mat) {}

	virtual const tmatrix& GetProjectionMatrix() const { return mProjectionMatrix; }
    virtual const tmatrix& GetViewMatrix() const { return mViewMatrix; }
    virtual const tmatrix& GetWorldMatrix() const { return mWorldMatrix; }
    virtual const tmatrix& GetTextureTransformMatrix() { return tmatrix::identity; }
    virtual const tmatrix& GetPreviousWorldMatrix() { return tmatrix::identity; }
    virtual const tmatrix& GetPreviousViewMatrix() { return tmatrix::identity; }

	virtual void SetScissorRect(int x, int y, int sx, int sy) {}
	virtual void EnableScissor(bool bEnable) {}


    // caps
	virtual bool SupportsDF24() { return false; }
	virtual int GetNumberMRT() { return 1; }

	virtual void SetDefaultRenderTarget(IRenderTarget *pTarget) {}

	// post process
	virtual void SetPPLuminance(float aval) {}
	virtual void SetPPMiddleGray(float aval) {}
	virtual void SetPPWhiteCutoff(float aval) {}
	virtual void SetPPfocalDist(float aval) {}
	virtual void SetPPfocalThreshold(float aval) {}
	virtual void SetPPfocalFactor(float aval) {}
	// if false, don't display 3D, only GUI layer
	virtual void Draw3DLayer(bool bDraw) {}
	virtual bool Is3DLayerEnable() { return true; }
	virtual void SetSepiaStrength(float aStrength) {}
	virtual void SetSepiaColor(const tcolor& col) {}

	// Captures
	virtual void CaptureZBuffer(const char *szZBufferTexture) {}
	virtual void CaptureColorBuffer(const char *szColorTexture) { mColorFrameBufferFileName = szColorTexture; }

	virtual void EnableSSAO(bool bEnable) {}
	virtual void SetSSAOParams(float p1, float p2, float p3, float p4) {}
	virtual const tvector4& GetSSAOParams() { return mSSAOParamsv; }

	virtual IRenderTarget * GetRttZ() { return NULL; }

	// resolutions
	virtual unsigned int GetNbPossibleResolutions() const { return int(mD3DEnum.mResolutions.size()); }
	virtual void GetResolution(int idx, int &width, int &height) const
	{
		width = mD3DEnum.mResolutions[idx].width;
		height = mD3DEnum.mResolutions[idx].height;
	}

	// for fullscreen/game, when not using swapchain
	virtual int GetWidth() { return mBackBufWidth; }
	virtual int GetHeight() { return mBackBufHeight; }

	virtual bool Create( INITPARAMS * pParam = NULL);
	virtual IDirect3DDevice9 *GetD3D9Device() const { return mD3DDevice; }

	//virtual void RenderView(ICamera *pCam, IWorld *pWorld, int SizeX, int SizeY, bool bMainView) {}

	virtual unsigned long GetMemoryUsed() const { return sizeof(ZDisplayDeviceDX9); }


	virtual void DrawIndexedPrimitives( uint8 aPrimitive, unsigned int BaseVertexIndex,
  unsigned int MinIndex,
  unsigned int NumVertices,
  unsigned int StartIndex,
  unsigned int IndexCount);

	virtual void DrawPrimitives( uint8 aPrimitive, unsigned int StartVertex,
		unsigned int  vertexCount);

	virtual float* GetPPLuminance() { return NULL; }
	virtual float* GetPPMiddleGray() { return NULL; }
	virtual float* GetPPWhiteCutoff() { return NULL; }
	virtual float* GetPPfocalDist() { return NULL; }
	virtual float* GetPPfocalThreshold() { return NULL; }
	virtual float* GetPPfocalFactor() { return NULL; }
	virtual float* GetSepiaStrength() { return &mSepiaStrength; }
	virtual tcolor* GetSepiaColor() { return &mSepiaColor; }

	virtual void DrawDeferedLights(VISIBLELIGHTS_t* mDeferedLights, int nbVisibleLights) {}
	virtual void ApplyPostProcess() {}

	virtual ZRenderQueue *GetQueueToBuild() const 
	{ 
		return (ZRenderQueue *)&mQueues[mQueueSwap&1];
	}
	virtual ZRenderQueue *GetQueueToRasterize() const 
	{ 
		return (ZRenderQueue *)&mQueues[(mQueueSwap+1)&1];
	}
	virtual void SwapQueues()
	{
		mQueueSwap++;
	}

	virtual void SetRenderingSize(int sx, int sy, int viewx, int viewy) {}

	virtual void SetClipPlane(int iplane, const tvector4& planev) {}
	virtual void EnableClipPlanes(int iPlanes) {}
	virtual void DisableClipPlanes() {}
	virtual void HandleScreenCapture() {}
	virtual IMesh* GetFSMesh() const { return NULL; }
	virtual IMesh* GetHalfFSMesh() const { return NULL; }
	virtual void ResetStatistics()
	{
		memset(&mStats, 0, sizeof(FrameStatistics_t));
	}
	virtual void FillStatisticsStruct(FrameStatistics_t& stats)
	{
		memcpy(&stats, &mStats, sizeof(FrameStatistics_t));
	}

	virtual void EnableAA(bool bEnable) {  }
	virtual bool IsAAEnabled() const { return false; }

	virtual void BuildMiniZ() { }
	virtual IIndexArray *GetParticlesIndexArray() { return NULL; }
	virtual void ApplyRandomTransition() {}
	virtual void TickTransition() {}
	virtual void Clear(unsigned int aDest, unsigned int color=0.f, float aDepth = 1.f, int aStencil = 0)
	{
		mD3DDevice->Clear( 0, NULL,
			((aDest&CLEAR_COLOR)?D3DCLEAR_TARGET:0)+
			((aDest&CLEAR_Z)?D3DCLEAR_ZBUFFER:0),
			color, aDepth, aStencil);
	}
	void OnLostDevice();
	void OnResetDevice();

	virtual int GetRenderProfile() { return mRenderProfile; }
public:
	int mRenderProfile;
	bool mbSketchRendering;
    tmatrix                 mProjectionMatrix, mViewMatrix, mWorldMatrix;
	FrameStatistics_t mStats;
	tvector4 mSSAOParamsv;

	// DX9
    LPDIRECT3D9             mD3D;
    LPDIRECT3DDEVICE9       mD3DDevice;
	bool					mBeginSceneDone;

    bool                    mHWPShader, mHWVShader, mTnL;
    uint32                  mTextureBpp;
    bool                    mTextureCompression;
    D3DCAPS9                mCaps;
    int                     mNbMRT;

    bool                    mSupportDF24;

	ZD3D9Enum               mD3DEnum;

	uint					mBackBufWidth, mBackBufHeight;

	// file capture
	tstring mColorFrameBufferFileName;
	// Render Queues
	unsigned int mQueueSwap;
	ZRenderQueue mQueues[2];

	//
	smartptr<ZTexture> mChecker;

	// postprocess
	tcolor mSepiaColor;
	float mSepiaStrength;
};


BEGIN_SERIALIZATION(ZDisplayDeviceDX9)
END_SERIALIZATION()



///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDisplayDeviceDX9::DrawIndexedPrimitives( uint8 aPrimitive, unsigned int BaseVertexIndex,
											  unsigned int MinIndex,
											  unsigned int NumVertices,
											  unsigned int StartIndex,
											  unsigned int IndexCount)
{
	HRESULT hr;
	switch(aPrimitive)
	{
	case PRIM_UNDEFINED:
		WARN("Mesh has undefined primitive type.\r\n");
		break;
	case PRIM_LINELIST:
		hr = GD3D9Device->DrawIndexedPrimitive(D3DPT_LINELIST , BaseVertexIndex, MinIndex, 
			NumVertices,StartIndex, IndexCount/2);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_LINESTRIP:
		hr = GD3D9Device->DrawIndexedPrimitive(D3DPT_LINESTRIP , BaseVertexIndex, MinIndex, 
			NumVertices,StartIndex, IndexCount-1);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_TRIANGLELIST:
		hr = GD3D9Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST , BaseVertexIndex, MinIndex, 
			NumVertices,StartIndex, IndexCount/3);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_TRIANGLESTRIP:
		hr = GD3D9Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP , BaseVertexIndex, MinIndex, 
			NumVertices,StartIndex, IndexCount-2);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_TRIANGLEFAN:
		hr = GD3D9Device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN , BaseVertexIndex, MinIndex, 
			NumVertices,StartIndex, IndexCount-2);
		mStats.mNbDrawCalls ++;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDisplayDeviceDX9::DrawPrimitives( uint8 aPrimitive, unsigned int StartVertex,  unsigned int  vertexCount)
{
	switch(aPrimitive)
	{
	case PRIM_UNDEFINED:
		WARN("Mesh has undefined primitive type.\r\n");
		break;
	case PRIM_LINELIST:
		GD3D9Device->DrawPrimitive(D3DPT_LINELIST , StartVertex, vertexCount/2);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_LINESTRIP:
		GD3D9Device->DrawPrimitive(D3DPT_LINESTRIP , StartVertex, vertexCount-1);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_TRIANGLELIST:
		GD3D9Device->DrawPrimitive(D3DPT_TRIANGLELIST , StartVertex, vertexCount/3);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_TRIANGLESTRIP:
		GD3D9Device->DrawPrimitive(D3DPT_TRIANGLESTRIP , StartVertex, vertexCount-2);
		mStats.mNbDrawCalls ++;
		break;
	case PRIM_TRIANGLEFAN:
		GD3D9Device->DrawPrimitive(D3DPT_TRIANGLEFAN , StartVertex, vertexCount-2);
		mStats.mNbDrawCalls ++;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDisplayDeviceDX9::Create( INITPARAMS * pParam)
{
	HWND aWindow;
	if (GEditorWindowHandle)
		aWindow = GEditorWindowHandle;
	else
		aWindow = GetActiveWindow();//GDeviceWindow;

	GDX9ThreadId = GetCurrentThreadId();

	INITPARAMS initParams;

	initParams.idAdapter = 0;
	initParams.refreshRate60hz = false;
	initParams.vSync = false;
	initParams.fullscreen = false;
	initParams.width = 800;
	initParams.height = 600;
	initParams.bpp = 32;
	initParams.antiAlias = false;

	if (pParam)
		memcpy(&initParams, pParam, sizeof(INITPARAMS));

	mNbMRT = 1;

	uint32 createFlags;


	LOG("Creating DisplayDeviceDX9.\r\n");



	HRESULT hr;

	// Init Direct X  
	if( (mD3D = Direct3DCreate9( D3D_SDK_VERSION )) == NULL )
	{
		ERR("Direct3DCreate9 failed.\r\n");
		return false;
	}
	mD3DEnum.Init(mD3D);

	// TnL
	if((mD3DEnum.adapters[initParams.idAdapter]->tnl))
	{
		if(mD3DEnum.adapters[initParams.idAdapter]->hwVertexShader)
		{
			createFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
			mHWVShader = true;
			LOG("HW Vertex shader supported.\r\n");
		}
		else
		{
			// Sert a rien : plus rapide en software
			//            createFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
			//            mHWVShader = false;
			createFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
			mTnL = false;
			mHWVShader = false;
			LOG("HW Vertex shader NOT supported.\r\n");
		}
		mTnL = true;
		LOG("TnL supported.\r\n");
	}
	else
	{
		createFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
		mTnL = false;
		mHWVShader = false;
		LOG("TnL NOT supported.\r\n");
	}

	// Pixel Shader
	if(mD3DEnum.adapters[initParams.idAdapter]->hwPixelShader)
	{
		LOG("HW Pixel shader supported.\r\n");
		mHWPShader = true;
	}
	else
	{
		LOG("HW Pixel shader NOT supported.\r\n");
		mHWPShader = false;
	}

	// 32 bits Texture
	if((mD3DEnum.adapters[initParams.idAdapter]->textureBpp == 32))
	{
		LOG("32Bits texture supported.\r\n");
		mTextureBpp = 32;
	}
	else
	{
		LOG("16Bits only texture supported.\r\n");
		mTextureBpp = 16;
	}

	// 32 bits Texture
	if((mD3DEnum.adapters[initParams.idAdapter]->textureCompression))
	{
		LOG("Texture compression supported.\r\n");
		mTextureCompression = true;
	}
	else
	{
		LOG("Texture compression NOT supported.\r\n");
		mTextureCompression = false;
	}

	//vSync = initParams.vSync;
	//refreshRate60hz = initParams.refreshRate60hz;
	//useDeclaration  = initParams.useDeclaration;
	//shadows             = initParams.shadows;

	// Create the rendering device
	int perfHUDadapter = -1;
	// instrumented driver check
	for (UINT Adapter=0;Adapter<mD3D->GetAdapterCount();Adapter++) 
	{ 
		D3DADAPTER_IDENTIFIER9 Identifier; 
		HRESULT Res; 
		Res = mD3D->GetAdapterIdentifier(Adapter,0,&Identifier); 
		if (strstr(Identifier.Description,"PerfHUD") != 0) 
		{ 
			perfHUDadapter = Adapter;

			break;
		} 
	} 
	// FullScreen
	if(initParams.fullscreen) 
	{
		ZD3D9VideoMode * mode;
		/*if(initParams.idMode == -1)
		{
		*/
		// Find the video mode
		if (! mD3DEnum.FindMode(&mode, 0, initParams.width, initParams.height, initParams.bpp))
		{
			//ERR("ERR: Cannot initialize device\r\nnUnknown video mode.\r\n" );
			GConfig->SetQuality("Width", 800);
			GConfig->SetQuality("Height", 600);
			mD3DEnum.FindMode(&mode, 0, 800, 600, 32);
		}
		/*
		}
		else
		{
		mode = mD3DEnum.adapters[initParams.idAdapter]->videoModes[initParams.idMode];
		}*/

		// Initialize present parameters
		memset(&d3dpp, 0, sizeof(d3dpp) );
		d3dpp.BackBufferWidth                        = mode->width;
		d3dpp.BackBufferHeight                        = mode->height;
		d3dpp.Windowed                                    = false;
		d3dpp.BackBufferFormat                        = mode->format;
		d3dpp.EnableAutoDepthStencil                = true;
		d3dpp.AutoDepthStencilFormat                = mode->formatZStencil;
		D3DFORMAT zBufferFormat                                    = mode->formatZStencil;

		mBackBufferFormatDX9 = mode->format;
		mBackBufferZStencilFormatDX9 = mode->formatZStencil;

		// Triple Buffering ?
		/*
		if(initParams.tripleBuffer)
		{
		d3dpp.BackBufferCount = 2;
		}
		else
		*/
		{
			d3dpp.BackBufferCount = 1;
		}

		// Anti Aliasing ?
		if((initParams.antiAlias) && (mode->antiAlias))
		{
			d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
			d3dpp.SwapEffect         = D3DSWAPEFFECT_DISCARD;
		}
		else
		{
			d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
			d3dpp.SwapEffect         = D3DSWAPEFFECT_FLIP;
		}

		// Vertical Synchro ?
		if(initParams.vSync)
		{
			d3dpp.PresentationInterval    = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dpp.PresentationInterval    = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		if((mode->refreshRate == 0) || initParams.refreshRate60hz)
		{
			d3dpp.FullScreen_RefreshRateInHz            = D3DPRESENT_RATE_DEFAULT;
		}
		else
		{
			d3dpp.FullScreen_RefreshRateInHz            = mode->refreshRate;
		}
		//d3dpp.PresentationInterval = 0;

		d3dpp.hDeviceWindow = aWindow;
/*
		D3DPRESENT_PARAMETERS d3dpp2=					// d3dpp Tells Windows How We Want Things To Be
		{
			1280,										// Back Buffer Width
			720,										// Back Buffer Height
			D3DFMT_R5G6B5,								// Back Buffer Format (Color Depth)
			1,											// Back Buffer Count (Double Buffer)
			D3DMULTISAMPLE_NONE,						// No Multi Sample Type
			0,											// No Multi Sample Quality
			D3DSWAPEFFECT_DISCARD,						// Swap Effect (Fast)
			aWindow,										// The Window Handle
			FALSE,								// Windowed or Fullscreen
			TRUE,										// Enable Auto Depth Stencil  
			D3DFMT_D16,									// 16Bit Z-Buffer (Depth Buffer)
			0,											// No Flags
			D3DPRESENT_RATE_DEFAULT,					// Default Refresh Rate
			D3DPRESENT_INTERVAL_DEFAULT					// Default Presentation Interval (vertical sync)
		};
*/


		if (perfHUDadapter != -1)
		{
			mD3D->CreateDevice(perfHUDadapter, D3DDEVTYPE_REF, aWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &mD3DDevice);
		}
		else
		{
			mD3D->CreateDevice(initParams.idAdapter, D3DDEVTYPE_HAL, aWindow, createFlags, &d3dpp, &mD3DDevice);
		}
		ZASSERT((mD3DDevice != NULL), "Cannot create render device\r\n");

		// Fill Device Infos
		initParams.fullscreen    = true;
		initParams.width            = initParams.width;
		initParams.height        = initParams.height;
		initParams.bpp            = initParams.bpp;

		mBackBufWidth = initParams.width;
		mBackBufHeight = initParams.height;
		/*
		if(PSM_System::GetInterface().manageWindow)
		{
		PSM_SystemInitParam systemInit;

		systemInit.SetDefault();
		systemInit.fullscreen    = true;
		systemInit.posX            = 0;
		systemInit.posY            = 0;
		systemInit.width            = width;
		systemInit.height            = height;

		// Show window
		PSM_System::GetInterface().ReInit(&systemInit);
		PSM_System::GetInterface().Show();
		}
		*/
	}
	// Windowed
	else
	{
		// Initialize present parameters
		memset(&d3dpp, 0, sizeof(d3dpp) );
		d3dpp.Windowed                        = true;
		d3dpp.SwapEffect                    = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat            = mD3DEnum.initialMode.format;
		d3dpp.EnableAutoDepthStencil    = true;
		d3dpp.AutoDepthStencilFormat    = mD3DEnum.initialMode.formatZStencil;
		d3dpp.BackBufferCount            = 1;
		//        d3dpp.zBufferFormat                        = mD3DEnum.initialMode.formatZStencil;
		d3dpp.BackBufferWidth                        = initParams.width;//1;
		d3dpp.BackBufferHeight                        = initParams.height;//1;

		mBackBufferFormatDX9 = mD3DEnum.initialMode.format;
		mBackBufferZStencilFormatDX9 = mD3DEnum.initialMode.formatZStencil;

		// Anti Aliasing ?
		if((initParams.antiAlias) && (mD3DEnum.initialMode.antiAliasWindowed))
		{
			d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
			d3dpp.SwapEffect         = D3DSWAPEFFECT_DISCARD;
		}
		else
		{
			d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
			d3dpp.SwapEffect         = D3DSWAPEFFECT_FLIP;
		}
		if(initParams.vSync)
		{
			d3dpp.PresentationInterval    = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			d3dpp.PresentationInterval    = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		d3dpp.hDeviceWindow = aWindow;

		// Create the rendering device
		if (perfHUDadapter != -1)
		{
			mD3D->CreateDevice(perfHUDadapter, D3DDEVTYPE_REF, aWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &mD3DDevice);
		}
		else
		{
			hr = mD3D->CreateDevice(initParams.idAdapter, 
				D3DDEVTYPE_HAL, 
				aWindow, 
				createFlags, 
				&d3dpp, 
				&mD3DDevice);
		}


		// --
		if(mD3DDevice == NULL)
		{
			tstring errorStr = "Cannot Create device: ";
			switch(hr)
			{
			case D3DERR_DEVICELOST: errorStr += "Lost Device"; break;
			case D3DERR_INVALIDCALL: errorStr += "Invalid Call"; break;
			case D3DERR_NOTAVAILABLE: errorStr += "Not available"; break;
			case D3DERR_OUTOFVIDEOMEMORY: errorStr += "Out of video memory"; break;
			default: errorStr += "Unknow error"; break;
			}

			ERR(errorStr);
			//PSM_MBoxLine(errorStr.c_str());
			return false;
		}

		mBackBufWidth = initParams.width;
		mBackBufHeight = initParams.height;

    // Build Common base
    //DisplayDeviceInitBase(this);

		// create swap chain
		/*
		RECT rc;
		::GetClientRect(aWindow, &rc);

		d3dpp.BackBufferWidth                        = rc.right;
		d3dpp.BackBufferHeight                        = rc.bottom;
		mD3DDevice->CreateAdditionalSwapChain( &d3dpp, &mpSwpChain);
		*/
		// Fill Device Infos
		/*
		fullscreen    = false;
		width            = initParams.width;
		height        = initParams.height;
		bpp            = initParams.bpp;
		*/
		/*
		// Change Win32 properties
		if(PSM_System::GetInterface().manageWindow)
		{
		D3DDISPLAYMODE pMode;
		d3d->GetAdapterDisplayMode(0, &pMode);

		tulong posX, posY;
		posX = (pMode.Width - width) /2;
		if(posX<0)
		{
		posX = 0;
		}

		posY = (pMode.Height - height) /2;
		if(posY<0)
		{
		posY = 0;
		}

		PSM_SystemInitParam systemInit;

		systemInit.SetDefault();
		systemInit.fullscreen    = false;
		systemInit.posX            = posX;
		systemInit.posY            = posY;
		systemInit.width            = width;
		systemInit.height            = height;
		/*
		// Show window
		PSM_System::GetInterface().ReInit(&systemInit);
		PSM_System::GetInterface().Show();

		}
		*/
	}
	GD3D9Device = mD3DDevice;

	mD3DDevice->GetDeviceCaps( &mCaps );
	LOG("%d MRT.\r\n",mCaps.NumSimultaneousRTs);
	mNbMRT = mCaps.NumSimultaneousRTs;
	if (mNbMRT<3) 
	{
		// force netbook mode
		pParam->RenderProfile = RENDER_PROFILE_NETBOOK;
	}

	LOG("Vertex Shader V %5.2f Pixel Shader V %5.2f\r\n", float(mCaps.VertexShaderVersion&0xFFFF)/256.0f, float(mCaps.PixelShaderVersion&0xFFFF)/256);
	if ( ((mCaps.PixelShaderVersion&0xFFFF)<0x200) || ((mCaps.VertexShaderVersion&0xFFFF)<0x200) )
		return false;


	mD3DDevice->SetRenderState(D3DRS_LIGHTING, 0);

	if( FAILED( mD3D->CheckDeviceFormat( initParams.idAdapter, D3DDEVTYPE_HAL,
		d3dpp.BackBufferFormat,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_TEXTURE,
		(D3DFORMAT)(MAKEFOURCC('D','F','2','4')) ) ) )
	{
		LOG("This hardware doesn't support Fetch4 (ATI).\r\n");
		mSupportDF24 = false;
	}
	else
	{
		LOG("This hardware supports Fetch4 (ATI).\r\n");
		mSupportDF24 = true;
		//return false;
	}
	mSupportDF24 = false;

	// create default CG context
	gDefaultCGcontext = cgCreateContext();
	cgD3D9SetDevice(mD3DDevice);
	cgD3D9RegisterStates(gDefaultCGcontext);
	cgD3D9SetManageTextureParameters(gDefaultCGcontext, CG_TRUE);
	cgSetErrorCallback(CGerrorCallback);
#ifdef DEBUG
	cgD3D9EnableDebugTracing(CG_TRUE);
#endif

	mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	mD3DDevice->SetRenderState(D3DRS_ZENABLE , TRUE);
	mD3DDevice->SetRenderState(D3DRS_LIGHTING , FALSE);
	mD3DDevice->SetRenderState(D3DRS_AMBIENT , 0xFFFFFFFF);
	mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE  , FALSE);
	mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE  , FALSE);

	mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE , D3DCOLORWRITEENABLE_ALPHA| D3DCOLORWRITEENABLE_BLUE| D3DCOLORWRITEENABLE_GREEN| D3DCOLORWRITEENABLE_RED );

	mD3DDevice->SetRenderState(D3DRS_COLORVERTEX , FALSE);
	mD3DDevice->SetRenderState(D3DRS_CULLMODE , D3DCULL_CCW);
	
/*
mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
*/

	ZFx *pfx = DirectLoad("./ZenithDatas/DebugShader.fx");
	mChecker.Attach(DirectLoadTex("./ZenithDatas/checker.dds"));

	mRenderProfile = pParam->RenderProfile;
	InitShadowSystem();



	LOG("DisplayDeviceDX9 complete.\r\n");

	// test render target
	/*
	extern IRenderTarget *girt;
	IRenderTarget *irt = this->NewRenderTarget();
	girt = irt;
	irt->Init(256, 256, RT_RGBA8, 0);
	irt->Bind(0);
	void *phand = irt->GetPlatformHandle();
	*/
	return true;
}
//IRenderTarget *girt;

///////////////////////////////////////////////////////////////////////////////////////////////////
void CEGuiPostResetDevice();
void CEGuiPreResetDevice();

void ZDisplayDeviceDX9::OnLostDevice()
{
	LOG("Entering ZDisplayDeviceDX9::OnLostDevice\n");
	// free textures/index arrays/vertex arrays
	IIndexArray *mgIA = (IIndexArray*)FirstInstanceOf(ZIndexArrayDX9);
	while (mgIA)
	{
		mgIA->Reset();
		mgIA=(IIndexArray*)mgIA->GetNext();
	}
	// vertex array
	IVertexArray *mgVA = (IVertexArray*)FirstInstanceOf(ZVertexArrayDX9);
	while (mgVA)
	{
		mgVA->Reset();
		mgVA=(IVertexArray*)mgVA->GetNext();
	}
	/*
	// textures array
	ZTexture *mgTex = (ZTexture*)FirstInstanceOf(ZTexture);
	while (mgTex)
	{
		mgTex->Reset();
		if (mgTex->mNextInstance)
			mgTex=(ZTexture*)mgTex->mNextInstance->QueryInstance();
		else
			mgTex = NULL;
	}
	*/
	CEGuiPreResetDevice();
}


void ZDisplayDeviceDX9::OnResetDevice()
{
	LOG("Entering ZDisplayDeviceDX9::OnResetDevice\n");
	// index array
	IIndexArray *mgIA = (IIndexArray*)FirstInstanceOf(ZIndexArrayDX9);
	while (mgIA)
	{
		mgIA->Init(mgIA->GetIndexCount());
		mgIA->Lock(VAL_WRITE);
		mgIA->Unlock();
		mgIA=(IIndexArray*)mgIA->GetNext();
	}
	// vertex array
	IVertexArray *mgVA = (IVertexArray*)FirstInstanceOf(ZVertexArrayDX9);
	while (mgVA)
	{
		mgVA->Init(mgVA->GetFormat(), mgVA->GetVertexCount());
		mgVA->Lock(VAL_WRITE);
		mgVA->Unlock();
		mgVA=(IVertexArray*)mgVA->GetNext();
	}
	CEGuiPostResetDevice();
}

 bool ZDisplayDeviceDX9::BeginScene() 
 {
#if 0
	 HRESULT result = mD3DDevice->TestCooperativeLevel();
	 if ( FAILED( result ) )
	 {
		 if ( result == D3DERR_DEVICELOST )
		 {
			 Sleep( 50 );
			 return false;

		 }
		 else
		 {
			 OnLostDevice();
			 d3dpp.Windowed = false;
			 HRESULT resetres = mD3DDevice->Reset(&d3dpp);
			 if ( resetres == S_OK)//D3DERR_DEVICELOST )
			 {
				 OnResetDevice();
			 }
			 else
			 {
				 // Device is lost still
				 Sleep( 50 );
				 return false;

			 }
			 /*
			 else
			 {
				 

			 }
			 */

		 }

	 } 
#endif
HRESULT hr = mD3DDevice->TestCooperativeLevel();

	//Wait when dev is lost but not resetable
	if( hr == D3DERR_DEVICELOST )
	{
		Sleep(20);
		return false;
	}

	//Exit if driver is faulty
	else if( hr == D3DERR_DRIVERINTERNALERROR )
	{
		//ZManager->AskForQuit();//throw std::exception("The driver of the 3D device returned an internal error");
		printf("RHAHAAHAHA 0x%x\n", hr);
		exit(-1);
		return false;
	}
	else if( hr == D3DERR_DEVICENOTRESET)
	{
		OnLostDevice();
		mD3DDevice->Reset(&d3dpp);
		OnResetDevice();
		//Device recovered
		return false;
	}
	/*
	else if (hr != S_OK);
	{
		//ZManager->AskForQuit();
		//Not lost at all
		//exit(-1);
		printf("RHAHAAHAHA 0x%x\n", hr);
		exit(-1);
		return false;
	}
	*/


	 D3DVIEWPORT9 vp;
	 vp.MinZ= 0.f;
	 vp.MaxZ = 1.f;
	 vp.Height = GetHeight();
	 vp.Width = GetWidth();
	 vp.X = 0;
	 vp.Y = 0;
	 mD3DDevice->SetViewport(&vp);

	 mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	 mD3DDevice->SetRenderState(D3DRS_ZENABLE , TRUE);
	 mD3DDevice->SetRenderState(D3DRS_LIGHTING , FALSE);
	 mD3DDevice->SetRenderState(D3DRS_AMBIENT , 0xFFFFFFFF);
	 mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE  , FALSE);
	 mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE  , FALSE);

	 mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE , D3DCOLORWRITEENABLE_ALPHA| D3DCOLORWRITEENABLE_BLUE| D3DCOLORWRITEENABLE_GREEN| D3DCOLORWRITEENABLE_RED );

	 mD3DDevice->SetRenderState(D3DRS_COLORVERTEX , FALSE);
	 mD3DDevice->SetRenderState(D3DRS_CULLMODE , D3DCULL_CCW);


	 mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	 mD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	 mD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);



	 mD3DDevice->BeginScene();

	 mBeginSceneDone = true;
	 return true;
 }

 ///////////////////////////////////////////////////////////////////////////////////////////////////
 
void ZDisplayDeviceDX9::SetRenderingState(RenderingState_t& renderingState)
{
	if( GRenderingState.mBlending != renderingState.mBlending )
	{
		//if (GRenderingState.mBlending == 4)
		{
			mD3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
		}

		switch( renderingState.mBlending )
		{
		case 0: // disabled
			mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			break;
		case 1: // add
			mD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE );
			mD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_ONE );
			mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			break;
		case 2: // transparency
			mD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
			mD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
			mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			break;
		case 3: // source * dest
			//GL_DST_COLOR, GL_ZERO
			mD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_DESTCOLOR );
			mD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_ZERO );
			mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			break;
		case 4: // separated alpha for off screen transparency rendering
			mD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
			mD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
			mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			mD3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			mD3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA , D3DBLEND_ONE);
			mD3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA , D3DBLEND_INVSRCALPHA);
			break;

		case 5: // separated alpha for off screen transparency rendering
			mD3DDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE);
			mD3DDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);
			mD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			/*
			mD3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA , D3DBLEND_ONE);//D3DBLEND_ONE );
			mD3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA , D3DBLEND_INVSRCALPHA);//GA2);//D3DBLEND_SRCALPHA );
			mD3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			*/
			break;

		}
	}

	if (GRenderingState.mZWriteEnable != renderingState.mZWriteEnable)
	{
		mD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, renderingState.mZWriteEnable?TRUE:FALSE);
	}
	if (GRenderingState.mAlphaTestEnable != renderingState.mAlphaTestEnable) // Used in opengl
	{
		mD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, renderingState.mAlphaTestEnable?TRUE:FALSE);
	}
	if (GRenderingState.mZTestType != renderingState.mZTestType)
	{
		switch (renderingState.mZTestType)
		{
		case 0: // disabled
			mD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			break;
		case 1: // LessEqual
			if (!GRenderingState.mZTestType)
				mD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			mD3DDevice->SetRenderState(D3DRS_ZFUNC , D3DCMP_LESSEQUAL);
			break;
		}
	}

	if (GRenderingState.mFillMode != renderingState.mFillMode)
	{
		switch (renderingState.mFillMode)
		{
		case 0:
			mD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			break;
		case 1:
			mD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			break;
		}
	}



	extern bool GBInvertCulling;
	static int culi[3]={0,2,1};

	renderingState.mCulling = GBInvertCulling?culi[renderingState.mCulling]:renderingState.mCulling;
	if (GRenderingState.mCulling != renderingState.mCulling)
	{
		switch (renderingState.mCulling)
		{
		case 0: // disabled
			mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
			break;
		case 1: // CCW
			mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );  // Used in opengl
			break;
		case 2: // CW
			mD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
			break;
		}
	}

	renderingState.mCulling = GBInvertCulling?culi[renderingState.mCulling]:renderingState.mCulling;

	// global state is assigned to this pass state
	GRenderingState = renderingState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDisplayDeviceDX9::SetSamplerState( int index, const samplerparam_t& smpt, ZTexture* texture )
{
	samplerparam_t & lastv = GSamplerState[index];

	texture->Bind(index);

	if ((smpt.minFilter != lastv.minFilter)&&(smpt.minFilter))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_MINFILTER, smpt.minFilter);
		lastv.minFilter = smpt.minFilter;
	}

	if ((smpt.mipFilter != lastv.mipFilter)&&(smpt.mipFilter))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, smpt.mipFilter);
		lastv.mipFilter = smpt.mipFilter;
	}

	if ((smpt.magFilter != lastv.magFilter)&&(smpt.magFilter))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, smpt.magFilter);
		lastv.magFilter = smpt.magFilter;
	}

	if ((smpt.AddressU != lastv.AddressU)&&(smpt.AddressU))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_ADDRESSU, smpt.AddressU);
		lastv.AddressU = smpt.AddressU;
	}

	if ((smpt.AddressV != lastv.AddressV)&&(smpt.AddressV))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_ADDRESSV, smpt.AddressV);
		lastv.AddressV = smpt.AddressV;
	}

	if ((smpt.AddressW != lastv.AddressW)&&(smpt.AddressW))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_ADDRESSW, smpt.AddressW);
		lastv.AddressW = smpt.AddressW;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
