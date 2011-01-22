///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../libbase/LibBase.h"
#include "wxZ3DEdit.h"
//#include "../libplatform/libplatform.h"
#include "wxMainFrame.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
#include "../libplatform/ZSoundFMOD.h"
extern MyFrame *mMainFrame;
///////////////////////////////////////////////////////////////////////////////////////////////////

Zen3DEdit::Zen3DEdit()
{
	// gizmo

	mGizmoEditMove = (IGizmo*)_New(CGizmoTransformMove);
	mGizmoEditRotate = (IGizmo*)_New(CGizmoTransformRotate);
	mGizmoEditScale = (IGizmo*)_New(CGizmoTransformScale);

	mGizmoEditMove->SetSnap(tvector3(8,8,8));
	mGizmoEditRotate->SetSnap(ZPI/4.0f);
	mGizmoEditScale->SetSnap(10.0f);

	mGizmoEdit = NULL;

	mbGizmoEditing = false;
	mEditAnimationKey = 0;
	mEditRoadMeshIndex = -1;


	mCurrentBaseClassEditing = NULL;
	mCurrentSubBaseClassEditing = NULL;

	mbDocumentModified = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Zen3DEdit::~Zen3DEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Zen3DEdit::SelectLight(ZLight *pLight)
{
	mCurrentBaseClassEditing = pLight;
	mCurrentSubBaseClassEditing = NULL;
	if (mGizmoEdit)
		mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
//	mZEditEnv->BuildInterface();
}

void Zen3DEdit::SelectOccluder(ZOccluderBox *pOcc)
{
	mCurrentBaseClassEditing = pOcc;
	mCurrentSubBaseClassEditing = NULL;
	if (mGizmoEdit)
		mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
//	mZEditEnv->BuildInterface();
}

#ifdef FIXED
void Zen3DEdit::SelectEnv(ISky* pcls)
{
	mCurrentBaseClassEditing = pcls;
	mCurrentSubBaseClassEditing = NULL;
	if (mGizmoEdit)
		mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
	mZEditEnv->BuildInterface(true);
}

void Zen3DEdit::SelectMetaEmitter(IMetaEmitter* pcls)
{
	mCurrentBaseClassEditing = pcls;
	mCurrentSubBaseClassEditing = NULL;
	if (mGizmoEdit)
		mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
	mZEditEnv->BuildInterface();
}

void Zen3DEdit::SelectParticleSys(IMetaEmitter* pme, IParticleSystem* pcls)
{
	mCurrentBaseClassEditing = pme;
	mCurrentSubBaseClassEditing = pcls;
	if (mGizmoEdit)
		mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
	mZEditEnv->BuildInterface();
}
#endif
void Zen3DEdit::SelectSoundInstance(ZSoundInstance *psnd)
{
	mCurrentBaseClassEditing = psnd;
	mCurrentSubBaseClassEditing = NULL;
	if (mGizmoEdit)
		mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
	//mZEditEnv->BuildInterface();
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void Zen3DEdit::SetEditMode(EDIT_MODES aMode)
{

	switch (aMode)
	{
	case EDIT_MODE_SELECT:
		mGizmoEdit = NULL;
		break;
	case EDIT_MODE_MOVE:
		mGizmoEdit = mGizmoEditMove;
		break;
	case EDIT_MODE_ROTATE:
		mGizmoEdit = mGizmoEditRotate;
		break;
	case EDIT_MODE_SCALE:
		mGizmoEdit = mGizmoEditScale;
		break;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Zen3DEdit::SetGizmoSnap(bool bSnapMove, bool bSnapRotate, bool bSnapScale)
{
	mGizmoEditMove->UseSnap(bSnapMove);
	mGizmoEditRotate->UseSnap(bSnapRotate);
	mGizmoEditScale->UseSnap(bSnapScale);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Zen3DEdit::DeleteSelected()
{
#ifdef FIXED
	if ( (mCurrentBaseClassEditing) && (mCurrentBaseClassEditing->GetClassID() == ClassIDZOccluderBox) )
	{
		((ZMeshInstance*)mCurrentBaseClassEditing->QueryInstance())->GetTransform()->RemoveFromScene();
		mCurrentBaseClassEditing->SubRef();
		GScene->AddCluterizeJob();
	}
	else
	{
		if ( (mCurrentBaseClassEditing) && (mCurrentBaseClassEditing->GetClassID() == ClassIDZMeshInstance) )
		{
			ZTransform *ptr = ((ZMeshInstance*)mCurrentBaseClassEditing->QueryInstance())->GetTransform()->GetRootTransform();
			ptr->RemoveFromScene();
			ptr->SubRef();
			//mCurrentBaseClassEditing->SubRef();
			GScene->AddCluterizeJob();
		}
		else if (mEditRoadMeshIndex > -1)
		{
			GTrack->DeleteMeshInstance(mEditRoadMeshIndex);
			GTrack->Compute(false);
			GScene->AddCluterizeJob();
		}
		else if ( (mCurrentBaseClassEditing) && (mCurrentBaseClassEditing->GetClassID() == ClassIDZAnimation) )
		{
			GTrack->GetAnimation()->RemoveKey(mEditAnimationKey);
			GTrack->GetAnimation()->ResampleKeys(1.0f);
			GTrack->Compute(false);
			GScene->AddCluterizeJob();
		}
		else if ( (mCurrentBaseClassEditing) && (mCurrentBaseClassEditing->GetClassID() == ClassIDZSoundInstance) )
		{
			((ZSoundInstance*)mCurrentBaseClassEditing)->stop();
			delete mCurrentBaseClassEditing;

		}
		else if (mCurrentBaseClassEditing)
		{
			delete mCurrentBaseClassEditing;
		}
	}
#endif
	mCurrentBaseClassEditing = NULL;
	mCurrentSubBaseClassEditing = NULL;

	mEditRoadMeshIndex = -1;
	if (mGizmoEdit)
		mGizmoEdit->SetEditTransform(NULL);
#ifdef FIXED
	this->SetSelection(NULL);
#endif
	//wxNavigator::UpdateList();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
PSM_ParticleInstance *wx3DPanel::CreateDefaultParticle()
{

PSM_ParticleInstance *pins = new PSM_ParticleInstance;
PSM_ParticleSystem * ps = PSM_ParticleSystem::CreateFromString("","./fire.ini", tvector3::zero, tvector3::zero, tvector3::zero, "", NULL, true);


pins->SetParticleSystem(ps);
pins->SetName("NewParticle%02d", gSerializableClasses[PSM_ParticleSystem::mClassID].mCount);

gCurrentWorld->UpdateNodeCluster(pins);

return pins;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void Zen3DEdit::SetLocate(EDIT_LOCATE mLocate)
{
	if (!mGizmoEditMove) return;
	switch (mLocate)
	{
	case LOCATE_VIEW:
		mGizmoEditMove->SetLocation(IGizmo::LOCATE_WORLD);
		mGizmoEditRotate->SetLocation(IGizmo::LOCATE_LOCAL);
		mGizmoEditScale->SetLocation(IGizmo::LOCATE_LOCAL);
		break;
	case LOCATE_WORLD:
		mGizmoEditMove->SetLocation(IGizmo::LOCATE_WORLD);
		mGizmoEditRotate->SetLocation(IGizmo::LOCATE_WORLD);
		mGizmoEditScale->SetLocation(IGizmo::LOCATE_LOCAL);
		break;
	case LOCATE_LOCAL:
		mGizmoEditMove->SetLocation(IGizmo::LOCATE_LOCAL);
		mGizmoEditRotate->SetLocation(IGizmo::LOCATE_LOCAL);
		mGizmoEditScale->SetLocation(IGizmo::LOCATE_LOCAL);
		break;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Zen3DEdit::OnLButtonUp(int X, int Y)
{
	if ((mGizmoEdit)&&(mbGizmoEditing))
	{
		mGizmoEdit->OnMouseUp(X, Y);

		mbGizmoEditing=false;

		if (mCurrentBaseClassEditing->GetClassID() == ClassIDZAnimation)
		{
			SetDocumentModified();
			//GDD->GetShadowManager()->ClearMeshes();
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();//false);
			this->SetSelection(GTrack->GetKeyMesh(mEditAnimationKey));
		}
		GScene->AddCluterizeJob();
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Zen3DEdit::OnMouseMotion(int X, int Y)
{
	if (mGizmoEdit)
	{


		mGizmoEdit->OnMouseMove( X, Y );

#ifdef FIXED
		if (mCurrentBaseClassEditing)
			if (mCurrentBaseClassEditing->GetClassID() == ClassIDZMetaEmitter)
			{
				((IMetaEmitter*)mCurrentBaseClassEditing)->SetGenerationMatrix(mMetaEmitEditMatrix);
			}
#endif
			//ApplyMatrixModification();
			//return;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Zen3DEdit::OnLButtonDown(int X, int Y, bool bHasShiftDown, int wSizeX, int wSizeY)
{
	if ( (mGizmoEdit)&&(mCurrentBaseClassEditing) )
	{

		//wxSize aSize = GetSize();
		mGizmoEdit->SetScreenSize(wSizeX, wSizeY);
		TClassID clsID = mCurrentBaseClassEditing->GetClassID();

		if ((bHasShiftDown)&&(clsID == ClassIDZSoundInstance))
		{
			ZSoundInstance *pInst = (ZSoundInstance *)mCurrentBaseClassEditing->QueryInstance();
			ZSoundInstance *nInst = (ZSoundInstance*)_New(ZSoundInstance);
			ZSoundSample *pSamp = pInst->GetSample();
/*
			pSamp->setLoop(true);
			// parameters
			pSamp->setInnerAttenuationRadius(pInst->getInnerAttenuationRadius());
			pSamp->setOuterAttenuationRadius(pInst->getOuterAttenuationRadius());
			pSamp->setAttenuationType(pInst->getAttenuationType());
			pSamp->setNode(pInst->getNode()->Clone());

			nInst->SetSample(true, pSamp);//((ZSoundInstance*)mDeferred3DSoundsLoading[i])->mSample);
			nInst->play();

			wxNavigator::UpdateList();
			*/
		}

		if ((bHasShiftDown)&&(clsID == ClassIDZOccluderBox))
		{
			ZTransform *nTrans = ((ZMeshInstance*)mCurrentBaseClassEditing->QueryInstance())->GetTransform()->Clone();
			nTrans->AddToScene(GScene);
			nTrans->Update();
			SetDocumentModified();
			//wxNavigator::UpdateList();
		}
		if ((bHasShiftDown)&&(clsID == ClassIDZMeshInstance))
		{
			ZTransform *nTrans = ((ZMeshInstance*)mCurrentBaseClassEditing->QueryInstance())->GetTransform()->Clone();
			nTrans->AddToScene(GScene);
			nTrans->Update();
			SetDocumentModified();
			//wxNavigator::UpdateList();
		}

		if ((bHasShiftDown)&&(clsID == ClassIDZLight))
		{
			ZTransform *nTrans = ((ZLight*)mCurrentBaseClassEditing->QueryInstance())->GetTransform()->Clone();
			nTrans->AddToScene(GScene);
			nTrans->Update();
			SetDocumentModified();
			//wxNavigator::UpdateList();
		}

		if (mGizmoEdit->OnMouseDown(X, Y))
		{
			mbGizmoEditing=true;
			//CaptureMouse();
			return true;
		}
	}
/*
	if (PSM_ZenCameraView::LButtonDown(X, Y, event.ControlDown(), event.ShiftDown(), event.AltDown()))
	{
		//CaptureMouse();
		return true;
	}
	else
	*/
	{



		// simple selection
		tvector3 rayOrig, rayDir;
		tvector3 hitPos, hitNorm;
		/*
		PSM_Node *hitNode;
		gCurrentWorld->GetCameraManager().GetCurrent()->BuildRay(X, Y,
		PSM_RenderDevice::GetInterfacePtr()->width,
		PSM_RenderDevice::GetInterfacePtr()->height,
		rayOrig, rayDir);
		*/


		this->SetSelection(NULL);

		//wxSize sz = GetSize();
		GetActiveCamera()->BuildRay(X, Y, wSizeX, wSizeY, rayOrig, rayDir);


		//ZRushTrack *GTrack = GTrack;//(ZRushTrack *)FirstInstanceOf(ZGTrackerator);
		ZAnimation *mAnim = NULL;

		if (GTrack->PickAnimationKey(rayOrig, rayDir, &mAnim, &mEditAnimationKey))
		{
			this->SetSelection(GTrack->GetKeyMesh(mEditAnimationKey));
			//wxMessageBox("Hit");
			mCurrentBaseClassEditing = mAnim;
			mCurrentSubBaseClassEditing = NULL;
			if (mGizmoEdit)
				mGizmoEdit->SetAxisMask(IGizmo::AXIS_Z);
			
			return false;
		}
		else
		{
			float t= 1000;
			ZSpatialEntity *pse = GScene->Pick(rayOrig, rayDir, hitNorm, t);
			if (!pse)
			{
				this->SetSelection(NULL);
				return false;
			}
			TClassID clsID = pse->GetClassID();

			//ZMeshInstance *pInst = NULL;//((IWorld*)FirstInstanceOf(ZWorld))->PickRay2(rayOrig, rayDir, hitNorm, t);
			if (clsID == ClassIDZMeshInstance)
			{
				ZMeshInstance *pInst = (ZMeshInstance*)pse->QueryInstance();
				mEditRoadMeshIndex = GTrack->GetRoadMeshInstanceIndex(pInst);
				if (mEditRoadMeshIndex != -1)
				{
					this->SetSelection(pse);


					mCurrentBaseClassEditing = NULL;
					mCurrentSubBaseClassEditing = NULL;
					return false;
				}
				this->SetSelection(pse);

				//pInst = (ZMeshInstance*)pInst->GetRootTransformSpatialEntity()->QueryIntance();
				mCurrentBaseClassEditing = pInst->GetRootTransformSpatialEntity();
				mCurrentSubBaseClassEditing = NULL;
				if (mGizmoEdit)
					mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
			}

			else if (clsID == ClassIDZLight)
			{
				SelectLight((ZLight*)pse->QueryInstance());
				this->SetSelection(pse);
				return false;
			}

		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Zen3DEdit::SetDocumentModified() 
{ 
	mbDocumentModified = true; 
	mMainFrame->UpdateTitleBar();
}

void Zen3DEdit::ClearDocumentModifiedFlag()
{ 
	mbDocumentModified = false; 
	if (mMainFrame)
		mMainFrame->UpdateTitleBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
