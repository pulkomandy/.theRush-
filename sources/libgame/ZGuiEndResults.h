///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiEndResult.h
// Creation : 13/04/2009
// Author : Cedric Guillemet
// Description :
// Reliability : 
//
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

#ifndef GuiEndResult_H__
#define GuiEndResult_H__

#include "CEGUI.h"

class ZProtoGUI;
using namespace CEGUI;

class GuiEndResult
{
public:
	GuiEndResult() :winMgr(WindowManager::getSingleton())
	{
		
	}
	virtual ~GuiEndResult()
	{
	}
	void Build(IProtoGUI *pGUI);
	void Tick();
	void Show();
	void Hide();
	void ForcedHide();
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;
	ZProtoGUI *mGUI;


	Window* logoWinner;
	Window* label1;
	FrameWindow* endresfrm;
	bool QuitToMainMenu(const CEGUI::EventArgs& e);

	PushButton* replayButton;
	PushButton* goToLobbyButton;
	PushButton* nextRaceButton;
	Window *waitingSyncLib;

	bool OnReplayRace(const CEGUI::EventArgs& e);
	bool OnServerGoToLobby(const CEGUI::EventArgs& e);
	void OnlanBack();
	void OnNextRaceBack();
	bool OnNextRace(const CEGUI::EventArgs& e);

};

#endif

