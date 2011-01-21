///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiNetStates.cpp
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

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZGuiNetStates.h"
#include "ZProtoGUI.h"
#include "ZGuiUtils.h"
#include "libbase/ZTimer.h"
#include "ZLocalisation.h"
#include "ZRushGame.h"
#include "CameraMotion.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::Build(ZProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	// -- Player/Spectator GUI --------------------------------------------------------------------

	mPlayerSpectator = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "PlayerOrSpectatorWnd");
	mPlayerSpectator->setTitleBarEnabled(false);
	mPlayerSpectator->setCloseButtonEnabled(false);
	root->addChildWindow(mPlayerSpectator);
	mPlayerSpectator->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.3f)));
	mPlayerSpectator->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	mPlayerSpectator->setAlpha(0.88f);
	mPlayerSpectator->setSizingEnabled(false);
	mPlayerSpectator->hide();


	PushButton *btn  = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "SpectatorBtn"));
	mPlayerSpectator->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 1.f)));
	btn->setText(GLoc->GetString("SPECTATOR").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiNetStates::OnPlaySpectator, this));
	btn->setFont(mFontArial8);



	mPlayerNbSlotsBtn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "PlayerBtn"));
	mPlayerSpectator->addChildWindow(mPlayerNbSlotsBtn);
	mPlayerNbSlotsBtn->setPosition(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.0f)));
	mPlayerNbSlotsBtn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 1.f)));
	mPlayerNbSlotsBtn->setFont(mFontArial8);
	mPlayerNbSlotsBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiNetStates::OnAskSlot, this));

	// -- Spectator -------------------------------------------------------------------------------
#if 0
	mSpectatorGUI = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "IngameMenuSpectator");
	mSpectatorGUI->setTitleBarEnabled(false);
	mSpectatorGUI->setCloseButtonEnabled(false);
	root->addChildWindow(mSpectatorGUI);
	mSpectatorGUI->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.10f)));
	mSpectatorGUI->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	mSpectatorGUI->setAlpha(0.88f);
	mSpectatorGUI->hide();


	/*
	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Resume Game"));
	InGameLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(decal), cegui_reldim( 0.f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btn->setText(indicators[indicatoridx--] + GLoc->GetString("RESUMEGAME").c_str());

	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ZProtoGUI::HandleIngameMenuResume, this));
	decal -= 0.2f;
	*/


	Window* label2 = winMgr.createWindow("TaharezLook/StaticText", "IGMS/Label2");
	mSpectatorGUI->addChildWindow(label2);
	label2->setProperty("FrameEnabled", "false");
	label2->setProperty("BackgroundEnabled", "false");
	label2->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.00f)));
	label2->setSize(UVector2(cegui_reldim(0.4f), cegui_reldim( 1.f)));
	label2->setText(GLoc->GetString("SPECTATOR").c_str());
	label2->setVerticalAlignment(VA_CENTRE);
	label2->setHorizontalAlignment(HA_LEFT);

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGMS/Config" ));
	mSpectatorGUI->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btn->setText(String(".03 ") + GLoc->GetString("CONFIG").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiNetStates::HandleIngameSpectatorConfig, this));


	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGMS/Leave Game"));
	mSpectatorGUI->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btn->setText(String(".04 ") + GLoc->GetString("LEAVEGAME").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiNetStates::HandleSpectatorLeaveGame, this));
#endif
	// -- Net Player Selection --------------------------------------------------------------------

	mLanShipSel = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "LanShipSelection");
	mLanShipSel->setTitleBarEnabled(false);
	mLanShipSel->setCloseButtonEnabled(false);
	root->addChildWindow(mLanShipSel);
	mLanShipSel->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.20f)));
	mLanShipSel->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	mLanShipSel->setAlpha(0.88f);
	mLanShipSel->setSizingEnabled(false);
	mLanShipSel->hide();


	Window* label1 = winMgr.createWindow("TaharezLook/StaticText", "LSS/Label1");
	mLanShipSel->addChildWindow(label1);
	label1->setProperty("FrameEnabled", "false");
	label1->setProperty("BackgroundEnabled", "false");
	label1->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.0f)));
	label1->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 1.f)));
	label1->setText(GLoc->GetString("SELSHIP").c_str());
	label1->setVerticalAlignment(VA_CENTRE);
	label1->setHorizontalAlignment(HA_LEFT);


	// --

	mCBteam = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "LSS/TEAM"));
	mLanShipSel->addChildWindow(mCBteam);
	mCBteam->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.35f)));
	mCBteam->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 3.0f)));
	mCBteam->setReadOnly(true);
	mCBteam->hide();
	mCBteam->setClippedByParent(false);


	mCBship = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "LSS/SHIP"));
	mLanShipSel->addChildWindow(mCBship);
	mCBship->setPosition(UVector2(cegui_reldim(0.55f), cegui_reldim( 0.35f)));
	mCBship->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 3.0f)));
	mCBship->setReadOnly(true);
	mCBship->hide();
	mCBship->setClippedByParent(false);



	mCBteam->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiNetStates::handleSShipTeamChanger, this));
	mCBship->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiNetStates::handleSShipShipChanged, this));



	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LSS/OK"));
	mLanShipSel->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.83f), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 1.f)));
	btn->setText(String(".03 ") + GLoc->GetString("READY").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiNetStates::OnLanSelShipOK, this));

	// -- Net State Info -----------------------------------------------------------------------------

	mStateInfo = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "NetInfosWnd");
	mStateInfo->setTitleBarEnabled(false);
	mStateInfo->setCloseButtonEnabled(false);
	root->addChildWindow(mStateInfo);
	mStateInfo->setAlpha(0.88f);
	mStateInfo->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.05f)));
	mStateInfo->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.12f)));
	mStateInfo->setSizingEnabled(false);
	mStateInfo->hide();

	mNetInfosLib = winMgr.createWindow("TaharezLook/StaticText", "NetInfosWndLib");
	mStateInfo->addChildWindow(mNetInfosLib);
	mNetInfosLib->setProperty("FrameEnabled", "false");
	mNetInfosLib->setProperty("BackgroundEnabled", "false");
	mNetInfosLib->setPosition(UVector2(cegui_reldim(0.10f), cegui_reldim( 0.1f)));
	mNetInfosLib->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.8f)));
	mNetInfosLib->setText("");

	mNetInfosLib->setText(GLoc->GetString("WAITINGSERVERLOADING").c_str());

	mCountDownTime = 0.f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiNetStates::handleSShipShipChanged(const CEGUI::EventArgs& e)
{
	LoadSelShipShip(mSelShipCurTeam, (int)mCBship->getItemIndex (mCBship->getSelectedItem()));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::TeamChangedTo(unsigned int teamIdx)
{
	FillCBWithShips(mCBship, teamIdx);

	mSelShipCurTeam = teamIdx;
	mSelShipCurShip = 0;
	LoadSelShipShip(teamIdx, 0);
	CEGUICBSel(mCBship,0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiNetStates::handleSShipTeamChanger(const CEGUI::EventArgs& e)
{
	TeamChangedTo(mCBteam->getItemIndex (mCBteam->getSelectedItem()));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ValidateLanSelShip()
{
	if (mLanShipSel->isVisible())
	{
		HideSelShip();
		((ZRushGame*)GGame->GetClientGame())->GUIShipChosen();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiNetStates::OnLanSelShipOK(const CEGUI::EventArgs& e)
{
	ValidateLanSelShip();
	ZHideCursor();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
bool GuiNetStates::HandleIngameSpectatorConfig(const CEGUI::EventArgs& e)
{
	mSpectatorGUI->hide();
	mGUI->mConfigGui.Show(ZSubscriberSlot(&GuiNetStates::SpectatorBackFromConfig, this));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::SpectatorBackFromConfig()
{
	mSpectatorGUI->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiNetStates::HandleSpectatorLeaveGame(const CEGUI::EventArgs& e)
{
	mSpectatorGUI->hide();
	LeaveQuestion(ZSubscriberSlot(&GuiNetStates::OnSpectatorQuitYes, this), 
		ZSubscriberSlot(&GuiNetStates::OnSpectatorQuitNo, this));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::OnSpectatorQuitYes()
{
//	GetProtoGui()->LoadMainMenu();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::OnSpectatorQuitNo()
{
	mSpectatorGUI->show();
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiNetStates::OnPlaySpectator(const CEGUI::EventArgs& e)
{
	Hide();
	((ZRushGame*)GGame->GetClientGame())->GUIAsksForSpectator();
	mGUI->mSpectatorGui.Show();
	ZHideCursor();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiNetStates::OnAskSlot(const CEGUI::EventArgs& e)
{
	Hide();
	((ZRushGame*)GGame->GetClientGame())->GUIAsksForSlot();
	ZHideCursor();
//	((GameNet*)GClientGame)->SendMessageAskSlot();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::Tick()
{
	if (mCountDownTime>0)
	{
		mCountDownTime -= gTimer.GetEllapsed();
		if (mCountDownTime < 0)
		{
			DecStackCount();
			mStateInfo->hide();
		}
		else
		{
			char tmps[512];
			snprintf(tmps, 512, mCountDownString/*GLoc->GetString("BEFOREENDS")*/, mCountDownTime);
			mNetInfosLib->setText(tmps);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::HideSelShip()
{
	if (mLanShipSel->isVisible())
	{
		mLanShipSel->hide();
		mCBship->hide();
		mCBteam->hide();
		DecStackCount();
	}
}

void GuiNetStates::Hide()
{
	if (mStateInfo->isVisible())
	{
		mStateInfo->hide();
		DecStackCount();
	}

	HideSelShip();

	if (mPlayerSpectator->isVisible())
	{
		mPlayerSpectator->hide();
		DecStackCount();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowPlayerSpectator()
{
	IncStackCount();
	ZShowCursor();
 
	mPlayerSpectator->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::SetNbSlotsAvailable(int nbSlots)
{
	mNbSlotsAvailable = nbSlots;
	tstring runwithslots;
	runwithslots.Printf((char*)GLoc->GetString("DORUN").c_str(), mNbSlotsAvailable);
	mPlayerNbSlotsBtn->setText(runwithslots.c_str());

	if (mNbSlotsAvailable)
		mPlayerNbSlotsBtn->activate();
	else
		mPlayerNbSlotsBtn->deactivate();

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowLanShipSelection()
{
	FillCBWithTeams(mCBteam);
	FillCBWithShips(mCBship, 0);
	CEGUICBSel(mCBteam, 0);//((ZProtoGUI*)GetProtoGui())->mSelShipCurTeam);
	CEGUICBSel(mCBship, 0);


	mSelShipCurTeam = 0;
	mSelShipCurShip = 0;
	mLanShipSel->show();
	mCBship->show();
	mCBteam->show();
	/*
	mCBship->moveToFront();
	mCBteam->moveToFront();
	*/

	IncStackCount();
	ZShowCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowDeconnection(const char *szReason)
{
	mGUI->mMessageboxGui.Show(szReason,".01 OK", ZSubscriberSlot(&GuiNetStates::OnDeconnectionOK, this));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::OnDeconnectionOK()
{
	mGUI->mMainmenuGui.Show();
	mGUI->BuildGameSetForMenu();
	gCameraMotion.SetCameraUnused();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowUnableToConnect(const char *szReason)
{

	mGUI->mMessageboxGui.Show(szReason,".01 OK", ZSubscriberSlot(&GuiNetStates::OnUnableToConnectOK, this));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::OnUnableToConnectOK()
{
	mGUI->ShowLan(ZSubscriberSlot(&GuiMainmenu::OnLanBack, &mGUI->mMainmenuGui));
	//GameBuilder::BuildMainMenu();
//	mGUI->mLanGui.Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowTimeBeforeNextMap(float timebefore)
{
	IncStackCount();
	mStateInfo->show();
	mCountDownTime = timebefore;
	mCountDownString = GLoc->GetString("BEFORENEXTMAP");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowTimeBeforeCountDown(float timebefore)
{
	IncStackCount();
	mStateInfo->show();
	mCountDownTime = timebefore;
	mCountDownString = GLoc->GetString("BEFORECOUNT");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowConnectingToServer(const char *szServer)
{
	String ttlib1 = GLoc->GetString("CONSERV").c_str();//"Connecting server ";

	mGUI->mMessageboxGui.ShowWithAnimatedIcon((ttlib1 + szServer).c_str(), 
		String(String(".99 ")+GLoc->GetString("CANCEL")).c_str(),
		ZSubscriberSlot(&GuiNetStates::CancelConnectToServer,this));
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowTimeBeforeRaceEnds(float timebefore)
{
	IncStackCount();
	mStateInfo->show();
	mCountDownTime = timebefore;
	mCountDownString = GLoc->GetString("BEFOREENDS");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::ShowWaitingForPlayers()
{
	if (mCountDownTime<=0.f)
	{
		IncStackCount();
		mStateInfo->show();
		mCountDownTime = 9999;
		mCountDownString = GLoc->GetString("WAITINGOTHERS");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiNetStates::CancelConnectToServer()
{
	//GGame->CleanRushGames();
	mGUI->mMessageboxGui.Hide();
	mGUI->ShowLan(ZSubscriberSlot(&GuiMainmenu::OnLanBack, &mGUI->mMainmenuGui));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

