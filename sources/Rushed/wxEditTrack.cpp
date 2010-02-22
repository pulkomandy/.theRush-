///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxZEditTrack.cpp
// Creation : 08/08/2007
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

#include <wx/wx.h>
#include "wxEditTrack.h"
#include "../LibEdit/LibEdit.h"
#include "../libgame/libgame.h"
#include "wxNavigator.h"
#include "wxZ3DEdit.h"
#include "wxMainFrame.h"

extern MyFrame *mMainFrame;
///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::SetSelection(ZSpatialEntity *pse)
{ 
	m3DEdit->SetSelection(NULL); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::HandleButtonClick(unsigned int aButtonID)
{

	ZAnimation *mEditAnimation = NULL;
	if (m3DEdit->mCurrentBaseClassEditing)
		mEditAnimation = (m3DEdit->mCurrentBaseClassEditing->GetClassID()==ClassIDZAnimation)?(ZAnimation*)m3DEdit->mCurrentBaseClassEditing:NULL;

	switch (aButtonID)
	{
	case 140://Insert Key After

		if (mEditAnimation)
		{
			if (m3DEdit->mEditAnimationKey>0)
			{
				float samp = m3DEdit->mEditAnimationKey-0.5;

				ZAnimation* dt = (ZAnimation*)mEditAnimation;
				ZAnimationTrack<tvector3>* zat = (ZAnimationTrack<tvector3>*)dt->m_AnimTrackList[0];
				ZAnimationTrack<tquaternion>* zar = (ZAnimationTrack<tquaternion>*)dt->m_AnimTrackList[1];

				tvector3 res;
				tquaternion resq;
				tlong curkey=0;

				dt->m_AnimTrackList[0]->Interpolate(&res, samp, curkey);
				curkey=0;
				dt->m_AnimTrackList[1]->Interpolate(&resq, samp, curkey);


				m3DEdit->mEditAnimationKey = zat->InsertKey(samp, res);
				zar->InsertKey(samp, resq);
				mEditAnimation->ResampleKeys(1.0f);


				GTrack->GetAnimation()->ResampleKeys(1.0f);
				//                    GDD->GetShadowManager()->ClearMeshes();
							GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
				GScene->AddCluterizeJob();
				this->SetSelection(NULL);
				m3DEdit->SetDocumentModified();
			}
		}
		break;
	case 141://Insert Key Before
		if (mEditAnimation)
		{
			{
				float samp = m3DEdit->mEditAnimationKey+0.5;

				ZAnimation* dt = (ZAnimation*)mEditAnimation;
				ZAnimationTrack<tvector3>* zat = (ZAnimationTrack<tvector3>*)dt->m_AnimTrackList[0];
				ZAnimationTrack<tquaternion>* zar = (ZAnimationTrack<tquaternion>*)dt->m_AnimTrackList[1];

				tvector3 res;
				tquaternion resq;
				tlong curkey=0;

				dt->m_AnimTrackList[0]->Interpolate(&res, samp, curkey);
				curkey=0;
				dt->m_AnimTrackList[1]->Interpolate(&resq, samp, curkey);


				m3DEdit->mEditAnimationKey = zat->InsertKey(samp, res);
				zar->InsertKey(samp, resq);
				mEditAnimation->ResampleKeys(1.0f);


				GTrack->GetAnimation()->ResampleKeys(1.0f);
				//GDD->GetShadowManager()->ClearMeshes();
							GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
				GScene->AddCluterizeJob();
				this->SetSelection(NULL);
				m3DEdit->SetDocumentModified();
			}
		}
		break;
	case 142://Remove Key
		if (mEditAnimation)
		{
			GTrack->GetAnimation()->RemoveKey(m3DEdit->mEditAnimationKey);
			GTrack->GetAnimation()->ResampleKeys(1.0f);
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
			GScene->AddCluterizeJob();
			mEditAnimation = NULL;
			m3DEdit->mEditAnimationKey = 0;
			this->SetSelection(NULL);
			m3DEdit->SetDocumentModified();
		}
		break;
	case 143://Replace Mesh
		{
			const char *szLogicName = gRushGameContent->GetBrickName(currentPiece);
			for (int i=0;i<mReplaceMeshCount;i++)
			{
				if (m3DEdit->mEditRoadMeshIndex > 0) 
					GTrack->ReplaceBrick(m3DEdit->mEditRoadMeshIndex+i, szLogicName);
			}
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
			GScene->AddCluterizeJob();
			this->SetSelection(NULL);
			m3DEdit->SetDocumentModified();
		}
		break;
	case 144://Add Mesh After
		{
			const char *szLogicName = gRushGameContent->GetBrickName(currentPiece);
			for (int i=0;i<mAddMeshAfterCount;i++)
			{
				GTrack->InsertBrickAfter(m3DEdit->mEditRoadMeshIndex, szLogicName);
			}
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
			GScene->AddCluterizeJob();
			m3DEdit->mEditRoadMeshIndex = -1;
			this->SetSelection(NULL);
			m3DEdit->SetDocumentModified();
		}

		break;
	case 145://Add Mesh Before
		{
			const char *szLogicName = gRushGameContent->GetBrickName(currentPiece);
			for (int i=0;i<mAddMeshBeforeCount;i++)
			{
				if (m3DEdit->mEditRoadMeshIndex > 0) 
					GTrack->InsertBrickBefore(m3DEdit->mEditRoadMeshIndex, szLogicName);
			}
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
			GScene->AddCluterizeJob();
			m3DEdit->mEditRoadMeshIndex = -1;
			this->SetSelection(NULL);
			m3DEdit->SetDocumentModified();
		}
		break;
	case 146://Remove mesh
		if (m3DEdit->mEditRoadMeshIndex > -1)
		{
			/*
	AddNumEdit<unsigned int>("Replace Count", 1, 20, &mReplaceMeshCount, 1);
	AddButton("Replace Mesh", 143);
	AddNumEdit<unsigned int>("Add Count", 1, 20, &mAddMeshAfterCount, 1);
	AddButton("Add Mesh After", 144);
	AddNumEdit<unsigned int>("Add Count", 1, 20, &mAddMeshBeforeCount, 1);
	AddButton("Add Mesh Before", 145);
	AddNumEdit<unsigned int>("Remove Count", 1, 20, &mRemoveMeshCount, 1);
	*/
			for (int i=0;i<mRemoveMeshCount;i++)
			{
				if (m3DEdit->mEditRoadMeshIndex > 0) 
					GTrack->DeleteBrick(m3DEdit->mEditRoadMeshIndex);
			}
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
			GScene->AddCluterizeJob();
			m3DEdit->mEditRoadMeshIndex = -1;
			this->SetSelection(NULL);
			m3DEdit->SetDocumentModified();
		}
		break;
#if 0
	case 200: // save track
		{
			bool mbForOpening = false;
			wxFileDialog fDialog(GetParent(), "Choose a file", "", "", "Track|*.Track",
				mbForOpening?(wxFD_OPEN|wxFD_FILE_MUST_EXIST):(wxFD_SAVE|wxFD_OVERWRITE_PROMPT));

			if (fDialog.ShowModal() == wxID_OK)
			{
				const char* fname = fDialog.GetPath().mb_str();
				//gContentManager->SaveTrack(fname);//LoadGameResources(mCurrentMapName.c_str(), true);//"./ProtoType/Worlds/Earth/Tracks/track1.Track");

			}
		}
		break;
	case 201: // load track
		{
			bool mbForOpening = true;
			wxFileDialog fDialog(GetParent(), "Choose a file", "", "", "Track|*.Track",
				mbForOpening?(wxFD_OPEN|wxFD_FILE_MUST_EXIST):(wxFD_SAVE|wxFD_OVERWRITE_PROMPT));

			//extern IGizmo *        mGizmoEdit;
			if (m3DEdit->mGizmoEdit)
				m3DEdit->mGizmoEdit->SetEditTransform(NULL);

			if (fDialog.ShowModal() == wxID_OK)
			{
				/*
				const char* fname = fDialog.GetPath().mb_str();
				GScene->Clear();


				gContentManager->LoadGameResources(fname);
				WaitForEmptyLoadingPipe();
				// clear externs
							GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
				GTrack->SetDebugEditGizmoVisible(true);
				GTrack->SetKeyMeshCreation(true);
				GScene->AddCluterizeJob();
				GScene->UpdateLoaded3DSounds();
				GDD->Draw3DLayer(true);

				mEditRoadMeshIndex = -1;
				CreateToolMeshes();

				mEditAnimation = NULL;
				mEditAnimationKey = 0;
*/
				wxNavigator::UpdateList();
			}
		}
		break;
#endif
		/*
		case 301: // lighting baking
		{
		extern int mZLightingPrecalcIndex;
		mZLightingPrecalcIndex = 0;
		}
		break;
		*/

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::HandleValueChanged(unsigned int aID)
{

	// update particle gradient selected value
	//ZRushTrack* GTrack = (ZRushTrack*)FirstInstanceOf(ZGTrackerator);
	//{

	switch (aID)
	{
	case 1: // change world
		{
			if (m3DEdit->IsDocumentModified())
			{
				mMainFrame->CheckModifiedFlagDocumentForNewDocument();
			}
			m3DEdit->LoadDefaultTrack(worldNumber);
		}
		break;
	case 2:// listbox
		//wxMessageBox(GetGameContent()->GetEntry(curWorld,prefabNumber));
		{
#if HASPREFAB
			tstring prefab = GetGameContent()->GetEntry(curWorld,prefabNumber);

			ZTransform *pRefInst = gContentManager->GetExternalRef(prefab.GetFileName().c_str());
			if (pRefInst)
			{
				// should select it
				ZTransform *nTrans = pRefInst->Clone();
				nTrans->AddToScene(GScene);
				nTrans->Update();

				GScene->AddCluterizeJob();

				ZSpatialEntity *pse = nTrans->GetFirstWithSpatialEntity()->GetSpatialEntity();
				mCurrentBaseClassEditing = pse;
				mCurrentSubBaseClassEditing = NULL;
				this->SetSelection(pse);

			}
			else
			{

				// Add It

				ZMeshInstance *pmi = (ZMeshInstance *)mGLoaderService->CreateInstance(prefab.c_str())->QueryInstance();
/*
				WaitForEmptyLoadingPipe();
				ZTransform *pInstTemplate =  pmi->GetTransform()->GetRootTransform();
				pInstTemplate->SetExternalRef(prefab.GetFileName().c_str());
				gContentManager->AddExternalRef(prefab.GetFileName().c_str(), pInstTemplate);


				ZTransform *pInst = pInstTemplate->Clone();
				pInst->AddToScene(GScene);
				pInst->Update();



				GScene->AddCluterizeJob();

				ZSpatialEntity *pse = pInst->GetFirstWithSpatialEntity()->GetSpatialEntity();
				mCurrentBaseClassEditing = pse;
				mCurrentSubBaseClassEditing = NULL;
				this->SetSelection(pse);
				*/
			}
#endif
			if (m3DEdit->mGizmoEdit)
				m3DEdit->mGizmoEdit->SetAxisMask(IGizmo::AXIS_ALL);
			wxNavigator::UpdateList();
		}
		break;
	case 300: // bonus left
	case 301: // bonus right
	case 302:
		if (m3DEdit->mEditRoadMeshIndex!=-1)
		{
			//GTrack->SetBonus(mEditRoadMeshIndex, mBonusLeft, mBonusRight);
			GTrack->SetBonus(m3DEdit->mEditRoadMeshIndex, mBonusLeft, mBonusRight, mBonusDecal);
			GTrack->BuildBonus();
			m3DEdit->SetDocumentModified();
			/*
			GTrack->RemoveTrackPiecesFromScene();
			GTrack->Compute();
			GScene->AddCluterizeJob();
			*/
		}
		break;
	default:
		break;

	}
	// change generator/motion

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::HandleGradientSelChanged(wxGradientControl *pControl)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::OnRefresh(wxEvent& event)
{
	/*
	PSM_Node *oldNode = mNode;
	mNode = NULL;
	SetSelectedNode(oldNode);
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::AskRefresh()
{
	wxSizeEvent event;
	event.SetEventObject(this);
	event.SetEventType(14000);
	AddPendingEvent(event);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditTrack::BuildInterface()
{
	int i;
	prefabNumber = 0;

	if (!gRushGameContent)
		return;

	BeginPanel();
	/*
	AddFolder("Track");
	AddButton("Save Track", 200);
	AddButton("Load Track", 201);
	*/
	AddFolder("Style");
	// get worlds
	tstring worlds = "";
	curWorld = "World|";
	worldNumber = 0;

	for (i=0;i<gRushGameContent->GetWorlds().size(); i++)
	{
		if (i == worldNumber) curWorld += gRushGameContent->GetWorlds()[i];
		worlds += gRushGameContent->GetWorlds()[i];
		worlds += "|";
	}
	//tstring
	AddCombo("World",worlds.c_str(), &worldNumber, 1);
	//AddCombo("Generator","Point|Plane|Disk|Sphere|Object", &tmpGenerator, 1);
	//:AddCombo("Motion","None|Physic|Spline|Direction|Point", &tmpMotion, 2);
#ifdef GOTDECORS	
	AddFolder("Decor");

	worldTrack = curWorld;
	curWorld+="|Decor";
	tstring prefabList = "";
	for (i=0;i<GetGameContent()->GetNumberOfEntries(curWorld); i++)
	{
		prefabList += tstring(GetGameContent()->GetEntry(curWorld,i)).GetBaseName();
		prefabList += "|";
	}

	AddTextList("Prefabs", prefabList.c_str(), &prefabNumber, 2);
#endif
	AddFolder("Keys");
	AddButton("Insert Key Before", 140);
	AddButton("Insert Key After", 141);
	AddButton("Remove Key", 142);
	AddFolder("Track Meshes");

	tstring mWorldTrackPieces;
	worldTrack += "|TrackPieces";
	for (i=0;i<gRushGameContent->GetWorldBricksCount(); i++)
	{
		mWorldTrackPieces += gRushGameContent->GetBrickName(i);
		mWorldTrackPieces += "|";
	}
	AddTextList("Pieces",mWorldTrackPieces.c_str(),&currentPiece,3, false); //click, no dbl click

	AddNumEdit<unsigned int>("Replace Count", 1, 20, &mReplaceMeshCount, 1);
	AddButton("Replace Mesh", 143);
	AddNumEdit<unsigned int>("Add Count", 1, 20, &mAddMeshAfterCount, 1);
	AddButton("Add Mesh After", 144);
	AddNumEdit<unsigned int>("Add Count", 1, 20, &mAddMeshBeforeCount, 1);
	AddButton("Add Mesh Before", 145);
	AddNumEdit<unsigned int>("Remove Count", 1, 20, &mRemoveMeshCount, 1);
	AddButton("Remove Mesh", 146);

	AddFolder("Bonus");
	AddCombo("Bonus Left","None|Speed|Bonus", &mBonusLeft, 300);
	AddCombo("Bonus Right","None|Speed|Bonus", &mBonusRight, 301);
	AddNumEdit<float>("Decal", 0.f, 30.f, &mBonusDecal, 0.1f, 302);

	
/*
	AddFolder("Lighting");
	AddButton("Ambient Baking", 301);
*/
	EndPanel();

}

void wxZEditTrack::Refresh()
{
	if (m3DEdit->mEditRoadMeshIndex > -1)
	{
		char tLeft, tRight;
		GTrack->GetBonus(m3DEdit->mEditRoadMeshIndex, tLeft, tRight, mBonusDecal);
		mBonusLeft = tLeft;
		mBonusRight = tRight;
	}
	wxZEdit::UpdateAllControls();
}