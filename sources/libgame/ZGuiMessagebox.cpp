///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiMessagebox.cpp
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
#include "ZGuiMessagebox.h"
#include "ZProtoGUI.h"
#include "../libplatform/libplatform.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMessagebox::Build(ZProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	wnd = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "MessageBoxYESNO");
	wnd->setTitleBarEnabled(false);
	wnd->setCloseButtonEnabled(false);
	wnd->setSizingEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	root->addChildWindow(wnd);

	// Windows are in Relative metrics mode by default.  This means that we can
	// specify sizes and positions without having to know the exact pixel size
	// of the elements in advance.  The relative metrics mode co-ordinates are
	// relative to the parent of the window where the co-ordinates are being set.
	// This means that if 0.5f is specified as the width for a window, that window
	// will be half as its parent window.
	//
	// Here we set the FrameWindow so that it is half the size of the display,
	// and centered within the display.
	//wnd->setPosition(UVector2(cegui_reldim(0.33f), cegui_reldim( 0.33f)));
	//wnd->setSize(UVector2(cegui_reldim(0.33f), cegui_reldim( 0.33f)));
	wnd->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.30f)));
	wnd->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	wnd->setAlpha(0.88f);

	// now we set the maximum and minum sizes for the new window.  These are
	// specified using relative co-ordinates, but the important thing to note
	// is that these settings are aways relative to the display rather than the
	// parent window.
	//
	// here we set a maximum size for the FrameWindow which is equal to the size
	// of the display, and a minimum size of one tenth of the display.
	wnd->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
	wnd->setMinSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));

	//wnd->setText(szTitle);

	label2 = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label2");
	wnd->addChildWindow(label2);
	label2->setProperty("FrameEnabled", "false");
	label2->setProperty("BackgroundEnabled", "false");
	label2->setPosition(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.1f)));
	label2->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.6f)));



	btnYes = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "MBYES"));
	wnd->addChildWindow(btnYes);
	btnYes->setPosition(UVector2(cegui_reldim(0.60f), cegui_reldim( 0.0f)));
	btnYes->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));

	btnYes->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMessagebox::handleMBYES, this));


	btnNo = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "MBNO"));
	wnd->addChildWindow(btnNo);
	btnNo->setPosition(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.0f)));
	btnNo->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));
	btnNo->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiMessagebox::handleMBNO, this));


	loadingAnim = winMgr.createWindow("TaharezLook/StaticImage", "MBloadingAnimWnd");
	loadingAnim->setPosition(UVector2(cegui_reldim(0.03f), cegui_reldim( 0.00f)));
	loadingAnim->setSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.9f)));
	loadingAnim->setProperty("FrameEnabled", "false");
	loadingAnim->setProperty("BackgroundEnabled", "false");
	loadingAnim->setAlpha(1.f);
	loadingAnim->setProperty("Image", "set:LoadingAnim00 image:full_image");
	wnd->addChildWindow(loadingAnim);


	wnd->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMessagebox::Show(const char *szLib, const char *szLib1, const ZSubscriberSlot& onyes, const char *szLib2, const ZSubscriberSlot& onno)
{
	
	mOnYesSlot = onyes;
	mOnNoSlot = onno;

	label2->setText(szLib);

	if (szLib1 && szLib2)
	{
		GProtoGui->PlayQuestionSound();

		btnYes->setPosition(UVector2(cegui_reldim(0.60f), cegui_reldim( 0.0f)));
		btnYes->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));

		btnNo->setPosition(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.0f)));
		btnNo->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));

		btnYes->setText(szLib1);
		btnNo->setText(szLib2);
		btnNo->show();
		btnYes->show();
	}
	else
	{
		GProtoGui->PlayErrorSound();

		btnYes->setPosition(UVector2(cegui_reldim(0.70f), cegui_reldim( 0.0f)));
		btnYes->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 1.f)));

		btnYes->setText(szLib1);
		btnNo->hide();
		btnYes->show();
	}
	wnd->show();
	loadingAnim->setVisible(false);
	IncStackCount();
	ZShowCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMessagebox::ShowWithAnimatedIcon(const char *szLib, const char *szLib1, const ZSubscriberSlot& onyes, const char *szLib2, const ZSubscriberSlot& onno)
{
	Show(szLib, szLib1, onyes, szLib2, onno);
	loadingAnim->setVisible(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMessagebox::Tick()
{
	// loading anim
	if (wnd->isVisible()&&loadingAnim->isVisible())
	{
		static int loadinganimidx = 0;
		static float anmdlaid = 0.f;
		char tmps[512];
		sprintf_s(tmps, 512, "set:LoadingAnim%02d image:full_image", loadinganimidx);
		anmdlaid+=gTimer.GetEllapsed();
		if (anmdlaid >0.125f)
		{
			loadinganimidx++;
			loadinganimidx&=0xF;
			anmdlaid = 0.f;
		}

		loadingAnim->setProperty("Image", tmps);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMessagebox::Hide()
{
	wnd->hide();
	DecStackCount();
	ZHideCursor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
