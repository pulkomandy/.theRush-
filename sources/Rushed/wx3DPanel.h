///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wx3DPanel.h
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

#ifndef WX3DPANEL_H__
#define WX3DPANEL_H__

#include <wx/aui/aui.h>
#include "PSM_ZenCameraView.h"
//#include "../libplatform/libplatform.h"
#include "wxZ3DEdit.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
#include "ZenFWGizmosNode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class PSM_ParticleInstance;
class IGizmo;
class ZOccluderBox;
class wxZEditTrack;
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

class wx3DPanel : public wxPanel, public PSM_ZenCameraView, public Zen3DEdit
{
public:

    wx3DPanel(wxWindow* parent, wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     wxAuiManager* mgr = NULL);

    virtual ~wx3DPanel();

    wxAuiDockArt* GetDockArt();
	wxZEditTrack *mEditTrack;
    
	virtual void LoadWorld(const char *szWorldFileName);
	virtual void SaveWorld(const char *szWorldFileName);
	virtual void LoadDefaultTrack(int worldNumber);
	virtual void SetSelection(ZSpatialEntity *pse);

	void RunGame();
private:

	void OnEraseBackground(wxEraseEvent& event) {}
    void OnIdle(wxIdleEvent &event);

    void OnLButtonDown(wxMouseEvent& event);
    void OnLButtonUp(wxMouseEvent& event);
    void OnMButtonDown(wxMouseEvent& event);
    void OnMButtonUp(wxMouseEvent& event);
    void OnRButtonDown(wxMouseEvent& event);
    void OnRButtonUp(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    void OnSize(wxSizeEvent& event);

	void OnChar(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);



private:


    DECLARE_EVENT_TABLE();



	ZenFWManager mManager;
	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;
	ZenFWMeshModifiers mMeshModifiers;
	ZenFWRenderQueueBuilder mRQueueBuilder;
	ZenFWPresent mPresenter;
	ZenFWRenderer aRenderer;
	ZenFWViewer aViewer;
	ZenFWGame aGame;

	ZenFWGizmosNode mGizmos;

	IInput* mInputDevice;

	bool mbNeedInterfaceRefresh;
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
