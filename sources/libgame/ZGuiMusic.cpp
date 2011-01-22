///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiMusic.cpp
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

#include "ZGuiMusic.h"
#include "ZProtoGUI.h"
#include "ZProtoConfig.h"
#include "IProtoGUI.h"
#include "libbase/ZTimer.h"

#ifndef WIN32
#include <sys/time.h>
#else
#include <Mmsystem.h>
#endif

void GuiMusic::Build(IProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;

	WindowManager& winMgr = WindowManager::getSingleton();

#if CEGUI_VERSION_MINOR <= 6
	ImagesetManager::getSingleton().createImagesetFromImageFile("MusiqueIcone", "./Prototype/Common/Menu/musique.tga");
#else
	ImagesetManager::getSingleton().createFromImageFile("MusiqueIcone", "./Prototype/Common/Menu/musique.tga");
#endif

	mMusicInfoFrame = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "ChatWindow");
	mMusicInfoFrame->setTitleBarEnabled(false);
	mMusicInfoFrame->setCloseButtonEnabled(false);
	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	root->addChildWindow(mMusicInfoFrame);
	mMusicInfoFrame->setAlwaysOnTop(true);
	mMusicInfoFrame->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.80f)));
	mMusicInfoFrame->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	mMusicInfoFrame->setVisible(false);
	mMusicInfoFrame->setAlpha(0);

	Window* label1 = winMgr.createWindow("TaharezLook/StaticText", "MusicInfo/Artist");
	mMusicInfoFrame->addChildWindow(label1);
	label1->setProperty("FrameEnabled", "false");
	label1->setProperty("BackgroundEnabled", "false");
	label1->setPosition(UVector2(cegui_reldim(0.10f), cegui_reldim( 0.1f)));
	label1->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.4f)));
	label1->setText("");
	label1->setFont(mFontArial24);

	Window* label2 = winMgr.createWindow("TaharezLook/StaticText", "MusicInfo/Song");
	mMusicInfoFrame->addChildWindow(label2);
	label2->setProperty("FrameEnabled", "false");
	label2->setProperty("BackgroundEnabled", "false");
	label2->setPosition(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.5f)));
	label2->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.4f)));
	//label2->setHorzFormatting(HF_RIGHT_ALIGNED);
	label2->setText("");
	label2->setFont(mFontArial8);


	Window *logoMusique = winMgr.createWindow("TaharezLook/StaticImage", "MusiqueIcone");
	logoMusique->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	logoMusique->setSize(UVector2(cegui_reldim(0.10f), cegui_reldim( 1.f)));
	logoMusique->setProperty("FrameEnabled", "false");
	logoMusique->setProperty("BackgroundEnabled", "false");
	logoMusique->setAlpha(0.75f);
	logoMusique->setProperty("Image", "set:MusiqueIcone image:full_image");
	mMusicInfoFrame->addChildWindow(logoMusique);


	mMISState = MIS_INVISIBLE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////


void GuiMusic::PlayNewMusic(bool bShowInfo , const char *szForceMusic  )
{
	WindowManager& winMgr = WindowManager::getSingleton();

	if (!szForceMusic)
		if (!GConfig->IsEnable("CHKMUSIC")) return;

#ifdef WIN32
	srand(timeGetTime());
#else
	unsigned long ticks;
	struct timeval now;
	gettimeofday(&now, NULL);
	ticks = now.tv_sec * 1000 + now.tv_usec / 1000;
	srand(ticks);
#endif
	
	int musicsCount = gRushGameContent->GetMusics().size();
	srand(int(gTimer.GetTime()));
	const char *MusicToPlay = gRushGameContent->GetMusics()[(rand()%musicsCount)];

	if (szForceMusic)
		MusicToPlay = szForceMusic;


	if (mMusic)
		mMusic->stop();

	ZSoundSample *pSamp = GSoundManager->NewSoundSample(false, true);
	pSamp->Preload(MusicToPlay);
	mMusic = pSamp->Instance();
	
	mMusic->setCurrentVolume(float(GConfig->GetQuality("MUSICVOLUME"))*0.01f);
	mMusic->play();
	mMISTime = 0;

	if (!bShowInfo) return ;
	mMusicInfoFrame->setVisible(true);
	mMISState = MIS_APPEARING;


	Window* winArtist = winMgr.getWindow("MusicInfo/Artist");
	Window* winSong = winMgr.getWindow("MusicInfo/Song");

	unsigned int alen;
	const char *aName;
	aName = pSamp->GetArtist(alen);
	
	if (aName)
	{
		tstring strtmp(aName, alen);
		winArtist->setText(strtmp.c_str());
	}
	else
		winArtist->setText("Unknown Artist");


	aName = pSamp->GetSongName(alen);
	
	String txtSong;
	if (aName)
	{
		tstring strtmp(aName, alen);
		txtSong = strtmp;
	}
	else
		txtSong = "Untitled song";
	aName = pSamp->GetYear(alen);

	if(aName)
	{
		tstring strtmp(aName, alen);
		txtSong += " (";
		txtSong += strtmp.c_str();
		txtSong += ") ";
		winSong->setText(txtSong);
	}

	IncStackCount();

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiMusic::Tick()
{
	if (mMusicInfoFrame->isVisible())
	{
		//		mMusicInfoFrame->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( (cos(gTimer.GetTime())+1)*0.2f )));
		switch (mMISState)
		{

		case MIS_APPEARING:
			mMusicInfoFrame->setAlpha(mMISTime);
			mMISTime+=gTimer.GetEllapsed();
			if (mMISTime>1.0f)
			{
				mMISTime = 0;
				mMISState = MIS_DISPLAYING;
			}
			break;
		case MIS_DISPLAYING:
			mMusicInfoFrame->setAlpha(1.f);
			mMISTime+=gTimer.GetEllapsed();
			if (mMISTime>3.0f)
			{
				mMISTime = 0;
				mMISState = MIS_DISAPPEARING;
			}
			break;
		case MIS_DISAPPEARING:
			mMusicInfoFrame->setAlpha(1.f-mMISTime);
			mMISTime+=gTimer.GetEllapsed();
			if (mMISTime>1.0f)
			{
				mMISTime = 0;
				mMISState = MIS_INVISIBLE;
				mMusicInfoFrame->setVisible(false);
				DecStackCount();
			}
			break;

		}
	}
}

void GuiMusic::MusicVolumeChanged()
{
	if (mMusic)
	{
		mMusic->setCurrentVolume(float(GConfig->GetQuality("MUSICVOLUME"))*0.01f);
	}
}
