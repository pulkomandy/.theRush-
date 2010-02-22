///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiStatistics.h
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

#ifndef GuiStatistics_H__
#define GuiStatistics_H__


class ZProtoGUI;
#include "CEGUI.h"
using namespace CEGUI;

class GuiStatistics
{
public:
	GuiStatistics() :winMgr(WindowManager::getSingleton())
	{
	}
	virtual ~GuiStatistics()
	{
	}
	void Build(IProtoGUI *pGUI);
	void Tick();
	void Show();
	void Hide();
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;

	FrameWindow *StatWindow;
	Window* txtlib;
	ZProtoGUI *mGUI;

	bool OnStatsOK(const CEGUI::EventArgs& e);
};

#endif

