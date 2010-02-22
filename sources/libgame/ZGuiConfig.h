///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiConfig.h
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

#ifndef GUICONFIG_H__
#define GUICONFIG_H__

#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;
extern int GGUIStackCount;
class GuiConfig
{
public:
	GuiConfig() :winMgr(WindowManager::getSingleton())
	{
		mbControlBinding = false;
	}
	virtual ~GuiConfig()
	{
	}
	void Build(ZProtoGUI *pGUI);
	void Tick();

	void Show(const ZSubscriberSlot& onok)
	{
		mOnOK = onok;
		mCondigWindow->show();
		IncStackCount();
		ZShowCursor();

	}
	void Hide()
	{
		mCondigWindow->hide();
		DecStackCount();
		ZHideCursor();
	}

	bool IsVisible() { return mCondigWindow->isVisible(); }
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;
	FrameWindow* mCondigWindow;
	int mBindingButtonIndex;
	bool mbControlBinding;
	ZProtoGUI *mGUI;
	ZSubscriberSlot mOnOK;

	// resolution
	typedef struct resval_t
	{
		resval_t(int _w, int _h)
		{
			width = _w;
			height = _h;
		}
		int width, height;
	} resval_t;
	std::vector<resval_t> mResolutions;


	void RemoveBind(int aDevice, int aButton, int aAxe);
	void SetBindedControlString(int allCtrl, Window* txtBut);

	bool HandleConfigOK(const CEGUI::EventArgs& e);

	// config quality
	bool handleShadowQualityChanged(const CEGUI::EventArgs& e);
	bool handleWaterQualityChanged(const CEGUI::EventArgs& e);
	bool handleReflectionQualityChanged(const CEGUI::EventArgs& e);
	//bool handleFSChanged(const CEGUI::EventArgs& e);
	bool handleDOFChanged(const CEGUI::EventArgs& e);
	bool handleMBLURChanged(const CEGUI::EventArgs& e);
	bool handleChkMusicChanged(const CEGUI::EventArgs& e);
	bool handleSFXVolChanged(const CEGUI::EventArgs& e);
	bool handleMusicVolChanged(const CEGUI::EventArgs& e);
	//bool handleVSYNCChanged(const CEGUI::EventArgs& e);
	bool HandleIngameSpectatorConfig(const CEGUI::EventArgs& e);

	// tools
	tstring FormatInput(int aDevice, int aButton, int aAxe);
	bool handleConfigControlsBut(const CEGUI::EventArgs& e);
};


#endif
