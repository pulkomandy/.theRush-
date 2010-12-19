///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiSpectator.h
// Creation : 05/01/2010
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

#ifndef GuiSpectator_H__
#define GuiSpectator_H__


class ZProtoGUI;
class IProtoGUI;
#include "CEGUI.h"
using namespace CEGUI;

class GuiSpectator
{
public:
	GuiSpectator() :winMgr(WindowManager::getSingleton())
	{
	}
	virtual ~GuiSpectator()
	{
	}
	void Build(IProtoGUI *pGUI);
	void Tick();
	void Show();
	void Hide();
	void SetNbSlotsAvailable(int nbSlots);
	void UpdateComboboxPlayers();
	void NicknameChanged();
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;

	FrameWindow *SpecWindow;
	PushButton* mPlayerNbSlotsBtn;

	Window* txtlib;
	ZProtoGUI *mGUI;
	Combobox *cbPlayer;
	Combobox *cbCamera;

	bool OnAskSlot(const CEGUI::EventArgs& e);
	
	bool handleCameraModeChanger(const CEGUI::EventArgs& e);

};

#endif

