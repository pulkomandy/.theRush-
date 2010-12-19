///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiCredits.h
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

#ifndef GUICREDITS_H__
#define GUICREDITS_H__

#include "IProtoGUI.h"

#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;

class GuiCredits
{
public:
	GuiCredits() :winMgr(WindowManager::getSingleton())
	{
		mbCreditsVisible = false;
	}
	virtual ~GuiCredits()
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
	ZProtoGUI *mGUI;

	FrameWindow* crdfrm;

	void UpdateCreditsRoll();
	const char* GetNextCreditLine();
	bool OnCreditsOK(const CEGUI::EventArgs& e);

	
	Window* CreditsText[30];
	int mTextAV;

	bool mbCreditsVisible;
	void ResetCredits();

};

#endif

