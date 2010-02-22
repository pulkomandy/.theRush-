///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiLan.cpp
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

#include "stdafx.h"
#include "ZGuiLan.h"
#include "ZProtoGUI.h"
#include "ZGuiUtils.h"
#include "ZRushGameContent.h"
#include "../libplatform/libplatform.h"
#include "ZGuiLapChoser.h"

#define ZENITH_TEST_SERVER "127.0.0.1"

///////////////////////////////////////////////////////////////////////////////////////////////////

GuiLan::~GuiLan()
{
	delete lapc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;


// Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
	mLanMainWindow = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Lan Main");
	mLanMainWindow->setTitleBarEnabled(false);
	mLanMainWindow->setCloseButtonEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	root->addChildWindow(mLanMainWindow);

	mLanMainWindow->hide();
	mLanMainWindow->setSizingEnabled(false);


	FrameWindow* wnd1 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "lantabPage0");
	wnd1->setTitleBarEnabled(false);
	wnd1->setFrameEnabled(false);//>setTitleBarEnabled(false);
	wnd1->setCloseButtonEnabled(false);
	wnd1->setText(GLoc->GetString("LAN").c_str());

	FrameWindow* wnd2 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "lantabPage1");
	wnd2->setTitleBarEnabled(false);
	wnd2->setFrameEnabled(false);//>setTitleBarEnabled(false);
	wnd2->setCloseButtonEnabled(false);
	wnd2->setText(GLoc->GetString("INTERNET").c_str());


	FrameWindow* wnd3 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "lantabPage2");
	wnd3->setTitleBarEnabled(false);
	wnd3->setCloseButtonEnabled(false);
	wnd3->setText(GLoc->GetString("FAVORITES").c_str());
	wnd3->setFrameEnabled(false);//
/*
	FrameWindow* wnd4 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "tabPage3");
	wnd4->setTitleBarEnabled(false);
	wnd4->setFrameEnabled(false);//>setTitleBarEnabled(false);
	wnd4->setCloseButtonEnabled(false);
	wnd4->setText(GLoc->GetString("SOUND").c_str());
	//wnd->addChildWindow (winMgr.loadWindowLayout ("TabControlDemo.layout", "TabControlDemo/"));
*/
	TabControl *tc = (TabControl *)winMgr.createWindow("TaharezLook/TabControl", "Lan/Tabs");

	mLanMainWindow->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.f)));
	mLanMainWindow->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.9f)));
	tc->setArea(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.1f)), 
		UVector2(cegui_reldim(1.f), cegui_reldim( 0.8f)) );




	// Add some pages to tab control
	tc->addTab (wnd1);
	tc->addTab (wnd2);
	tc->addTab (wnd3);
	/*
	tc->addTab (wnd4);
*/
	tc->setTabHeight(UDim (0.06f, 0.1f));
	tc->setTabTextPadding(UDim (0.06f, 0.1f));


	mLanMainWindow->addChildWindow(tc);


	mLanMainWindow->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.15f)));
	mLanMainWindow->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.70f)));
	mLanMainWindow->setAlpha(0.88f);


	wnd1->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd1->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));


	wnd2->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd2->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));

	wnd3->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd3->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));

    // create the main list.
    MultiColumnList* mcl1 = static_cast<MultiColumnList*>(winMgr.createWindow("TaharezLook/MultiColumnList", "ML/LAN"));
    wnd1->addChildWindow(mcl1);
    mcl1->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f)));
    mcl1->setSize(UVector2(cegui_reldim(0.98f), cegui_reldim( 0.97f)));

    mcl1->addColumn(GLoc->GetString("NAME").c_str(), 100, cegui_reldim(0.4f));
	mcl1->addColumn(GLoc->GetString("PLAYERS").c_str(), 101, cegui_reldim(0.1f));
	mcl1->addColumn(GLoc->GetString("STATUS").c_str(), 102, cegui_reldim(0.1f));
	mcl1->addColumn(GLoc->GetString("TRACK").c_str(), 103, cegui_reldim(0.15f));
	mcl1->addColumn(GLoc->GetString("MODE").c_str(), 104, cegui_reldim(0.15f));
	mcl1->addColumn(GLoc->GetString("PING").c_str(), 105, cegui_reldim(0.08f));

    MultiColumnList* mcl2 = static_cast<MultiColumnList*>(winMgr.createWindow("TaharezLook/MultiColumnList", "ML/INTERNET"));
    wnd2->addChildWindow(mcl2);
    mcl2->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f)));
    mcl2->setSize(UVector2(cegui_reldim(0.98f), cegui_reldim( 0.97f)));

    mcl2->addColumn(GLoc->GetString("NAME").c_str(), 100, cegui_reldim(0.4f));
	mcl2->addColumn(GLoc->GetString("PLAYERS").c_str(), 101, cegui_reldim(0.1f));
	mcl2->addColumn(GLoc->GetString("STATUS").c_str(), 102, cegui_reldim(0.1f));
	mcl2->addColumn(GLoc->GetString("TRACK").c_str(), 103, cegui_reldim(0.15f));
	mcl2->addColumn(GLoc->GetString("MODE").c_str(), 104, cegui_reldim(0.15f));
	mcl2->addColumn(GLoc->GetString("PING").c_str(), 105, cegui_reldim(0.08f));	


    mclFavorites = static_cast<MultiColumnList*>(winMgr.createWindow("TaharezLook/MultiColumnList", "ML/FAVORITES"));
    wnd3->addChildWindow(mclFavorites);
    mclFavorites->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.01f)));
    mclFavorites->setSize(UVector2(cegui_reldim(0.98f), cegui_reldim( 0.97f)));

    mclFavorites->addColumn(GLoc->GetString("NAME").c_str(), 100, cegui_reldim(0.4f));
	mclFavorites->addColumn(GLoc->GetString("PLAYERS").c_str(), 101, cegui_reldim(0.1f));
	mclFavorites->addColumn(GLoc->GetString("STATUS").c_str(), 102, cegui_reldim(0.1f));
	mclFavorites->addColumn(GLoc->GetString("TRACK").c_str(), 103, cegui_reldim(0.15f));
	mclFavorites->addColumn(GLoc->GetString("MODE").c_str(), 104, cegui_reldim(0.15f));
	mclFavorites->addColumn(GLoc->GetString("PING").c_str(), 105, cegui_reldim(0.08f));	

/*
	AddServer(mcl3, ZENITH_TEST_SERVER, 1, "-", "-", "-", 999);
	AddServer(mcl3, "zalem.zapto.org", 1, "-", "-", "-", 999);
	*/
	RefreshFavorites();

	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LANJOIN"));
	mLanMainWindow->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.77f), cegui_reldim( 0.90f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btn->setText(GLoc->GetString("LANJOIN").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanJoin, this));

	PushButton* btnbk = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LANback"));
	mLanMainWindow->addChildWindow(btnbk);
	btnbk->setPosition(UVector2(cegui_reldim(0.035f), cegui_reldim( 0.90f)));
	btnbk->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btnbk->setText(GLoc->GetString("BACK").c_str());
	btnbk->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanBack, this));


	PushButton* btncreate = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LANCREATE"));
	mLanMainWindow->addChildWindow(btncreate);
	btncreate->setPosition(UVector2(cegui_reldim(0.035f), cegui_reldim( 0.02f)));
	btncreate->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btncreate->setText(GLoc->GetString("LANCREATE").c_str());
	btncreate->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanCreate, this));

	PushButton* btnaddserv = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "ADDSERVER"));
	mLanMainWindow->addChildWindow(btnaddserv);
	btnaddserv->setPosition(UVector2(cegui_reldim(0.335f), cegui_reldim( 0.02f)));
	btnaddserv->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btnaddserv->setText(GLoc->GetString("ADDSERVER").c_str());
	btnaddserv->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanAddServ, this));

	PushButton* btnrefresh = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "REFRESHLIST"));
	mLanMainWindow->addChildWindow(btnrefresh);
	btnrefresh->setPosition(UVector2(cegui_reldim(0.635f), cegui_reldim( 0.02f)));
	btnrefresh->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btnrefresh->setText(GLoc->GetString("REFRESH").c_str());
	btnrefresh->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanRefresh, this));

	// lan refresh
	DoLanRefresh();
	//mCurrentState = GUI_LANMAIN;

	BuildAddServer();

	BuildLanServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::BuildAddServer()
{
// Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
	AddServerFrm = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "LANADDSERVERMODAL");
	AddServerFrm->setTitleBarEnabled(false);
	AddServerFrm->setCloseButtonEnabled(false);
	root->addChildWindow(AddServerFrm);
	AddServerFrm->setAlpha(0.88f);

	AddServerFrm->setPosition(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.33f)));
	AddServerFrm->setSize(UVector2(cegui_reldim(0.68f), cegui_reldim( 0.33f)));


	AddServerFrm->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.30f)));
	AddServerFrm->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	AddServerFrm->setSizingEnabled(false);

	//AddServerFrm->setText(szTitle);

	Window* label2 = winMgr.createWindow("TaharezLook/StaticText", "LANADDSERV/Label2");
	AddServerFrm->addChildWindow(label2);
	label2->setProperty("FrameEnabled", "false");
	label2->setProperty("BackgroundEnabled", "false");
	label2->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05f)));
	label2->setSize(UVector2(cegui_reldim(0.65f), cegui_reldim( 0.5f)));
	label2->setText(GLoc->GetString("TYPESERVER").c_str());

	urlBoxAddServer = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "LANADDSERVEREDIT"));
	AddServerFrm->addChildWindow(urlBoxAddServer);
	urlBoxAddServer->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.5f)));
	urlBoxAddServer->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.4f)));
	//urlBox->setValidationString("*");
	urlBoxAddServer->setText("");



	PushButton* btnYes = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LAOK"));
	AddServerFrm->addChildWindow(btnYes);
	btnYes->setPosition(UVector2(cegui_reldim(0.70f), cegui_reldim( 0.f)));
	btnYes->setSize(UVector2(cegui_reldim(0.15f), cegui_reldim( 1.f)));
	btnYes->setText(String(".01 ")+GLoc->GetString("OK").c_str());
	btnYes->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanAddServOK, this));


	PushButton* btnNo = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LACANCEL"));
	AddServerFrm->addChildWindow(btnNo);
	btnNo->setPosition(UVector2(cegui_reldim(0.85f), cegui_reldim( 0.f)));
	btnNo->setSize(UVector2(cegui_reldim(0.15f), cegui_reldim( 1.f)));
	btnNo->setText(String(".02 ")+GLoc->GetString("CANCEL").c_str());
	btnNo->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanAddServCancel, this));

	AddServerFrm->hide();
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::BuildLanServer()
{
	mMenuCurrentTrackWorld=mMenuCurrentTrackIndex=-1;
	// --

	mMainServerWindow = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "LS/Lan Main");
	mMainServerWindow->setTitleBarEnabled(false);
	mMainServerWindow->setCloseButtonEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	// --
	mMainServerWindow->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.15f)));
	mMainServerWindow->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.70f)));

	root->addChildWindow(mMainServerWindow);
	mMainServerWindow->setAlpha(0.88f);
	mMainServerWindow->setSizingEnabled(false);
	mMainServerWindow->hide();


	// Thumbnail image

	srvthmbimg = winMgr.createWindow("TaharezLook/StaticImage", "LS/ServerThumbImg");
	srvthmbimg->setPosition(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.01f)));
	srvthmbimg->setSize(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.3f)));
	srvthmbimg->setProperty("FrameEnabled", "false");
	srvthmbimg->setProperty("BackgroundEnabled", "false");
	//srvthmbimg->setProperty("Image", "set:ThumbImageImageSet image:full_image");
	mMainServerWindow->addChildWindow(srvthmbimg);
	srvthmbimg->setAlpha(1.0f);

	// world
    cbworld = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "SERVERWORLD"));
    mMainServerWindow->addChildWindow(cbworld);
    cbworld->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.01f)));
    cbworld->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.33f)));
	cbworld->setReadOnly(true);
	cbworld->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&GuiLan::LanServerWorldChanged, this));

	Window* txtwlib = winMgr.createWindow("TaharezLook/StaticText", "SRVWST");

	txtwlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.00f)));
    txtwlib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.08f)));
	txtwlib->setText(GLoc->GetString("WORLD").c_str());
	txtwlib->setProperty("FrameEnabled", "false");
	txtwlib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txtwlib);




	// map
    cbtrack = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "SERVERTRACK"));
    mMainServerWindow->addChildWindow(cbtrack);
    cbtrack->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.10f)));
    cbtrack->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.33f)));
	cbtrack->setReadOnly(true);
	cbtrack->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&GuiLan::LanServerTrackChanged, this));

	

	Window* txttlib = winMgr.createWindow("TaharezLook/StaticText", "SRVMST");

	txttlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.08f)));
    txttlib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.08f)));
	txttlib->setText(GLoc->GetString("TRACK").c_str());
	txttlib->setProperty("FrameEnabled", "false");
	txttlib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txttlib);
	
	// game mode
    cbgamemode = BuildGameModesCombo("LanServerCombo");
		
    mMainServerWindow->addChildWindow(cbgamemode);
    cbgamemode->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.19f)));
    cbgamemode->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.33f)));
	

	lapc = new ZGuiLapChoser();
	lapc->Init(mMainServerWindow, "lanserver", GLoc->GetString("NLAPS"), 1, 9, 3);
    lapc->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.27f)));
    lapc->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.14f)));



	Window* txtgtlib = winMgr.createWindow("TaharezLook/StaticText", "SRVGTST");

	txtgtlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.17f)));
    txtgtlib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.10f)));
	txtgtlib->setText(GLoc->GetString("GAMETYPE").c_str());
	txtgtlib->setProperty("FrameEnabled", "false");
	txtgtlib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txtgtlib);

	


	
/*
    Spinner* nblaps = (Spinner*)winMgr.createWindow("TaharezLook/Spinner", "xxspinlabs");
    mMainServerWindow->addChildWindow(nblaps);
    nblaps->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.27f)));
    nblaps->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.06f)));
	nblaps->setTextInputMode(Spinner::Integer);
	nblaps->setCurrentValue(3);
	nblaps->setStepSize(1);
	nblaps->setMaximumValue (9);
	nblaps->setMinimumValue(1);
*/

	Window* txtnbllib = winMgr.createWindow("TaharezLook/StaticText", "txtnbllib");
	txtnbllib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.25f)));
    txtnbllib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.10f)));
	txtnbllib->setText(GLoc->GetString("LAPS").c_str());
	txtnbllib->setProperty("FrameEnabled", "false");
	txtnbllib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txtnbllib);
	



	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LS/LANJOIN"));
	mMainServerWindow->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.77f), cegui_reldim( 0.90f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btn->setText(GLoc->GetString("LAUNCH").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanLaunchServer, this));

	mLanBack = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LS/LANback"));
	mMainServerWindow->addChildWindow(mLanBack);
	mLanBack->setPosition(UVector2(cegui_reldim(0.035f), cegui_reldim( 0.90f)));
	mLanBack->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	mLanBack->setText(GLoc->GetString("BACK").c_str());
	mLanBack->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanEndRaceBackToMain, this));
	

	BuildChat(mMainServerWindow);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::Tick()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////


void GuiLan::ConnectToServer(const char *szURL)
{
	String ttlib1 = GLoc->GetString("CONSERV").c_str();//"Connecting server ";

	mGUI->mMessageboxGui.Show((ttlib1 + szURL).c_str(), 
		String(String(".99 ")+GLoc->GetString("CANCEL")).c_str(),
		ZSubscriberSlot(&GuiLan::CancelConnectToServer,this));

	GGame->BuildClient();
	GGame->Connect(szURL, 49888);
//	GameBuilder::BuildClient(szURL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::CancelConnectToServer()
{
	mGUI->mMessageboxGui.Hide();
//	GameBuilder::BuildMainMenu();
	mLanMainWindow->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::handleConnectToServer(const CEGUI::EventArgs& e)
{
	tstring mPlayerName;
	Window *pplf = winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/playerNameBox");
	if (pplf)
		mPlayerName = GConfig->GetPlayerName();

	String aURLStr = winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/logURLBox")->getText();
	

	ConnectToServer(aURLStr.c_str());
	// event was handled
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::OnOptionsFromLobby(const CEGUI::EventArgs& e)
{
	mMainServerWindow->hide();
	mGUI->mConfigGui.Show(ZSubscriberSlot(&GuiLan::BackFromLobbyConfig,this));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::BackFromLobbyConfig()
{
	mGUI->mConfigGui.Hide();
	mMainServerWindow->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void GuiLan::CleanLanServers()
{
	MultiColumnList* mcl1 = static_cast<MultiColumnList*>(winMgr.getWindow("ML/LAN"));
	mcl1->resetList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::AddLanServer(const char *serverName, int nbPlayers, 
					  const char *szStatus, const char *szMapName, const char *szMode, int aPing)
{
	MultiColumnList* mcl1 = static_cast<MultiColumnList*>(winMgr.getWindow("ML/LAN"));
	AddServer(mcl1, serverName, nbPlayers, szStatus, szMapName, szMode, aPing);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::AddServer(MultiColumnList* mcl, const char *serverName, int nbPlayers, 
						  const char *szStatus, const char *szMapName, const char *szMode, int aPing)
{	
	char iit1[512];
	char iit2[512];
	sprintf_s(iit1, 512, "%d", nbPlayers);
	sprintf_s(iit2, 512, "%d", aPing);

	int rowid = 0;
    ListboxTextItem* item = new ListboxTextItem(serverName);
    item->setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush"));
	rowid = mcl->addRow(item, 100);
    item = new ListboxTextItem(iit1);
    item->setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush"));
	mcl->setItem(item, 101, rowid);
    item = new ListboxTextItem(szStatus);
    item->setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush"));
	mcl->setItem(item, 102, rowid);
    item = new ListboxTextItem(szMapName);
    item->setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush"));
	mcl->setItem(item, 103, rowid);
    item = new ListboxTextItem(szMode);
    item->setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush"));
	mcl->setItem(item, 104, rowid);
    item = new ListboxTextItem(iit2);
    item->setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush"));
	mcl->setItem(item, 105, rowid);
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanJoin(const CEGUI::EventArgs& e)
{
	//LanJoin
	mLanMainWindow->hide();
	
	if (winMgr.isWindowPresent("Lan/Tabs"))
	{
		TabControl *tc = (TabControl *)winMgr.getWindow("Lan/Tabs");
		switch(tc->getSelectedTabIndex ())
		{
		case 0:
			{
				MultiColumnList* mcl1 = static_cast<MultiColumnList*>(winMgr.getWindow("ML/LAN"));
				ListboxTextItem *it = (ListboxTextItem*)mcl1->getFirstSelectedItem();
				if (it)
				{
					tstring tt = it->getText().c_str();
					tt.Replace(':',0);
					ConnectToServer(tt.c_str());
				}
			}
			break;
		case 1:
			break;
		case 2:
			//
			{
				MultiColumnList* mcl3 = static_cast<MultiColumnList*>(winMgr.getWindow("ML/FAVORITES"));
				ListboxTextItem *it = (ListboxTextItem*)mcl3->getFirstSelectedItem();
				if (it)
				{
					ConnectToServer(it->getText().c_str());
				}
			} 
			//
			break;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanBack(const CEGUI::EventArgs& e)
{
	Hide();
	mGUI->mMainmenuGui.Show();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::CleanLanLobby()
{
	mMainServerWindow->hide();
	mGUI->mConfigGui.Hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanAddServ(const CEGUI::EventArgs& e)
{
	mLanMainWindow->setVisible(false);
	AddServerFrm->show();

	//LanAddServ
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::RefreshFavorites()
{
	mclFavorites->resetList();
	for (int i=0; i< GConfig->GetNbFavoriteURL(); i++)
	{
		AddServer(mclFavorites, GConfig->GetFavoriteURL(i), 1, "-", "-", "-", 999);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanAddServOK(const CEGUI::EventArgs& e)
{
	mLanMainWindow->setVisible(true);
	AddServerFrm->hide();
	// sent ask info to server

	GConfig->AddFavoriteURL(urlBoxAddServer->getText().c_str());

	TabControl *tc = (TabControl *)winMgr.getWindow("Lan/Tabs");
	tc->setSelectedTabAtIndex(2);

	RefreshFavorites();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanAddServCancel(const CEGUI::EventArgs& e)
{
	mLanMainWindow->setVisible(true);
	AddServerFrm->hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::DoLanRefresh()
{
	if (winMgr.isWindowPresent("REFRESHLIST"))
	{
		if (winMgr.isWindowPresent("Lan/Tabs"))
		{
			TabControl *tc = (TabControl *)winMgr.getWindow("Lan/Tabs");
			switch(tc->getSelectedTabIndex ())
			{
			case 0:
				/*
				//LanRefresh
				if (!gConnectedServerBrowse)
				{
					gConnectedServerBrowse = GetNet()->ActAsServer(0);
				}
				*/
				CleanLanServers();
				//((INetServer*)gConnectedServerBrowse)->Ping(ZENITH_GAME_UDP_PORT);
				break;
			case 1:
				break;
			default:
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanRefresh(const CEGUI::EventArgs& e)
{
	DoLanRefresh();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanLaunchServer(const CEGUI::EventArgs& e)
{
	mMainServerWindow->hide();
	
	//Combobox* cbworld = static_cast<Combobox*>(winMgr.getWindow("SERVERWORLD"));
	//Combobox* cbtrack = static_cast<Combobox*>(winMgr.getWindow("SERVERTRACK"));
	//Combobox* cbgamemode = static_cast<Combobox*>(winMgr.getWindow("SERVERGAMETYPE"));
	//Spinner* nblapsSpinner = static_cast<Spinner*>(winMgr.getWindow("spinlabs"));



		

		int worldIdx	=  cbworld->getItemIndex(cbworld->getSelectedItem());//>getUserData();	
		int trackIdx	=  cbtrack->getItemIndex(cbtrack->getSelectedItem());

		tstring trackname = GetTrackFileName( worldIdx, trackIdx);

		if (!GGame->HasClientServer())
			GGame->BuildClientServer(49888);
		GGame->SetGameSet(GameSet_t(gRushGameContent->GetWorlds()[worldIdx], trackname, GetGameMode(cbgamemode), true, lapc->GetValue() ));
#if 0
		const char * const	worldName	= cbworld->getSelectedItem()->getText().c_str();
		CEGUI::String		*trackName	= "";//(CEGUI::String*)cbtrack->getSelectedItem()->getUserData();
		const char * const	typeName	= cbgamemode->getSelectedItem()->getText().c_str();
		int nbLaps = (int)nblapsSpinner->getCurrentValue();

		const size_t worldNameSize  = strlen(worldName);
		const size_t trackNameSize = trackName->size();
		const size_t typeNameSize  = strlen(typeName);

		if (worldIdx && worldNameSize > 0  && trackName && trackNameSize > 0 && typeNameSize > 0 )
		{
			/*
			GameplayMap_t agp(trackName->c_str(), nbLaps, 0);
			
			//GServerState->SetGameplay(agp);
			GClientState->SetGameplay(agp);
			*/
			return true;
		}
#endif
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void GuiLan::BuildLanClient()
{

	mMainServerWindow = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Lan Main");
	mMainServerWindow->setTitleBarEnabled(false);
	mMainServerWindow->setCloseButtonEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	root->addChildWindow(mMainServerWindow);
	mMainServerWindow->setAlpha(0.88f);

/*
	// world
    Combobox* cbworld = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "SERVERWORLD"));
    mMainServerWindow->addChildWindow(cbworld);
    cbworld->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.06f)));
    cbworld->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.33f)));
	cbworld->setReadOnly(true);

	Window* txtwlib = winMgr.createWindow("TaharezLook/StaticText", "SRVWST");

	txtwlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.04f)));
    txtwlib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.08f)));
	txtwlib->setText(GLoc->GetString("WORLD").c_str());
	txtwlib->setProperty("FrameEnabled", "false");
	txtwlib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txtwlib);


	for (int i=0;i<gContentManager->GetWorldCount();i++)
	{
		CEGUI::ListboxTextItem *it = new ListboxTextItem(gContentManager->GetWorldName(i));
		int * iPtr = new int(i);
		it->setUserData((void*) iPtr);
		cbworld->addItem (it);
	}
	cbworld->setItemSelectState ((size_t )0,true);
	cbworld->setText(gContentManager->GetWorldName(0));


	// map
    Combobox* cbtrack = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "SERVERTRACK"));
    mMainServerWindow->addChildWindow(cbtrack);
    cbtrack->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.15f)));
    cbtrack->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.33f)));
	cbtrack->setReadOnly(true);

	Window* txttlib = winMgr.createWindow("TaharezLook/StaticText", "SRVMST");

	txttlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.13f)));
    txttlib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.08f)));
	txttlib->setText(GLoc->GetString("TRACK").c_str());
	txttlib->setProperty("FrameEnabled", "false");
	txttlib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txttlib);
	
	for (int i=0;i<gContentManager->GetTrackCount(0);i++)
	{
		tstring trackn = gContentManager->GetTrackName(0, i);		
		CEGUI::ListboxTextItem *it = new ListboxTextItem(trackn.GetBaseName().c_str());
		CEGUI::String *filepath = new String(trackn.c_str());
		it->setUserData ((void*)filepath);
		cbtrack->addItem (it);
	}
	cbtrack->setItemSelectState ((size_t )3,true);
	cbtrack->setText(tstring(gContentManager->GetTrackName(0, 3)).GetBaseName().c_str());

	// game mode
    Combobox* cbgamemode = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "SERVERGAMETYPE"));
    mMainServerWindow->addChildWindow(cbgamemode);
    cbgamemode->setPosition(UVector2(cegui_reldim(0.18f), cegui_reldim( 0.24f)));
    cbgamemode->setSize(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.33f)));
	cbgamemode->setReadOnly(true);

	Window* txtgtlib = winMgr.createWindow("TaharezLook/StaticText", "SRVGTST");

	txtgtlib->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.22f)));
    txtgtlib->setSize(UVector2(cegui_reldim(0.16f), cegui_reldim( 0.08f)));
	txtgtlib->setText(GLoc->GetString("GAMETYPE").c_str());
	txtgtlib->setProperty("FrameEnabled", "false");
	txtgtlib->setProperty("BackgroundEnabled", "false");
	mMainServerWindow->addChildWindow(txtgtlib);

	ListboxTextItem *lbgm = new ListboxTextItem(GLoc->GetString("SIMPLERACE").c_str());
	cbgamemode->addItem (lbgm);
	cbgamemode->addItem (new ListboxTextItem(GLoc->GetString("SURVIVOR").c_str()));

	cbgamemode->setItemSelectState ((size_t )0, true);
	cbgamemode->setText(GLoc->GetString("SIMPLERACE").c_str());
*/
	// --
	mMainServerWindow->setPosition(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.05f)));
	mMainServerWindow->setSize(UVector2(cegui_reldim(0.88f), cegui_reldim( 0.9f)));
/*
	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LANJOIN"));
	mMainServerWindow->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.77f), cegui_reldim( 0.90f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btn->setText(GLoc->GetString("LAUNCH").c_str());
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanLaunchServer, this));
*/
	PushButton* btnbk = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LANback"));
	mMainServerWindow->addChildWindow(btnbk);
	btnbk->setPosition(UVector2(cegui_reldim(0.035f), cegui_reldim( 0.90f)));
	btnbk->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btnbk->setText(GLoc->GetString("BACK").c_str());
	btnbk->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::LanBack, this));


	BuildChat(mMainServerWindow);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanCreate(const CEGUI::EventArgs& e)
{
	//LanCreate
	mLanMainWindow->hide();
	mMainServerWindow->show();
	ThumbnailChange(srvthmbimg, 0, 0);

	// build client/serveur
//	GameBuilder::BuildClientServer("");
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanServerWorldChanged(const CEGUI::EventArgs& e)
{

	Combobox* cbworld = static_cast<Combobox*>(winMgr.getWindow("SERVERWORLD"));
	if (!winMgr.isWindowPresent("SERVERTRACK")) 
		return true;
	Combobox* cbtrack = static_cast<Combobox*>(winMgr.getWindow("SERVERTRACK"));



	int wnb = (cbworld->getItemIndex (cbworld->getSelectedItem()));

	FillTrackListComboBox(cbtrack, wnb);


	ThumbnailChange(srvthmbimg, 
		wnb,
		(cbtrack->getItemIndex (cbtrack->getSelectedItem())));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanServerTrackChanged(const CEGUI::EventArgs& e)
{

	Combobox* cbworld = static_cast<Combobox*>(winMgr.getWindow("SERVERWORLD"));
	Combobox* cbtrack = static_cast<Combobox*>(winMgr.getWindow("SERVERTRACK"));

	if ( (!cbtrack->getSelectedItem()) || (!cbworld->getSelectedItem())) 
		return true;


	ThumbnailChange(srvthmbimg, 
		(cbworld->getItemIndex (cbworld->getSelectedItem())),
		(cbtrack->getItemIndex (cbtrack->getSelectedItem())));
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::BuildChatClient(Window *parent)
{
	FrameWindow* wnd;
	float aHeight=1;
	if (!parent)
	{
		wnd = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "ZChatWindow");
		wnd->setTitleBarEnabled(false);
		wnd->setCloseButtonEnabled(false);
		// Here we attach the newly created FrameWindow to the previously created
		// DefaultWindow which we will be using as the root of the displayed gui.
		root->addChildWindow(wnd);
	wnd->setPosition(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.05f)));
	wnd->setSize(UVector2(cegui_reldim(0.88f), cegui_reldim( 0.9f)));

	}
	else
	{
		wnd = (FrameWindow*)parent;
		aHeight = 0.5f;
	}




	Editbox* speakBox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/speakBox"));
	wnd->addChildWindow(speakBox);

	//speakBox->setValidationString("*");
	speakBox->setText("");


	MultiLineEditbox* chatBox = static_cast<MultiLineEditbox *>(winMgr.createWindow("TaharezLook/MultiLineEditbox", "Demo6/ControlPanel/ColumnPanel/chatBox"));
	wnd->addChildWindow(chatBox);


	Listbox *nicksList = static_cast<Listbox *>(winMgr.createWindow("TaharezLook/Listbox", "nicklistbox"));
	wnd->addChildWindow(nicksList);



	chatBox->setReadOnly(true);

	wnd->setAlpha(0.88f);
	speakBox->subscribeEvent(Editbox::EventTextAccepted, Event::Subscriber(&GuiLan::handleTextChat, this));

	if (parent)
	{
		speakBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.80f)));
		speakBox->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.06f)));
		chatBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.35f)));
		chatBox->setSize(UVector2(cegui_reldim(0.66f), cegui_reldim( 0.44f)));

		nicksList->setPosition(UVector2(cegui_reldim(0.67f), cegui_reldim( 0.35f)));
		nicksList->setSize(UVector2(cegui_reldim(0.33f), cegui_reldim( 0.44f)));
	}
	else
	{
		speakBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.9f)));
		speakBox->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.066f)));
		chatBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.1f)));
		chatBox->setSize(UVector2(cegui_reldim(0.66f), cegui_reldim( 0.79f)));

		nicksList->setPosition(UVector2(cegui_reldim(0.67f), cegui_reldim( 0.1f)));
		nicksList->setSize(UVector2(cegui_reldim(0.33f), cegui_reldim( 0.79f)));

	}



}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::BuildChat(Window *parent)
{
	FrameWindow* wnd;
	float aHeight=1;
	if (!parent)
	{
		wnd = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "ZChatWindow");
		wnd->setTitleBarEnabled(false);
		wnd->setCloseButtonEnabled(false);
		// Here we attach the newly created FrameWindow to the previously created
		// DefaultWindow which we will be using as the root of the displayed gui.
		root->addChildWindow(wnd);
		wnd->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.5)));
		wnd->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.5f)));

	}
	else
	{
		wnd = (FrameWindow*)parent;
		aHeight = 0.5f;
	}




	Editbox* speakBox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/speakBox"));
	wnd->addChildWindow(speakBox);

	//speakBox->setValidationString("*");
	speakBox->setText("");


	MultiLineEditbox* chatBox = static_cast<MultiLineEditbox *>(winMgr.createWindow("TaharezLook/MultiLineEditbox", "Demo6/ControlPanel/ColumnPanel/chatBox"));
	wnd->addChildWindow(chatBox);


	Listbox *nicksList = static_cast<Listbox *>(winMgr.createWindow("TaharezLook/Listbox", "nicklistbox"));
	wnd->addChildWindow(nicksList);



	chatBox->setReadOnly(true);

	wnd->setAlpha(0.88f);
	speakBox->subscribeEvent(Editbox::EventTextAccepted, Event::Subscriber(&GuiLan::handleTextChat, this));

	if (parent)
	{
		speakBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.80f)));
		speakBox->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.06f)));
		chatBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.35f)));
		chatBox->setSize(UVector2(cegui_reldim(0.66f), cegui_reldim( 0.44f)));

		nicksList->setPosition(UVector2(cegui_reldim(0.67f), cegui_reldim( 0.35f)));
		nicksList->setSize(UVector2(cegui_reldim(0.33f), cegui_reldim( 0.44f)));
	}
	else
	{
		speakBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.9f)));
		speakBox->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.1f)));
		chatBox->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.f)));
		chatBox->setSize(UVector2(cegui_reldim(0.66f), cegui_reldim( 0.9f)));

		nicksList->setPosition(UVector2(cegui_reldim(0.67f), cegui_reldim( 0.9f)));
		nicksList->setSize(UVector2(cegui_reldim(0.33f), cegui_reldim( 0.9f)));

	}

	PushButton* btnbk = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "LANOPTIONS"));
	wnd->addChildWindow(btnbk);
	btnbk->setPosition(UVector2(cegui_reldim(0.4f), cegui_reldim( 0.90f)));
	btnbk->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btnbk->setText(GLoc->GetString("CONFIG").c_str());
	btnbk->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiLan::OnOptionsFromLobby, this));

	// update nicks list
	/*
	INetMessageHandler *pHand = ((INetMessageHandler*)FirstInstanceOf(ZProtoNetHandlerClient));
	if (pHand)
		pHand->UpdateGUINickList();
		*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::handleTextChat(const CEGUI::EventArgs& e)
{
	String spktxt = winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/speakBox")->getText();

	//AppendChatString(spktxt.c_str());

	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/speakBox")->setText("");

	// message
	/*
	INetMessage *pMsg = gConnectedClient->CreateOutgoingMessage(ZENITH_CHAT_ALL);
	pMsg->PushString(spktxt.c_str());
	*/
	//((GameNet*)GClientGame)->SendMessageChatString(spktxt.c_str());

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiLan::LanEndRaceBackToMain(const CEGUI::EventArgs& e)
{
	mMainServerWindow->hide();
	mLanMainWindow->hide();
	mOnBack();
	//mGUI->mMainmenuGui.Show();
//	GameBuilder::BuildMainMenu();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::Show(const ZSubscriberSlot& onBack)
{
	mOnBack = onBack;
	IncStackCount();
	ZShowCursor();
	mLanMainWindow->show();

	FillWorldListComboBox(cbworld);
	FillTrackListComboBox(cbtrack, 0);

	CEGUICBSel(cbworld, 0);//((ZProtoGUI*)GetProtoGui())->mSelShipCurTeam);
	CEGUICBSel(cbtrack, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void GuiLan::ShowLanServer( const ZSubscriberSlot& onBack )
{
	mOnBack = onBack;
	IncStackCount();
	ZShowCursor();

	FillWorldListComboBox(cbworld);
	FillTrackListComboBox(cbtrack, 0);

	CEGUICBSel(cbworld, 0);//((ZProtoGUI*)GetProtoGui())->mSelShipCurTeam);
	CEGUICBSel(cbtrack, 0);

	ThumbnailChange(srvthmbimg, 0, 0);

	mMainServerWindow->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiLan::Hide()
{
	DecStackCount();
	mLanMainWindow->hide();
	ZHideCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#if 0


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::GoForCurrentSelectedShip()
{
	if (winMgr.isWindowPresent("LanShipSelection"))
		winMgr.getWindow("LanShipSelection")->destroy();
	if (winMgr.isWindowPresent("TEAM"))
		winMgr.getWindow("TEAM")->destroy();
	if (winMgr.isWindowPresent("SHIP"))
		winMgr.getWindow("SHIP")->destroy();

	// send message to server
	/*
	INetMessage *pMsg = gConnectedClient->CreateOutgoingMessage(ZENITH_PLAYER_SELECTEDSHIP);
	pMsg->PushString(mCurrentLanShipString);
	*/
	GClientGame->SendMessageShipSelected(mCurrentLanShipString.c_str(), GClientGame->mClientShip);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZProtoGUI::OnLanSelShipOK(const CEGUI::EventArgs& e)
{
	GoForCurrentSelectedShip();
	ShowWaitingPlayers();
	/*
	//GetGame()->SetObservedShip(NULL);

	// set new state
	GetGame()->GetHumanShip()->GetCamera()->SetActive();²&²²²²²²²²
	GetHUD()->Enable(true);
	*/
	mCurrentState = GUI_UNDEF;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::LanTeamChangedTo(unsigned int teamIdx)
{
	Combobox *cbship = static_cast<Combobox*>(winMgr.getWindow("SHIP"));

	tstring maketeam = "Team|";
	maketeam += GetGameContent()->GetEntry("Team", teamIdx);
	for (int j=0;j<GetGameContent()->GetNumberOfEntries(maketeam.c_str());j++)
	{
		//Zenith.GetGameContent():AddContent("Team|TeamTest","DebugShip")
		const char *szship = GetGameContent()->GetEntry(maketeam.c_str(), j);
		cbship->addItem (new ListboxTextItem(szship));

	}
	mSelShipCurTeam = teamIdx;
	mSelShipCurShip = 0;
	InLanGameLoadSelShipShip(teamIdx, 0);
	CEGUICBSel(cbship,0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZProtoGUI::handleLanSShipTeamChanged(const CEGUI::EventArgs& e)
{
	static_cast<Combobox*>(winMgr.getWindow("SHIP"))->resetList();
	Combobox *pcb = static_cast<Combobox*>(winMgr.getWindow("TEAM"));
	LanTeamChangedTo((int)pcb->getItemIndex (pcb->getSelectedItem()));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZProtoGUI::handleLanSShipShipChanged(const CEGUI::EventArgs& e)
{
	Combobox *pcb = static_cast<Combobox*>(winMgr.getWindow("SHIP"));
	mSelShipCurShip = (int)pcb->getItemIndex (pcb->getSelectedItem());
	InLanGameLoadSelShipShip(mSelShipCurTeam, mSelShipCurShip);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::InLanGameLoadSelShipShip(unsigned int aTeam, unsigned int aShip)
{
	tstring cs = gContentManager->GetShipCreationString(aTeam, aShip);

	if (cs != mCurrentLanShipString)
	{
		
		mCurrentLanShipString = cs;
		/*
		GetGame()->RemoveShip(GetGame()->GetHumanShip());
		INetMessageHandler *pHand = ((INetMessageHandler*)FirstInstanceOf(ZProtoNetHandlerClient));
		if (pHand)
			pHand->RecreateHumanShip(cs.c_str());
		//GetGame()->SetObservedShip(GetGame()->GetHumanShip(), 10);
		*/
		LUA()->CallShipCreationMethod(GClientGame->mClientShip, mCurrentLanShipString.c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::ShowLanShipSelection(IRaceShip *pShip)
{
	mCurrentLanShipString = "";
	InLanGameLoadSelShipShip(0, 0);
	mCurrentState = GUI_LANSELSHIP;


	//GetGame()->SetObservedShip(GetGame()->GetHumanShip(), 10);

	if (winMgr.isWindowPresent("LanShipSelection"))
		winMgr.getWindow("LanShipSelection")->destroy();

	
}

#endif



