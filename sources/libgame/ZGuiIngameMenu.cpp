///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiIngameMenu.cpp
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

#include "ZGuiIngameMenu.h"

#include "../libbase/ZPlatformUtils.h"
#include "ZProtoGUI.h"
#include "ZGuiUtils.h"
//extern GameContentManager *gContentManager;

///////////////////////////////////////////////////////////////////////////////////////////////////

float GetInputValueReleased(const char *szCtrl);

Combobox* cbtrack;
	bool GuiIngameMenu::handleChangeTrack(const CEGUI::EventArgs& e)
	{
		Hide();

		ListboxItem *resib = cbtrack->getSelectedItem();
		if (resib)
		{
			int selres = cbtrack->getItemIndex(resib);

			if (selres == 0)
				GGame->SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track", GameSet_t::GAME_TYPE_CLASSIC));
			else
				GGame->SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos2.track", GameSet_t::GAME_TYPE_CLASSIC));
		}

		return true;
	}


void GuiIngameMenu::Build(ZProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	
	InGameLeftMenu = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "IngameMenuFRM");
	((FrameWindow*)InGameLeftMenu)->setTitleBarEnabled(false);
	((FrameWindow*)InGameLeftMenu)->setCloseButtonEnabled(false);
	root->addChildWindow(InGameLeftMenu);
	InGameLeftMenu->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.30f)));
	InGameLeftMenu->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	InGameLeftMenu->setAlpha(0.88f);
	InGameLeftMenu->hide();
	((FrameWindow*)InGameLeftMenu)->setSizingEnabled(false);


	// Resume
	btnResume = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGM/Resume Game"));
	InGameLeftMenu->addChildWindow(btnResume);
	btnResume->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btnResume->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiIngameMenu::HandleResume, this));

	// config
	btnConfig = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGM/Config" ));
	InGameLeftMenu->addChildWindow(btnConfig);
	btnConfig->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btnConfig->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiIngameMenu::HandleConfig, this));

	//if (GameGeneric::GetInstanceGameMode() == GAME_IS_SOLO)
	{
		btnRestart = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGM/Restart" ));
		InGameLeftMenu->addChildWindow(btnRestart);
		btnRestart->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
		btnRestart->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiIngameMenu::HandleRestart, this));
	}

	//if (mCareerModeOn)
	{
		btnCareer = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGM/BackToCareer" ));
		InGameLeftMenu->addChildWindow(btnCareer);
		btnCareer->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
		btnCareer->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiIngameMenu::HandleBackToCareer, this));
		

	}


	btnLeave = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "IGM/Leave Game"));
	InGameLeftMenu->addChildWindow(btnLeave);
	btnLeave->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btnLeave->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiIngameMenu::HandleLeave, this));


	// Fake combo pour changement de map
	cbtrack = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "QRtrack"));
    root->addChildWindow(cbtrack);
	cbtrack->setPosition(UVector2(cegui_reldim(0.35f), cegui_reldim( 0.2f)));
	cbtrack->setSize(UVector2(cegui_reldim(0.30f), cegui_reldim( 0.80f)));
	cbtrack->setReadOnly(true);
	cbtrack->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiIngameMenu::handleChangeTrack, this));

	char * trackNames[]={"Stratos 1", "Stratos 2"};

	for (int i=0;i<2;i++)
	{
		CEGUI::ListboxTextItem *it = new ListboxTextItem(trackNames[i]);
		int * iPtr = new int(i);
		//it->setUserData((void*) iPtr);
		cbtrack->addItem (it);
	}
	cbtrack->setItemSelectState ((size_t )0,true);
	cbtrack->setText(trackNames[0]);

	cbtrack->hide();

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::Tick()
{
	/*
	static int CNTREL = 0;
	if (GetInputDevice()->KeyReleased(TKEY_ESC))
		LOG("%d RELEASED§§§§\n", CNTREL++);
		*/

	if (mShouldTick)
	{


		static const float mSwitchSpeed = 2.f;
		if (mbInGameMenuVisible)
		{
			mInterp += gTimer.GetEllapsed() * mSwitchSpeed;
		}
		else
		{
			mInterp -= gTimer.GetEllapsed() * mSwitchSpeed;
			if (mInterp <= 0)
			{
				//GetHUD()->Enable(true);
				mShouldTick = false;
			}
		}

		mInterp = Clamp(mInterp, 0.f, 1.f);

		if (mInterp>0.f)
		{
			GDD->SetSepiaStrength(LERP(mGameSepiaStrength, 1.f, mInterp));
			GDD->SetSepiaColor(LERP(mGameSepia, tcolor(0.6f, 0.65f, 0.8f, 1.f), mInterp));

			GGame->SetGameSpeed(1.f-mInterp);
			/*

			if (GameGeneric::GetInstanceGameMode() == GAME_IS_SOLO)
			{
				GClientGame->SetGameSpeed(1.f-mInterp);
			}
			GetHUD()->Enable(false);
			*/
		}
		if (mInterp>=1.f)
		{
			mShouldTick = false;
		}
	}	
	else
	{
		if (GetInputDevice()->KeyReleased(TKEY_ESC)||GetInputValueReleased("PauseInGame"))
		{
			if (InGameLeftMenu->isVisible())
				Hide();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::Show()
{
	if (InGameLeftMenu->isVisible())
		return;


	bool mCareerModeOn = false;
	mbInGameMenuVisible = true;
	mShouldTick = true;
	mInterp = 0.f;

	static const String indicators[]={".01 ", ".02 ", ".03 ", ".04 ", ".05 "};
	float decal = 0.8f;
	int indicatoridx = 4;

	bool bIsSolo = (GGame->GetClientGame()->GetClassID() == ClassIDZRushGame);
	if (bIsSolo)/*GameGeneric::GetInstanceGameMode() == GAME_IS_SOLO)*/ indicatoridx--;
	if (mCareerModeOn) indicatoridx--;

	btnResume->setPosition(UVector2(cegui_reldim(decal), cegui_reldim( 0.f)));
	btnResume->setText(indicators[indicatoridx--] + GLoc->GetString("RESUMEGAME").c_str());
	decal -= 0.2f;

	btnConfig->setPosition(UVector2(cegui_reldim(decal), cegui_reldim( 0.f)));
	btnConfig->setText(indicators[indicatoridx--] + GLoc->GetString("CONFIG").c_str());
	decal -= 0.2f;

	if (bIsSolo) //GameGeneric::GetInstanceGameMode() == GAME_IS_SOLO)
	{
		btnRestart->setPosition(UVector2(cegui_reldim(decal), cegui_reldim( 0.f)));
		btnRestart->setText(indicators[indicatoridx--] + "Photo mode"/*GLoc->GetString("RESTART").c_str()*/);
		decal -= 0.2f;
		btnRestart->show();

	}
	else
		btnRestart->hide();

	if (mCareerModeOn)
	{
		btnCareer->setPosition(UVector2(cegui_reldim(decal), cegui_reldim( 0.f)));
		btnCareer->setText(indicators[indicatoridx--] + GLoc->GetString("BACK2CAREER").c_str());
		decal -= 0.2f;
		btnCareer->show();
	}
	else
		btnCareer->hide();

	btnLeave->setPosition(UVector2(cegui_reldim(decal), cegui_reldim( 0.f)));
	btnLeave->setText(indicators[indicatoridx--] + GLoc->GetString("LEAVEGAME").c_str());

	InGameLeftMenu->show();

	// save sepia
	mGameSepia = *GDD->GetSepiaColor();
	mGameSepiaStrength = *GDD->GetSepiaStrength();
#ifdef _DEBUG
	cbtrack->show();
#endif
	IncStackCount();
	ZShowCursor();

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::Hide()
{
	if (InGameLeftMenu->isVisible())
	{
		mbInGameMenuVisible = false;
		mShouldTick = true;
		InGameLeftMenu->hide();
		DecStackCount();

		cbtrack->hide();
		ZHideCursor();
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiIngameMenu::HandleResume(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();

	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiIngameMenu::HandleConfig(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();

	InGameLeftMenu->hide();
	DecStackCount();
	cbtrack->hide();
	mGUI->mConfigGui.Show(ZSubscriberSlot(&GuiIngameMenu::OnConfigOK, this));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::OnConfigOK()
{
	IncStackCount();
	InGameLeftMenu->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiIngameMenu::HandleRestart(const CEGUI::EventArgs& e)
{
	InGameLeftMenu->hide();
	cbtrack->hide();
/* for photo mode
	String lib = ".99 ";
	lib += GLoc->GetString("RESTART").c_str();
	String Yes = ".01 ";
	Yes += GLoc->GetString("YES").c_str();
	String No = ".02 ";
	No += GLoc->GetString("NO").c_str();
	mGUI->mMessageboxGui.Show(lib.c_str(), Yes.c_str(), ZSubscriberSlot(&GuiIngameMenu::OnRestartYes, this), 
		No.c_str(), ZSubscriberSlot(&GuiIngameMenu::OnRestartNo, this));
		*/

	GGame->SetActive(false);
	GViewer->SetActive(true);
	mShouldTick = false;

	DecStackCount();
	ZHideCursor();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiIngameMenu::HandleBackToCareer(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();

	Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiIngameMenu::HandleLeave(const CEGUI::EventArgs& e)
{
	InGameLeftMenu->hide();
	DecStackCount();
	cbtrack->hide();
	LeaveQuestion(ZSubscriberSlot(&GuiIngameMenu::OnQuitYes, this),
		ZSubscriberSlot(&GuiIngameMenu::OnQuitNo, this));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::OnQuitNo()
{
	InGameLeftMenu->show();
	IncStackCount();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::OnQuitYes()
{
	mShouldTick = false;
	GDD->SetSepiaStrength(mGameSepiaStrength);
	GDD->SetSepiaColor(mGameSepia);
	//ZManager->AskToQuit();
	//GetProtoGui()->LoadMainMenu();
	mGUI->mMainmenuGui.Show();

	mGUI->mSpectatorGui.Hide();
	//GGame->CleanRushGames();
	gCameraMotion.SetCameraUnused();
	mGUI->BuildGameSetForMenu();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::OnRestartNo()
{
	InGameLeftMenu->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::OnRestartYes()
{
	mShouldTick = false;
	GDD->SetSepiaStrength(mGameSepiaStrength);
	GDD->SetSepiaColor(mGameSepia);
	//GClientGame->SetGameSpeed(1.0f);
	GGame->SetGameSpeed(1.f);
	//GClientGame->Restart();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiIngameMenu::ToggleVisibility()
{
	if (InGameLeftMenu->isVisible())
		Hide();
	else
	{
		if (mGUI->mMessageboxGui.IsVisible())
		{
			mGUI->mMessageboxGui.Hide();
			Hide();
			return;
		}
		else if (mGUI->mConfigGui.IsVisible())
		{
			mGUI->mConfigGui.Hide();
			Hide();
			return;
		}
		Show();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
