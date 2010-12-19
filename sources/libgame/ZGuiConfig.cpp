///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiConfig.cpp
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

#include "ZGuiConfig.h"
#include "ZProtoGUI.h"
#include "../libplatform/libplatform.h"
#include "ZGuiUtils.h"
#include <SDL/SDL.h>
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct CTRLEntry
{
	const char *mName;
	CEGUI::Window *mWindow;
} CTRLEntry;

static CTRLEntry aCtrlList[]={
	{"Accelerate", NULL},
	{"LeftBrake", NULL},
	{"RightBrake", NULL},
	{"Booster", NULL},
	{"AttackBonus", NULL},
	{"ViewChange", NULL},
	{"LeftTurn", NULL},
	{"RightTurn", NULL},
	{"PauseInGame", NULL}
};

int offsetCtrlBind[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };


///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiConfig::handleShadowQualityChanged(const CEGUI::EventArgs& e)
{
	Combobox *pcb = (Combobox*)winMgr.getWindow("SHADOWQUALITY");
	GConfig->SetQuality("SHADOWQUALITY", (int)pcb->getItemIndex (pcb->getSelectedItem()) );
	return true;
}
bool GuiConfig::handleWaterQualityChanged(const CEGUI::EventArgs& e)
{
	Combobox *pcb = (Combobox*)winMgr.getWindow("WATERQUALITY");
	GConfig->SetQuality("WATERQUALITY", (int)pcb->getItemIndex (pcb->getSelectedItem()) );
	return true;
}
bool GuiConfig::handleReflectionQualityChanged(const CEGUI::EventArgs& e)
{
	Combobox *pcb = (Combobox*)winMgr.getWindow("REFLECTIONQUALITY");
	GConfig->SetQuality("REFLECTIONQUALITY", (int)pcb->getItemIndex (pcb->getSelectedItem()) );
	return true;
}



bool GuiConfig::handleDOFChanged(const CEGUI::EventArgs& e)
{
	Checkbox *pcb = (Checkbox*)winMgr.getWindow("CHKDOF");
	GConfig->SetEnable("CHKDOF", (int)pcb->isSelected() );
	return true;
}
bool GuiConfig::handleMBLURChanged(const CEGUI::EventArgs& e)
{
	Checkbox *pcb = (Checkbox*)winMgr.getWindow("CHKMBLUR");
	GConfig->SetEnable("CHKMBLUR", (int)pcb->isSelected() );
	return true;
}

bool GuiConfig::handleChkMusicChanged(const CEGUI::EventArgs& e)
{
	Checkbox *pcb = (Checkbox*)winMgr.getWindow("CHKMUSIC");
	GConfig->SetEnable("CHKMUSIC", (int)pcb->isSelected() );
	return true;
}
bool GuiConfig::handleSFXVolChanged(const CEGUI::EventArgs& e)
{
	Scrollbar *pcb = (Scrollbar*)winMgr.getWindow("SFXVOLUME");
	GConfig->SetQuality("SFXVOLUME", (int)pcb->getScrollPosition() );
	return true;
}
bool GuiConfig::handleMusicVolChanged(const CEGUI::EventArgs& e)
{
	Scrollbar *pcb = (Scrollbar*)winMgr.getWindow("MUSICVOLUME");
	GConfig->SetQuality("MUSICVOLUME", (int)(pcb->getScrollPosition()) );
	GProtoGui->MusicVolumeChanged();
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiConfig::RemoveBind(int aDevice, int aButton, int aAxe)
{
	for (int allCtrl = 0;allCtrl<sizeof(aCtrlList)/sizeof(CTRLEntry); allCtrl++)
	{
	int aDevice1, aButton1, aAxe1;
	int aDevice2, aButton2, aAxe2;

		GConfig->GetControl(aCtrlList[allCtrl].mName, 0, aDevice1, aButton1, aAxe1);
		if ((aDevice1 == aDevice) && (aButton1 == aButton) && (aAxe1 == aAxe))
			GConfig->SetControl(aCtrlList[allCtrl].mName, 0, 0, 0, 0);

		GConfig->GetControl(aCtrlList[allCtrl].mName, 1, aDevice2, aButton2, aAxe2);
		if ((aDevice2 == aDevice) && (aButton2 == aButton) && (aAxe2 == aAxe))
			GConfig->SetControl(aCtrlList[allCtrl].mName, 1, 0, 0, 0);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiConfig::SetBindedControlString(int allCtrl, Window* txtBut)
{
	int aDevice1, aButton1, aAxe1;
	int aDevice2, aButton2, aAxe2;

	GConfig->GetControl(aCtrlList[allCtrl].mName, 0, aDevice1, aButton1, aAxe1);
	GConfig->GetControl(aCtrlList[allCtrl].mName, 1, aDevice2, aButton2, aAxe2);

	if (aDevice2 || aButton2 || aAxe2)
	{
		if (aDevice1 || aButton1 || aAxe1)
		{
			txtBut->setText((FormatInput(aDevice1, aButton1, aAxe1) + " / " +
				FormatInput(aDevice2, aButton2, aAxe2)).c_str() );
		}
		else
		{
			txtBut->setText(FormatInput(aDevice2, aButton2, aAxe2).c_str() );
		}
	}
	else
	{
		if (aDevice1 || aButton1 || aAxe1)
		{
			txtBut->setText(FormatInput(aDevice1, aButton1, aAxe1).c_str() );
		}
		else
		{
			txtBut->setText(GLoc->GetString("UNDEFINED").c_str() );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiConfig::handleConfigControlsBut(const CEGUI::EventArgs& e)
{
	WindowEventArgs* evt = (WindowEventArgs*)&e;
	FrameWindow* wnd = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "BinderPopup");
	wnd->setTitleBarEnabled(false);
	wnd->setCloseButtonEnabled(false);
	root->addChildWindow(wnd);

	wnd->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.30f)));
	wnd->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.15f)));
	wnd->setAlpha(0.88f);

	wnd->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
	wnd->setMinSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));

	Window* label2 = winMgr.createWindow("TaharezLook/StaticText", "BinderPopupLabel");
	wnd->addChildWindow(label2);
	label2->setProperty("FrameEnabled", "false");
	label2->setProperty("BackgroundEnabled", "false");
	label2->setPosition(UVector2(cegui_reldim(0.10f), cegui_reldim( 0.1f)));
	label2->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.8f)));
	label2->setText(GLoc->GetString("AskForInput").c_str());


	winMgr.getWindow("Config Window")->setVisible(false);
//	mCurrentState = GUI_GETCONTROLBIND;
	mbControlBinding = true;

	mBindingButtonIndex = evt->window->getID();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tstring GuiConfig::FormatInput(int aDevice, int aButton, int aAxe)
{
	tstring formatedThing;
	switch (aDevice)
	{
	case CTRL_KBD:
		{
#if 0			
			switch(aButton)
			{

			case 0x01: return GLoc->GetString("ESCAPE");
			case 0x0E: return GLoc->GetString("BACK");                /* backspace */
			case 0x0F: return GLoc->GetString("TAB");
			case 0x1C: return GLoc->GetString("RETURN");              /* Enter on main keyboard */
			case 0x1D: return GLoc->GetString("LCONTROL");
			case 0x2A: return GLoc->GetString("LSHIFT");
			case 0x2B: return GLoc->GetString("BACKSLASH");
			case 0x36: return GLoc->GetString("RSHIFT");
			case 0x38: return GLoc->GetString("LMENU");               /* left Alt */
			case 0x39: return GLoc->GetString("SPACE");
			case 0x3A: return GLoc->GetString("CAPITAL");
			case 0x3B: return GLoc->GetString("F1");
			case 0x3C: return GLoc->GetString("F2");
			case 0x3D: return GLoc->GetString("F3");
			case 0x3E: return GLoc->GetString("F4");
			case 0x3F: return GLoc->GetString("F5");
			case 0x40: return GLoc->GetString("F6");
			case 0x41: return GLoc->GetString("F7");
			case 0x42: return GLoc->GetString("F8");
			case 0x43: return GLoc->GetString("F9");
			case 0x44: return GLoc->GetString("F10");
			case 0x45: return GLoc->GetString("NUMLOCK");
			case 0x46: return GLoc->GetString("SCROLL");              /* Scroll Lock */
			case 0x47: return GLoc->GetString("NUMPAD7");
			case 0x48: return GLoc->GetString("NUMPAD8");
			case 0x49: return GLoc->GetString("NUMPAD9");
			case 0x4B: return GLoc->GetString("NUMPAD4");
			case 0x4C: return GLoc->GetString("NUMPAD5");
			case 0x4D: return GLoc->GetString("NUMPAD6");
			case 0x4F: return GLoc->GetString("NUMPAD1");
			case 0x50: return GLoc->GetString("NUMPAD2");
			case 0x51: return GLoc->GetString("NUMPAD3");
			case 0x52: return GLoc->GetString("NUMPAD0");
			case 0x53: return GLoc->GetString("DECIMAL");             /* . on numeric keypad */
			case 0x70: return GLoc->GetString("KANA");                /* (Japanese keyboard)            */
			case 0x99: return GLoc->GetString("NEXTTRACK");           /* Next Track */
			case 0x9C: return GLoc->GetString("NUMPADENTER");         /* Enter on numeric keypad */
			case 0x9D: return GLoc->GetString("RCONTROL");
			case 0xA0: return GLoc->GetString("MUTE");                /* Mute */
			case 0xA1: return GLoc->GetString("CALCULATOR");          /* Calculator */
			case 0xA2: return GLoc->GetString("PLAYPAUSE");           /* Play / Pause */
			case 0xA4: return GLoc->GetString("MEDIASTOP");           /* Media Stop */
			case 0xAE: return GLoc->GetString("VOLUMEDOWN");          /* Volume - */
			case 0xB0: return GLoc->GetString("VOLUMEUP");            /* Volume + */
			case 0xB2: return GLoc->GetString("WEBHOME");             /* Web home */
			case 0xB7: return GLoc->GetString("SYSRQ");
			case 0xB8: return GLoc->GetString("RMENU");               /* right Alt */
			case 0xC5: return GLoc->GetString("PAUSE");               /* Pause */
			case 0xC7: return GLoc->GetString("HOME");                /* Home on arrow keypad */
			case 0xC8: return GLoc->GetString("UP");                  /* UpArrow on arrow keypad */
			case 0xC9: return GLoc->GetString("PRIOR");               /* PgUp on arrow keypad */
			case 0xCB: return GLoc->GetString("LEFT");                /* LeftArrow on arrow keypad */
			case 0xCD: return GLoc->GetString("RIGHT");               /* RightArrow on arrow keypad */
			case 0xCF: return GLoc->GetString("END");                 /* End on arrow keypad */
			case 0xD0: return GLoc->GetString("DOWN");                /* DownArrow on arrow keypad */
			case 0xD1: return GLoc->GetString("NEXT");                /* PgDn on arrow keypad */
			case 0xD2: return GLoc->GetString("INSERT");              /* Insert on arrow keypad */
			case 0xD3: return GLoc->GetString("DELETE");              /* Delete on arrow keypad */
			case 0xDB: return GLoc->GetString("LWIN");                /* Left Windows key */
			case 0xDC: return GLoc->GetString("RWIN");                /* Right Windows key */
			case 0xDD: return GLoc->GetString("APPS");                /* AppMenu key */
			case 0xDE: return GLoc->GetString("POWER");               /* System Power */
			case 0xDF: return GLoc->GetString("SLEEP");               /* System Sleep */
			case 0xE3: return GLoc->GetString("WAKE");                /* System Wake */
			case 0xE5: return GLoc->GetString("WEBSEARCH");           /* Web Search */
			case 0xE6: return GLoc->GetString("WEBFAVORITES");        /* Web Favorites */
			case 0xE7: return GLoc->GetString("WEBREFRESH");          /* Web Refresh */
			case 0xE8: return GLoc->GetString("WEBSTOP");             /* Web Stop */
			case 0xE9: return GLoc->GetString("WEBFORWARD");          /* Web Forward */
			case 0xEA: return GLoc->GetString("WEBBACK");             /* Web Back */
			case 0xEB: return GLoc->GetString("MYCOMPUTER");          /* My Computer */
			case 0xEC: return GLoc->GetString("MAIL");                /* Mail */
			case 0xED: return GLoc->GetString("MEDIASELECT");         /* Media Select */
			//default: return "???";
			}
#endif
			unsigned short result[2];
			scan2ascii(aButton, result);
			formatedThing = "";
			formatedThing += SDL_GetKeyName((SDLKey)aButton);//(char)aButton;//((unsigned char*)result)[0];
			formatedThing.ToUpper();
		}
		break;
	case CTRL_MOUSE:
		formatedThing = GLoc->GetString("Mouse");
		formatedThing+= " ";
		if (aAxe == 1) formatedThing +=" X";
		if (aAxe == 2) formatedThing +=" Y";
		if (aAxe == 0)
		{
			tstring msbt;
			msbt.Printf("%d", aButton);
			formatedThing += GLoc->GetString("Button");
			formatedThing += msbt;
		}
		break;
	case CTRL_PAD0:
		break;
	}
	return formatedThing;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiConfig::Build(ZProtoGUI *pGUI)
{
	mGUI = (ZProtoGUI*)pGUI;
	root = mGUI->root;
	mFontArial8 = mGUI->mFontArial8;
	mFontArial24 = mGUI->mFontArial24;


	// Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
	mCondigWindow = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Config Window");
	mCondigWindow->setTitleBarEnabled(false);
	mCondigWindow->setCloseButtonEnabled(false);
	mCondigWindow->setSizingEnabled(false);
	root->addChildWindow(mCondigWindow);
	mCondigWindow->setAlpha(0.6f);
	mCondigWindow ->hide();
	mCondigWindow->setSizingEnabled(false);


	FrameWindow* wnd1 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "tabPage0");
	wnd1->setTitleBarEnabled(false);
	wnd1->setFrameEnabled(false);//>setTitleBarEnabled(false);
	wnd1->setCloseButtonEnabled(false);
	wnd1->setText(GLoc->GetString("PLAYER").c_str());


	FrameWindow* wnd2 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "tabPage1");
	wnd2->setTitleBarEnabled(false);
	wnd2->setFrameEnabled(false);//>setTitleBarEnabled(false);
	wnd2->setCloseButtonEnabled(false);
	wnd2->setText(GLoc->GetString("Controls").c_str());


	FrameWindow* wnd3 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "tabPage2");
	wnd3->setTitleBarEnabled(false);
	wnd3->setCloseButtonEnabled(false);
	wnd3->setText(GLoc->GetString("Graphics").c_str());
	wnd3->setFrameEnabled(false);//

	FrameWindow* wnd4 = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "tabPage3");
	wnd4->setTitleBarEnabled(false);
	wnd4->setFrameEnabled(false);//>setTitleBarEnabled(false);
	wnd4->setCloseButtonEnabled(false);
	wnd4->setText(GLoc->GetString("SOUND").c_str());
	//wnd->addChildWindow (winMgr.loadWindowLayout ("TabControlDemo.layout", "TabControlDemo/"));

	TabControl *tc = (TabControl *)winMgr.createWindow("TaharezLook/TabControl", "Config/Tabs");

	mCondigWindow->setPosition(UVector2(cegui_reldim(0.f), cegui_reldim( 0.f)));
	mCondigWindow->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 0.9f)));
	tc->setArea(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.05f)), UVector2(cegui_reldim(0.9f), cegui_reldim( 0.85f)) );



	// Add some pages to tab control
	tc->addTab (wnd1);
	tc->addTab (wnd2);
	tc->addTab (wnd3);
	tc->addTab (wnd4);

	tc->setTabHeight(UDim (0.06f, 0.13f));
	tc->setTabTextPadding(UDim (0.06f, 0.1f));


	mCondigWindow->addChildWindow(tc);

	mCondigWindow->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0.15f)));
	mCondigWindow->setSize(UVector2(cegui_reldim(1.1f), cegui_reldim( 0.70f)));
	mCondigWindow->setAlpha(0.88f);


	wnd1->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd1->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));

	wnd2->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd2->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));

	wnd3->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd3->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));

	wnd4->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	wnd4->setSize(UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));

	// controls


	for (int allCtrl = 0;allCtrl<sizeof(aCtrlList)/sizeof(CTRLEntry); allCtrl++)
	{
		static const float interligne = 0.06f*1.6f;
		static const float intersize = 0.05f*1.4f;
		tstring numb;
		numb.Printf("%d", allCtrl);

		Window* txtlib = winMgr.createWindow("TaharezLook/StaticText", String(aCtrlList[allCtrl].mName)+String("CtrlLib")+String(numb.c_str()));

		txtlib->setText(GLoc->GetString(aCtrlList[allCtrl].mName).c_str());
		txtlib->setProperty("FrameEnabled", "false");
		txtlib->setProperty("BackgroundEnabled", "false");
		//txtlib->setHorizontalAlignment(HA_CENTRE);
		txtlib->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.01f + interligne*allCtrl)));
		txtlib->setSize(UVector2(cegui_reldim(0.19f), cegui_reldim( intersize )));

		wnd2->addChildWindow(txtlib);



		PushButton* txtBut = (PushButton*)winMgr.createWindow("TaharezLook/Button", String(aCtrlList[allCtrl].mName)+String("CtrlBut")+String(numb.c_str()));
		txtBut->setPosition(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.01f + interligne*allCtrl)));
		txtBut->setSize(UVector2(cegui_reldim(0.50f), cegui_reldim( intersize )));

		SetBindedControlString(allCtrl, txtBut);


		//txtBut->setText("A or PAD Button 1");
		txtBut->setHorizontalAlignment(HA_CENTRE);

		wnd2->addChildWindow(txtBut);

		txtBut->subscribeEvent ( PushButton::EventClicked,
			Event::Subscriber (&GuiConfig::handleConfigControlsBut, this));

		txtBut->setID(allCtrl);
		aCtrlList[allCtrl].mWindow = (Window*)txtBut;

	}
	// -- PLAYER

	Editbox* playerName = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "CONFIGPLAYERNAME"));
	wnd1->addChildWindow(playerName);
	playerName->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.05f)));
	playerName->setSize(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.08f)));
	//playerName->setValidationString("*");
	playerName->setText(GConfig->GetPlayerName());

	Window* txtlib = winMgr.createWindow("TaharezLook/StaticText", "CONFIGPLAYERNAMESTATIC");

	txtlib->setText(GLoc->GetString("PLAYERNAME").c_str());
	txtlib->setProperty("FrameEnabled", "false");
	txtlib->setProperty("BackgroundEnabled", "false");
	txtlib->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05f)));
	txtlib->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));

	wnd1->addChildWindow(txtlib);



	// --

	// -- SOUND

	Scrollbar* sfxslider = static_cast<Scrollbar*>(winMgr.createWindow("TaharezLook/HorizontalScrollbar", "SFXVOLUME"));
	sfxslider->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.05f)));
	sfxslider->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.08f)));
	sfxslider->setDocumentSize (100);
	sfxslider->subscribeEvent ( Scrollbar::EventScrollPositionChanged, Event::Subscriber (&GuiConfig::handleSFXVolChanged, this));
	sfxslider->setScrollPosition(float(GConfig->GetQuality("SFXVOLUME")));
	wnd4->addChildWindow(sfxslider);


	Window* txtlibsfx = winMgr.createWindow("TaharezLook/StaticText", "SFXVOLUMESTATIC");
	txtlibsfx->setText(GLoc->GetString("SFXVOLUME").c_str());
	txtlibsfx->setProperty("FrameEnabled", "false");
	txtlibsfx->setProperty("BackgroundEnabled", "false");
	txtlibsfx->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05f)));
	txtlibsfx->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	wnd4->addChildWindow(txtlibsfx);



	Scrollbar* musicslider = static_cast<Scrollbar*>(winMgr.createWindow("TaharezLook/HorizontalScrollbar", "MUSICVOLUME"));
	musicslider->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.15f)));
	musicslider->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.08f)));
	musicslider->setDocumentSize (100);
	musicslider->subscribeEvent ( Scrollbar::EventScrollPositionChanged, Event::Subscriber (&GuiConfig::handleMusicVolChanged, this));
	musicslider->setScrollPosition(float(GConfig->GetQuality("MUSICVOLUME")));
	wnd4->addChildWindow(musicslider);


	Window* txtlibmusic = winMgr.createWindow("TaharezLook/StaticText", "MUSICVOLUMESTATIC");
	txtlibmusic->setText(GLoc->GetString("MUSICVOLUME").c_str());
	txtlibmusic->setProperty("FrameEnabled", "false");
	txtlibmusic->setProperty("BackgroundEnabled", "false");
	txtlibmusic->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.15f)));
	txtlibmusic->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	wnd4->addChildWindow(txtlibmusic);


    Checkbox* checkMusic = static_cast<Checkbox*>(winMgr.createWindow("TaharezLook/Checkbox", "CHKMUSIC"));
    wnd4->addChildWindow(checkMusic);
	checkMusic->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.25f)));
	checkMusic->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	checkMusic->setText(GLoc->GetString("CHKMUSIC").c_str());
	checkMusic->subscribeEvent ( Checkbox::EventCheckStateChanged, Event::Subscriber (&GuiConfig::handleChkMusicChanged, this));
	checkMusic->setSelected (GConfig->IsEnable("CHKMUSIC"));

	// --

	// -- VIDEO


    Combobox* cbresolution = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "RESOLUTION"));
    wnd3->addChildWindow(cbresolution);
    cbresolution->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.05f)));
    cbresolution->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.75f)));
	cbresolution->setReadOnly(true);

	Window* txtres = winMgr.createWindow("TaharezLook/StaticText", "RESOLUTIONTXT");
	txtres->setText(GLoc->GetString("RESOLUTION").c_str());
	txtres->setProperty("FrameEnabled", "false");
	txtres->setProperty("BackgroundEnabled", "false");
	txtres->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.05f)));
	txtres->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	wnd3->addChildWindow(txtres);


	int optWidth = GConfig->GetQuality("Width");
	int optHeight = GConfig->GetQuality("Height");

	int awidth = -1, aheight = -1;
	int avresolutions = 0;
	for (unsigned int rs = 0;rs<GDD->GetNbPossibleResolutions(); rs++)
	{
		char tmps[512];
		int width, height;
		GDD->GetResolution(rs, width, height);
		if ((awidth != width) || (aheight != height))
		{
			awidth = width;
			aheight = height;

			mResolutions.push_back(resval_t(width, height));
			snprintf(tmps, 512, "%d x %d", width, height);
			cbresolution->addItem (new ListboxTextItem(tmps));

			if ((width == optWidth)&&(height == optHeight))
				CEGUICBSel(cbresolution, avresolutions);

			avresolutions++;
		}
	}



	// --

    Checkbox* checkFS = static_cast<Checkbox*>(winMgr.createWindow("TaharezLook/Checkbox", "CHKBFS"));
    wnd3->addChildWindow(checkFS);
	checkFS->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.15f)));
	checkFS->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	checkFS->setText(GLoc->GetString("FULLSCREEN").c_str());
	//checkFS->subscribeEvent ( Checkbox::EventCheckStateChanged, Event::Subscriber (&GuiConfig::handleFSChanged, this));
	checkFS->setSelected (GConfig->IsEnable("CHKBFS"));


    Checkbox* checkVSync = static_cast<Checkbox*>(winMgr.createWindow("TaharezLook/Checkbox", "CHKBVSYNC"));
    wnd3->addChildWindow(checkVSync);
	checkVSync->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.75f)));
	checkVSync->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	checkVSync->setText(GLoc->GetString("VSYNC").c_str());
	//checkVSync->subscribeEvent ( Checkbox::EventCheckStateChanged, Event::Subscriber (&GuiConfig::handleVSYNCChanged, this));
	checkVSync->setSelected (GConfig->IsEnable("CHKBVSYNC"));

	// --

    Combobox* cbshad = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "SHADOWQUALITY"));
    wnd3->addChildWindow(cbshad);
    cbshad->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.25f)));
    cbshad->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.33f)));
	cbshad->setReadOnly(true);

	Window* txtshad = winMgr.createWindow("TaharezLook/StaticText", "SHADOWQUALITYTXT");
	txtshad->setText(GLoc->GetString("SHADOWQUALITY").c_str());
	txtshad->setProperty("FrameEnabled", "false");
	txtshad->setProperty("BackgroundEnabled", "false");
	txtshad->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.25f)));
	txtshad->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	wnd3->addChildWindow(txtshad);

	cbshad->addItem (new ListboxTextItem(GLoc->GetString("DISABLED").c_str()));
	cbshad->addItem (new ListboxTextItem(GLoc->GetString("MEDIUM").c_str()));
	cbshad->addItem (new ListboxTextItem(GLoc->GetString("HIGH").c_str()));
	cbshad->addItem (new ListboxTextItem(GLoc->GetString("VERYHIGH").c_str()));

	cbshad->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiConfig::handleShadowQualityChanged, this));
	CEGUICBSel(cbshad, GConfig->GetQuality("SHADOWQUALITY"));
	// --

    Combobox* cbrefl = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "REFLECTIONQUALITY"));
    wnd3->addChildWindow(cbrefl);
    cbrefl->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.35f)));
    cbrefl->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.33f)));
	cbrefl->setReadOnly(true);

	Window* txtrefl = winMgr.createWindow("TaharezLook/StaticText", "REFLECTIONQUALITYTXT");
	txtrefl->setText(GLoc->GetString("REFLECTIONQUALITY").c_str());
	txtrefl->setProperty("FrameEnabled", "false");
	txtrefl->setProperty("BackgroundEnabled", "false");
	txtrefl->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.35f)));
	txtrefl->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	wnd3->addChildWindow(txtrefl);

	cbrefl->addItem (new ListboxTextItem(GLoc->GetString("DISABLED").c_str()));
	cbrefl->addItem (new ListboxTextItem(GLoc->GetString("MEDIUM").c_str()));
	cbrefl->addItem (new ListboxTextItem(GLoc->GetString("HIGH").c_str()));
	cbrefl->addItem (new ListboxTextItem(GLoc->GetString("VERYHIGH").c_str()));
	cbrefl->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiConfig::handleReflectionQualityChanged, this));
	CEGUICBSel(cbrefl, GConfig->GetQuality("REFLECTIONQUALITY"));
	// --
    Combobox* cbwater = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", "WATERQUALITY"));
    wnd3->addChildWindow(cbwater);
    cbwater->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.45f)));
    cbwater->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.33f)));
	cbwater->setReadOnly(true);

	Window* txtwater = winMgr.createWindow("TaharezLook/StaticText", "WATERQUALITYTXT");
	txtwater->setText(GLoc->GetString("WATERQUALITY").c_str());
	txtwater->setProperty("FrameEnabled", "false");
	txtwater->setProperty("BackgroundEnabled", "false");
	txtwater->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.45f)));
	txtwater->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	wnd3->addChildWindow(txtwater);

	cbwater->addItem (new ListboxTextItem(GLoc->GetString("LOW").c_str()));
	cbwater->addItem (new ListboxTextItem(GLoc->GetString("MEDIUM").c_str()));
	cbwater->addItem (new ListboxTextItem(GLoc->GetString("HIGH").c_str()));
	cbwater->addItem (new ListboxTextItem(GLoc->GetString("VERYHIGH").c_str()));
	cbwater->subscribeEvent ( Combobox::EventListSelectionAccepted, Event::Subscriber (&GuiConfig::handleWaterQualityChanged, this));
	CEGUICBSel(cbwater, GConfig->GetQuality("WATERQUALITY"));
	// --

    Checkbox* checkDOF = static_cast<Checkbox*>(winMgr.createWindow("TaharezLook/Checkbox", "CHKDOF"));
    wnd3->addChildWindow(checkDOF);
	checkDOF->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.55f)));
	checkDOF->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	checkDOF->setText(GLoc->GetString("DEPTHOFFIELD").c_str());
	checkDOF->subscribeEvent ( Checkbox::EventCheckStateChanged, Event::Subscriber (&GuiConfig::handleDOFChanged, this));
	checkDOF->setSelected (GConfig->IsEnable("CHKDOF"));

    Checkbox* checkMBLUR = static_cast<Checkbox*>(winMgr.createWindow("TaharezLook/Checkbox", "CHKMBLUR"));
    wnd3->addChildWindow(checkMBLUR);
	checkMBLUR->setPosition(UVector2(cegui_reldim(0.05f), cegui_reldim( 0.65f)));
	checkMBLUR->setSize(UVector2(cegui_reldim(0.24f), cegui_reldim( 0.08f )));
	checkMBLUR->setText(GLoc->GetString("MOTIONBLUR").c_str());
	checkMBLUR->subscribeEvent ( Checkbox::EventCheckStateChanged, Event::Subscriber (&GuiConfig::handleMBLURChanged, this));
	checkMBLUR->setSelected (GConfig->IsEnable("CHKMBLUR"));
/*
	Resolution

	Enable FullScreen

	shadows quality (disabled, medium, high, ultrahigh)
	0, 1024, 2048, 4096
	reflection quality (disabled, low, medium, high)
	0, 256, 512, 1024
	water quality (low, medium, high, ultra high)

	enable Depth of field
	enable motion blur

	*/

	// --
	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "configOK"));
	mCondigWindow->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.77f), cegui_reldim( 0.90f)));
	btn->setSize(UVector2(cegui_reldim(0.20f), cegui_reldim( 0.065f)));
	btn->setText("OK");
	btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&GuiConfig::HandleConfigOK, this));

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool GuiConfig::HandleConfigOK(const CEGUI::EventArgs& e)
{
	Editbox* playerName = static_cast<Editbox*>(winMgr.getWindow("CONFIGPLAYERNAME"));
/*
	if (_stricmp(GConfig->GetPlayerName(), playerName->getText().c_str()))
	{
		if (GClientGame)
		{
			GClientGame->SendMessageNick(playerName->getText().c_str());
		}

	}
	*/
	bool bScreenConfigHasChanged = false;

	{
		Checkbox *pcb = (Checkbox*)winMgr.getWindow("CHKBFS");
		bScreenConfigHasChanged |= GConfig->IsEnable("CHKBFS") != pcb->isSelected();
		GConfig->SetEnable("CHKBFS", (int)pcb->isSelected() );
	}
	{
		Checkbox *pcb = (Checkbox*)winMgr.getWindow("CHKBVSYNC");
		bScreenConfigHasChanged |= GConfig->IsEnable("CHKBVSYNC") != pcb->isSelected();
		GConfig->SetEnable("CHKBVSYNC", (int)pcb->isSelected() );
	}


	GConfig->SetPlayerName(playerName->getText().c_str());


	// Resolution
	Combobox* resCB = static_cast<Combobox*>(winMgr.getWindow("RESOLUTION"));
	ListboxItem *resib = resCB->getSelectedItem();
	if (resib)
	{
		int selres = resCB->getItemIndex(resib);

		bScreenConfigHasChanged |= (GConfig->GetQuality("Width") != mResolutions[selres].width);
		bScreenConfigHasChanged |= (GConfig->GetQuality("Height") != mResolutions[selres].height);

		GConfig->SetQuality("Width", mResolutions[selres].width);
		GConfig->SetQuality("Height", mResolutions[selres].height);
	}


	if (bScreenConfigHasChanged)
	{
		tstring onoksschanged = ".00 ";
		onoksschanged += GLoc->GetString("OK");
		((ZProtoGUI*)GProtoGui)->mMessageboxGui.Show(GLoc->GetString("SCREENCHANGED"), onoksschanged.c_str(), mOnOK);

		Hide();
	}
	else
	{
		mOnOK();
		Hide();
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void GuiConfig::Tick()
{
	if (mbControlBinding)
	{
		int aDevice, aButton, aAxe;
		if (GetInputDevice()->GetAnyChange(aDevice, aButton, aAxe))
		{
			RemoveBind(aDevice, aButton, aAxe); // replace with 0 any control using it

			int aDevice1, aButton1, aAxe1;
			int aDevice2, aButton2, aAxe2;

			GConfig->GetControl(aCtrlList[mBindingButtonIndex].mName, 0, aDevice1, aButton1, aAxe1);
			GConfig->GetControl(aCtrlList[mBindingButtonIndex].mName, 1, aDevice2, aButton2, aAxe2);
			if (aDevice1 || aButton1 || aAxe1)
			{
				if (aDevice2 || aButton2 || aAxe2)
				{
					GConfig->SetControl(aCtrlList[mBindingButtonIndex].mName,
						offsetCtrlBind[mBindingButtonIndex]&1, aDevice, aButton, aAxe);
					offsetCtrlBind[mBindingButtonIndex]++;
				}
				else
				{
					GConfig->SetControl(aCtrlList[mBindingButtonIndex].mName, 1, aDevice, aButton, aAxe);
				}
			}
			else
			{
				GConfig->SetControl(aCtrlList[mBindingButtonIndex].mName, 0, aDevice, aButton, aAxe);

			}

			winMgr.getWindow("BinderPopup")->destroy();
			winMgr.getWindow("Config Window")->setVisible(true);
			mbControlBinding = false;
			//aCtrlList[mBindingButtonIndex].mWindow->setText(FormatInput(aDevice, aButton, aAxe).c_str() );
			for (int allCtrl = 0;allCtrl<sizeof(aCtrlList)/sizeof(CTRLEntry); allCtrl++)
			{
				SetBindedControlString(allCtrl, aCtrlList[allCtrl].mWindow);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
