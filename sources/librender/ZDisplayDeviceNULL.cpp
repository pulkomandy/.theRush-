///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZDisplayDeviceNULL.cpp
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


///////////////////////////////////////////////////////////////////////////////////////////////////
IDisplayDevice * GDD;

void CGerrorCallback()
{
	WARN("Cg error: %s\n", cgGetErrorString(cgGetError()));
}

bool GBInvertCulling = false;

RenderingState_t GRenderingState;
samplerparam_t GSamplerState[8];

ZTexture *gCurrentFrameBufferTexture      = NULL;
ZTexture *gCurrentMiniFrameBufferTexture  = NULL;
ZTexture *gCurrentZBufferTexture          = NULL;
ZTexture* m_pJitterTexture                = NULL;
ZTexture* GpSMZTexture                    = NULL;
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

class ZDisplayDeviceNULL : public IDisplayDevice
{
public:
    DECLARE_SERIALIZABLE
public:
    ZDisplayDeviceNULL()
    {
        GDD = this;
		mbSketchRendering = false;
		
    }

    virtual ~ZDisplayDeviceNULL()
    {
    }

	virtual void EnableSketchRendering(bool bEnable) { }
	virtual bool& IsSketchRenderingEnabled() { return mbSketchRendering; }
    virtual IVertexArray *  NewVertexArray() { return (IVertexArray*)_New(ZVertexArray); }
    virtual IIndexArray *   NewIndexArray() { return (IIndexArray*)_New(ZIndexArray); }
    virtual ZTexture *      NewTexture() { return (ZTexture*)_New(ZTexture);}
	virtual IRenderTarget* NewRenderTarget() { return NULL; }
	virtual IOcclusionQuery * NewOcclusionQuery() { return NULL; }
    virtual bool BeginScene() { return true; }
    virtual void EndScene() {}
    virtual void Present(RECT *pSrcRect = NULL) {}

	  virtual void SetViewport(RECT *pRC) { }
    virtual void SetRenderingState(RenderingState_t& renderingState) {}
    virtual void SetSamplerState( int index, const samplerparam_t& samplerParam, ZTexture* texture ) {}

    virtual void SetProjectionMatrix(const tmatrix & mat)
    {
        mProjectionMatrix = mat;
    }

    virtual void SetViewMatrix(const tmatrix & mat)
    {
        mViewMatrix = mat;
    }

    virtual void SetWorldMatrix(const tmatrix & mat)
    {
        mWorldMatrix = mat;
    }



	virtual void SetTextureTransformMatrix(const tmatrix & mat) {}
    virtual void SetPreviousWorldMatrix(const tmatrix & mat) {}
    virtual void SetPreviousViewMatrix(const tmatrix & mat) {}

	virtual const tmatrix& GetProjectionMatrix() const { return tmatrix::identity; }
    virtual const tmatrix& GetViewMatrix() const { return tmatrix::identity; }
    virtual const tmatrix& GetWorldMatrix() const { return tmatrix::identity; }
    virtual const tmatrix& GetTextureTransformMatrix() { return tmatrix::identity; }
    virtual const tmatrix& GetPreviousWorldMatrix() { return tmatrix::identity; }
    virtual const tmatrix& GetPreviousViewMatrix() { return tmatrix::identity; }

	virtual void SetScissorRect(int x, int y, int sx, int sy) {}
	virtual void EnableScissor(bool bEnable) {}


    // caps
	virtual bool SupportsDF24() { return false; }
	virtual int GetNumberMRT() { return 1; }

    // texture target
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
	virtual void CaptureColorBuffer(const char *szColorTexture) {}

	virtual void EnableSSAO(bool bEnable) {}
	virtual void SetSSAOParams(float p1, float p2, float p3, float p4) {}
	virtual const tvector4& GetSSAOParams() { return mSSAOParamsv; }

	virtual IRenderTarget * GetRttZ() { return NULL; }

	// resolutions
	virtual unsigned int GetNbPossibleResolutions() const { return 0; }
	virtual void GetResolution(int idx, int &width, int &height) const { }

	// for fullscreen/game, when not using swapchain
	virtual int GetWidth() { return 1; }
	virtual int GetHeight() { return 1; }

	virtual bool Create( INITPARAMS * pParam = NULL) 
	{ 
		mChecker.Attach(DirectLoadTex("./ZenithDatas/checker.dds"));
		return true; 
	}
	virtual IDirect3DDevice9 *GetD3D9Device() const { return NULL; }

	//virtual void RenderView(ICamera *pCam, IWorld *pWorld, int SizeX, int SizeY, bool bMainView) {}

	virtual void DrawIndexedPrimitives( uint8 aPrimitive, unsigned int BaseVertexIndex,
  unsigned int MinIndex,
  unsigned int NumVertices,
  unsigned int StartIndex,
  unsigned int IndexCount) {}

	virtual void DrawPrimitives( uint8 aPrimitive, unsigned int StartVertex,
		unsigned int  vertexCount) {}

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
	virtual ZRenderQueue *GetQueueToBuild() const { return NULL; }
	virtual ZRenderQueue *GetQueueToRasterize() const { return NULL; }
	virtual void SwapQueues() {	}

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
	}
	virtual int GetRenderProfile() { return 0; }
public:
	bool mbSketchRendering;
    tmatrix                 mProjectionMatrix, mViewMatrix, mWorldMatrix;
	FrameStatistics_t mStats;
	tvector4 mSSAOParamsv;
	// postprocess
	tcolor mSepiaColor;
	float mSepiaStrength;
	smartptr<ZTexture> mChecker;

};


BEGIN_SERIALIZATION(ZDisplayDeviceNULL)
END_SERIALIZATION()
