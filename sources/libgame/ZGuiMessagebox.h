///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiMessagebox.h
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

#ifndef GUIMESSAGEBOX_H__
#define GUIMESSAGEBOX_H__

#include "../libbase/ZFunctionSlot.h"
#include "IProtoGUI.h"

#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;


class GuiMessagebox
{
public:
	GuiMessagebox() :winMgr(WindowManager::getSingleton())
	{
	}
	virtual ~GuiMessagebox()
	{
	}
	void Build(ZProtoGUI *pGUI);

	void Show(const char *szLib, const char *szLib1, const ZSubscriberSlot& onyes, const char *szLib2 = NULL, const ZSubscriberSlot& onno=ZSubscriberSlot());
	void ShowWithAnimatedIcon(const char *szLib, const char *szLib1, const ZSubscriberSlot& onyes, const char *szLib2 = NULL, const ZSubscriberSlot& onno=ZSubscriberSlot());
	void Hide();
	void Tick();

	void Close();
	bool IsVisible() { return wnd->isVisible(); }

protected:
	FrameWindow* wnd;
	WindowManager& winMgr;
	Window* label2;
	PushButton* btnNo, *btnYes;
	Window *loadingAnim;
	ZProtoGUI *mGUI;

	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;

	ZSubscriberSlot mOnYesSlot, mOnNoSlot;


	bool handleMBYES(const CEGUI::EventArgs& e)
	{
		GProtoGui->PlayOKSound();

		mOnYesSlot();
		wnd->hide();
		ZHideCursor();
		return true;
	}

	bool handleMBNO(const CEGUI::EventArgs& e)
	{
		GProtoGui->PlayCancelSound();

		mOnNoSlot();
		wnd->hide();
		ZHideCursor();
		return true;
	}


};

#endif
