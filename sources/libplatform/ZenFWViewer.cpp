#include "ZenFWViewer.h"

#include "IInput.h"
#include "..\libgame\ZRushTrack.h"
#include "..\libgame\ZRushGame.h"
#include "..\libworld\ZRigidBodyBullet.h"
#include "ZenFWLoaderNode.h"
#include "../libworld/ZAtmosphere.h"
#include "../libbase/ZTimer.h"

ZenFWViewer *GViewer = NULL;

ZenFWViewer::~ZenFWViewer()
{
	if (GTrack)
		delete GTrack;
	if (GViewer == this)
		GViewer = NULL;
}


void BuildCEGUITest();

void ZenFWViewer::Init()
{

//	mCameraBody = 
	
	mbHasFocus = true;
	GetInputDevice()->SetExclusive(mbHasFocus);
}


void ZenFWViewer::SetActive(bool bActive)
{
	ZenFWNode::SetActive(bActive);
	if (bActive)
	{
		const tmatrix& mat = GetActiveCamera()->GetTransform()->GetWorldMatrix();
		tmatrix transUp;
		transUp.Translation(0,5,0);

		if (!mCameraBody)
		{
			mCameraBody = GGame->GetClientGame()->GetPhysicWorld()->CreateRigidBodySphere(1.f, 0.1f);
		}

		mCameraBody->SetMatrix(mat*transUp);
		mCameraBody->SetGravity(tvector3(0,0.f, 0));
	}
}

void TickCEGui(float Ellapsed);
void ShowCEGUITest(bool bShow);
void ZenFWViewer::Run()
{
	if (!this->IsActive())
		return;
	PROFILER_START(ZenFWViewer::Run());

	GetInputDevice()->Tick();

	if (mbNewFileToView)
	{
		mGLoaderService->CreateInstance(mFile.c_str(), mViewerResources);
		mbNewFileToView = false;
		mbAllResourceToScene = true;
		gAtmosphere->LoadSkybox("./Prototype/Worlds/Stratos/pond.dds");

	}
	if (LoadingPipeIsEmpty() && mbAllResourceToScene)
	{
		mbAllResourceToScene = false;
		mViewerResources->AddAllToScene();
		if (GTrack)
			GTrack->Compute();
		GScene->AddCluterizeJob();
	}


	if (mbHasFocus)
	{
		if(mCameraBody)
		{
			tmatrix mPhysicalCameraMatrix;
			mCameraBody->GetMatrix(&mPhysicalCameraMatrix);
			const tvector4& physPos = mPhysicalCameraMatrix.V4.position;

			tmatrix camwMat = GetActiveCamera()->GetTransform()->GetWorldMatrix();
			camwMat.V4.position = physPos;
			GetActiveCamera()->GetTransform()->SetLocalMatrix(camwMat);


			float ellapsed = gTimer.GetEllapsed()*20.f;

			if (GetInputDevice()->KeyDown(TKEY_a))
			{
				mCameraBody->ApplyForce(tvector3(camwMat.V4.right)*(-4)*ellapsed);
			}
			if (GetInputDevice()->KeyDown(TKEY_d))
			{
				mCameraBody->ApplyForce(tvector3(camwMat.V4.right)*(4)*ellapsed);
			}

			if (GetInputDevice()->KeyDown(TKEY_w))
			{
				mCameraBody->ApplyForce(tvector3(camwMat.V4.dir)*(4)*ellapsed);
			}
			if (GetInputDevice()->KeyDown(TKEY_s))
			{
				mCameraBody->ApplyForce(tvector3(camwMat.V4.dir)*(-4)*ellapsed);
			}
			if (GetInputDevice()->KeyDown(TKEY_SPACE))
			{
				mCameraBody->ApplyForce(tvector3(camwMat.V4.up)*(4)*ellapsed);
			}
			if (GetInputDevice()->KeyDown(TKEY_CONTROL))
			{
				mCameraBody->ApplyForce(tvector3(camwMat.V4.up)*(-4)*ellapsed);
			}



		}
		else
			GetActiveCamera()->UpdateFPSMoves();

		GetActiveCamera()->Update();

		int X, Y;
		GetInputDevice()->MouseXYViewer(&X, &Y);//
		/*
		float fx = >MouseDeltaX();
		float fy = GetInputDevice()->MouseDeltaY();
		*/
/*
		if ((fx != 0.f)||(fy != 0.f))
			LOG("fx,fy : %5.2f %5.2f\n", fx, fy);
*/
		GetActiveCamera()->ComputeFPSView( (float)X, (float)Y);
		GetActiveCamera()->Update();
	}
/*
	if (GetInputDevice()->KeyReleased(TKEY_F1))
	{
		GetInputDevice()->SetExclusive(!GetInputDevice()->IsExclusive());
		ShowCEGUITest(!GetInputDevice()->IsExclusive());
	}
*/
	if (GetInputDevice()->KeyReleased(TKEY_o))
	{
		tstring dir;
		unsigned long imgcnt = (unsigned long)(gTimer.GetTime()* 100.f);
		dir.Printf("%s\\TheRush_Picture_%d", GetPictureDirectory(), imgcnt );

		GDD->CaptureColorBuffer(dir.c_str());
	}

	if (GetInputDevice()->KeyReleased(TKEY_ESC))
	{
		if (GGame)
		{
			SetActive(false);
			GGame->SetActive(true);
		}
		else
		{
			ZManager->AskToQuit();
		}
	}

	GGame->GetClientGame()->GetPhysicWorld()->Tick(gTimer.GetEllapsed());

	mbHasFocus = GetInputDevice()->IsExclusive();


	
	// Present
	PROFILER_END();
}
