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
#ifndef ZPROTOGUI_H__
#define ZPROTOGUI_H__

#include "ZGuiProgress.h"
#include "ZGuiMessagebox.h"
#include "ZGuiIngameMenu.h"
#include "ZGuiConfig.h"
#include "ZGuiNetStates.h"
#include "ZGuiCredits.h"
#include "ZGuiLan.h"
#include "ZGuiMainMenu.h"
#include "ZGuiEndResults.h"
#include "ZGuiMusic.h"

class ZProtoGUI;
using namespace CEGUI;

#include "ZGuiStatistics.h"
#include "ZGuiQuickrace.h"
#include "ZGuiSpectator.h"

//~ #include "libplatform/libplatform.h"
#include "libplatform/ZSoundFMOD.h"
#include "ZRushGameContent.h"

class ZProtoGUI : public IProtoGUI
{
public:
    DECLARE_SERIALIZABLE
public:
	ZProtoGUI();
	virtual ~ZProtoGUI();
	virtual void Tick();

	virtual void BuildIngameMenu();
#if 0
	void HandleEchap();
	void BuildMainMenu();
	void SetDebugInfos(const tstring& str)
	{
		mDebugCtrl->setText(str.c_str());
	}
#endif
	virtual void BuildLoadingScreen(const char *szImgName);
	virtual void SetProgress(float aVal);


	DefaultWindow* root;
	Window *mDebugCtrl;
	WindowManager& winMgr;
	// Fonts
	Font *mFontArial8, *mFontArial24;


	// Music
	GuiMusic mMusicGui;

	// Progress
	GuiProgress mProgressGui;
	// Message box
	GuiMessagebox mMessageboxGui;
	// Config
	GuiConfig mConfigGui;
	// ingame menu
	GuiIngameMenu mIngameMenuGui;
	// Net State
	GuiNetStates mNetStatesGui;

	// Mainmenu
	GuiMainmenu mMainmenuGui;
	// credits
	GuiCredits mCreditsGui;
	// Lan
	GuiLan mLanGui;
	// End Result
	GuiEndResult mEndResultGui;
	// Quick race
	GuiQuickrace mQuickraceGui;
	// stats
	GuiStatistics mStatisticsGui;
	// spectator
	GuiSpectator mSpectatorGui;
#if 0


	// console/chat
	GuiConsoleChat mConsoleChatGui;

#endif

	virtual void Hide()
	{
		mNetStatesGui.Hide();
	}
	/*
	virtual void SetNbSlotsAvailable(int nbSlots) = 0;
	virtual void ShowLanShipSelection() = 0;

	virtual void ShowTimeBeforeNextMap(float timebefore) = 0;
	virtual void ShowTimeBeforeCountDown(float timebefore) = 0;
*/
	// Net states
	virtual void ShowTimeBeforeNextMap(float timebefore)
	{
		mNetStatesGui.ShowTimeBeforeNextMap(timebefore);
	}
	virtual void ShowTimeBeforeCountDown(float timebefore)
	{
		mNetStatesGui.ShowTimeBeforeCountDown(timebefore);
	}
	void BuildDisconnection(const char *szReason)
	{
		mNetStatesGui.ShowDeconnection(szReason);
	}
	virtual void BuildUnableToConnect(const char *szReason)
	{
		mNetStatesGui.ShowUnableToConnect(szReason);
	}
	virtual void ShowConnectingToServer(const char *szServer)
	{
		mNetStatesGui.ShowConnectingToServer(szServer);
	}
	void HideWaitingPlayers()
	{
		mNetStatesGui.Hide();
	}
	virtual void SetNbSlotsAvailable(int nbSlots)
	{
		mNetStatesGui.SetNbSlotsAvailable(nbSlots);
		mSpectatorGui.SetNbSlotsAvailable(nbSlots);
	}
	virtual void BuildSpectatorPlayerGui()
	{
		mNetStatesGui.ShowPlayerSpectator();
	}
	virtual void ShowLanShipSelection()
	{
		mNetStatesGui.ShowLanShipSelection();
	}
	virtual void ShowTimeBeforeRaceEnds(float timebefore)
	{
		mNetStatesGui.ShowTimeBeforeRaceEnds(timebefore);
	}
	virtual void ShowWaitingForPlayers()
	{
		mNetStatesGui.ShowWaitingForPlayers();
	}

	virtual void ValidateLanSelShip()
	{
		mNetStatesGui.ValidateLanSelShip();
	}

	virtual void ShowEndResult()
	{
		mEndResultGui.Show();
	}

	void PlayNewMusic()
	{
		mMusicGui.PlayNewMusic();
	}
	void MusicVolumeChanged()
	{
		mMusicGui.MusicVolumeChanged();
	}


	virtual void ShowLan(const ZSubscriberSlot& onBack)
	{
		mLanGui.Show( onBack );
	}

	virtual void ShowLanServer(const ZSubscriberSlot& onBack)
	{
		mLanGui.ShowLanServer( onBack );
	}

	virtual void ShowMainMenu()
	{
		mNetStatesGui.Hide();
		mMainmenuGui.Show();
	}

	virtual void HideInGameMenu()
	{
		mIngameMenuGui.Hide();
	}
	virtual void HideEndResults()
	{
		mEndResultGui.ForcedHide();
	}

	virtual void ShowQuickRace(const ZSubscriberSlot& onBack)
	{
		mQuickraceGui.Show(onBack);
	}

	void BuildGameSetForMenu();

	virtual void UpdateConnectedPlayers()
	{
		mSpectatorGui.UpdateComboboxPlayers();
	}

	virtual void ShowSpectator()
	{
		mSpectatorGui.Show();
	}

	virtual void NicknameChanged()
	{
		mSpectatorGui.NicknameChanged();
	}
	virtual void PlayErrorSound();
	virtual void PlayQuestionSound();
	virtual void PlayOKSound();
	virtual void PlayCancelSound();

	smartptr<ZSoundSample> mSoundErr, mSoundQuestion, mSoundOK, mSoundCancel;

	virtual tstring& GetChosenShipString()
	{
		if ((!mChosenShipString.c_str()) || (mChosenShipString.Empty()) )
			mChosenShipString = gRushGameContent->GetShip(0,0);
		return mChosenShipString;

	}
	void SetChosenShipString(const tstring& shpName)
	{
		mChosenShipString = shpName;
	}
	tstring mChosenShipString;
};

#endif
