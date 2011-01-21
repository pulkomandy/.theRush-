///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiProgress.cpp
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

#include "ZGuiProgress.h"
#include "ZProtoGUI.h"
#include "ZLocalisation.h"
#include "libworld/ZMeshBuilder.h"
#include "libbase/ZTimer.h"
#include "libbase/ZFile.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiProgress::Build(ZProtoGUI *pGUI)
{
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim00", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0000.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim01", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0001.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim02", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0002.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim03", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0003.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim04", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0004.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim05", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0005.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim06", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0006.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim07", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0007.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim08", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0008.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim09", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0009.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim10", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0010.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim11", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0011.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim12", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0012.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim13", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0013.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim14", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0014.tga");
	ImagesetManager::getSingleton().createImagesetFromImageFile("LoadingAnim15", "./Prototype/Common/Menu/LoadingAnim/LoadingAnim0015.tga");

	ImagesetManager::getSingleton().createImagesetFromImageFile("BackUniform", "./Prototype/Common/Menu/backuniform.tga");


	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;


	mLoadingfrm = winMgr.createWindow("TaharezLook/FrameWindow", "LOADING_wnd");
	root->addChildWindow(mLoadingfrm);
	mLoadingfrm->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.72f)));
	mLoadingfrm->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.18f)));
	((FrameWindow*)mLoadingfrm)->setTitleBarEnabled(false);
	((FrameWindow*)mLoadingfrm)->setCloseButtonEnabled(false);
	mLoadingfrm->setAlpha(0.88f);


	backuniform = winMgr.createWindow("TaharezLook/StaticImage", "backuniform");
	backuniform->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.0f)));
	backuniform->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));
	backuniform->setProperty("FrameEnabled", "false");
	backuniform->setProperty("BackgroundEnabled", "false");
	backuniform->setAlpha(0.25f);
	backuniform->setProperty("Image", "set:BackUniform image:full_image");
	root->addChildWindow(backuniform);

	Window *loadingAnim = winMgr.createWindow("TaharezLook/StaticImage", "loadingAnimWnd");
	loadingAnim->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.0f)));
	loadingAnim->setSize(UVector2(cegui_reldim(0.08f), cegui_reldim( 1.f)));
	loadingAnim->setProperty("FrameEnabled", "false");
	loadingAnim->setProperty("BackgroundEnabled", "false");
	loadingAnim->setAlpha(1.f);
	loadingAnim->setProperty("Image", "set:LoadingAnim00 image:full_image");
	mLoadingfrm->addChildWindow(loadingAnim);



	Window* label2 = winMgr.createWindow("TaharezLook/StaticText", "LOADING/Loading");
	mLoadingfrm->addChildWindow(label2);
	label2->setProperty("FrameEnabled", "false");
	label2->setProperty("BackgroundEnabled", "false");
	label2->setPosition(UVector2(cegui_reldim(0.14f), cegui_reldim( 0.05f)));
	label2->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.5f)));
	label2->setText(GLoc->GetString("NINENINELOADING").c_str());
	label2->setFont(mFontArial24);
	label2->setProperty("TextColours","tl:FF161616 tr:FF161616 bl:FF161616 br:FF161616");
	//((Text*)label2)->setColor();



	trckNfo = winMgr.createWindow("TaharezLook/StaticText", "LOADING/mapname");
	mLoadingfrm->addChildWindow(trckNfo);
	trckNfo->setProperty("FrameEnabled", "false");
	trckNfo->setProperty("BackgroundEnabled", "false");
	trckNfo->setPosition(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.5f)));
	trckNfo->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.5f)));

	trckNfo->setFont(mFontArial8);
	trckNfo->setHorizontalAlignment(HA_RIGHT);
	trckNfo->setProperty("TextColours","tl:FF161616 tr:FF161616 bl:FF161616 br:FF161616");	

	mLoadingfrm->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiProgress::Show(const char *szImgName)
{
	// preventive
	mGUI->mMessageboxGui.Hide();
	// build rect
	
	
	tstring backimg = szImgName;
	backimg.Replace(".track",".dds");

	/* blocking texture loading
	*/
	
	ZTexture *tex = GDD->NewTexture();
	
	ZFile file;
	if (file.Open(backimg.c_str(), ZOPEN_READ, false))
	{
		unsigned char *tmpTex = new unsigned char [file.GetSize()];
		file.Read(tmpTex, file.GetSize());
		tex->LoadDDSFromMemory(tmpTex, file.GetSize() );
		
		delete [] tmpTex;
	}
	// --
	backimg.Replace(".dds","\0");
	backimg.ToLower();

	trckNfo->setText(backimg.c_str());


	mLoadingRect = AddRect(0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f);
	mLoadingRectTransform = mLoadingRect->GetTransform();


	ZMaterial *mat = mLoadingRect->GetMaterial(0);
	
	mat->setEffect(guifx);
	mat->addTexture(tex);

	mat->connectEffect(true, false);
	FFxSetParam *paramcolor = mat->getNamedParam("col");
	if (paramcolor)
		paramcolor->setVector(vector4(1.f));

	mLoadingRect->SetVisible(true);
	
	//mLoadingRect->AddRef();
	

	// Show GUI
	mLoadingfrm->show();

	IncStackCount();
	// transition
/*
	GDD->ApplyRandomTransition();
	// reset post process things
	
	GDD->SetPPfocalFactor(0.f);
	GDD->SetSepiaStrength(0.f);
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiProgress::Tick()
{
	if (mLoadingRect)
	{
		float tmell = gTimer.GetEllapsed();
		float cs = 0.005f * tmell;

		ZTransform *trans = mLoadingRect->GetTransform();

		trans->PreScale(tvector3(1+cs*2,1+cs*2,1));
		trans->PostTranslate(tvector3(-cs,-cs,0));
		trans->Update();

		// loading anim
		static int loadinganimidx = 0;
		static float anmdlaid = 0.f;
		char tmps[512];
		snprintf(tmps, 512, "set:LoadingAnim%02d image:full_image", loadinganimidx);
		anmdlaid+=gTimer.GetEllapsed();
		if (anmdlaid >0.125f)
		{
			loadinganimidx++;
			loadinganimidx&=0xF;
			anmdlaid = 0.f;
		}
		/*
		float a1 = winMgr.getWindow("backuniform")->getAlpha() + cs*10.f;
		a1 = Clamp(a1,0,1.f);
		float a2 = winMgr.getWindow("LOADING_wnd")->getAlpha() + cs*10.f;
		a2 = Clamp(a2,0,1.f);

		winMgr.getWindow("backuniform")->setAlpha( a1 );
		winMgr.getWindow("LOADING_wnd")->setAlpha( a2 );
		*/

		winMgr.getWindow("loadingAnimWnd")->setProperty("Image", tmps);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiProgress::Hide()
{
	if (mLoadingRect&&mLoadingfrm->isVisible())
	{
		mLoadingfrm->hide();
		backuniform->hide();

		//GDD->GetContextToBuild()->AddDeferredDeletion(mLoadingRect);
		mLoadingRect->SetVisible(false);

		GDD->ApplyRandomTransition();
		DecStackCount();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
