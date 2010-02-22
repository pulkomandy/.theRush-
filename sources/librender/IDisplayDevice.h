///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : IDisplayDevice.h
// Creation : 13/06/2007
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

#ifndef IDISPLAYDEVICE_H__
#define IDISPLAYDEVICE_H__

/*! \file IDisplayDevice.h
    \brief Display Device Interface
    Handles polygonal display
*/
enum IMESH_PRIMITIVE
{
    PRIM_UNDEFINED = 0,
    PRIM_LINELIST,
    PRIM_LINESTRIP,
    PRIM_TRIANGLELIST,
    PRIM_TRIANGLESTRIP,
    PRIM_TRIANGLEFAN
};

DECLAREZCLASS(ZDisplayDeviceDX9);
DECLAREZCLASS(ZDisplayDeviceNULL);
DECLAREZCLASS(ZDisplayDeviceOGL);
DECLAREZCLASS(ZRenderTargetDX9);
DECLAREZCLASS(ZRenderTargetOGL);
DECLAREZCLASS(ZOcclusionQueryDX9);


class IVertexArray;
class IIndexArray;
class ZTexture;
class IMesh;
class ZRenderQueue;

struct VISIBLELIGHTS_t;
struct tmatrix;
struct RenderingState_t;
struct samplerparam_t;
#ifdef WIN32
#include <D3D9.h>
#else
class IDirect3DDevice9;
#endif

class IWorld;
class ICamera;

enum RT_FORMAT
{
	RT_RGBA8,
	RT_R32F,
	RT_R16G16B16A16F,
	RT_ZBUFFER,
	RT_SHADOWMAP
};

enum CLEAR_TARGET
{
	CLEAR_COLOR = 1,
	CLEAR_Z = 2,
	CLEAR_STENCIL = 4
};

// sampler param
typedef struct samplerparam_t
{
	const char *szSamplerName;
	int minFilter, mipFilter, magFilter;
	int AddressU, AddressV, AddressW;
} samplerparam_t;

// rendering state
typedef struct RenderingState_t
{
	RenderingState_t()
	{
		// Default Rendering State
		mBlending = 0;
		mZWriteEnable = 1;
		mZTestType = 1;
		mCulling = 1;
		mAlphaTestEnable = 0;
		mFillMode = 0;
	}

	unsigned mBlending: 4; // 0 = none, 1 = (one, one), 2 = (SRCALPHA, INVSRCALPHA)
								// default = 0
	unsigned mZWriteEnable : 1; // default = 1
	unsigned mZTestType : 3; // 0 = none, 1=LessEgal,
							  // default = 1
	unsigned mCulling : 2; // 0 = none, 1=CCW, 2=CW
							// default = 1
	unsigned mAlphaTestEnable : 1; // default = off
	unsigned mFillMode : 1; // 0 = fill, 1 = wire (default = 0)
} RenderingState_t;

typedef struct FrameStatistics_t
{
	unsigned int mNbDrawCalls;
} FrameStatistics_t;
/**
    IRenderTarget
*/
class IRenderTarget : public virtual ZTexture
{
public:
	virtual void Init(int sizeX, int sizeY, RT_FORMAT format, WindowHandle_t aAdditionalSwapchain) = 0;
    virtual void BindAsTarget(int level) = 0;
	virtual void UnBindAsTarget(int level) = 0;
	virtual void Resize(int sizex, int sizey) = 0;
	virtual void Present(RECT *pSrcRect=NULL) = 0;
	virtual void DrawFSQuad() = 0;
	virtual void DrawFSQuad(int texWidth, int texHeight, float tu, float tv) = 0;
	virtual void DrawFSQuadAO() = 0;
	virtual void* GetAPITexture(bool wantZTexture = false) = 0;
	virtual void Save(const char *szFile) = 0;
	virtual uint32 GetWidth() = 0;
	virtual uint32 GetHeight() = 0;
	virtual ZTexture * GetTexture() = 0;
	virtual void StretchCopyTo(IRenderTarget *pDest, RECT *pSrcRect = NULL, RECT *pDstRect = NULL) = 0;
	virtual ZTexture *BuildCopy() = 0;
	virtual void GetDominantColor(tcolor& dstColor) = 0;
};

class IOcclusionQuery : public ZBaseClass
{
public:
	virtual void StartOcclusionQuery() = 0;
	virtual void EndOcclusionQuery() = 0;
	virtual unsigned int GetNumberOfPixelsRendered() = 0;
};

enum RENDER_PROFILE
{
	RENDER_PROFILE_DEFAULT = 0,
	RENDER_PROFILE_NETBOOK
};

typedef struct INITPARAMS
{
    int idAdapter;
    bool refreshRate60hz;
    bool vSync;
    bool fullscreen;
    int width, height, bpp;
    bool antiAlias;
	int RenderProfile;
} INITPARAMS;

/**
    IDisplayDevice
*/
class IDisplayDevice : public ZBaseClass
{
public:

    virtual bool Create( INITPARAMS * pParam = NULL) = 0;
    virtual IDirect3DDevice9 *GetD3D9Device() const = 0;


    virtual IVertexArray *  NewVertexArray() = 0;
    virtual IIndexArray *   NewIndexArray() = 0;
    virtual ZTexture *      NewTexture() = 0;
	virtual IOcclusionQuery * NewOcclusionQuery() = 0;
	virtual IRenderTarget* NewRenderTarget() = 0;

    virtual bool BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void Present(RECT *pSrcRect = NULL) = 0;

	virtual void ResetStatistics() = 0;
	virtual void FillStatisticsStruct(FrameStatistics_t& stats) = 0;

    virtual void SetProjectionMatrix(const tmatrix & mat) = 0;
    virtual void SetViewMatrix(const tmatrix & mat) = 0;
    virtual void SetWorldMatrix(const tmatrix & mat) = 0;
    virtual void SetTextureTransformMatrix(const tmatrix & mat) = 0;
    virtual void SetPreviousWorldMatrix(const tmatrix & mat) = 0;
    virtual void SetPreviousViewMatrix(const tmatrix & mat) = 0;

    virtual const tmatrix& GetProjectionMatrix() const = 0;
    virtual const tmatrix& GetViewMatrix() const = 0;
    virtual const tmatrix& GetWorldMatrix() const = 0;
    virtual const tmatrix& GetTextureTransformMatrix() = 0;
    virtual const tmatrix& GetPreviousWorldMatrix() = 0;
    virtual const tmatrix& GetPreviousViewMatrix() = 0;

    virtual void SetScissorRect(int x, int y, int sx, int sy) = 0;
    virtual void EnableScissor(bool bEnable) = 0;

	  virtual void SetViewport(RECT *pRC) = 0;
    virtual void SetRenderingState(RenderingState_t& renderingState) = 0;
    virtual void SetSamplerState( int index, const samplerparam_t& samplerParam, ZTexture* texture ) = 0;

    // caps
    virtual bool SupportsDF24() = 0;
    virtual int GetNumberMRT() = 0;

	virtual void SetDefaultRenderTarget(IRenderTarget *pTarget) = 0;

	// post process
	virtual void SetPPLuminance(float aval) = 0;
	virtual void SetPPMiddleGray(float aval) = 0;
	virtual void SetPPWhiteCutoff(float aval) = 0;
	virtual void SetPPfocalDist(float aval) = 0;
	virtual void SetPPfocalThreshold(float aval) = 0;
	virtual void SetPPfocalFactor(float aval) = 0;
	// if false, don't display 3D, only GUI layer
	virtual void Draw3DLayer(bool bDraw) = 0;
	virtual bool Is3DLayerEnable() = 0;
	virtual void SetSepiaStrength(float aStrength) = 0;
	virtual void SetSepiaColor(const tcolor& col) = 0;



	virtual float* GetPPLuminance() = 0;
	virtual float* GetPPMiddleGray() = 0;
	virtual float* GetPPWhiteCutoff() = 0;
	virtual float* GetPPfocalDist() = 0;
	virtual float* GetPPfocalThreshold() = 0;
	virtual float* GetPPfocalFactor() = 0;
	virtual float* GetSepiaStrength() = 0;
	virtual tcolor* GetSepiaColor() = 0;

	virtual void EnableSketchRendering(bool bEnable) = 0;
	virtual bool& IsSketchRenderingEnabled() = 0;


	// Captures
	virtual void CaptureZBuffer(const char *szZBufferTexture) = 0;
	virtual void CaptureColorBuffer(const char *szColorTexture) = 0;

	virtual void EnableSSAO(bool bEnable) = 0;
	virtual void SetSSAOParams(float p1, float p2, float p3, float p4) = 0;
	virtual const tvector4& GetSSAOParams() = 0;

	virtual IRenderTarget * GetRttZ() = 0;

	// resolutions
	virtual unsigned int GetNbPossibleResolutions() const = 0;
	virtual void GetResolution(int idx, int &width, int &height) const = 0;

	// for fullscreen/game, when not using swapchain
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

	virtual void DrawIndexedPrimitives( uint8 aPrimitive, unsigned int BaseVertexIndex,
		unsigned int MinIndex,
		unsigned int NumVertices,
		unsigned int StartIndex,
		unsigned int IndexCount) = 0;

	virtual void DrawPrimitives( uint8 aPrimitive, unsigned int StartVertex,
		unsigned int  vertexCount) = 0;


	// defered lights
	virtual void DrawDeferedLights(VISIBLELIGHTS_t* mDeferedLights, int nbVisibleLights) = 0;

	virtual void ApplyPostProcess() = 0;
	virtual void SetRenderingSize(int sx, int sy, int viewx, int viewy) = 0;
	// Rendering Context
	virtual ZRenderQueue *GetQueueToBuild() const = 0;
	virtual ZRenderQueue *GetQueueToRasterize() const = 0;
	virtual void SwapQueues() = 0;

	// clipping planes
	virtual void SetClipPlane(int iplane, const tvector4& planev) = 0;
	virtual void EnableClipPlanes(int iPlanes) = 0;
	virtual void DisableClipPlanes() = 0;

	// screen grab
	virtual void HandleScreenCapture() = 0;
	virtual IMesh* GetFSMesh() const = 0;
	virtual IMesh* GetHalfFSMesh() const = 0;

	virtual void BuildMiniZ() = 0;

	// AA
	virtual void EnableAA(bool bEnable) = 0;
	virtual bool IsAAEnabled() const = 0;

	// particles index array
	virtual IIndexArray *GetParticlesIndexArray() = 0;

	// transitions
	virtual void ApplyRandomTransition() = 0;
	virtual void TickTransition() = 0;

	virtual void Clear(unsigned int aDest, unsigned int color=0.f, float aDepth = 1.f, int aStencil = 0) = 0;
	virtual int GetRenderProfile() = 0;
};

extern IDisplayDevice * GDD;


/** \fn IDisplayDevice * GDD
\brief Returns the Interface to the display device
\return IDisplayDevice Interface
*/

/*
#ifdef _DEBUG
#define DPVA(x) 		{ \
			char tmps[512];\
			sprintf(tmps, "IVA 0x%x %s [%d] \n", x, __FILE__, __LINE__);\
			OutputDebugString(tmps);\
		}

#define DPIA(x) 		{ \
			char tmps[512];\
			sprintf(tmps, "IIA 0x%x %s [%d] \n", x, __FILE__, __LINE__);\
			OutputDebugString(tmps);\
		}

#else
		*/
#define DPVA(x)
#define DPIA(x)
//#endif
#if 0
// shadow parameters
void SetDepthBias(float db);
void SetBiasSlope(float bs);
void SetSplitLambda(float sl);

void SetAB(int a1, int a2);
void EnableHalfRTT(bool bEnable);
void STLB(float v);
#endif
#endif
