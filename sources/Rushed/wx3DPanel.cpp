///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wx3DPanel.cpp
// Creation : 08/11/2006
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
//#include "wxStdAfx.h"
#include <wx/wx.h>

#include "../libbase/LibBase.h"
#include "../LibEdit/LibEdit.h"
#include "wx3DPanel.h"
#include "../librender/librender.h"
#include "../libworld/libworld.h"
#include "wxNavigator.h"
#include "wxMainFrame.h"
#include "wxEditTrack.h"

extern MyFrame *mMainFrame;
DECLAREZCLASS(ZLoggerwxConsole);

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wx3DPanel, wxPanel)
    EVT_ERASE_BACKGROUND(wx3DPanel::OnEraseBackground)
    EVT_IDLE(wx3DPanel::OnIdle)
    EVT_LEFT_DOWN(wx3DPanel::OnLButtonDown) 	    //Process a wxEVT_LEFT_DOWN event. The handler of this event should normally call event.Skip() to allow the default processing to take place as otherwise the window under mouse wouldn't get the focus.
    EVT_LEFT_UP(wx3DPanel::OnLButtonUp) 	        //Process a wxEVT_LEFT_UP event.
    EVT_MIDDLE_DOWN(wx3DPanel::OnMButtonDown) 	    //Process a wxEVT_MIDDLE_DOWN event.
    EVT_MIDDLE_UP(wx3DPanel::OnMButtonUp) 	        //Process a wxEVT_MIDDLE_UP event.
    EVT_RIGHT_DOWN(wx3DPanel::OnRButtonDown) 	    //Process a wxEVT_RIGHT_DOWN event.
    EVT_RIGHT_UP(wx3DPanel::OnRButtonUp) 	        //Process a wxEVT_RIGHT_UP event.
    EVT_RIGHT_DOWN(wx3DPanel::OnMButtonDown) 	    //Process a wxEVT_RIGHT_DOWN event.
    EVT_RIGHT_UP(wx3DPanel::OnMButtonUp) 	        //Process a wxEVT_RIGHT_UP event.
    EVT_MOTION(wx3DPanel::OnMouseMotion) 	        //Process a wxEVT_MOTION event.
    EVT_MOUSEWHEEL(wx3DPanel::OnMouseWheel)
    EVT_SIZE(wx3DPanel::OnSize)
	EVT_CHAR(wx3DPanel::OnChar)
	EVT_KEY_DOWN(wx3DPanel::OnKeyDown)
	EVT_KEY_UP(wx3DPanel::OnKeyUp)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnChar(wxKeyEvent& event)
{
#ifdef FIXED
	if (GetCurrentFocusOwner())
	{
		//long aAscii =char(event.m_keyCode&0xFF);
		GetCurrentFocusOwner()->OnChar(event.m_keyCode);

	}

	InjectChar(event.m_keyCode);
	#endif
}



void wx3DPanel::OnKeyDown(wxKeyEvent& event)
{
#ifdef FIXED
	InjectKeyDown(event.m_rawFlags>>16);
#endif
	event.Skip();
}

void wx3DPanel::OnKeyUp(wxKeyEvent& event)
{
	if ((event.m_rawCode == 46) /*&& (mEditMeshInstance)*/)
	{
		DeleteSelected();
		return;
	}
	else if (event.m_rawCode == 0xde)
	{
#ifdef FIXED
		GProtoGui->ToggleConsoleInput();
#endif
		return;
	}
#ifdef FIXED
	InjectKeyUp(event.m_rawFlags>>16);
#endif
	event.Skip();
}
///////////////////////////////////////////////////////////////////////////////////////////////////

wx3DPanel::wx3DPanel(wxWindow* parent, wxWindowID id ,
                     const wxPoint& pos ,
                     const wxSize& size ,
                     wxAuiManager* mgr)
                     : wxPanel(parent, id, pos, size, wxNO_BORDER), PSM_ZenCameraView(),
					 mGizmos(this)
{
	PROFILER_ENABLE;

	mEditTrack = NULL;

	GLOG.SetLog((ILogger*)_New(ZLoggerwxConsole));
	GLOG.SetConsole((ILogger*)_New(ZLoggerwxConsole));//ZLoggerMB));
	GLOG.SetWarn((ILogger*)_New(ZLoggerwxConsole));//ZLoggerMB));
	GLOG.SetError((ILogger*)_New(ZLoggerwxConsole));//ZLoggerMB));

	LOG("Starting of rushed console logging.\n");

	mManager.Init(1);
	mManager.DumpTimings(false);
	mManager.ManualStartIOLane();
	aRenderer.InitForEditor((HWND)GetHandle());

#ifdef WIN32
	mInputDevice = (IInput*)_New(ZDirectInput8);
#else
	mInputDevice = (IInput*)_New(ZInputSDL);
#endif
	mInputDevice->Create();

	aViewer.Init();
	aViewer.SetActive(false);
	aGame.BuildSolo();
	//aGame.SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Default.track", GameSet_t::GAME_TYPE_EDITOR));
	LoadDefaultTrack(0);
	
	mGizmos.Init();

#ifdef FIXED
	GZenithApp->mGizmosRendering.Init();
	GDD->Draw3DLayer(true);
#endif

	

	// GTrack
    

	mbNeedInterfaceRefresh = true;

	wxNavigator::UpdateList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

wx3DPanel::~wx3DPanel()
{
	mManager.AskToQuit();
#ifdef _DEBUG
	_CrtSetDbgFlag(0);
#endif

	GLOG.SetLog(NULL);
	GLOG.SetWarn(NULL);
	GLOG.SetError(NULL);

//	PROFILER_DISABLE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnIdle(wxIdleEvent &event)
{
	PROFILER_START(MainLoop);

	mManager.TickOneFrame();


#ifdef FIXED

	GZenithApp->Renderer.SetSize(GetSize().x, GetSize().y);

	ZManager->TickOneFrame();
#endif

	if (LoadingPipeIsEmpty() && mbNeedInterfaceRefresh)
	{
		mMainFrame->RefreshInterface();
		mbNeedInterfaceRefresh = false;
	}

#ifdef FIXED

				// controls
		if (GClientGame && GClientGame->mClientShip)
		{
			keysStruct nks;
			GClientGame->mClientShip->GetHumanControls(nks);
			GClientGame->mClientShip->GetAutoPilotControls(nks);
			GClientGame->mClientShip->ControlsToPhysic(nks);
		}
#endif




	if (GetInputDevice()->KeyReleased(TKEY_ESC))
	{
		if (aGame.GetClientGame())
		{
			aGame.CleanRushGames();
			aGame.BuildSolo();
			gCameraMotion.SetCameraUnused();
			GTrack->ShowEditGizmos(true);
		}
	}


	if (mGizmoEdit)
		mGizmoEdit->SetScreenSize(GetSize().x, GetSize().y);

	PSM_ZenCameraView::Tick();
	PROFILER_END();

    event.RequestMore();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnLButtonDown(wxMouseEvent& event)
{
#ifdef FIXED
	InjectMouseButtons(0, true);
#endif
    event.Skip();

	// used before for career's sphere
	//GProtoGui->OnClick(event.m_x, event.m_y);

	if (Zen3DEdit::OnLButtonDown(event.m_x, event.m_y, event.m_shiftDown, GetSize().x, GetSize().y ))
		CaptureMouse();

	wxNavigator::UpdateList();

    
    //(pCam);
    /*
    if (gCurrentWorld->TestRayCollideNearest(rayOrig, rayDir, &hitNode, hitPos, hitNorm))
    {
        gMyFrame->SelectNode(hitNode);
    }
    */
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnLButtonUp(wxMouseEvent& event)
{
	if (Zen3DEdit::OnLButtonUp( event.m_x, event.m_y ))
		ReleaseMouse();
    else if (PSM_ZenCameraView::StopCameraManipulation()) 
		ReleaseMouse();

#ifdef FIXED
	if (GetScreenWindow())
		GetScreenWindow()->OnLButtonUp(event.m_x, event.m_y);

	InjectMouseButtons(0, false);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnRButtonDown(wxMouseEvent& event)
{
    if (PSM_ZenCameraView::RButtonDown(event.m_x, event.m_y,
        event.ControlDown(), event.ShiftDown(), event.AltDown()))
        CaptureMouse();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnRButtonUp(wxMouseEvent& event)
{
    if (PSM_ZenCameraView::StopCameraManipulation()) 
		ReleaseMouse();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnMButtonDown(wxMouseEvent& event)
{
    if (PSM_ZenCameraView::MButtonDown(event.m_x, event.m_y,
        event.ControlDown(), event.ShiftDown(), event.AltDown()))
        CaptureMouse();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnMButtonUp(wxMouseEvent& event)
{
    if (PSM_ZenCameraView::StopCameraManipulation()) 
		ReleaseMouse();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnMouseMotion(wxMouseEvent& event)
{
	Zen3DEdit::OnMouseMotion( event.m_x, event.m_y );

    PSM_ZenCameraView::OnMouseMove(event.m_x, event.m_y);
#ifdef FIXED
	InjectMouseMove(event.m_x, event.m_y);
#endif
   
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnMouseWheel(wxMouseEvent& event)
{
    PSM_ZenCameraView::OnMouseWheel(event.GetWheelRotation());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::OnSize(wxSizeEvent& event)
{
    wxPanel::OnSize(event);

    wxSize mSize = event.GetSize();

	ZCamera *pCam = GetActiveCamera();
	pCam->SetRatio(((float)mSize.x)/ ((float)mSize.y));
	//rtt->Init(640, 480, RT_RGBA8, (HWND)GetHandle());
    //PSM_RenderDevice::GetInterfacePtr()->ResizeWindowed(mSize.x, mSize.y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::SetSelection(ZSpatialEntity *pse) 
{
	mGizmos.SetSelection(pse);
	if (mEditTrack)
		mEditTrack->Refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::LoadWorld(const char *szWorldFileName)
{
	if (GTrack)
		GTrack->RemoveTrackPiecesFromScene();

	//GScene->Tick();
	this->SetSelection(NULL);
	//aGame.BuildSolo();
	tstring extractWorld = szWorldFileName;
	extractWorld.Replace('\\','/');
	tulong aPosStart, aPosEnd; 
	extractWorld.Find("Worlds/",&aPosStart);
	extractWorld.Find("/Tracks/",&aPosEnd);
	
	tstring worldName = extractWorld.GetSubString(aPosStart+7, aPosEnd);
	aGame.SetGameSet(GameSet_t(worldName, szWorldFileName, GameSet_t::GAME_TYPE_EDITOR));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::SaveWorld(const char *szWorldFileName)
{
	ZFile mFile;
	mFile.Open( szWorldFileName, ZOPEN_WRITE );
	GTrack->SaveTrack( mFile );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::LoadDefaultTrack(int worldNumber)
{
	tstring trackName;
	trackName.Printf("./Prototype/Worlds/%s/Tracks/Default.track",gRushGameContent->GetWorlds()[worldNumber].c_str());
	this->LoadWorld(trackName.c_str());
	this->ClearDocumentModifiedFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wx3DPanel::RunGame()
{
	ZRushGame *pGame = (ZRushGame *)aGame.GetClientGame();
	pGame->BuildEditorGameTest();
}

///////////////////////////////////////////////////////////////////////////////////////////////////