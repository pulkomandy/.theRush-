///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZDirectInput8.h
// Creation : 24/06/2007
// Author : Cedric Guillemet
// Description :
// Reliability :
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

#ifndef ZDIRECTINPUT8_H__
#define ZDIRECTINPUT8_H__

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>

#define KEYDOWN(name, key) (name[key] & 0x80)
#define KEYUP(name, key) (!(name[key] & 0x80))


#include "IInput.h"

////////////////////////////////////////////////////////////


class ZDirectInput8 : IInput
{
public:
    DECLARE_SERIALIZABLE
public:
    ZDirectInput8();
    virtual ~ZDirectInput8();

    virtual bool Create(bool exclusive = false);


    virtual void HandleKeyboard();
    virtual void HandleMouse();
	virtual void HandlePad();

    //virtual void HandlePad() = 0;
    // must be called when a keyboard/mouse/has been removed/plugged
    //virtual void HandleDevicesChanged() = 0;

    // keyboard polling
    virtual bool KeyReleased(unsigned short keyID, tulong keyboardID = 0) { return (KEYUP(mKBbufferFront, keyID)&&KEYDOWN(mKBbufferBack, keyID)); }
	virtual bool KeyPushed(unsigned short keyID, tulong keyboardID = 0) { return (KEYDOWN(mKBbufferFront, keyID)&&KEYUP(mKBbufferBack, keyID)); }

    virtual bool KeyDown(unsigned short keyID, tulong keyboardID = 0) { return (KEYDOWN(mKBbufferFront, keyID)!=0); }
    virtual bool KeyUp(unsigned short keyID, tulong keyboardID = 0) { return KEYUP(mKBbufferFront, keyID); }

    // Mouse state polling shortcut
    virtual bool MouseButReleased(tushort butID, tulong mouseID = 0) { return false; }
	virtual bool MouseButPushed(tushort butID, tulong mouseID = 0) { return false; }
    virtual bool MouseButDown(tushort butID, tulong mouseID = 0) { return (!m_bMouseUp[butID-1]); }
    virtual bool MouseButUp(tushort butID, tulong mouseID = 0) { return (m_bMouseUp[butID-1]!=0); }

    virtual float MouseDeltaX(tulong mouseID = 0) { return (float)m_pMousePos.x; }
    virtual float MouseDeltaY(tulong mouseID = 0) { return (float)m_pMousePos.y; }
    virtual float MouseDeltaZ(tulong mouseID = 0) { return 0; }

	// PAD
    virtual bool PadButReleased(tushort butID, tulong padID = 0);
	virtual bool PadButPushed(tushort butID, tulong padID = 0);
    virtual bool PadButDown(tushort butID, tulong padID = 0);
    virtual bool PadButUp(tushort butID, tulong padID = 0);

    virtual float PadDeltaX(tulong padID = 0);
    virtual float PadDeltaY(tulong padID = 0);
    virtual float PadDeltaZ(tulong padID = 0);

	// Generic

	virtual bool GetAnyChange(int &aDevice, int &aButton, int &aAxe);
	virtual float GetValue(int aDevice, int aButton, int aAxe);
	virtual float GetValueReleased(int aDevice, int aButton, int aAxe);
	virtual float GetValuePushed(int aDevice, int aButton, int aAxe);
	// --

    virtual void Tick();

	void MouseXY(int *x,int *y);
	void MouseXYViewer(int *x,int *y);


    virtual tulong GetMemoryUsed() const { return sizeof(ZDirectInput8); }
	virtual void SetExclusive(bool bExclusive);
	virtual bool IsExclusive() const;
protected:
    //Set cursor hot spot
    void SetHotSpot(POINT hspot);

    //8 Element Array, TRUE if Button [n] is up
    bool m_bMouseUp[8];

    //Mouse Position
    POINT m_pMousePos;

    //Cursor HotSpot
    POINT m_pHotSpot;

    //DI Device Objects
    LPDIRECTINPUTDEVICE8 m_pDIJoy;
    LPDIRECTINPUTDEVICE8 m_pDIKeybrd;
    LPDIRECTINPUTDEVICE8 m_pDIMouse;

    //Device State Buffers
    DIJOYSTATE2 m_js;
    DIMOUSESTATE2 m_MouseState;

    char mKBbufferFront[256];
    char mKBbufferBack[256];
	BYTE mJoyButBack[128];

    DWORD m_dwElements;

    //DI Object
    LPDIRECTINPUT8 m_pDI;

    //DI Device Capabilities
    DIDEVCAPS m_DIDevCaps;

    //Initialize Direct Input
    //HRESULT InitDI(HWND hWnd, HINSTANCE hInstance, DWORD ToUse, bool bExclusive);

    //Overridable Input Processing Methods
    virtual HRESULT ProcessMouse();
    virtual HRESULT ProcessKB();
    virtual HRESULT ProcessJoy();

    //Check if Mouse Buttons Up
    bool MButtonUp(BYTE button);
    bool AllMouseUp();

    //Release and Delete DI Objects
    void DIShutdown();

    //Construction/Destruction


	HRESULT AcquireDevice(LPDIRECTINPUTDEVICE8 device);

    //Mouse Handle
    HANDLE m_hMouse;
    HRESULT hr;

    //Joystick callbacks
    friend bool __stdcall EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
    friend bool __stdcall EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

    bool mbExclusive;
    HWND mhWnd;
};


#endif // !defined(AFX_DIRECTINPUT8_H__C7F3B7B1_FF8E_4F74_8A80_10A7E827F1B8__INCLUDED_)
