///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiNetStates.h
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

#ifndef GuiNetStates_H__
#define GuiNetStates_H__

#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;


class GuiNetStates
{
public:
	GuiNetStates() :winMgr(WindowManager::getSingleton())
	{
		mNbSlotsAvailable = 0;
	}
	virtual ~GuiNetStates()
	{
	}
	void Build(ZProtoGUI *pGUI);
	void Tick();

	void ShowPlayerSpectator();
	void SetNbSlotsAvailable(int nbSlots);
	void ShowLanShipSelection();

	void ShowTimeBeforeNextMap(float timebefore);
	void ShowTimeBeforeCountDown(float timebefore);
	void ShowTimeBeforeRaceEnds(float timebefore);

	void ShowDeconnection(const char *szReason);
	void ShowUnableToConnect(const char *szReason);
	void ShowConnectingToServer(const char *szServer);
	void ShowWaitingForPlayers();

	void ValidateLanSelShip();
	void Hide();


protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;

	FrameWindow *StatWindow;
	Window* txtlib;
	ZProtoGUI *mGUI;

	void OnDeconnectionOK();
	void OnUnableToConnectOK();
	// spectator/player
	FrameWindow *mPlayerSpectator;
	bool OnPlaySpectator(const CEGUI::EventArgs& e);
	bool OnAskSlot(const CEGUI::EventArgs& e);
	unsigned int mNbSlotsAvailable;
	PushButton* mPlayerNbSlotsBtn;
	// spectator
	/*FrameWindow *mSpectatorGUI;
	bool HandleIngameSpectatorConfig(const CEGUI::EventArgs& e);
	bool HandleSpectatorLeaveGame(const CEGUI::EventArgs& e);
	void SpectatorBackFromConfig();
	void OnSpectatorQuitYes();
	void OnSpectatorQuitNo();
	*/
	// Lan Ship Selection
	FrameWindow* mLanShipSel;
	bool OnLanSelShipOK(const CEGUI::EventArgs& e);
	Combobox* mCBship, *mCBteam;
	bool handleSShipTeamChanger(const CEGUI::EventArgs& e);
	bool handleSShipShipChanged(const CEGUI::EventArgs& e);
	void TeamChangedTo(unsigned int teamIdx);
	unsigned int mSelShipCurTeam, mSelShipCurShip;
	void CancelConnectToServer();

	// net infos
	FrameWindow *mStateInfo;
	Window* mNetInfosLib;
	//float mTimeBeforeCountDown, mTimeBeforeNextMap, mTimeBeforeRaceEnds;

	float mCountDownTime;
	const char* mCountDownString;
	void HideSelShip();
};

#endif

