///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiIngameMenu.h
// Creation : 10/04/2009
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

#ifndef GuiIngameMenu_H__
#define GuiIngameMenu_H__

#include "libbase/ZBaseMaths.h"

#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;


class GuiIngameMenu
{
public:
	GuiIngameMenu() :winMgr(WindowManager::getSingleton())
	{
		mbInGameMenuVisible = false;
		mInterp = 0.f;
		mShouldTick = false;
	}
	virtual ~GuiIngameMenu()
	{
	}
	void Build(ZProtoGUI *pGUI);
	void Tick();
	void Show();
	void Hide();
	void ToggleVisibility();
	
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;
	ZProtoGUI *mGUI;

	Window *InGameLeftMenu;
	tcolor mGameSepia;
	float mGameSepiaStrength;
	float mInterp;
	bool mbInGameMenuVisible;
	bool mShouldTick;

	PushButton* btnResume, *btnConfig, *btnRestart, *btnCareer, *btnLeave;


	bool HandleResume(const CEGUI::EventArgs& e);
	bool HandleConfig(const CEGUI::EventArgs& e);
	bool HandleRestart(const CEGUI::EventArgs& e);
	bool HandleBackToCareer(const CEGUI::EventArgs& e);
	bool HandleLeave(const CEGUI::EventArgs& e);

	void OnConfigOK();
	void OnRestartNo();
	void OnRestartYes();
	void OnQuitNo();
	void OnQuitYes();


	bool handleChangeTrack(const CEGUI::EventArgs& e);
};

#endif

