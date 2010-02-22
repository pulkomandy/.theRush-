///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiStatistics.cpp
// Creation : 11/04/2009
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
#include "ZGuiStatistics.h"
#include "ZProtoGUI.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiStatistics::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	// Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
	StatWindow = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "StatWindow");
	StatWindow->setTitleBarEnabled(false);
	StatWindow->setCloseButtonEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	root->addChildWindow(StatWindow);
	StatWindow->setAlpha(0.6f);

	StatWindow->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.15f)));
	StatWindow->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.70f)));
	StatWindow->setAlpha(0.88f);
	StatWindow->setSizingEnabled(false);


	txtlib = winMgr.createWindow("TaharezLook/StaticText", "SoloStatString");


	txtlib->setProperty("FrameEnabled", "false");
	txtlib->setProperty("BackgroundEnabled", "false");
	//txtlib->setHorizontalAlignment(HA_CENTRE);
	txtlib->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05f)));
	txtlib->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 0.86f )));

	StatWindow->addChildWindow(txtlib);




	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "StatsOK"));
	StatWindow->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.68f), cegui_reldim( 0.91f)));
	btn->setSize(UVector2(cegui_reldim(0.29f), cegui_reldim( 0.06f)));
	btn->setText("Ok");
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiStatistics::OnStatsOK, this));

	StatWindow->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiStatistics::Tick()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiStatistics::Show()
{
	IncStackCount();
	ZShowCursor();
	tstring szs = "Stats";//GetSoloGame()->BuildPlayerSolorSate(0);
	txtlib->setText(szs.c_str());

	StatWindow->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiStatistics::Hide()
{
	StatWindow->hide();
	DecStackCount();
	ZHideCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiStatistics::OnStatsOK(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();

	mGUI->mMainmenuGui.Show();
	Hide();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
