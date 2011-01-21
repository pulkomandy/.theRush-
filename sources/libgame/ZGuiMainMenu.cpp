///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiMainmenu.cpp
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
#include "ZGuiMainMenu.h"
#include "ZProtoGUI.h"
#include "ZLocalisation.h"
#include "CameraMotion.h"
#include "ZGuiUtils.h"
//#include "ZGuiMusic.h"
#include "ZGuiConfig.h"
#include "ZGuiProgress.h"
#include "ZGuiMessagebox.h"
#include "ZGuiMainMenu.h"
#include "ZGuiStatistics.h"
#include "ZGuiCredits.h"
#include "ZGuiQuickrace.h"
#include "ZGuiIngameMenu.h"
//#include "ZGuiConsoleChat.h"

//extern GameContentManager *gContentManager;
//extern GuiMusic mMusicGui;
extern GuiConfig mConfigGui;
extern GuiProgress mProgressGui;
extern GuiMessagebox mMessageboxGui;
extern GuiMainmenu mMainmenuGui;
//extern GuiQuickrace mQuickraceGui;
//extern GuiStatistics mStatisticsGui;
extern GuiCredits mCreditsGui;
extern GuiIngameMenu mIngameMenuGui;
//extern GuiConsoleChat mConsoleChatGui;

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	ImagesetManager::getSingleton().createImagesetFromImageFile("logoRush", "./Prototype/Common/Menu/MenuLogo.tga");



	logoRush = winMgr.createWindow("TaharezLook/StaticImage", "logorushwnd");
	logoRush->setPosition(UVector2(cegui_reldim(0.70f), cegui_reldim( 0.89f)));
	logoRush->setSize(UVector2(cegui_reldim(0.30f), cegui_reldim( 0.11f)));
	logoRush->setProperty("FrameEnabled", "false");
	logoRush->setProperty("BackgroundEnabled", "false");
	logoRush->setAlpha(0.75f);
	logoRush->setProperty("Image", "set:logoRush image:full_image");
	root->addChildWindow(logoRush);
	logoRush->hide();
	((FrameWindow*)logoRush)->setSizingEnabled(false);



	// here we will use a StaticImage as the root, then we can use it to place a background image
	MainLeftMenu = winMgr.createWindow("TaharezLook/FrameWindow", "MAIN_wnd");
	root->addChildWindow(MainLeftMenu);

	// set position and size
	MainLeftMenu->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.30f)));
	MainLeftMenu->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	((FrameWindow*)MainLeftMenu)->setTitleBarEnabled(false);
	((FrameWindow*)MainLeftMenu)->setCloseButtonEnabled(false);
	MainLeftMenu->setAlpha(0.88f);
	MainLeftMenu->setFont(mFontArial8);
	((FrameWindow*)MainLeftMenu)->setSizingEnabled(false);
	MainLeftMenu->hide();
	((FrameWindow*)MainLeftMenu)->setSizingEnabled(false);


	//Career
	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Career"));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".01 \n")+GLoc->GetString("CAREER").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleCareer, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);


	// quick race
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "quick race"));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f + 0.125*1), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".02 \n")+GLoc->GetString("QUICKRACE").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleQuickRace, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);

	// lan/internet
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "laninternet"));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f + 0.125*2), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".03 \n")+GLoc->GetString("LANINTERNET").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleBuildLanMain, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);

	// Stats
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Stats"));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f + 0.125*3), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".04 \n")+GLoc->GetString("Statistics").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleStatsMain, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);


	// config
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Config" ));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f + 0.125*4), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".05 \n")+GLoc->GetString("CONFIG").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleBuildConfig, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);

	// credits
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "credits" ));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f + 0.125*5), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".06 \n")+GLoc->GetString("CREDITS").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleCredits, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);


	// quit
	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Quit"));
	MainLeftMenu->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.0625f + 0.125*6), cegui_reldim( 0.0f)));
	btn->setSize(UVector2(cegui_reldim(0.125f), cegui_reldim( 1.f)));
	btn->setText(CEGUI::String(".07 \n")+GLoc->GetString("LEAVEGAME").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMainmenu::HandleLeaveProto, this));
	btn->setVerticalAlignment(VA_CENTRE);
	btn->setHorizontalAlignment(HA_LEFT);
	btn->setFont(mFontArial8);

}



///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::Show()
{
	IncStackCount();
	ZShowCursor();
	logoRush->show();
	MainLeftMenu->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::Tick()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::Hide()
{
	ZHideCursor();
	DecStackCount();
	logoRush->hide();
	MainLeftMenu->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::OnLanBack()
{
	Show();
}

bool GuiMainmenu::HandleBuildLanMain(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	mGUI->mLanGui.Show(ZSubscriberSlot(&GuiMainmenu::OnLanBack, this));
	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiMainmenu::HandleBuildConfig(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	mGUI->mConfigGui.Show(ZSubscriberSlot(&GuiMainmenu::OnConfigOK, this));
	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiMainmenu::HandleLeaveProto(const CEGUI::EventArgs& e)
{
	
	/*
	String lib = ".99 ";
	lib += GLoc->GetString("LEAVEGAME").c_str();
	String Yes = ".01 ";
	Yes += GLoc->GetString("YES").c_str();
	String No = ".02 ";
	No += GLoc->GetString("NO").c_str();

	mGUI->mMessageboxGui.Show(lib.c_str(), Yes.c_str(), ZSubscriberSlot(&GuiMainmenu::OnQuitYes, this),
		No.c_str(), ZSubscriberSlot(&GuiMainmenu::OnQuitNo, this));
		*/
	LeaveQuestion(ZSubscriberSlot(&GuiMainmenu::OnQuitYes, this), ZSubscriberSlot(&GuiMainmenu::OnQuitNo, this));
	Hide();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::QuickRaceBackToMainMenu()
{
	Show();
	gCameraMotion.SetCameraMapObserverMenu();
}

bool GuiMainmenu::HandleQuickRace(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	mGUI->mQuickraceGui.Show(ZSubscriberSlot(&GuiMainmenu::QuickRaceBackToMainMenu, this));
	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiMainmenu::HandleCareer(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	/*
	Hide();
	//mGUI->mEndResultGui.Show(2);
	mGUI->mIngameMenuGui.Show();
*/
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiMainmenu::HandleStatsMain(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	mGUI->mStatisticsGui.Show();
	Hide();
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiMainmenu::HandleCredits(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	mGUI->mCreditsGui.Show();
	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMainmenu::OnQuitYes()
{
	ZManager->AskToQuit();
}

void GuiMainmenu::OnQuitNo()
{
	Show();
}

void GuiMainmenu::OnConfigOK()
{
	Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
