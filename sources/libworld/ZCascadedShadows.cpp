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
#include "stdafx.h"
#include <math.h>
#include "ZCascadedShadows.h"

#define DegreeToRadian(fDegrees) ((3.14159265f/180.0f)*fDegrees)

IRenderTarget* SMRenderTarget[4] = { 0, 0, 0, 0 };
ZFx *mShadowEffect = NULL;
static FFxTechnique* mShadowTechnique = NULL;


#ifdef WIN32
float m_iDepthBias = 0.f;
float m_fBiasSlope = 4.0f;
float gLightDistance = 300.f;
float _fSplitSchemeLambda=0.99f;

float fShadowPlane1 = 20, fShadowPlane2 = 90, fShadowPlane3 = 300;
static int _iNumSplits=4;
// split scheme


static float *_pSplitDistances=NULL;




void SetDepthBias(float db)
{
	m_iDepthBias = db;
}

void SetBiasSlope(float bs)
{
	m_fBiasSlope = bs;
}

void SetSplitLambda(float sl)
{
	_fSplitSchemeLambda = sl;
}


ZFx *mDefshad = NULL;
extern ZTexture* GpSMZTexture;

LPDIRECT3DTEXTURE9 GpSMColorTexture = NULL;

/*
LPDIRECT3DSURFACE9 m_pSMColorSurface[4] = {0,0,0,0}, m_pSMZSurface[4] = {0,0,0,0};//NULL;
LPDIRECT3DTEXTURE9 m_pSMColorTexture[4] = {0,0,0,0}, m_pSMZTexture[4] = {0,0,0,0};// = NULL;

// for DF24
IDirect3DSurface9* m_pColorBuffer;
IDirect3DTexture9* m_pDepthStencilTexture;  // Handle to our shadow map texture
IDirect3DSurface9* m_pDepthStencilSurface;  // Handle to our shadow map depth texture surface
*/
extern tmatrix mShadowMap;

float fCameraAspect;

//RenderTexture _ShadowMapTexture;
static int _iShadowMapSize=1024;


// camera
static tmatrix _mCameraView;
static tmatrix _mCameraProj;
static tvector3 _vCameraSource(0,20,-110);
static tvector3 _vCameraTarget(0,20,-109);
static tvector3 _vCameraUpVector(0,1,0);
static float _fCameraNear=0.1f;
static float _fCameraFar=500.0f; // dynamically adjusted
static float _fCameraFarMax=500.0f;
static float _fCameraFOV=90.0f;


// light
static tmatrix _mLightView;
static tmatrix _mLightProj;

static float _fLightNear=10.0f;
static float _fLightFar=400.0f; // dynamically adjusted
static float _fLightFarMax=400.0f;
float _fLightFOV=DegreeToRadian(90.0f);
static tvector3 _vLightSource; // dynamically adjusted
static tvector3 _vLightTarget; // dynamically adjusted

// scene
static tvector3 _vSceneAABB[8];




// Jitter
#ifdef WIN32
static const int JITTER_SIZE = 32;
static const int JITTER_SAMPLES = 8;
// LPDIRECT3DVOLUMETEXTURE9 m_pJitterTexture;
extern ZTexture* m_pJitterTexture;

void BuildJitterShadowTexture()
{
#if 0
  LPDIRECT3DVOLUMETEXTURE9 pD3D9JitterTexture;
	IDirect3DDevice9 * pd3dDevice = GDD->GetD3D9Device();
	if(FAILED(pd3dDevice->CreateVolumeTexture(JITTER_SIZE, JITTER_SIZE, JITTER_SAMPLES*JITTER_SAMPLES/2, 1,
		D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pD3D9JitterTexture, NULL)))
		return;


  m_pJitterTexture->ForceTextureHandle( (void*)pD3D9JitterTexture );

	// Build the jitter texture
	D3DLOCKED_BOX lb;
	pD3D9JitterTexture->LockBox(0, &lb, NULL, 0);

	unsigned char *data = (unsigned char *)lb.pBits;

	for (int i = 0; i<JITTER_SIZE; i++) {
		for (int j = 0; j<JITTER_SIZE; j++) {
			float rot_offset = ((float)rand() / RAND_MAX - 1) * 2 * 3.1415926f;

			for (int k = 0; k<JITTER_SAMPLES*JITTER_SAMPLES/2; k++) {

				int x, y;
				float v[4];

				x = k % (JITTER_SAMPLES / 2);
				y = (JITTER_SAMPLES - 1) - k / (JITTER_SAMPLES / 2);

				v[0] = (float)(x * 2 + 0.5f) / JITTER_SAMPLES;
				v[1] = (float)(y + 0.5f) / JITTER_SAMPLES;
				v[2] = (float)(x * 2 + 1 + 0.5f) / JITTER_SAMPLES;
				v[3] = v[1];

				// jitter
				v[0] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
				v[1] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
				v[2] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
				v[3] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;

				// warp to disk
				float d[4];
				d[0] = sqrtf(v[1]) * cosf(2 * 3.1415926f * v[0] + rot_offset);
				d[1] = sqrtf(v[1]) * sinf(2 * 3.1415926f * v[0] + rot_offset);
				d[2] = sqrtf(v[3]) * cosf(2 * 3.1415926f * v[2] + rot_offset);
				d[3] = sqrtf(v[3]) * sinf(2 * 3.1415926f * v[2] + rot_offset);

				d[0] = (d[0] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 0] = (unsigned char)(d[0] * 255);
				d[1] = (d[1] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 1] = (unsigned char)(d[1] * 255);
				d[2] = (d[2] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 2] = (unsigned char)(d[2] * 255);
				d[3] = (d[3] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 3] = (unsigned char)(d[3] * 255);
			}
		}
	}
	pD3D9JitterTexture->UnlockBox(0);
#endif
	//D3DXSaveTextureToFile("c:\\temp\\jitter.dds", D3DXIFF_DDS , m_pJitterTexture, 0);
}

void RenderSplitOnHUD(int iSplit)
{
	//if(!_bHUDEnabled) return;
#ifdef WIN32
	//GDD->GetD3D9Device()->SetTexture(0, m_pSMColorTexture[iSplit]);//_ShadowMapTexture.GetColorTexture());
	SMRenderTarget[iSplit]->Bind(0);

	GDD->GetD3D9Device()->SetPixelShader(NULL);
	GDD->GetD3D9Device()->SetVertexShader(NULL);
	GDD->GetD3D9Device()->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE);

	tmatrix mIdentity;
	mIdentity.Identity();
	GDD->GetD3D9Device()->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX*)&mIdentity);
	GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)&mIdentity);
	GDD->GetD3D9Device()->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)&mIdentity);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZENABLE,TRUE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_LIGHTING,FALSE);

	// make sure its drawn on front of everything
	//
	D3DVIEWPORT9 Viewport, oldviewport;
	GDD->GetD3D9Device()->GetViewport(&Viewport);
	GDD->GetD3D9Device()->GetViewport(&oldviewport);
	Viewport.MinZ=0;
	Viewport.MaxZ=0;
	GDD->GetD3D9Device()->SetViewport(&Viewport);

	float fAspect=Viewport.Width/(float)Viewport.Height;

	float fSizeX=(1.8f-0.1f*_iNumSplits)/_iNumSplits;
	if(fSizeX>0.25f) fSizeX=0.25f;
	float fSizeY=fSizeX;

	fSizeX/=fAspect;

	float fOffset=fSizeX*iSplit+iSplit*0.1f;

	float fStartX=-0.9f+fOffset;
	float fStartY=-0.9f+fSizeY;
	float fEndX=-0.9f+fOffset+fSizeX;
	float fEndY=-0.9f;
	DWORD iColor=0xFFFFFFFF;
	float pVertices[4][6]=
	{
		{ fStartX , fStartY, 0, *(float*)&iColor, 0,0},
		{ fEndX   , fStartY, 0, *(float*)&iColor, 1,0},
		{ fStartX , fEndY  , 0, *(float*)&iColor, 0,1},
		{ fEndX   , fEndY  , 0, *(float*)&iColor, 1,1}
	};

	GDD->GetD3D9Device()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(float)*6);

	// reset states

	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZENABLE,TRUE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	GDD->GetD3D9Device()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	GDD->GetD3D9Device()->SetTexture(0,NULL);


	GDD->GetD3D9Device()->SetViewport(&oldviewport);
#endif
}
#endif
// Computes corner points of a frustum
//
//
void CalculateFrustumCorners(tvector3 *pPoints,
							 const tvector3 &vSource,
							 const tvector3 &vTarget,
							 const tvector3 &vUp,
							 float fNear, float fFar,
							 float fFOV, float fAspect, float fScale)
{

	tvector3 vZ=vTarget-vSource;
	vZ.Normalize();

	tvector3 vX;
	vX.Cross(vUp, vZ);
	vX.Normalize();

	tvector3 vY;
	vY.Cross(vZ, vX);


	float fNearPlaneHeight = tanf(fFOV * 0.5f) * fNear;
	float fNearPlaneWidth = fNearPlaneHeight * fAspect;

	float fFarPlaneHeight = tanf(fFOV * 0.5f) * fFar;
	float fFarPlaneWidth = fFarPlaneHeight * fAspect;

	tvector3 vNearPlaneCenter = vSource + vZ * fNear;
	tvector3 vFarPlaneCenter = vSource + vZ * fFar;

	pPoints[0]=tvector3(vNearPlaneCenter - vX*fNearPlaneWidth - vY*fNearPlaneHeight);
	pPoints[1]=tvector3(vNearPlaneCenter - vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[2]=tvector3(vNearPlaneCenter + vX*fNearPlaneWidth + vY*fNearPlaneHeight);
	pPoints[3]=tvector3(vNearPlaneCenter + vX*fNearPlaneWidth - vY*fNearPlaneHeight);

	pPoints[4]=tvector3(vFarPlaneCenter - vX*fFarPlaneWidth - vY*fFarPlaneHeight);
	pPoints[5]=tvector3(vFarPlaneCenter - vX*fFarPlaneWidth + vY*fFarPlaneHeight);
	pPoints[6]=tvector3(vFarPlaneCenter + vX*fFarPlaneWidth + vY*fFarPlaneHeight);
	pPoints[7]=tvector3(vFarPlaneCenter + vX*fFarPlaneWidth - vY*fFarPlaneHeight);

	// calculate center of points
	tvector3 vCenter(0,0,0);
	for(int i=0;i<8;i++) vCenter+=pPoints[i];
	vCenter/=8;

	// for each point
	for(int i=0;i<8;i++)
	{
		// scale by adding offset from center
		pPoints[i]+=(pPoints[i]-vCenter)*(fScale-1);
	}
}



// Builds a standard perspective view and projection matrix
//
//
void CalculateViewProj(tmatrix &mView, tmatrix &mProj,
					   tvector3 vSource, tvector3 vTarget, tvector3 vUpVector,
					   float fFOV, float fNear, float fFar, float fAspect)
{
	// view matrix
	mView.LookAt(vSource, vTarget, vUpVector);

	// projection matrix
	mProj.PerspectiveFovLH2(fFOV, fAspect, fNear, fFar);
}



// Adjusts the camera planes to contain the visible scene
// as tightly as possible. This implementation is not very
// accurate.
//
void AdjustCameraPlanes(void)
{
	// calculate standard matrices
	CalculateViewProj(_mCameraView,_mCameraProj,
		_vCameraSource,_vCameraTarget, _vCameraUpVector,
		_fCameraFOV, _fCameraNear, _fCameraFarMax,1);

	// Find the most distant point of AABB
	//
	float fMaxZ=0;
	for(int i=0;i<8;i++)
	{
		// transform z coordinate with view matrix
		float fZ=_vSceneAABB[i].x*_mCameraView.m[0][2]+
			_vSceneAABB[i].y*_mCameraView.m[1][2]+
			_vSceneAABB[i].z*_mCameraView.m[2][2]+1*_mCameraView.m[3][2];

		// check if its largest
		if(fZ>fMaxZ) fMaxZ=fZ;
	}

	// use largest Z coordinate as new far plane
	_fCameraFar=fMaxZ+_fCameraNear;
}





// Calculates the frustum split distances, or in other
// words, the near and far planes for each split.
//
//
void CalculateSplitDistances(void)
{
	// Reallocate array in case the split count has changed
	//
	/*
	_fCameraNear = GetActiveCamera()->GetNearPlane();//
	_fCameraFar = GetActiveCamera()->GetFarPlane();//
	*/
	delete[] _pSplitDistances;
	_pSplitDistances=new float[_iNumSplits+1];


#ifdef PREVIOUS_METHOD
	// Practical split scheme:
	//
	// CLi = n*(f/n)^(i/numsplits)
	// CUi = n + (f-n)*(i/numsplits)
	// Ci = CLi*(lambda) + CUi*(1-lambda)
	//
	// lambda scales between logarithmic and uniform
	//

	_fSplitSchemeLambda=Clamp(_fSplitSchemeLambda,0.0f,1.0f);

	float cnear = _fCameraNear;
	float cfar = _fCameraFar;


	for(int i=0;i<_iNumSplits;i++)
	{
		float fIDM=i/(float)_iNumSplits;
		float fLog=cnear*powf((cfar/cnear),fIDM);
		float fUniform=cnear+(cfar-cnear)*fIDM;
		_pSplitDistances[i]=fLog*_fSplitSchemeLambda+fUniform*(1-_fSplitSchemeLambda);
	}

	// make sure border values are right
	_pSplitDistances[0]= cnear;
	_pSplitDistances[_iNumSplits]=cfar;
#else
	_pSplitDistances[0] = _fCameraNear;
	_pSplitDistances[1] = fShadowPlane1;
	_pSplitDistances[2] = fShadowPlane2;
	_pSplitDistances[3] = fShadowPlane3;
	_pSplitDistances[4] = _fCameraFar;
#endif
}



// Calculates the view and projection matrix for a light. The projection
// matrix is "zoomed in" on the given frustum split.
//
//
void CalculateLightForFrustum(tvector3 *pCorners)
{

	// calculate standard view and projection matrices for light
	CalculateViewProj(_mLightView,_mLightProj,
		_vLightSource,_vLightTarget, tvector3(0,1,0),
		_fLightFOV, _fLightNear, _fLightFarMax,1);


	// Next we will find the min and max values of the current
	// frustum split in lights post-projection space
	// (where coordinate range is from -1.0 to 1.0)
	//
	float fMaxX=-FLT_MAX;
	float fMaxY=-FLT_MAX;
	float fMinX=FLT_MAX;
	float fMinY=FLT_MAX;
	float fMaxZ=0;

	tmatrix mLightViewProj=_mLightView * _mLightProj;

	// for each corner point
	for(int i=0;i<8;i++)
	{
		// transform point
		tvector4 vTransformed;
		vTransformed.Transform(vector4(pCorners[i],1), mLightViewProj);

		// project x and y
		vTransformed.x/=vTransformed.w;
		vTransformed.y/=vTransformed.w;

		// find min and max values
		if(vTransformed.x>fMaxX) fMaxX=vTransformed.x;
		if(vTransformed.y>fMaxY) fMaxY=vTransformed.y;
		if(vTransformed.y<fMinY) fMinY=vTransformed.y;
		if(vTransformed.x<fMinX) fMinX=vTransformed.x;

		// find largest z distance
		if(vTransformed.z>fMaxZ) fMaxZ=vTransformed.z;
	}


	// set values to valid range (post-projection)
	fMaxX=Clamp(fMaxX,-1.0f,1.0f);
	fMaxY=Clamp(fMaxY,-1.0f,1.0f);
	fMinX=Clamp(fMinX,-1.0f,1.0f);
	fMinY=Clamp(fMinY,-1.0f,1.0f);

	// Adjust the far plane of the light to be at the farthest
	// point of the frustum split. Some bias may be necessary.
	//
	_fLightFar=fMaxZ+_fLightNear+1.5f;

	// re-calculate lights matrices with the new far plane
	CalculateViewProj(_mLightView,_mLightProj,
		_vLightSource,_vLightTarget, tvector3(0,1,0),
		_fLightFOV, _fLightNear, _fLightFar, 1);


	// Next we build a special matrix for cropping the lights view
	// to only contain points of the current frustum split
	//

	float fScaleX=2.0f/(fMaxX-fMinX);
	float fScaleY=2.0f/(fMaxY-fMinY);

	float fOffsetX=-0.5f*(fMaxX+fMinX)*fScaleX;
	float fOffsetY=-0.5f*(fMaxY+fMinY)*fScaleY;

	tmatrix mCropView(  fScaleX,     0.0f,  0.0f,   0.0f,
		0.0f,  fScaleY,  0.0f,   0.0f,
		0.0f,     0.0f,  1.0f,   0.0f,
		fOffsetX, fOffsetY,  0.0f,   1.0f );

	// multiply the projection matrix with it
	_mLightProj*=mCropView;

	// finally modify projection matrix for linearized depth
	_mLightProj.m[2][2]/=_fLightFar;
	_mLightProj.m[3][2]/=_fLightFar;
}


void CalculateLightForFrustumOrtho(tvector3 *pCorners)
{

  // calculate standard view and projection matrices for light

  CalculateViewProj(_mLightView,_mLightProj,
                    _vLightSource,_vLightTarget, tvector3(0,1,0),
                    _fLightFOV, _fLightNear, _fLightFarMax,1);


  // Next we will find the min and max values of the current
  // frustum split in lights post-projection space
  // (where coordinate range is from -1.0 to 1.0)
  //
  float fMaxX=-FLT_MAX;
  float fMaxY=-FLT_MAX;
  float fMinX=FLT_MAX;
  float fMinY=FLT_MAX;
  float fMaxZ=-FLT_MAX;
  float fMinZ=FLT_MAX;

  const tmatrix& mLightViewProj=_mLightView;

  // for each corner point
  for(int i=0;i<8;i++)
  {
    // transform point
    tvector4 vTransformed;
	vTransformed.Transform(vector4(pCorners[i],1), mLightViewProj);

    // project x and y
    vTransformed.x/=vTransformed.w;
    vTransformed.y/=vTransformed.w;

    // find min and max values
    if(vTransformed.x>fMaxX) fMaxX=vTransformed.x;
    if(vTransformed.y>fMaxY) fMaxY=vTransformed.y;
    if(vTransformed.y<fMinY) fMinY=vTransformed.y;
    if(vTransformed.x<fMinX) fMinX=vTransformed.x;

    // find largest z distance
    if(vTransformed.z>fMaxZ) fMaxZ=vTransformed.z;

	if(vTransformed.z<fMinZ) fMinZ=vTransformed.z;

  }

	//tmatrixOrthoOffCenterLH(&_mLightProj, fMinX, fMaxX, fMinY, fMaxY, 0/*fMinZ*/, fMaxZ);

  _mLightProj.OrthoOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, 0/*fMinZ*/, fMaxZ);
}

bool m_bSupportsHWShadowMaps, m_bSupportsPixelShaders20;
bool mbIinited = false;
HRESULT RestoreDeviceObjects(IDirect3DDevice9* m_pd3dDevice)
{
	if (mbIinited)
		return S_OK;

	mbIinited = true;
	//HRESULT hr;

	assert(m_pd3dDevice);

	//  hardware shadow maps are enabled by creating a texture with a depth format (D16, D24X8, D24S8),
	//  with usage DEPTHSTENCIL set.
	//  set this texture as the depth/stencil buffer when rendering the shadow map, and as a texture
	//  when performing the shadow comparison.

	D3DFORMAT zFormat = D3DFMT_D24S8;
	//m_bitDepth = 24;
	/*
	if(FAILED(CheckResourceFormatSupport(m_pd3dDevice, zFormat, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL)))
	{
	LOG("Device/driver does not support hardware shadow maps, using R32F.");
	m_bSupportsHWShadowMaps = false;
	}
	else
	*/
	m_bSupportsHWShadowMaps = true;

	D3DCAPS9 deviceCaps;
	m_pd3dDevice->GetDeviceCaps( &deviceCaps );

	m_bSupportsPixelShaders20 = (deviceCaps.PixelShaderVersion >= D3DPS_VERSION(2,0));




	D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;

	int shadowTexWidth = (m_bSupportsPixelShaders20) ? _iShadowMapSize : _iShadowMapSize;
	int shadowTexHeight = (m_bSupportsPixelShaders20) ? _iShadowMapSize : _iShadowMapSize;

	// FIXME
	//SMRenderTarget[0] = GDD->CreateRenderTarget(shadowTexWidth, shadowTexHeight, RT_SHADOWMAP, NULL );

	if (GDD->SupportsDF24())
	{
		m_bSupportsHWShadowMaps = true;

		for( int i=1; i<4; i++ )
		{
			SMRenderTarget[i] = SMRenderTarget[0];
		}
	}
	else
	{
		for( int i=1; i<4; i++ )
		{
			// FIXME
			//SMRenderTarget[i] = GDD->CreateRenderTarget(shadowTexWidth, shadowTexHeight, RT_SHADOWMAP, NULL );
		}
	}

	/*if (GDD->SupportsDF24())
	{
		m_bSupportsHWShadowMaps = true;
		//
		// Here we create the Depth Stencil Texture (DST). We will bind the surface of this texture
		// to the IDirect3DDevice9's Depth/Stencil Surface for rendering our scene's depth
		// information. Afterwards, we bind this texture (shadow map) to a sampler for looking up
		// depth info in a subsequent pass.
		//
		if( FAILED( GDD->GetD3D9Device()->CreateTexture( shadowTexWidth, shadowTexHeight, 1, D3DUSAGE_DEPTHSTENCIL, (D3DFORMAT)(MAKEFOURCC('D','F','2','4')), D3DPOOL_DEFAULT, &m_pDepthStencilTexture, NULL ) ) )
		{
			LOG("This device does not support the DF24 surface format.");
			return false;
		}
		m_pDepthStencilTexture->GetSurfaceLevel( 0, &m_pDepthStencilSurface );

		// Now we create an offscreen render target that matches the multisample type of our DST.
		// Here we create a really small color buffer to conserve space. If the debug runtime is
		// detected we create a full size color buffer though to be friendly with its validation
		// code.
		// Since our backbuffer is 4xAA by default (framework settings), we must create a non-MSAA
		// color buffer to cobind with our DST.
		if (GDD->GetD3D9Device()->CreateRenderTarget( 1,1, D3DFMT_R5G6B5, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pColorBuffer, NULL ) != D3D_OK)
		{
			LOG("Couldn't create render target.");
			return false;
		}
	}
	else
		if (m_bSupportsHWShadowMaps)
		{
			for (int i=0;i<4;i++)
			{
				int sx = shadowTexWidth;///(1<<i);
				int sy = shadowTexWidth;///(1<<i);

				if(FAILED(m_pd3dDevice->CreateTexture(sx, sy, 1, D3DUSAGE_RENDERTARGET, colorFormat,
					D3DPOOL_DEFAULT, &m_pSMColorTexture[i], NULL)))
					return E_FAIL;
				if(FAILED(m_pd3dDevice->CreateTexture(sx, sy, 1, D3DUSAGE_DEPTHSTENCIL, zFormat,
					D3DPOOL_DEFAULT, &m_pSMZTexture[i], NULL)))
					return E_FAIL;
				if(!m_pSMColorTexture[i] || !m_pSMZTexture[i] )
					return E_FAIL;

				// Retrieve top-level surfaces of our shadow buffer (need these for use with SetRenderTarget)
				if(FAILED(m_pSMColorTexture[i]->GetSurfaceLevel(0, &m_pSMColorSurface[i])))
					return E_FAIL;
				if(FAILED(m_pSMZTexture[i]->GetSurfaceLevel(0, &m_pSMZSurface[i])))
					return E_FAIL;
				if(!m_pSMColorSurface[i] || !m_pSMZSurface[i])
					return E_FAIL;
			}
		}*/

		return S_OK;
}
//-----------------------------------------------------------------------------
//  PracticalPSM::SetRenderStates()
//     sets the correct render states for the current render pass
//-----------------------------------------------------------------------------
static D3DVIEWPORT9 oldViewport;
DWORD GCurCullMode;


void D3D9SetPrograms(ZFx* pEffect);

void DisplayShadowMaps()
{
#ifdef WIN32
	for(int iSplit=0;iSplit<_iNumSplits;iSplit++)
	{
		RenderSplitOnHUD(iSplit);
	}
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////



void ComputeCascadedShadowInfos(ZCamera *pCam, shadowmap_t* aShdmi)
{
	// world bounds
	tvector3 vMin  = GScene->GetBVolume().GetBoxMin();
	tvector3 vMax  = GScene->GetBVolume().GetBoxMax();

	_vSceneAABB[0] = tvector3( vMin.x, vMin.y, vMin.z );
	_vSceneAABB[1] = tvector3( vMax.x, vMin.y, vMin.z );
	_vSceneAABB[2] = tvector3( vMin.x, vMin.y, vMax.z );
	_vSceneAABB[3] = tvector3( vMax.x, vMin.y, vMax.z );
	_vSceneAABB[4] = tvector3( vMin.x, vMax.y, vMin.z );
	_vSceneAABB[5] = tvector3( vMax.x, vMax.y, vMin.z );
	_vSceneAABB[6] = tvector3( vMin.x, vMax.y, vMax.z );
	_vSceneAABB[7] = tvector3( vMax.x, vMax.y, vMax.z );


	// camera management ------------------------------------------------


	//ICamera *pCam = GetActiveCamera();
	fCameraAspect = pCam->GetRatio();
	tmatrix camv  = pCam->GetTransform()->GetWorldMatrix();
	camv.Inverse();

	tvector3 tt = camv.V4.position;
	tt += camv.V4.dir;


	_vLightSource  = *gAtmosphere->GetSunVector();
	_vLightSource *= gLightDistance;
	_vLightSource += (tvector3&)camv.V4.position;
	_vLightTarget  = (tvector3&)camv.V4.position;//tvector3(0,0,0);

	tvector3 sundir(_vLightTarget.x - _vLightSource.x,
		              _vLightTarget.y - _vLightSource.y,
		              _vLightTarget.z - _vLightSource.z);
	sundir.Normalize();

	_vCameraSource  = tvector3(camv.V4.position.x, camv.V4.position.y, camv.V4.position.z);
	_vCameraTarget  = tvector3(tt.x, tt.y, tt.z);
	_vCameraUpVector= tvector3(camv.V4.up.x, camv.V4.up.y, camv.V4.up.z);
	_fCameraNear    = pCam->GetNearPlane();
	_fCameraFar     = _fCameraFarMax = pCam->GetFarPlane();
	_fCameraFOV     = pCam->GetVFov();

	// ---------------------------------------------------------------------

	// Position the camera far plane as near as possible
	// to minimize the amount of empty space
	//
	AdjustCameraPlanes();

	// Calculate the distances of split planes
	// according to the selected split scheme
	//
	CalculateSplitDistances();

	for(int iSplit = 0 ; iSplit < _iNumSplits ; iSplit++ )
	{
		// init
		aShdmi[iSplit].mNbCasterMeshes = 0;
		aShdmi[iSplit].mNbCastedMeshes = 0;

		// -- Shadow map related

		// near and far planes for current frustum split
		float fNear=_pSplitDistances[iSplit];
		float fFar=_pSplitDistances[iSplit+1];

		float fScale=1.1f;
		tvector3 pCorners[8];

		CalculateFrustumCorners(pCorners,
                            _vCameraSource, _vCameraTarget, _vCameraUpVector,
			                      fNear, fFar,
                            _fCameraFOV, fCameraAspect,
                            fScale);

		// Calculate view and projection matrices
		//
		CalculateLightForFrustumOrtho(pCorners);

		tmatrix mt;
		mt.Inverse( _mLightView );

		aShdmi[iSplit].mShdView = _mLightView;
		aShdmi[iSplit].mShdProj = _mLightProj;
		aShdmi[iSplit].mShadowFrustum.Update(mt, _mLightProj);

		// Calculate a matrix to transform points to shadowmap texture coordinates
		// (this should be exactly like in your standard shadowmap implementation)
		//

		float fTexOffset = 0.5f+(0.5f/(float)(_iShadowMapSize/*/(1<<iSplit)*/));

		tmatrix mTexScale(  0.5f,               0.0f,      0.0f,   0.0f,
			                  0.0f,              -0.5f,      0.0f,   0.0f,
			                  0.0f,               0.0f,      1.0f,   0.0f,
			                  fTexOffset,    fTexOffset,     0.0f,   1.0f );

		aShdmi[iSplit].mShdMat = _mLightView * _mLightProj * mTexScale;

		// use the current splits near and far plane
		// when calculating matrices for the camera
		CalculateViewProj(_mCameraView, _mCameraProj,
			                _vCameraSource,_vCameraTarget,_vCameraUpVector,
			                _fCameraFOV, fNear, fFar, fCameraAspect);

		//tmatrix mt;
		mt.Inverse( _mCameraView );
		aShdmi[iSplit].mViewFrustum.Update(mt, _mCameraProj);

		aShdmi[iSplit].mNearPlane = _pSplitDistances[iSplit];
		aShdmi[iSplit].mFarPlane = _pSplitDistances[iSplit+1];
	}
}


void RenderCascadedShadowMaps(shadowmap_t * aShdmi)
{
	if (GDD->GetRenderProfile() == RENDER_PROFILE_NETBOOK)
		return ;

	

	LPDIRECT3DSURFACE9 m_pBackBuffer = NULL, m_pZBuffer = NULL;
	IDirect3DDevice9 * pDev = GDD->GetD3D9Device();

	//setup buffers
	pDev->GetRenderTarget(0, &m_pBackBuffer);
	pDev->GetDepthStencilSurface(&m_pZBuffer);

	if(!m_pBackBuffer || !m_pZBuffer)
		return ;

	PROFILER_START(RenderCascadedShadowMaps);

	// for each split
#if 0
	// default sampling for alpha tested textures
	DWORD sampstU, sampstV;
	pDev->GetSamplerState(0, D3DSAMP_ADDRESSU, &sampstU);
	pDev->GetSamplerState(0, D3DSAMP_ADDRESSV, &sampstV);
	/*
	if (sampstU != D3DTADDRESS_WRAP)
		pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	if (sampstV != D3DTADDRESS_WRAP)
		pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
*/
	//depth bias
	float depthBias = m_iDepthBias / 16777215.f;
	pDev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&depthBias);
	pDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&m_fBiasSlope);
	pDev->GetRenderState(D3DRS_CULLMODE, &GCurCullMode);
	if (GCurCullMode != D3DCULL_CCW)
		pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pDev->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	DWORD bBlendingEnable, zWriteEnable;

	pDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &bBlendingEnable);
	if (bBlendingEnable)
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->GetRenderState(D3DRS_ZWRITEENABLE, &zWriteEnable);
	if (!zWriteEnable)
		pDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

#endif
	//  preserve old viewport
	pDev->GetViewport(&oldViewport);
	//  set new viewport for shadow map
	/*

	D3DVIEWPORT9 newViewPort;
	newViewPort.MinZ = 0.f;
	newViewPort.MaxZ = 1.f;
	newViewPort.X	= 0;
	newViewPort.Y		= 0;
	newViewPort.Width= _iShadowMapSize;
	newViewPort.Height	= _iShadowMapSize;

	pDev->SetViewport( &newViewPort );
*/
	for (int i=0;i<_iNumSplits;i++)
	{
		shadowmap_t& smi = aShdmi[i];

		// TODO : Think if it's possible to do that only once
		//smi.m_pSMZTexture->ForceTextureHandle( SMRenderTarget[i]->GetAPITexture(true) /*(void*)m_pSMZTexture[i]*/ );

		if (smi.mNbCasterMeshes)
		{
			// bind render targets
			SMRenderTarget[i]->BindAsTarget(0);

			/*if (GDD->SupportsDF24())
			{
				// Then we manually set the render target and depth-stencil surface
				pDev->SetRenderTarget( 0, m_pColorBuffer );
				pDev->SetDepthStencilSurface( m_pDepthStencilSurface );
			}
			else
			{
				//set render target to shadow map surfaces
				pDev->SetRenderTarget(0, m_pSMColorSurface[i]);
				//set depth stencil
				pDev->SetDepthStencilSurface(m_pSMZSurface[i]);
			}*/

/*
			D3DVIEWPORT9 newViewport;
			newViewport.X = 0;
			newViewport.Y = 0;
			newViewport.Width  = _iShadowMapSize;
			newViewport.Height = _iShadowMapSize;
			newViewport.MinZ = 0.0f;
			newViewport.MaxZ = 1.0f;
			pDev->SetViewport(&newViewport);
*/
			// clear Z
			// pDev->Clear(0, NULL, /*D3DCLEAR_TARGET |*/ D3DCLEAR_ZBUFFER, 0, 1.0f, 0L);
			GDD->Clear( CLEAR_COLOR|CLEAR_Z );

			GDD->SetViewMatrix(smi.mShdView);
			GDD->SetProjectionMatrix(smi.mShdProj);


			// instancing entry point test ***
			/*
			MeshGroup_t meshGroups[128];
			int nbMeshGrps = GroupMeshPropsDC(smi.mCasterMeshes, smi.mCasterMeshesMatrix,
			smi.mNbCasterMeshes, meshGroups, mShadowEffect);
			RenderMeshGroups(meshGroups, nbMeshGrps);
			*/


			// instancing entry point test ***
			for (uint kjh = 0;kjh<smi.mNbCasterMeshes;kjh++)
			{
				ZMeshInstance *pCaster = smi.mCasterMeshes[kjh];
				const tmatrix& wldm = smi.mCasterMeshesMatrix[kjh];
				GDD->SetWorldMatrix(wldm);

				mShadowEffect->setViewDependentMatrices();
				D3D9SetPrograms(mShadowEffect);

				uint nbsu = pCaster->GetMesh()->GetNbSubMeshes();
				for (uint iasm = 0;iasm<nbsu; iasm++)
				{
					/*
					// set First texture(diffuse)
					if (!pCaster->GetMaterial(iasm)->getTextureArray().empty())
						pCaster->GetMaterial(iasm)->getTextureArray()[0]->Bind(0);
					*/

					// TODO : Portable call but need to be done mostly outside from this loop
					// Only texture binding must be done here & only the diffuse one
					mShadowTechnique->mCurMaterial = pCaster->GetMaterial(iasm);
					mShadowTechnique->setPassState(0);

					pCaster->GetMesh()->Draw(iasm);
				}
				pCaster->SubRef();
			}

		}
	}

	//set render target back to normal back buffer / depth buffer
	pDev->SetRenderTarget(0, m_pBackBuffer);
	m_pBackBuffer->Release();
	pDev->SetDepthStencilSurface(m_pZBuffer);
	m_pZBuffer->Release();

	// restore RS
	pDev->SetRenderState(D3DRS_DEPTHBIAS, 0);
	pDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	pDev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);

	if (GCurCullMode != D3DCULL_CW)
		pDev->SetRenderState(D3DRS_CULLMODE, GCurCullMode);


/*
	// sampler back
	if (sampstU != D3DTADDRESS_WRAP)
		pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, sampstU);
	if (sampstV != D3DTADDRESS_WRAP)
		pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, sampstV);
	if (bBlendingEnable)
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	if (!zWriteEnable)
		pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
*/
	pDev->SetViewport(&oldViewport);
	PROFILER_END();
}
#endif



void InitShadowSystem()
{
	if (GDD->GetRenderProfile() == RENDER_PROFILE_NETBOOK)
		return ;

	int shadowTexWidth = 1024;
	for (int i=0;i<4;i++)
	{
		SMRenderTarget[i] = GDD->NewRenderTarget();
		SMRenderTarget[i]->Init( shadowTexWidth, shadowTexWidth, RT_SHADOWMAP, NULL );
	}

	mShadowEffect = DirectLoad("./ZenithDatas/PracticalPSM.fx");
	
	// shader technic
	if (GDD->SupportsDF24())
	{
		mShadowTechnique = mShadowEffect->selectTechniqueByIndex(4); // "GenHardwareShadowMapDF24"
	}
	else
	{
		mShadowTechnique = mShadowEffect->selectTechniqueByIndex(3); //"GenHardwareShadowMap20"
	}
}

void UninitShadowSystem()
{
	for (int i=0;i<4;i++)
		delete SMRenderTarget[i];
}

