///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiEndResult.cpp
// Creation : 13/04/2009
// Author : Cedric Guillemet
// Description :
// Reliability :
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the te-r*ms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZGuiEndResults.h"
#include "ZProtoGUI.h"
#include "../libplatform/libplatform.h"
#include "../libbase/LibBase.h"

#include <CEGUI.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::Build(IProtoGUI *pGUI)
{

	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	PushButton* btn;


	// --


	ImagesetManager::getSingletonPtr()->createFromImageFile("logoWinnerFirst", "./Prototype/Common/Menu/First.tga");
	ImagesetManager::getSingletonPtr()->createFromImageFile("logoWinnerSecond", "./Prototype/Common/Menu/Second.tga");
	ImagesetManager::getSingletonPtr()->createFromImageFile("logoWinnerThird", "./Prototype/Common/Menu/Third.tga");



	endresfrm = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "ResultBackFrm");
	root->addChildWindow(endresfrm);
	// set position and size
	endresfrm->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.15f)));
	endresfrm->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.70f)));
	endresfrm->setTitleBarEnabled(false);
	endresfrm->setCloseButtonEnabled(false);
	endresfrm->setAlpha(0.88f);
	endresfrm->hide();
	endresfrm->setSizingEnabled(false);



	// here we will use a StaticImage as the root, then we can use it to place a background image
	logoWinner = winMgr.createWindow("TaharezLook/StaticImage", "endracebitmap");
	// set position and size
	logoWinner->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f)));
	logoWinner->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.5f)));
	// disable frame and standard background
	logoWinner->setProperty("FrameEnabled", "false");
	logoWinner->setProperty("BackgroundEnabled", "false");
	logoWinner->setAlpha(0.75f);
	// set the background image

	// install this as the root GUI sheet
	endresfrm->addChildWindow(logoWinner);


	// times
	label1 = winMgr.createWindow("TaharezLook/StaticText", "EndLibTime");
	endresfrm->addChildWindow(label1);
	label1->setProperty("FrameEnabled", "false");
	label1->setProperty("BackgroundEnabled", "false");
	label1->setPosition(UVector2(cegui_reldim(0.275f), cegui_reldim( 0.02f /*+ i*0.07f*/)));
	label1->setSize(UVector2(cegui_reldim(0.625f), cegui_reldim( 0.98f)));
	//label1->setText(stimes);
	label1->setText("");
	label1->setFont(mFontArial24);


	// SOLO : replay
	replayButton = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "ERREPLAYRACE"));
	endresfrm->addChildWindow(replayButton);
	replayButton->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.6f)));
	replayButton->setSize(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.06f)));
	replayButton->setText(GLoc->GetString("REPLAYRACE").c_str());
	replayButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiEndResult::OnReplayRace, this));
	replayButton->hide();

	// SOLO : next race
	nextRaceButton = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "ERNEXTRACE"));
	endresfrm->addChildWindow(nextRaceButton);
	nextRaceButton->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.7f)));
	nextRaceButton->setSize(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.06f)));
	nextRaceButton->setText(GLoc->GetString("NEXTRACE").c_str());
	nextRaceButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiEndResult::OnNextRace, this));
	nextRaceButton->hide();


	// SERVER when CLIENT/SERVER: goto lobby
	goToLobbyButton = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "gotolobby"));
	endresfrm->addChildWindow(goToLobbyButton);
	goToLobbyButton->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.6f)));
	goToLobbyButton->setSize(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.06f)));
	goToLobbyButton->setText(GLoc->GetString("GOTOLOBBY").c_str());
	goToLobbyButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiEndResult::OnServerGoToLobby, this));
	goToLobbyButton->hide();

	// EVERYONE : quit to main
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "ERQUITTOMAINMENU"));
	endresfrm->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.8f)));
	btn->setSize(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.06f)));
	btn->setText(GLoc->GetString("QUITTOMAINMENU").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiEndResult::QuitToMainMenu, this));

	// CLIENT Pure ones only
	waitingSyncLib = winMgr.createWindow("TaharezLook/StaticText", "waitingsynchro1");
	waitingSyncLib->setText(GLoc->GetString("WAITINGSERVERLOADING").c_str());
	waitingSyncLib->setProperty("FrameEnabled", "false");
	waitingSyncLib->setProperty("BackgroundEnabled", "false");
	waitingSyncLib->setPosition(UVector2(cegui_reldim(0.35f), cegui_reldim( 0.05f)));
	waitingSyncLib->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.06f)));
	endresfrm->addChildWindow(waitingSyncLib);
	waitingSyncLib->hide();

    // --
#if 0
	if (GServerGame)
	{
		/*
		btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "GOTOLOBBY"));
		root->addChildWindow(btn);
		btn->setPosition(UVector2(cegui_reldim(0.35f), cegui_reldim( 0.7f)));
		btn->setSize(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.06f)));
		btn->setText(GLoc->GetString("GOTOLOBBY").c_str());
		btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ZProtoGUI::OnServerGoToLobby, this));
		*/
	}
	else if (GameGeneric::GetInstanceGameMode() == GAME_IS_SOLO)
	{
		if (mServerAskedToGoToLobby)
		{
			ServerAskedToGoToLobby();
		}
		else
		{

		}
	}
	else
	{

		btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "REPLAYRACE"));
		root->addChildWindow(btn);
		btn->setPosition(UVector2(cegui_reldim(0.35f), cegui_reldim( 0.73f)));
		btn->setSize(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.06f)));
		btn->setText(GLoc->GetString("REPLAYRACE").c_str());
		btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ZProtoGUI::OnReplayRace, this));
/*
		if (this->IsCareerModeOn())
		{
			btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "BackToCareer" ));
			root->addChildWindow(btn);
			btn->setPosition(UVector2(cegui_reldim(0.35f), cegui_reldim( 0.81f)));
			btn->setSize(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.06f)));
			btn->setText(GLoc->GetString("BACK2CAREER").c_str());
			btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ZProtoGUI::HandleInGameBackToCareerEndMenu, this));
		}
		*/
	}
/*
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "QUITTOMAINMENU"));
	root->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.35f), cegui_reldim( 0.9f)));
	btn->setSize(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.06f)));à
	btn->setText(GLoc->GetString("QUITTOMAINMENU").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ZProtoGUI::OnSoloQuitToMainMenu, this));
*/

#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::Tick()
{
	if (GGame)
	{
		ZRushGame *pGame = ((ZRushGame*)GGame->GetClientGame());
		if (endresfrm->isVisible()&&pGame)
		{
			tstring strres = pGame->GetLapsTimes().BuildEndResults(pGame);
			label1->setText(strres.c_str());
			int aRanking = pGame->GetLapsTimes().GetShipRanking(pGame->GetClientShip());

			if (aRanking == 1)
				logoWinner->setProperty("Image", "set:logoWinnerFirst image:full_image");
			else if (aRanking == 2)
				logoWinner->setProperty("Image", "set:logoWinnerSecond image:full_image");
			else if (aRanking == 3)
				logoWinner->setProperty("Image", "set:logoWinnerThird image:full_image");

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::Show()
{
	IncStackCount();
	ZShowCursor();


	replayButton->hide();
	nextRaceButton->hide();
	waitingSyncLib->hide();
	goToLobbyButton->hide();

	ZRushGame *pclientGame = ((ZRushGame*)GGame->GetClientGame());
	// Solo:
	if (pclientGame->GetClassID() == ClassIDZRushGame)
	{
		replayButton->show();
		nextRaceButton->show();
	}
	
	// SERVER when CLIENT/SERVER: goto lobby
	if (GGame->GetServerGame())
	{
		goToLobbyButton->show();
	}

	// NETWORK CLIENT Pure ones only
	if (pclientGame && (!GGame->GetServerGame()) && (pclientGame->GetClassID() == ClassIDZRushNetGameClient) )
	{
		waitingSyncLib->show();
	}

	
	label1->setText("");
	endresfrm->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::Hide()
{
	DecStackCount();
	

	endresfrm->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::ForcedHide()
{
	if (endresfrm->isVisible())
	{
		Hide();
		ZHideCursor();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiEndResult::QuitToMainMenu(const CEGUI::EventArgs& e)
{
	Hide();

	mGUI->mMainmenuGui.Show();
	//GGame->CleanRushGames();
	mGUI->BuildGameSetForMenu();
	gCameraMotion.SetCameraUnused();//.SetCameraMapObserver();
	ZHideCursor();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiEndResult::OnReplayRace(const CEGUI::EventArgs& e)
{
	Hide();
	((ZRushGame*)GGame->GetClientGame())->RestartRace();
	ZHideCursor();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiEndResult::OnServerGoToLobby(const CEGUI::EventArgs& e)
{
	mGUI->ShowLanServer(ZSubscriberSlot(&GuiEndResult::OnlanBack, this));
	Hide();
	ZHideCursor();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::OnNextRaceBack()
{
	Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiEndResult::OnNextRace(const CEGUI::EventArgs& e)
{
	mGUI->ShowQuickRace(ZSubscriberSlot(&GuiEndResult::OnNextRaceBack, this));
	Hide();
	ZHideCursor();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiEndResult::OnlanBack()
{
	Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
