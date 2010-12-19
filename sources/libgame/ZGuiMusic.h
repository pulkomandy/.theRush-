///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiMusic.h
// Creation : 09/04/2009
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

#ifndef GUIMUSIC_H__
#define GUIMUSIC_H__


#include "IProtoGUI.h"

#include "CEGUI.h"

class ZProtoGUI;
using namespace CEGUI;

class ZSoundInstance;

class GuiMusic
{
public:
	GuiMusic() :winMgr(WindowManager::getSingleton())
	{
		mMusic = NULL;
	}

	virtual ~GuiMusic()
	{
	}

	void Build(IProtoGUI *pGUI);

	void PlayNewMusic(bool bShowInfo = true, const char *szForceMusic = NULL );
	void Tick();
	void MusicVolumeChanged();


protected:
	FrameWindow* mMusicInfoFrame;
	WindowManager& winMgr;
	enum MUSIC_INFO_STATE
	{
		MIS_APPEARING,
		MIS_DISPLAYING,
		MIS_DISAPPEARING,
		MIS_INVISIBLE,
	};
	MUSIC_INFO_STATE mMISState;
	float mMISTime;

	ZSoundInstance *mMusic;
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	ZProtoGUI *mGUI;
};

#endif
