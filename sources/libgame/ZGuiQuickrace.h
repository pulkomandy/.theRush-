///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiQuickrace.h
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

#ifndef GuiQuickrace_H__
#define GuiQuickrace_H__

#include "IProtoGUI.h"

class ZProtoGUI;
#include "CEGUI.h"
using namespace CEGUI;
class ZGuiLapChoser;

class GuiQuickrace
{
public:
	GuiQuickrace() :winMgr(WindowManager::getSingleton())
	{

	}
	virtual ~GuiQuickrace();

	void Build(IProtoGUI *pGUI);
	void Tick();
	void Show(const ZSubscriberSlot& onBack);
	void Hide();
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;

	Combobox* cbship, *cbteam, *cbworld, *cbtrack;

	ZSubscriberSlot mOnBack;
	FrameWindow *qrf;
	ZProtoGUI *mGUI;
	Combobox *cbgamemode;

	Window *srvthmbimg;
	ZGuiLapChoser *lapc;


	bool handleSShipShipChanged(const CEGUI::EventArgs& e);
	void TeamChangedTo(unsigned int teamIdx);
	bool OnSelShipBack(const CEGUI::EventArgs& e);
	bool OnQuickRaceOK(const CEGUI::EventArgs& e);

	bool handleSShipTeamChanger(const CEGUI::EventArgs& e);
	bool LanServerWorldChanged(const CEGUI::EventArgs& e);
	bool LanServerTrackChanged(const CEGUI::EventArgs& e);

};

#endif

