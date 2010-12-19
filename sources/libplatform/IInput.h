///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : IImporter.h
// Creation : 13/06/2007
// Author : Cedric Guillemet
// Description :
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
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IINPUT_H__
#define IINPUT_H__

#include "..\libbase\ZSerializator.h"

#ifdef WIN32
DECLAREZCLASS(ZDirectInput8);
#endif
DECLAREZCLASS(ZInputSDL);

/*! \file IInput.h
    \brief Input Device Interface
    Handles keyboard/mouse
*/


#ifdef WIN32
#define USE_SDL
#endif

#ifdef LINUX
#define USE_SDL
#endif
/*
#ifdef USE_SDL

#define TKEY_LEFT  SDLK_LEFT
#define TKEY_RIGHT  SDLK_RIGHT
#define TKEY_UP  SDLK_UP
#define TKEY_DOWN  SDLK_DOWN
#define TKEY_SPACE  SDLK_SPACE
#define TKEY_CONTROL  SDLK_LCTRL
#define TKEY_BACKQUOTE SDLK_BACKQUOTE
#define TKEY_ALT SDLK_LALT
enum
{
	TKEY_a			= 97,
	TKEY_b			= 98,
	TKEY_c			= 99,
	TKEY_d			= 100,
	TKEY_e			= 101,
	TKEY_f			= 102,
	TKEY_g			= 103,
	TKEY_h			= 104,
	TKEY_i			= 105,
	TKEY_j			= 106,
	TKEY_k			= 107,
	TKEY_l			= 108,
	TKEY_m			= 109,
	TKEY_n			= 110,
	TKEY_o			= 111,
	TKEY_p			= 112,
	TKEY_q			= 113,
	TKEY_r			= 114,
	TKEY_s			= 115,
	TKEY_t			= 116,
	TKEY_u			= 117,
	TKEY_v			= 118,
	TKEY_w			= 119,
	TKEY_x			= 120,
	TKEY_y			= 121,
	TKEY_z			= 122,
	TKEY_DELETE		= 127
};

#define TKEY_ESC  SDLK_ESCAPE
#define TKEY_TAB SDLK_TAB

#define TKEY_F1 SDLK_F1 // (F1),
#define TKEY_F2 SDLK_F2
#define TKEY_F3 SDLK_F3
#define TKEY_F4 SDLK_F4
#define TKEY_F5 SDLK_F5
#define TKEY_F6 SDLK_F6
#define TKEY_F7 SDLK_F7
#define TKEY_F8 SDLK_F8
#define TKEY_F9 SDLK_F9
#define TKEY_F10 SDLK_F10

#else
#define TKEY_LEFT  203
#define TKEY_RIGHT  205
#define TKEY_UP  200
#define TKEY_DOWN  208
#define TKEY_SPACE  57
#define TKEY_CONTROL  29
#define TKEY_a 0x1E
#define TKEY_d 0x20
#define TKEY_Q 0x10
#define TKEY_w 0x11
#define TKEY_E 0x12
#define TKEY_s 0x1F
#define TKEY_ESC  1
#define TKEY_TAB 0x0F
#define TKEY_o 0x18
#define TKEY_P 0x19

#define TKEY_F1 0x3b // (F1),
#define TKEY_F2 0x3c
#define TKEY_F3 0x3d
#define TKEY_F4 0x3e
#define TKEY_F5 0x3f
#define TKEY_F6 0x40
#define TKEY_F7 0x41
#define TKEY_F8 0x42
#define TKEY_F9 0x43
#define TKEY_F10 0x44
#endif
*/
typedef unsigned short TKEYVAL;
extern TKEYVAL  TKEY_LEFT;
extern TKEYVAL  TKEY_RIGHT;
extern TKEYVAL  TKEY_UP;
extern TKEYVAL  TKEY_DOWN;
extern TKEYVAL  TKEY_SPACE;
extern TKEYVAL  TKEY_CONTROL;
extern TKEYVAL  TKEY_BACKQUOTE;
extern TKEYVAL  TKEY_ALT;
extern TKEYVAL 	TKEY_a;
extern TKEYVAL 	TKEY_b;
extern TKEYVAL 	TKEY_c;
extern TKEYVAL 	TKEY_d;
extern TKEYVAL 	TKEY_e;
extern TKEYVAL 	TKEY_f;
extern TKEYVAL 	TKEY_g;
extern TKEYVAL 	TKEY_h;
extern TKEYVAL 	TKEY_i;
extern TKEYVAL 	TKEY_j;
extern TKEYVAL 	TKEY_k;
extern TKEYVAL 	TKEY_l;
extern TKEYVAL 	TKEY_m;
extern TKEYVAL 	TKEY_n;
extern TKEYVAL 	TKEY_o;
extern TKEYVAL 	TKEY_p;
extern TKEYVAL 	TKEY_q;
extern TKEYVAL 	TKEY_r;
extern TKEYVAL 	TKEY_s;
extern TKEYVAL 	TKEY_t;
extern TKEYVAL 	TKEY_u;
extern TKEYVAL 	TKEY_v;
extern TKEYVAL 	TKEY_w;
extern TKEYVAL 	TKEY_x;
extern TKEYVAL 	TKEY_y;
extern TKEYVAL 	TKEY_z;
extern TKEYVAL 	TKEY_DELETE;		
extern TKEYVAL  TKEY_ESC;
extern TKEYVAL  TKEY_TAB;
extern TKEYVAL  TKEY_F1;
extern TKEYVAL  TKEY_F2; 
extern TKEYVAL  TKEY_F3;
extern TKEYVAL  TKEY_F4;
extern TKEYVAL  TKEY_F5; 
extern TKEYVAL  TKEY_F6;
extern TKEYVAL  TKEY_F7; 
extern TKEYVAL  TKEY_F8;
extern TKEYVAL  TKEY_F9;
extern TKEYVAL TKEY_F10;

enum CONTROL_DEVICE
{
	CTRL_KBD,
	CTRL_MOUSE,
	CTRL_PAD0,
	CTRL_PAD1
};
//#endif
/**
Input Interface
\brief Input
*/

class IInput : public ZBaseClass
{
public:

    // as the input needs every frame tick, when it's created, it's added to the MT core
    // after each tick, it's readded until it's destroyed.

    virtual bool Create(bool exclusive = false) = 0;

    //! ask for a keyboard.
    //! if no keyboard is present when this function is called, every keyboard related function will return false
    //! if a keyboard is later plugged, HandleDevicesChanged() will init it and keyboard related functions will properly work
    virtual void HandleKeyboard() = 0;
    virtual void HandleMouse() = 0;
	virtual void HandlePad() = 0;

    //virtual tvoid HandlePad() = 0;
    //! must be called when a keyboard/mouse/has been removed/plugged
    //virtual tvoid HandleDevicesChanged() = 0;

    //! keyboard polling
    virtual bool KeyReleased(unsigned short keyID, tulong keyboardID = 0) = 0;
    virtual bool KeyDown(unsigned short keyID, tulong keyboardID = 0) = 0;
    virtual bool KeyUp(unsigned short keyID, tulong keyboardID = 0) = 0;

    //! Mouse state polling shortcut
    virtual bool MouseButReleased(tushort butID, tulong mouseID = 0) = 0;
    virtual bool MouseButDown(tushort butID, tulong mouseID = 0) = 0;
    virtual bool MouseButUp(tushort butID, tulong mouseID = 0) = 0;

    virtual float MouseDeltaX(tulong mouseID = 0) = 0;
    virtual float MouseDeltaY(tulong mouseID = 0) = 0;
    virtual float MouseDeltaZ(tulong mouseID = 0) = 0;
	virtual void  MouseXY(int *X, int *Y) = 0;
	virtual void  MouseXYViewer(int *X, int *Y) = 0;
	//! PAD
    virtual bool PadButReleased(tushort butID, tulong padID = 0) = 0;
    virtual bool PadButDown(tushort butID, tulong padID = 0) = 0;
    virtual bool PadButUp(tushort butID, tulong padID = 0) = 0;

    virtual float PadDeltaX(tulong padID = 0) = 0;
    virtual float PadDeltaY(tulong padID = 0) = 0;
    virtual float PadDeltaZ(tulong padID = 0) = 0;

	// Generic
	virtual bool GetAnyChange(int &aDevice, int &aButton, int &aAxe) = 0;
	virtual float GetValue(int aDevice, int aButton, int aAxe) = 0;
	// only suitable for buttons
	virtual float GetValueReleased(int aDevice, int aButton, int aAxe) = 0;
	virtual float GetValuePushed(int aDevice, int aButton, int aAxe) = 0;

    /** Called once a frame by the framework.
    Don't call it in scripts
    */
    virtual void Tick() = 0;

		 
	virtual void SetExclusive(bool bExclusive) = 0;
	virtual bool IsExclusive() const = 0;
};

/** \fn IInput * GetInputDevice()
\brief Returns the Interface to the input device
\return IInput Interface
*/
IInput *GetInputDevice();
int scan2ascii(unsigned long scancode, unsigned short* result);

#endif
