///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiQuickrace.cpp
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
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZGuiQuickrace.h"

#include "ZProtoGUI.h"
#include "IProtoGUI.h"
#include "ZGuiUtils.h"
#include "ZGuiLapChoser.h"
#include "ZLocalisation.h"
#include "CameraMotion.h"
#include "ZRushGame.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

GuiQuickrace::~GuiQuickrace()
{
	delete lapc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiQuickrace::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	// quick race frame
	qrf = (FrameWindow *)winMgr.createWindow("TaharezLook/FrameWindow", "QUICKRACE_wnd");
	root->addChildWindow(qrf);
	qrf->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.20f)));
	qrf->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.25f)));
	qrf->setTitleBarEnabled(false);
	qrf->setCloseButtonEnabled(false);
	qrf->setAlpha(0.88f);
	qrf->hide();
	qrf->setSizingEnabled(false);

	// --

	Window* label1 = winMgr.createWindow("TaharezLook/StaticText", "LSS/QR01");
	qrf->addChildWindow(label1);
	label1->setProperty("FrameEnabled", "false");
	label1->setProperty("BackgroundEnabled", "false");
	label1->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.07f)));
	label1->setSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.08f)));
	label1->setText(GLoc->GetString(".01 Track").c_str());

	label1 = winMgr.createWindow("TaharezLook/StaticText", "LSS/QR02");
	qrf->addChildWindow(label1);
	label1->setProperty("FrameEnabled", "false");
	label1->setProperty("BackgroundEnabled", "false");
	label1->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.32f)));
	label1->setSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.08f)));
	label1->setText(GLoc->GetString(".02 Mode").c_str());

	label1 = winMgr.createWindow("TaharezLook/StaticText", "LSS/QR03");
	qrf->addChildWindow(label1);
	label1->setProperty("FrameEnabled", "false");
	label1->setProperty("BackgroundEnabled", "false");
	label1->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.57f)));
	label1->setSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.08f)));
	label1->setText(GLoc->GetString(".03 Ship").c_str());



	// team
    cbteam = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "QRTEAM"));
    qrf->addChildWindow(cbteam);
	cbteam->setPosition(UVector2(cegui_reldim(0.45f), cegui_reldim( 0.55f)));
    cbteam->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.8f)));

	cbteam->setReadOnly(true);
	cbteam->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiQuickrace::handleSShipTeamChanger, this));
	cbteam->setClippedByParent(false);

	//ship

    cbship = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "QRSHIP"));
    qrf->addChildWindow(cbship);
	cbship->setPosition(UVector2(cegui_reldim(0.17f), cegui_reldim( 0.55f)));
	cbship->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.70f)));
	cbship->setReadOnly(true);
	cbship->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiQuickrace::handleSShipShipChanged, this));
	cbship->setClippedByParent(false);

	// game modes
	cbgamemode = BuildGameModesCombo("QuickRaceCombo");
    qrf->addChildWindow(cbgamemode);
    cbgamemode->setPosition(UVector2(cegui_reldim(0.17f), cegui_reldim( 0.30f)));
    cbgamemode->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.70f)));


	lapc = new ZGuiLapChoser();
	lapc->Init(qrf, "quickrace", GLoc->GetString("NLAPS"), 1, 9, 3);
    lapc->setPosition(UVector2(cegui_reldim(0.45f), cegui_reldim( 0.32f)));
    lapc->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.14f)));

	// --


	// world
    cbworld = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "QRWORLD"));
    qrf->addChildWindow(cbworld);
	cbworld->setPosition(UVector2(cegui_reldim(0.17f), cegui_reldim( 0.05f)));
	cbworld->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.80f)));

	cbworld->setReadOnly(true);
	cbworld->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&GuiQuickrace::LanServerWorldChanged, this));
	cbworld->setClippedByParent(false);

	// map
	
    cbtrack = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "QRTRACK"));
    qrf->addChildWindow(cbtrack);
    cbtrack->setPosition(UVector2(cegui_reldim(0.45f), cegui_reldim( 0.05f)));
    cbtrack->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.7f)));
	cbtrack->setReadOnly(true);
	cbtrack->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&GuiQuickrace::LanServerTrackChanged, this));
	cbtrack->setClippedByParent(false);

	// buttons

	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "SELSHIPBACK"));
	qrf->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.17f), cegui_reldim( 0.72f)));
	btn->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.20f)));
	btn->setText(GLoc->GetString("BACK").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiQuickrace::OnSelShipBack, this));

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "SELSHIPOK"));
	qrf->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.45f), cegui_reldim( 0.72f)));
	btn->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.2f)));
	btn->setText("Ok");
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiQuickrace::OnQuickRaceOK, this));

	// Thumbnail image

	srvthmbimg = winMgr.createWindow("TaharezLook/StaticImage", "QR/ServerThumbImg");
	srvthmbimg->setPosition(UVector2(cegui_reldim(0.72f), cegui_reldim( 0.05f)));
	srvthmbimg->setSize(UVector2(cegui_reldim(0.27f), cegui_reldim( 0.90f)));
	srvthmbimg->setProperty("FrameEnabled", "false");
	srvthmbimg->setProperty("BackgroundEnabled", "false");
	//srvthmbimg->setProperty("Image", "set:ThumbImageImageSet image:full_image");
	qrf->addChildWindow(srvthmbimg);
	srvthmbimg->setAlpha(1.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiQuickrace::Tick()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiQuickrace::Show(const ZSubscriberSlot& onBack)
{
	mOnBack = onBack;
	//ThumbnailChange(srvthmbimg, 0, 0);
	gCameraMotion.SetCameraOrbitingClose(((ZRushGame*)GGame->GetClientGame())->GetFirstShip()->GetTransform());
/*
	mSelShipCurTeam = mSelShipCurShip = 0;
	mMenuCurrentTrackWorld = mMenuCurrentTrackIndex = 0;
*/
	// world/tracks list
	FillWorldListComboBox( cbworld );
	FillTrackListComboBox( cbtrack, 0 );
/*
	CEGUICBSel(cbteam, mSelShipCurTeam);
	CEGUICBSel(cbship, mSelShipCurShip);
	*/
	// teams/ships list
	FillCBWithTeams( cbteam );


	// current track thumbnail
	ThumbnailChange( srvthmbimg, 0, 0 );


	FillCBWithShips( cbship, 0 );

	TeamChangedTo( 0 );

	CEGUICBSel( cbteam, 0 );
	CEGUICBSel( cbship, 0 );



	//LoadSelShipShip(0, 0);
	qrf->show();
	IncStackCount();
	ZShowCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiQuickrace::Hide()
{
	DecStackCount();
	qrf->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiQuickrace::handleSShipShipChanged(const CEGUI::EventArgs& e)
{
	//Combobox *pcb = static_cast<Combobox*>(winMgr.getWindow("QRSHIP"));
	int shipIdx = cbship->getItemIndex( cbship->getSelectedItem() );
	int teamIdx = cbteam->getItemIndex( cbteam->getSelectedItem() );
	//mSelShipCurShip = (int)pcb->getItemIndex (pcb->getSelectedItem());
	LoadSelShipShip( teamIdx, shipIdx );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiQuickrace::TeamChangedTo(unsigned int teamIdx)
{
	//Combobox *cbship = static_cast<Combobox*>(winMgr.getWindow("QRSHIP"));

	FillCBWithShips(cbship, teamIdx);
/*
	mSelShipCurTeam = teamIdx;
	mSelShipCurShip = 0;
	*/
	LoadSelShipShip(teamIdx, 0);
	CEGUICBSel(cbship,0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiQuickrace::handleSShipTeamChanger(const CEGUI::EventArgs& e)
{
	static_cast<Combobox*>(winMgr.getWindow("QRSHIP"))->resetList();
	Combobox *pcb = static_cast<Combobox*>(winMgr.getWindow("QRTEAM"));
	TeamChangedTo((int)pcb->getItemIndex (pcb->getSelectedItem()));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiQuickrace::OnSelShipBack(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayCancelSound();
	Hide();
	mOnBack();
	ZHideCursor();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiQuickrace::OnQuickRaceOK(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	Hide();
	ZHideCursor();

	int wIdx = cbworld->getItemIndex( cbworld->getSelectedItem() );
	int trackIdx = cbtrack->getItemIndex( cbtrack->getSelectedItem() );
	tstring trackname = GetTrackFileName( wIdx, trackIdx );

	int teamIdx = cbteam->getItemIndex( cbteam->getSelectedItem() );
	int shipIdx = cbship->getItemIndex( cbship->getSelectedItem() );

	mGUI->SetChosenShipString( gRushGameContent->GetShip( teamIdx, shipIdx ) );
	GGame->BuildSolo();
	GGame->SetGameSet( GameSet_t( gRushGameContent->GetWorlds()[wIdx], trackname, GetGameMode(cbgamemode), true, lapc->GetValue() ) );
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiQuickrace::LanServerWorldChanged(const CEGUI::EventArgs& e)
{
	/*
	mMenuCurrentTrackWorld = ( cbworld->getItemIndex (cbworld->getSelectedItem()) );
	mMenuCurrentTrackIndex = ( cbtrack->getItemIndex (cbtrack->getSelectedItem()) );
	*/
	int trackIdx = cbtrack->getItemIndex( cbtrack->getSelectedItem() );
	int worldIdx = cbworld->getItemIndex( cbworld->getSelectedItem() );

	FillTrackListComboBox( cbtrack, worldIdx );

	ThumbnailChange( srvthmbimg, worldIdx, trackIdx );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiQuickrace::LanServerTrackChanged(const CEGUI::EventArgs& e)
{
	if ( (!cbtrack->getSelectedItem()) || (!cbworld->getSelectedItem()))
		return true;
/*
	mMenuCurrentTrackWorld = (cbworld->getItemIndex (cbworld->getSelectedItem()));
	mMenuCurrentTrackIndex = (cbtrack->getItemIndex (cbtrack->getSelectedItem()));
	*/
	int trackIdx = cbtrack->getItemIndex( cbtrack->getSelectedItem() );
	int worldIdx = cbworld->getItemIndex( cbworld->getSelectedItem() );


	ThumbnailChange( srvthmbimg, worldIdx, trackIdx );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

