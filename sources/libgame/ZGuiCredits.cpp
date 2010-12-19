///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiCredits.cpp
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

#include "ZGuiCredits.h"
#include "ZGuiMainMenu.h"
#include "ZProtoGUI.h"
#include "IProtoGUI.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
static const char *szCredits[]={{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{" Prototype v0.1 Alpha built "__DATE__" at "__TIME__},
{""},
{""},
{""},
{" Project Management"},
{""},
{"                                    Cedric Guillemet"},
{""},
{""},
{" Programming"},
{""},
{"                                    Cedric Guillemet"},
{"                                 Paul Cheyrou-Lagèze"},
{""},
{""},
{" Additionnal programming"},
{""},
{""},
{" Art Direction"},
{""},
{""},
{" 3D artists"},
{""},
{""},
{" Musics"},
{""},
{"         "},
{"         "},
{""},
{""},
{" Sounds and SFX"},
{""},
{"        resources on the Internet"},
{""},
{""},
{""},

{" Production babies"},
{" Arhur Guillemet born May 21 2008"},
{""},

{" Team would like to thank"},
{""},
{" NoFrag.com comunity for 'positive' feedbacks."},
{""},
{""},
{" Personal thanks from Cedric Guillemet"},
{""},
{" Wife, family and whole team at Orange Labs."},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{" Thanks for playing The Rush "},
{""},
{" Copyright 2007-2008 Cedric Guillemet and associates."},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""},
{""}
};
 */
static const char *szCredits[]={
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
" .the rush v0.2 // Network Release // Built "__DATE__" at "__TIME__,
"",
"",
"",
" Project Management",
"",
"                                    Cedric Guillemet",
"",
"",
" Programming",
"",
"                                    Cedric Guillemet",
"",
"",
"",
"",
" Art Direction",
"",
"",
" 3D artists",
"",
"",
" Musics",
"",
" Fractal by Erotic Dream        ",
"	in album Progressive Sensations",
"",
"",
" Passaros de Fogo by Psysun",
"	in album Web Promo         ",
"",
"",
" Nostromondo by Psysun",
"	in album Web Promo         ",
"",
"",
" XIS2 by Psysun",
"	in album Web Promo         ",
"",
"",
" Tesla's Device by Bass Tandem",
"	in album Free Electrons vol. 4",
"",
"",
" Elephantitis by Professor Kliq",
"	in album Community Service II",
"",
"",
" empty by Grace Valhalla",
"	in album PEAK         ",
"",
"",
" Worm In My Oscillator by Sonify",
"	in album Second Coming         ",
"",
"",
" Every sound track under the creative common.",
" All available on Jamendo.com",
" Check http://www.jamendo.com for more *GOOD* music",
"",
"",
" Sounds and SFX",
"",
"",
"",
"",
" Production babies",
" Arthur Guillemet born May 21 2008",
"",
" Team would like to thank",
"",
" NoFrag.com comunity for 'positive' feedbacks.",
"",
"",
" Personal thanks from Cedric Guillemet",
"",
" Wife, family and whole team at Orange Labs.",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
" Thanks for playing The Rush ",
"",
" Copyright 2007-2008 Cedric Guillemet and associates.",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
};


const char* GuiCredits::GetNextCreditLine()
{
	if (mTextAV<sizeof(szCredits)/sizeof(void*))
	{
		return szCredits[mTextAV++];
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiCredits::OnCreditsOK(const CEGUI::EventArgs& e)
{
	GProtoGui->PlayOKSound();
	/*
	for (int i=0;i<30;i++)
	{
		if (CreditsText[i])
			CreditsText[i]->destroy();
	}


	winMgr.getWindow("CreditsBackgrd")->destroy();
	//winMgr.getWindow("CreditsOK")->destroy();
	if (winMgr.isWindowPresent("MAIN_wnd"))
	{
		Window * mmn = winMgr.getWindow("MAIN_wnd");
		mmn->show();
	}

	mCurrentState = GUI_MAINMENU;
	*/

	Hide();
	mGUI->mMainmenuGui.Show();
	ZHideCursor();

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiCredits::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;


	crdfrm = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "CreditsBackgrd");
	root->addChildWindow(crdfrm);
	// set position and size
	crdfrm->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.15f)));
	crdfrm->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.70f)));
	crdfrm->setTitleBarEnabled(false);
	crdfrm->setCloseButtonEnabled(false);
	crdfrm->setSizingEnabled(false);
	crdfrm->setAlpha(0.88f);
	crdfrm->setSizingEnabled(false);


	
	memset(CreditsText, 0, sizeof(CreditsText));
	for (int i=0;i<26;i++)
	{
		char nn[512];
		snprintf(nn, 512, "CreditsString%d", i);
		CreditsText[i] = winMgr.createWindow("TaharezLook/StaticText", nn);
		crdfrm->addChildWindow(CreditsText[i]);
		CreditsText[i]->setProperty("FrameEnabled", "false");
		CreditsText[i]->setProperty("BackgroundEnabled", "false");
		CreditsText[i]->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 0.14f)));
		CreditsText[i]->setHorizontalAlignment(HA_CENTRE);
	}

	ResetCredits();
	
	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "CreditsOK"));
	crdfrm->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.7f), cegui_reldim( 0.83f)));
	btn->setSize(UVector2(cegui_reldim(0.29f), cegui_reldim( 0.08f)));
	btn->setText("Ok");
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiCredits::OnCreditsOK, this));

	crdfrm->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiCredits::ResetCredits()
{
	mTextAV = 0;

	for (int i=0;i<26;i++)
	{

		CreditsText[i]->setPosition(UVector2(cegui_reldim(0.10f), cegui_reldim( i*0.04f)));
		CreditsText[i]->setText(GetNextCreditLine());
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiCredits::UpdateCreditsRoll()
{
	for (int i=0;i<26;i++)
	{

		UVector2 cp = CreditsText[i]->getPosition();
		cp.d_y.d_scale -= (0.03f * gTimer.GetEllapsed() );
		CreditsText[i]->setPosition(UVector2(cp.d_x, cp.d_y));

		if (cp.d_y.d_scale <= (0.1f))
		{
			CreditsText[i]->setAlpha(Clamp(cp.d_y.d_scale*10.0f, 0, 1));
		}
		else if (cp.d_y.d_scale >= (0.8f))
		{
			CreditsText[i]->setAlpha(Clamp((0.9f-cp.d_y.d_scale)*10.0f, 0, 1));
		}
		else 
		{
			CreditsText[i]->setAlpha(1.f);
		}


		if (cp.d_y.d_scale <= (-0.03f))
		{
			CreditsText[i]->setPosition(UVector2(cegui_reldim(0.1f), cegui_reldim(1.f)));

			CreditsText[i]->setText(GetNextCreditLine());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiCredits::Tick()
{
	if (mbCreditsVisible)
	{
		UpdateCreditsRoll();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiCredits::Show()
{
	ZShowCursor();
	IncStackCount();
	ResetCredits();
	mbCreditsVisible = true;
	crdfrm->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiCredits::Hide()
{
	
	DecStackCount();
	mbCreditsVisible = false;
	crdfrm->hide();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
