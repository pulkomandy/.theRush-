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
#include "ZProtoConfig.h"
#include "ZProtoGUI.h"
#include "ZGuiProgress.h"
#include "libplatform/IInput.h"
#include "libplatform/ZenFWGame.h"

/*
#include "../LibBase/LibBase.h"
#include "../../Inc/Interfaces.h"
#include "ZProtoGUI.h"
#include "../LibGame/GameNet.h"
#include "../LibGame/NetShip.h"
#include <algorithm>
#include "../LibGame/GameContentManager.h"
#include "GameGeneric.h"
#include "GameBuilder.h"
#include "../LibPlatform/Framework/ZenFWFramework.h"

extern GameContentManager *gContentManager;
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZProtoGUI)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

IProtoGUI* GProtoGui = NULL;
int GGUIStackCount = 0;
int GGUIShowCursor = 0;

void IncStackCount()
{
	GGUIStackCount ++;
}
void DecStackCount()
{
	GGUIStackCount--;
	if (GGUIStackCount<0)
		GGUIStackCount = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShowCursor()
{
	GGUIShowCursor ++;
	GetInputDevice()->SetExclusive( (ZGetCursorVisibility() == 0) );
}

void ZHideCursor()
{
	GGUIShowCursor --;
	if (GGUIShowCursor < 0)
		GGUIShowCursor = 0;
	GetInputDevice()->SetExclusive( (ZGetCursorVisibility() == 0) );
}

int ZGetCursorVisibility()
{
	return GGUIShowCursor;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZProtoGUI::ZProtoGUI() : winMgr(WindowManager::getSingleton())
{
	GProtoGui = this;

	// scheme
	ImagesetManager::getSingleton().createImageset("./Prototype/Common/Menu/CEGUISkin/TaharezLook.imageset");
	//SchemeManager::getSingleton().load("./Prototype/Common/Menu/CEGUISkin/TaharezLook.scheme");

	// Fonts
	mFontArial8 = FontManager::getSingleton().createFont("./Prototype/Common/Menu/Arial.font");
	mFontArial24 = FontManager::getSingleton().createFont("./Prototype/Common/Menu/Arial24.font");

	//root
	root = (DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
	System::getSingleton().setGUISheet(root);

	// debug test
	mDebugCtrl = winMgr.createWindow("TaharezLook/StaticText", "DebugInfos");
	root->addChildWindow(mDebugCtrl);
	mDebugCtrl->setProperty("FrameEnabled", "false");
	mDebugCtrl->setProperty("BackgroundEnabled", "false");
	mDebugCtrl->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	mDebugCtrl->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));
	mDebugCtrl->setMousePassThroughEnabled(true);



/*
	pInstMenuOK = GetSound()->Preload("./ProtoType/Common/Sounds/MenuOK.wav")->Instance2D();
	pInstMenuCancel = GetSound()->Preload("./ProtoType/Common/Sounds/MenuCancel.wav")->Instance2D();
*/
	// init standard gui objects
	
	mProgressGui.Build(this);
	mIngameMenuGui.Build(this);
	mConfigGui.Build(this);
	mMessageboxGui.Build(this);
	mNetStatesGui.Build(this);
	mMainmenuGui.Build(this);
	mCreditsGui.Build(this);
	mLanGui.Build(this);
	mEndResultGui.Build(this);
	mMusicGui.Build(this);

	mQuickraceGui.Build(this);
	mStatisticsGui.Build(this);
	mSpectatorGui.Build(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZProtoGUI::~ZProtoGUI() 
{
	GProtoGui = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::Tick()
{
	PROFILER_START(ZProtoGUI);
/*
	// lan refresh
	mLanRefreshTime += gTimer.GetEllapsed();
	if (mLanRefreshTime >10)
	{
		DoLanRefresh();
		mLanRefreshTime = 0;
	}
	*/

	mProgressGui.Tick();
	mMessageboxGui.Tick();
	mConfigGui.Tick();
	mIngameMenuGui.Tick();
	mNetStatesGui.Tick();
	mMainmenuGui.Tick();
	mCreditsGui.Tick();
	mLanGui.Tick();
	mEndResultGui.Tick();
	mMusicGui.Tick();
	mQuickraceGui.Tick();
	mStatisticsGui.Tick();
	mSpectatorGui.Tick();

	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::BuildIngameMenu()
{
	//mIngameMenuGui.ToggleVisibility();
	mIngameMenuGui.Show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::BuildLoadingScreen(const char *szImgName)
{
	mProgressGui.Show(szImgName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::SetProgress(float aVal)
{
	if (aVal>=1.f)
	{
		mProgressGui.Hide();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::BuildGameSetForMenu()
{
	GGame->BuildSolo();
	GGame->SetGameSet(GameSet_t( "Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track", GameSet_t::GAME_TYPE_MENU, true, 9999));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::PlayErrorSound()
{
	if (!mSoundErr.ptr())
		mSoundErr = (ZSoundSample*)gRushGameContent->GetRes("MenuError");
	ZSoundInstance *pInst = mSoundErr->Instance();
	pInst->setCurrentVolume(float(GConfig->GetQuality("SFXVOLUME")) * 0.01f);
	pInst->play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::PlayQuestionSound()
{
	if (!mSoundQuestion.ptr())
		mSoundQuestion = (ZSoundSample*)gRushGameContent->GetRes("MenuQuestion");
	ZSoundInstance *pInst = mSoundQuestion->Instance();
	pInst->setCurrentVolume(float(GConfig->GetQuality("SFXVOLUME")) * 0.01f);
	pInst->play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::PlayOKSound()
{
	if (!mSoundOK.ptr())
		mSoundOK = (ZSoundSample*)gRushGameContent->GetRes("MenuYesOK");
	ZSoundInstance *pInst = mSoundOK->Instance();
	pInst->setCurrentVolume(float(GConfig->GetQuality("SFXVOLUME")) * 0.01f);
	pInst->play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZProtoGUI::PlayCancelSound()
{
	if (!mSoundCancel.ptr())
		mSoundCancel = (ZSoundSample*)gRushGameContent->GetRes("MenuNoCancel");
	ZSoundInstance *pInst = mSoundCancel->Instance();
	pInst->setCurrentVolume(float(GConfig->GetQuality("SFXVOLUME")) * 0.01f);
	pInst->play();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
