///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiSpectator.cpp
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

#include "stdafx.h"
#include "ZGuiSpectator.h"
#include "ZProtoGUI.h"
#include "../libplatform/libplatform.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	// Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
	SpecWindow = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "SpectatorWindow");
	SpecWindow->setTitleBarEnabled(false);
	SpecWindow->setCloseButtonEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	root->addChildWindow(SpecWindow);
	SpecWindow->setAlpha(0.6f);
	SpecWindow->setSizingEnabled(false);

	SpecWindow->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.10f)));
	SpecWindow->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.18f)));
	SpecWindow->setAlpha(0.88f);


	txtlib = winMgr.createWindow("TaharezLook/StaticText", "spectalib");
	txtlib->setProperty("FrameEnabled", "false");
	txtlib->setProperty("BackgroundEnabled", "false");
	txtlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.05f)));
	txtlib->setSize(UVector2(cegui_reldim(0.98f), cegui_reldim( 0.3f )));
	txtlib->setFont(mFontArial24);
	SpecWindow->addChildWindow(txtlib);







	Window *cbPlayerLib = winMgr.createWindow("TaharezLook/StaticText", "cbPlayerLib");
	cbPlayerLib->setProperty("FrameEnabled", "false");
	cbPlayerLib->setProperty("BackgroundEnabled", "false");
	cbPlayerLib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.40f)));
	cbPlayerLib->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.2f )));
	cbPlayerLib->setFont(mFontArial8);
	SpecWindow->addChildWindow(cbPlayerLib);
	cbPlayerLib->setText(String(".01 ")+GLoc->GetString("PLAYER"));

	Window *cbCameraLib = winMgr.createWindow("TaharezLook/StaticText", "cbCameraLib");
	cbCameraLib->setProperty("FrameEnabled", "false");
	cbCameraLib->setProperty("BackgroundEnabled", "false");
	cbCameraLib->setPosition(UVector2(cegui_reldim(0.51f), cegui_reldim( 0.40f)));
	cbCameraLib->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.2f )));
	cbCameraLib->setFont(mFontArial8);
	SpecWindow->addChildWindow(cbCameraLib);
	cbCameraLib->setText(String(".02 ")+GLoc->GetString("CAMERA"));

	// combo player
	cbPlayer = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "Spec/cbPlayer"));
	SpecWindow->addChildWindow(cbPlayer);
	cbPlayer->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.40f)));
	cbPlayer->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 3.0f)));
	cbPlayer->setReadOnly(true);
	cbPlayer->setClippedByParent(false);
	cbPlayer->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiSpectator::handleCameraModeChanger, this));

	// combo camera
	cbCamera = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "Spec/cbCamera"));
	SpecWindow->addChildWindow(cbCamera);
	cbCamera->setPosition(UVector2(cegui_reldim(0.73f), cegui_reldim( 0.40f)));
	cbCamera->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 3.0f)));
	cbCamera->setReadOnly(true);   
	cbCamera->setClippedByParent(false);
	cbCamera->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiSpectator::handleCameraModeChanger, this));


	cbCamera->addItem (new ListboxTextItem(GLoc->GetString("COCKPIT").c_str()));
	cbCamera->addItem (new ListboxTextItem(GLoc->GetString("FOLLOWER").c_str()));
	cbCamera->addItem (new ListboxTextItem(GLoc->GetString("ORBITINGNEAR").c_str()));
	cbCamera->addItem (new ListboxTextItem(GLoc->GetString("ORBITINGFAR").c_str()));

	cbCamera->setItemSelectState ((size_t )2,true);
	cbCamera->setText(GLoc->GetString("ORBITINGNEAR").c_str());


	// ask slot

	mPlayerNbSlotsBtn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "SpectatorPlayerBtn"));
	SpecWindow->addChildWindow(mPlayerNbSlotsBtn);
	mPlayerNbSlotsBtn->setPosition(UVector2(cegui_reldim(0.73f), cegui_reldim( 0.67f)));
	mPlayerNbSlotsBtn->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.28f)));
	mPlayerNbSlotsBtn->setFont(mFontArial8);
	mPlayerNbSlotsBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiSpectator::OnAskSlot, this));



	/*
* combo nicknames + lib ".01 Player"
* combo tracktype + lib ".02 Camera"
* bouton askslot
* lib ".00 XXX//Spectator"
*/
/*
	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "StatsOK"));
	SpecWindow->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.68f), cegui_reldim( 0.91f)));
	btn->setSize(UVector2(cegui_reldim(0.29f), cegui_reldim( 0.06f)));
	btn->setText("Ok");
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiStatistics::OnStatsOK, this));
*/
	SpecWindow->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiSpectator::handleCameraModeChanger(const CEGUI::EventArgs& e)
{
	ZRushGame * pGame = ((ZRushGame*)GGame->GetClientGame());
	if (!pGame->GetShips().empty())
	{
		int playerNb =  cbPlayer->getItemIndex(cbPlayer->getSelectedItem());
		int cameraModeNb =  cbCamera->getItemIndex(cbCamera->getSelectedItem());

		ZShip *pShipSelected = (*pGame->GetShips().begin());
		std::list<smartptr<ZShip> >::const_iterator iter = pGame->GetShips().begin();
		for (int avs = 0; avs<playerNb; ++iter,avs++)
		{
			pShipSelected = (*iter);
		}
		switch (cameraModeNb)
		{
		case 0:
			gCameraMotion.SetCameraPiloting(pShipSelected->GetTransform());
			gCameraMotion.SetInCockpit(true);
			break;
		case 1:
			gCameraMotion.SetCameraPiloting(pShipSelected->GetTransform());
			gCameraMotion.SetInCockpit(false);
			break;
		case 2:
			gCameraMotion.SetCameraOrbitingClose(pShipSelected->GetTransform());
			break;
		case 3:
			gCameraMotion.SetCameraOrbiting(pShipSelected->GetTransform());
			break;
		}

	}
	else
	{
		gCameraMotion.SetCameraMapObserver();
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::NicknameChanged()
{
	tstring specLib;
	specLib.Printf(".00 %s // %s", GConfig->GetPlayerName(), GLoc->GetString("SPECTATOR2").c_str());
	txtlib->setText(specLib.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiSpectator::OnAskSlot(const CEGUI::EventArgs& e)
{
	mGUI->mIngameMenuGui.Hide();
	((ZRushGame*)GGame->GetClientGame())->GUIAsksForSlot();
	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::Tick()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::UpdateComboboxPlayers()
{
	cbPlayer->resetList();

	ZRushGame * pGame = ((ZRushGame*)GGame->GetClientGame());
	if (!pGame->GetShips().empty())
	{
		std::list<smartptr<ZShip> >::const_iterator iter = pGame->GetShips().begin();
		for (; iter != pGame->GetShips().end(); ++iter)
		{
			cbPlayer->addItem ( new ListboxTextItem( pGame->GetNickName( (*iter) ) ) );
		}



		cbPlayer->setItemSelectState ((size_t )0,true);
	}
	else
	{
		gCameraMotion.SetCameraMapObserver();
	}
	//cbCamera->setText(GLoc->GetString("COCKPIT").c_str());
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::Show()
{
	NicknameChanged();
	IncStackCount();
	ZShowCursor();
	SpecWindow->show();
	handleCameraModeChanger(CEGUI::EventArgs());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::Hide()
{
	if (SpecWindow->isVisible())
	{
		SpecWindow->hide();
		DecStackCount();
		ZHideCursor();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiSpectator::SetNbSlotsAvailable(int nbSlots)
{
	//mNbSlotsAvailable = nbSlots;
	tstring runwithslots;
	runwithslots.Printf((char*)GLoc->GetString("DORUN").c_str(), nbSlots);
	mPlayerNbSlotsBtn->setText(runwithslots.c_str());

	if (nbSlots)
		mPlayerNbSlotsBtn->activate();
	else
		mPlayerNbSlotsBtn->deactivate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
bool GuiSpectator::OnStatsOK(const CEGUI::EventArgs& e)
{
	mGUI->mMainmenuGui.Show();
	Hide();

	return true;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
