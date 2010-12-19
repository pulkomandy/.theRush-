///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiMainmenu.h
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

#ifndef GUIMAINMENU_H__
#define GUIMAINMENU_H__


#include "IProtoGUI.h"

#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;


class GuiMainmenu
{
public:
	GuiMainmenu() :winMgr(WindowManager::getSingleton())
	{
	}
	virtual ~GuiMainmenu()
	{
	}
	void Build(IProtoGUI *pGUI);


	void Show();
	void Tick();
	void Hide();
	void OnLanBack();
protected:
	WindowManager& winMgr;



	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;

	Window *logoRush, *MainLeftMenu;
	ZProtoGUI *mGUI;


	bool HandleBuildLanMain(const CEGUI::EventArgs& e);
	bool HandleBuildConfig(const CEGUI::EventArgs& e);
	bool HandleLeaveProto(const CEGUI::EventArgs& e);
	bool HandleQuickRace(const CEGUI::EventArgs& e);
	bool HandleCareer(const CEGUI::EventArgs& e);
	bool HandleStatsMain(const CEGUI::EventArgs& e);
	bool HandleCredits(const CEGUI::EventArgs& e);


	// subs
	void OnQuitYes();
	void OnQuitNo();
	void OnConfigOK();

	void QuickRaceBackToMainMenu();

};

#endif
