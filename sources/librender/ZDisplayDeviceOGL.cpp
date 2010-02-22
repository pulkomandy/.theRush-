///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZDisplayDeviceOGL.cpp
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
#include "GLee.h"
#include "../libgame/libgame.h"
#ifdef MAC_OS
#import <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>

#endif

#if (defined(WIN32) || defined(MAC_OS))
#include "SDL.h"
#else
#include <SDL/SDL.h>
#endif
#include <Cg/cg.h>
#ifdef WIN32
#include <Cg/cgD3D9.h>
#endif
#include <Cg/cgGL.h>
#include "../libworld/libworld.h"
#include "ZRenderTargetOGL.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
extern IDisplayDevice * GDD;
extern CGcontext gDefaultCGcontext;
void CGerrorCallback();
unsigned long GOGLThreadId;

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





class ZDisplayDeviceOGL : public IDisplayDevice
{
public:
    DECLARE_SERIALIZABLE
public:
    ZDisplayDeviceOGL()
    {
        GDD = this;
		mbSketchRendering = false;
		mQueueSwap = 0;
    }

    virtual ~ZDisplayDeviceOGL()
    {
		UninitShadowSystem();
    }

	virtual void EnableSketchRendering(bool bEnable) { }
	virtual bool& IsSketchRenderingEnabled() { return mbSketchRendering; }
    virtual IVertexArray *  NewVertexArray() { return (IVertexArray*)_New(ZVertexArrayOGL); }
    virtual IIndexArray *   NewIndexArray() { return (IIndexArray*)_New(ZIndexArrayOGL); }
    virtual ZTexture *      NewTexture() 
	{ 
		ZTextureOGL *nt = new ZTextureOGL;
		_AddInstance(nt);
		return nt; 
	}
	virtual IRenderTarget* NewRenderTarget() 
	{ 
    	ZRenderTargetOGL *rt = new ZRenderTargetOGL;
    	_AddInstance(rt);
	    return rt; 
	}
	virtual IOcclusionQuery * NewOcclusionQuery() { return NULL; }
    virtual bool BeginScene()
	{
		//glEnable(GL_ZWRITE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);
	    glDepthFunc( GL_LEQUAL );
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);
	    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		//glDisable(GL_VERTEX_COLOR);

		/*
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
*/

		return true;
	}
    virtual void EndScene() 
	{

	}
    virtual void Present(RECT *pSrcRect = NULL) 
	{
		/* Draw it to the screen */
		SDL_GL_SwapBuffers( );

		if (!mColorFrameBufferFileName.Empty())
		{
			int values[4];
			glGetIntegerv(GL_VIEWPORT, values);
			unsigned long *texels = new unsigned long [values[2]*values[3]];
			glReadPixels(0,0,values[2], values[3], GL_RGBA, GL_UNSIGNED_BYTE, texels);
			ZFile file;
			tstring fileName = mColorFrameBufferFileName;
			if (file.Open(fileName+".tga", ZOPEN_WRITE))
			{
				// swap R/B
				char *ptr = (char*)texels;
				for (int i=0;i<values[2]*values[3];i++)
				{
					char temp = ptr[0];
					ptr[0] = ptr[2];
					ptr[2] = temp;

					ptr+=4;
				}
				TargaHeader_t header;
				memset(&header, 0, sizeof(TargaHeader_t));
				header.ImageType = 2;
				header.ImageWidth = values[2];
				header.ImageHeight = values[3];
				header.PixelDepth = 32;
				
				file << header;
				file.Write(texels, values[2]*values[3]*sizeof(unsigned long));
			}
			LOG("Screen Shot saved in %s.\n", mColorFrameBufferFileName.c_str());
			mColorFrameBufferFileName = "";
			delete [] texels;
		}
		
	}

	  virtual void SetViewport(RECT *pRC) { }
    virtual void SetRenderingState(RenderingState_t& renderingState);
    virtual void SetSamplerState( int index, const samplerparam_t& samplerParam, ZTexture* texture );

    virtual void SetProjectionMatrix(const tmatrix & mat)
    {
        mProjectionMatrix = mat;
		glMatrixMode(GL_PROJECTION_MATRIX);
		glLoadMatrixf(mProjectionMatrix.m16);
    }

    virtual void SetViewMatrix(const tmatrix & mat)
    {
        mViewMatrix = mat;
		mViewMatrix.Inverse();

		tmatrix mt;
		mt = mWorldMatrix*mViewMatrix;
		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadMatrixf(mt.m16);
    }

    virtual void SetWorldMatrix(const tmatrix & mat)
    {
        mWorldMatrix = mat;
		tmatrix mt;
		mt = mWorldMatrix*mViewMatrix;
		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadMatrixf(mt.m16);
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
	virtual void CaptureColorBuffer(const char *szColorTexture) { mColorFrameBufferFileName = szColorTexture; }

	virtual void EnableSSAO(bool bEnable) {}
	virtual void SetSSAOParams(float p1, float p2, float p3, float p4) {}
	virtual const tvector4& GetSSAOParams() { return mSSAOParamsv; }

	virtual IRenderTarget * GetRttZ() { return NULL; }

	// resolutions
	virtual unsigned int GetNbPossibleResolutions() const 
	{ 
		SDL_Rect** modes;
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

		/* Check if there are any modes available */
		if (modes == (SDL_Rect**)0) {
			//printf("No modes available!\n");
			exit(-1);
		}

		/* Check if our resolution is restricted */
		if (modes == (SDL_Rect**)-1) {
			//printf("All resolutions available.\n");
			return 0;
		}
		else{
			/* Print valid modes */
			//printf("Available Modes\n");
			int NbModes = 0;
			for (int i=0; modes[i]; ++i)
				NbModes++;//printf("  %d x %d\n", modes[i]->w, modes[i]->h);
			return NbModes;
		}
	}

	virtual void GetResolution(int idx, int &width, int &height) const 
	{ 
		SDL_Rect** modes;
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
		int ndx = GetNbPossibleResolutions()-idx-1;
		width = modes[ndx]->w;
		height = modes[ndx]->h;
	}

	// for fullscreen/game, when not using swapchain
	virtual int GetWidth() { return 1; }
	virtual int GetHeight() { return 1; }

	virtual bool Create( INITPARAMS * pParam = NULL);
	virtual IDirect3DDevice9 *GetD3D9Device() const { return NULL; }

	//virtual void RenderView(ICamera *pCam, IWorld *pWorld, int SizeX, int SizeY, bool bMainView) {}

	virtual unsigned long GetMemoryUsed() const { return sizeof(ZDisplayDeviceOGL); }


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
		tcolor col((tulong)color);
		glClearColor(col.r, col.g, col.b, col.a);
		glClearDepth(aDepth);
		/* Clear The Screen And The Depth Buffer */
		glClear( ((aDest&CLEAR_COLOR)?GL_COLOR_BUFFER_BIT:0) | ((aDest&CLEAR_Z)?GL_DEPTH_BUFFER_BIT:0) );
	}

	virtual int GetRenderProfile() { return mRenderProfile; }
public:
	int		mRenderProfile;

	bool mbSketchRendering;
    tmatrix                 mProjectionMatrix, mViewMatrix, mWorldMatrix;
	FrameStatistics_t mStats;
	tvector4 mSSAOParamsv;

	// SDL
	SDL_Surface *surface;
	// Render Queue
	unsigned int mQueueSwap;
	ZRenderQueue mQueues[2];
	// save buffers
	tstring mColorFrameBufferFileName;

	//
	smartptr<ZTexture> mChecker;

	// postprocess
	tcolor mSepiaColor;
	float mSepiaStrength;
};


BEGIN_SERIALIZATION(ZDisplayDeviceOGL)
END_SERIALIZATION()

int resizeWindow( int width, int height )
{
    /* Height / width ration */
    GLfloat ratio;
 
    /* Protect against a divide by zero */
   if ( height == 0 )
	height = 1;

    ratio = ( GLfloat )width / ( GLfloat )height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */	
	tmatrix mt;
	mt.PerspectiveFovLH2(45.f, ratio, 0.1f, 1000.f);
	glLoadMatrixf(mt.m16);
	

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return( TRUE );
}

bool ZDisplayDeviceOGL::Create( INITPARAMS * pParam)
{
	const SDL_VideoInfo *videoInfo;
	int videoFlags;


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
	
	GOGLThreadId = GetCurrentThreadId();

	/* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
	{
	    ERR( "Video query failed: %s\n", SDL_GetError( ) );
	    Zexit( 1 );
	}

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    //videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
	videoFlags |= SDL_HWSURFACE;
    else
	videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;
	if (initParams.fullscreen)
		videoFlags |= SDL_FULLSCREEN;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
	surface = SDL_SetVideoMode( initParams.width, initParams.height, 0,//SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
				videoFlags );

    /* Verify there is a surface */
    if ( !surface )
	{
	    //ERR( "Video mode set failed: %s\n", SDL_GetError( ) );
	    //Zexit( 1 );
		GConfig->SetQuality("Width", 800);
		GConfig->SetQuality("Height", 600);
		initParams.width = 800;
		initParams.height = 600;
		return Create(&initParams);
	}

/* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


	resizeWindow( initParams.width, initParams.height );

	gDefaultCGcontext = cgCreateContext();
    //cgD3D9SetDevice(mD3DDevice);
#ifdef _DEBUG
	cgGLSetDebugMode(CG_TRUE);
#endif
    cgGLRegisterStates(gDefaultCGcontext);
    cgGLSetManageTextureParameters(gDefaultCGcontext, CG_TRUE);
    cgSetErrorCallback(CGerrorCallback);

	ZFx *pfx = DirectLoad("./ZenithDatas/DebugShader.fx");
	LOG("Shader has %d techniques\n", pfx->mTechniques.size());
	mChecker.Attach(DirectLoadTex("./ZenithDatas/checker.dds"));


	mRenderProfile = pParam->RenderProfile;
	InitShadowSystem();

	LOG("DisplayDeviceOGL complete.\r\n");

	return true;
}



void ZDisplayDeviceOGL::DrawIndexedPrimitives( uint8 aPrimitive, unsigned int BaseVertexIndex,
								   unsigned int MinIndex,
								   unsigned int NumVertices,
								   unsigned int StartIndex,
								   unsigned int IndexCount)
{

	switch(aPrimitive)
	{
	case PRIM_UNDEFINED:
		WARN("Mesh has undefined primitive type.\r\n");
		break;
	case PRIM_LINELIST:
		/*
		mD3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST , BaseVertexIndex, MinIndex,
			NumVertices,StartIndex, IndexCount/2);
			*/
		glDrawElements(GL_LINES, IndexCount, GL_UNSIGNED_SHORT, (const GLvoid*)(StartIndex*2));
		break;
	case PRIM_LINESTRIP:
		/*
		mD3DDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP , BaseVertexIndex, MinIndex,
			NumVertices,StartIndex, IndexCount-1);
			*/
		glDrawElements(GL_LINE_STRIP, IndexCount, GL_UNSIGNED_SHORT, (const GLvoid*)(StartIndex*2));
		break;
	case PRIM_TRIANGLELIST:
		/*
		mD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST , BaseVertexIndex, MinIndex,
			NumVertices,StartIndex, IndexCount/3);
			*/
		glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_SHORT, (const GLvoid*)(StartIndex*2));
		break;
	case PRIM_TRIANGLESTRIP:
		/*
		mD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP , BaseVertexIndex, MinIndex,
			NumVertices,StartIndex, IndexCount-2);
			*/
		glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_SHORT, (const GLvoid*)(StartIndex*2));
		break;
	case PRIM_TRIANGLEFAN:
		/*
		mD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN , BaseVertexIndex, MinIndex,
			NumVertices,StartIndex, IndexCount-2);
			*/
		glDrawElements(GL_TRIANGLE_FAN, IndexCount, GL_UNSIGNED_SHORT, (const GLvoid*)(StartIndex*2));
		break;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDisplayDeviceOGL::DrawPrimitives( uint8 aPrimitive, unsigned int StartVertex,  unsigned int  vertexCount)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	switch(aPrimitive)
	{
	case PRIM_UNDEFINED:
		WARN("Mesh has undefined primitive type.\r\n");
		break;
	case PRIM_LINELIST:
		//mD3DDevice->DrawPrimitive(D3DPT_LINELIST , StartVertex, vertexCount/2);
		glDrawArrays(GL_LINES, StartVertex, vertexCount);
		break;
	case PRIM_LINESTRIP:
		//mD3DDevice->DrawPrimitive(D3DPT_LINESTRIP , StartVertex, vertexCount-1);
		glDrawArrays(GL_LINE_STRIP, StartVertex, vertexCount);
		break;
	case PRIM_TRIANGLELIST:
		//mD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST , StartVertex, vertexCount/3);
		glDrawArrays(GL_TRIANGLES, StartVertex, vertexCount);
		break;
	case PRIM_TRIANGLESTRIP:
		//mD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP , StartVertex, vertexCount-2);
		glDrawArrays(GL_TRIANGLE_STRIP, StartVertex, vertexCount);
		break;
	case PRIM_TRIANGLEFAN:
		//mD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN , StartVertex, vertexCount-2);
		glDrawArrays(GL_TRIANGLE_FAN, StartVertex, vertexCount);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDisplayDeviceOGL::SetRenderingState(RenderingState_t& renderingState)
{
	if( GRenderingState.mBlending != renderingState.mBlending )
	{
		switch( renderingState.mBlending )
		{
		case 0: // disabled
      glDisable( GL_BLEND );
			break;

		case 1: // add
      glBlendFunc( GL_ONE, GL_ONE );
      glEnable( GL_BLEND );
			break;

		case 2: // transparency
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      glEnable( GL_BLEND );
			break;

		case 3: // source * dest
			//GL_DST_COLOR, GL_ZERO
      glBlendFunc( GL_DST_COLOR, GL_ZERO );
      glEnable( GL_BLEND );
			break;

		case 4: // separated alpha for off screen transparency rendering
      glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
      glEnable( GL_BLEND );
			break;

		case 5: // separated alpha for off screen transparency rendering
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      // glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA /* GL_SRC_ALPHA */ );
      glEnable( GL_BLEND );
			break;

		}
	}

	if( GRenderingState.mZWriteEnable != renderingState.mZWriteEnable )
	{
    if( renderingState.mZWriteEnable == TRUE )
      glEnable( GL_DEPTH_TEST );
    else
      glDisable( GL_DEPTH_TEST );
	}
	if( GRenderingState.mAlphaTestEnable != renderingState.mAlphaTestEnable )
	{
    if( renderingState.mAlphaTestEnable == TRUE )
      glEnable( GL_ALPHA_TEST );
    else
      glDisable( GL_ALPHA_TEST );
	}
	if( GRenderingState.mZTestType != renderingState.mZTestType )
	{
		switch( renderingState.mZTestType )
		{
		case 0: // disabled
      glDisable( GL_DEPTH_TEST );
      glDepthFunc( GL_NEVER );
			break;

		case 1: // LessEqual
      if (!GRenderingState.mZTestType)
        glEnable( GL_DEPTH_TEST );
      glDepthFunc( GL_LEQUAL );
			break;
		}
	}

	if( GRenderingState.mFillMode != renderingState.mFillMode )
	{
		switch( renderingState.mFillMode )
		{
		case 0:
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			break;

		case 1:
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			break;
		}
	}

	extern bool GBInvertCulling;
	static int culi[3]={0,2,1};

	renderingState.mCulling = GBInvertCulling ? culi[renderingState.mCulling] : renderingState.mCulling;
	if( GRenderingState.mCulling != renderingState.mCulling )
	{
		switch( renderingState.mCulling )
		{
		case 0: // disabled
      glDisable( GL_CULL_FACE );
			break;

		case 1: // CCW
      glCullFace( GL_BACK );
      glEnable( GL_CULL_FACE );
			break;

		case 2: // CW
      glCullFace( GL_FRONT );
      glEnable( GL_CULL_FACE );
			break;
		}
	}

	renderingState.mCulling = GBInvertCulling ? culi[renderingState.mCulling] : renderingState.mCulling;

	// global state is assigned to this pass state
	GRenderingState = renderingState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDisplayDeviceOGL::SetSamplerState( int index, const samplerparam_t& smpt, ZTexture* texture )
{
	samplerparam_t & lastv = GSamplerState[index];

	texture->Bind(index);

	if ((smpt.minFilter != lastv.minFilter)&&(smpt.minFilter))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smpt.minFilter);
		lastv.minFilter = smpt.minFilter;
	}

	if ((smpt.magFilter != lastv.magFilter)&&(smpt.magFilter))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smpt.minFilter);
		lastv.magFilter = smpt.magFilter;
	}

/*
	if ((smpt.mipFilter != lastv.mipFilter)&&(smpt.mipFilter))
	{
		mD3DDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, smpt.mipFilter);
		lastv.mipFilter = smpt.mipFilter;
	}
*/

	if ((smpt.AddressU != lastv.AddressU)&&(smpt.AddressU))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, smpt.minFilter);
		lastv.AddressU = smpt.AddressU;
	}

	if ((smpt.AddressV != lastv.AddressV)&&(smpt.AddressV))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, smpt.minFilter);
		lastv.AddressV = smpt.AddressV;
	}

	if ((smpt.AddressW != lastv.AddressW)&&(smpt.AddressW))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, smpt.minFilter);
		lastv.AddressW = smpt.AddressW;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
