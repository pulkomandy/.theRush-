#include "ZenFWRenderer.h"
#include "ZSoundFMOD.h"
#include "ZenFWVRAMService.h"
#include "librender/ZRenderQueue.h"
#include "IInput.h"
#include "libbase/ZTimer.h"

ZenFWRenderer::~ZenFWRenderer()
{
	delete GDD;
	GDD = NULL;

	if (mSoundRenderer)
		delete mSoundRenderer;
}

void InitCEGui();
void TickCEGui(float Ellapsed);
void InjectMouseMove(int x, int y);
void InjectMouseButtons(int BtNb, bool bDown);


void ZenFWRenderer::Init()
{
#ifdef MAC_OS
	GDD = (IDisplayDevice*)_New(ZDisplayDeviceOGL);
#else
	#ifdef LINUX
	GDD = (IDisplayDevice*)_New(ZDisplayDeviceOGL);
	#else
	GDD = (IDisplayDevice*)_New(ZDisplayDeviceDX9);
	//GDD = (IDisplayDevice*)_New(ZDisplayDeviceOGL);
	#endif
#endif
	INITPARAMS RendererInitParams;
	GConfig->BuildInitParams(&RendererInitParams);
	GDD->Create(&RendererInitParams);
	mRenderProfile = RendererInitParams.RenderProfile;

	PrivateInit(RendererInitParams);
}

void ZenFWRenderer::InitForEditor(WindowHandle_t windowHandle)
{
#ifdef WIN32
	extern WindowHandle_t GEditorWindowHandle;
	GEditorWindowHandle = windowHandle;
#endif

#ifdef MAC_OS
	GDD = (IDisplayDevice*)_New(ZDisplayDeviceOGL);
#else
	#ifdef LINUX
	GDD = (IDisplayDevice*)_New(ZDisplayDeviceOGL);
	#else
	GDD = (IDisplayDevice*)_New(ZDisplayDeviceDX9);
	//GDD = (IDisplayDevice*)_New(ZDisplayDeviceOGL);
	#endif
#endif
	INITPARAMS RendererInitParams;
	GConfig->BuildInitParams(&RendererInitParams);
	RendererInitParams.fullscreen = false;
	GDD->Create(&RendererInitParams);
	mRenderProfile = RendererInitParams.RenderProfile;

	PrivateInit(RendererInitParams);
}

void ZenFWRenderer::PrivateInit(const INITPARAMS& RendererInitParams)
{
	// Sound
	_New(ZSoundManager);
	mSoundRenderer = GSoundManager;
	//
	mAtmosphere.Attach((ZAtmosphere*)_New(ZAtmosphere));

	mCam = (ZCamera*)_New(ZCamera);
	mCam->SetActive();
	tmatrix tmpmat;
	tmpmat.LookAt(tvector3(150, 340, 300), tvector3(0, 0, 0)/*::zero*/, tvector3::YAxis);
	mCam->GetTransform()->SetLocalMatrix(tmpmat);
	mCam->Update();

	mCam->SetVFov(75.f*3.14159f / 180.f);
	mCam->SetPlanes(0.1f, 3000.0f);
	mCam->SetRatio( float(RendererInitParams.width)/float(RendererInitParams.height));

	InitCEGui();

	
	// GUI
	mProtoGui = (IProtoGUI*)_New(ZProtoGUI);
}

void ZenFWRenderer::Run()
{
	PROFILER_START(ZenFWRenderer::Run());
	// VRAM upload
	VRAMService->ProcessDatas();

	//mDefaultSkyColor
	if (mRenderProfile == RENDER_PROFILE_NETBOOK)
		GDD->Clear(	CLEAR_COLOR|CLEAR_Z|CLEAR_STENCIL, gAtmosphere->GetDefaultSkyColor()->ConvToRGBA());
	else
		GDD->Clear(	/*CLEAR_COLOR|*/CLEAR_Z|CLEAR_STENCIL);

	ZCamera *pCam = GetActiveCamera();
	GDD->SetProjectionMatrix(pCam->GetProjectionMatrix());
	GDD->SetViewMatrix(pCam->GetTransform()->GetWorldMatrix());
	if (GDD->BeginScene())
	{

		
		GDD->GetQueueToRasterize()->Rasterize(mRenderProfile);

		


		static bool bPreviousMouseButDown = false;

		int X, Y;
		GetInputDevice()->MouseXY(&X, &Y);
		bool bCurrentMouseButDown = GetInputDevice()->MouseButDown(1);
		InjectMouseMove(X, Y);
		if ( bCurrentMouseButDown && (!bPreviousMouseButDown))
			InjectMouseButtons(0, true);
		if ( (! bCurrentMouseButDown) && bPreviousMouseButDown)
			InjectMouseButtons(0, false);

		bPreviousMouseButDown = bCurrentMouseButDown;

		if (mProtoGui)
			mProtoGui->Tick();

		TickCEGui(gTimer.GetEllapsed());
	}
	if (mSoundRenderer)
		mSoundRenderer->Tick();

	PROFILER_END();
}
