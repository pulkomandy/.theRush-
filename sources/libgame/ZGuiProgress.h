///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiProgress.h
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

#ifndef GUIPROGRESS_H__
#define GUIPROGRESS_H__

#include "../libworld/ZMeshInstance.h"

class ZProtoGUI;
#include "CEGUI.h"
using namespace CEGUI;

class GuiProgress
{
public:
	GuiProgress() :winMgr(WindowManager::getSingleton())
	{
		guifx = DirectLoad("./ZenithDatas/tguil.fx");
		mLoadingRect = NULL;
	}
	virtual ~GuiProgress()
	{
	}

	void Build(ZProtoGUI *pGUI);

	void Show(const char *szImgName);

	void Tick();

	void Hide();
protected:
	// loading
	ZMeshInstance *mLoadingRect;
	ZFx *guifx;
	Window *mLoadingfrm;
	WindowManager& winMgr;
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	Window* trckNfo;
	ZProtoGUI *mGUI;
	smartptr<ZTransform> mLoadingRectTransform;
	Window *backuniform;

};

#endif
