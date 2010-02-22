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

#include "stdafx.h"

#ifdef WIN32

#include "ZDirectInput8.h"

#include <shlobj.h>
//#include <malloc.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZDirectInput8)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

/*
void CheckHeap()
{
	int  heapstatus;

	heapstatus = _heapchk();
   switch( heapstatus )
   {
   case _HEAPOK:
      printf(" OK - heap is fine\n" );
      break;
   case _HEAPEMPTY:
      printf(" OK - heap is empty\n" );
      break;
   case _HEAPBADBEGIN:
      printf( "ERROR - bad start of heap\n" );
      break;
   case _HEAPBADNODE:
      printf( "ERROR - bad node in heap\n" );
      break;
   }
}
*/


void ZSleep(int aMS)
{
	Sleep(aMS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
int scan2ascii(unsigned long scancode, unsigned short* result)
{
   static HKL layout=GetKeyboardLayout(0);
   static unsigned char State[256];

   if (GetKeyboardState(State)==FALSE)
      return 0;
   unsigned int vk=MapVirtualKeyEx(scancode,1,layout);
   return ToAsciiEx(vk,scancode,State,result,0,layout);
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

ZDirectInput8::ZDirectInput8()
{
    m_pDI            = NULL;
    m_pDIKeybrd        = NULL;
    m_pDIMouse        = NULL;
    m_pDIJoy        = NULL;
    m_pMousePos.x    = 0l;
    m_pMousePos.y    = 0l;
    m_pHotSpot.x    = 0l;
    m_pHotSpot.y    = 0l;
    for(int i=0; i<8; i++)
        m_bMouseUp[i] = TRUE;

	memset(mKBbufferFront,0, sizeof(char)*256);
	memset(mKBbufferBack,0, sizeof(char)*256);
	memset(mJoyButBack,0,sizeof(BYTE)*128);
	memset(&m_js,0,sizeof(m_js));

	extern IInput *GInputDevice;
	GInputDevice = this;	

	TKEY_LEFT =			203;
	TKEY_RIGHT =		205;
	TKEY_UP =			200;
	TKEY_DOWN =			208;
	TKEY_SPACE =		57;
	TKEY_CONTROL =		29;
	TKEY_BACKQUOTE =	0xFF;
	TKEY_ALT =			0xFF;
	TKEY_a =			0x1E;
	TKEY_b =			0xFF;
	TKEY_c =			0xFF;
	TKEY_d =			0x20;
	TKEY_e =			0x12;
	TKEY_f =			0xFF;
	TKEY_g =			0xFF;
	TKEY_h =			0xFF;
	TKEY_i =			0xFF;
	TKEY_j =			0xFF;
	TKEY_k =			0xFF;
	TKEY_l =			0xFF;
	TKEY_m =			0xFF;
	TKEY_n =			0xFF;
	TKEY_o =			0x18;
	TKEY_p =			0x19;
	TKEY_q =			0x10;
	TKEY_r =			0xFF;
	TKEY_s =			0x1F;
	TKEY_t =			0xFF;
	TKEY_u =			0xFF;
	TKEY_v =			0xFF;
	TKEY_w =			0x11;
	TKEY_x =			0xFF;
	TKEY_y =			0xFF;
	TKEY_z =			0xFF;
	TKEY_DELETE = 		0xFF;
	TKEY_ESC =			1;
	TKEY_TAB =			0x0F;
	TKEY_F1 =			0x3b; 
	TKEY_F2 =			0x3c;
	TKEY_F3 =			0x3d;
	TKEY_F4 =			0x3e;
	TKEY_F5 =			0x3f;
	TKEY_F6 =			0x40;
	TKEY_F7 =			0x41;
	TKEY_F8 =			0x42;
	TKEY_F9 =			0x43;
	TKEY_F10 =			0x44;		
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZDirectInput8::~ZDirectInput8()
{
    DIShutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::Create(bool exclusive)
{
	HWND aWindow = GetActiveWindow();

    if(FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
        IID_IDirectInput8, (void**)&m_pDI, NULL))) 
        return false;

    mbExclusive = exclusive;
    mhWnd = aWindow;

	HandleKeyboard();
	HandleMouse();
	HandlePad();

    return true;
}

void ZDirectInput8::HandleKeyboard()
{
    if(FAILED(m_pDI->CreateDevice(GUID_SysKeyboard, &m_pDIKeybrd, NULL)))
        return ;

    if(FAILED(m_pDIKeybrd->SetDataFormat(&c_dfDIKeyboard)))
        return ;

    if(FAILED(m_pDIKeybrd->SetCooperativeLevel(mhWnd, 
        (mbExclusive?(DISCL_FOREGROUND | DISCL_EXCLUSIVE):(DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) ) )))
        return ;

    if (m_pDIKeybrd) m_pDIKeybrd->Acquire(); 
    memset(mKBbufferFront,0,sizeof(mKBbufferFront));
}

void ZDirectInput8::HandleMouse()
{
    if(FAILED(m_pDI->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL)))
        return ;

    if(FAILED(m_pDIMouse->SetDataFormat(&c_dfDIMouse2)))
        return ;

    if(FAILED(m_pDIMouse->SetCooperativeLevel(mhWnd, 
                    DISCL_FOREGROUND | (mbExclusive?DISCL_EXCLUSIVE:DISCL_NONEXCLUSIVE))))
        return ;

    m_hMouse = CreateEvent(0,0,0,0);

    if (m_hMouse == NULL)
    {
        DIShutdown();
        return ;
    }

    hr = m_pDIMouse->SetEventNotification(m_hMouse);

    if (FAILED(hr))
    {
        DIShutdown();
        return ;
    }

    //initialize input buffer
    DIPROPDWORD dipdw; 
    
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = 16;

    hr = m_pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

    if (FAILED(hr))
    {
        DIShutdown();
        return ;
    }

    if (m_pDIMouse) m_pDIMouse->Acquire();
}

void ZDirectInput8::HandlePad()
{
	if( FAILED(m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACKW)EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY ) ) )
		return ;

	if (!m_pDIJoy) 
		return ;

	if( FAILED(m_pDIJoy->SetDataFormat( &c_dfDIJoystick2 ) ) )
		return ;
	/*
	if( FAILED(m_pDIJoy->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND ) ) )
	return E_FAIL;
	*/
	m_DIDevCaps.dwSize = sizeof(DIDEVCAPS);
	if ( FAILED(m_pDIJoy->GetCapabilities(&m_DIDevCaps) ) )
		return ;

	if ( FAILED(m_pDIJoy->EnumObjects((LPDIENUMDEVICEOBJECTSCALLBACKW) EnumAxesCallback, (VOID*)mhWnd, DIDFT_AXIS ) ) )
		return ;

	if(m_pDIJoy) 
		m_pDIJoy->Acquire();
}


/************************************************************************/
//Method:    AcquireDevice
//Purpose:    Try to acquire the device
/************************************************************************/
HRESULT ZDirectInput8::AcquireDevice(LPDIRECTINPUTDEVICE8 device)
{
	if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
	{
		hr = device->Acquire();
		while(hr == DIERR_INPUTLOST)
			hr = device->Acquire();
	}

	if(hr == DIERR_OTHERAPPHASPRIO)
		return S_OK;

	if(FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void ZDirectInput8::Tick()
{
	PROFILER_START(DI8Tick);
    ProcessKB();
    ProcessMouse();
	ProcessJoy();
	PROFILER_END();
}

/************************************************************************/
//Method:    ProcessKB()
//Purpose:    Capture Keyboard Input
/************************************************************************/
HRESULT ZDirectInput8::ProcessKB()
{
    if (m_pDIKeybrd)
    {
        memcpy(mKBbufferBack, mKBbufferFront, 256);

	    //Check for keyboard input and fill buffer
	    hr = m_pDIKeybrd->GetDeviceState(sizeof(mKBbufferFront),(LPVOID)&mKBbufferFront); 
	    if FAILED(hr) 
	    {
		    hr = AcquireDevice(m_pDIKeybrd);
		    if(FAILED(hr))
			    return E_FAIL;

		    hr = m_pDIKeybrd->GetDeviceState(sizeof(mKBbufferFront),(LPVOID)&mKBbufferFront);
		    if(FAILED(hr))
			    return E_FAIL;
	    }
    }

	return S_OK;       
}

/************************************************************************/
//Method:    ProcessMouse
//Purpose:    Capture Mouse Input
/************************************************************************/
HRESULT ZDirectInput8::ProcessMouse()
{
    if (m_pDIMouse)
    {
	    //Check for mouse input and fill buffer
	    hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState);
	    if(FAILED(hr))
	    {
		    hr = AcquireDevice(m_pDIMouse);
		    if(FAILED(hr))
			    return E_FAIL;

		    hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState);
		    if(FAILED(hr))
			    return E_FAIL;
	    }

	    m_pMousePos.x = m_MouseState.lX;
	    m_pMousePos.y = m_MouseState.lY;


        for (int i=0;i<8;i++)
            m_bMouseUp[i] = !m_MouseState.rgbButtons[i];
    }
	return S_OK;
}

/************************************************************************/
//Method:    ProcessJoy
//Purpose:    Capture Joystick Input
/************************************************************************/
HRESULT ZDirectInput8::ProcessJoy()
{
	memcpy(mJoyButBack, m_js.rgbButtons, 128*sizeof(BYTE));
	
	if (!m_pDIJoy) return S_OK;
    //Get joystick state and fill buffer
    if(FAILED(m_pDIJoy->Poll()))  
    {
		/*
        while( m_pDIJoy->Acquire() == DIERR_INPUTLOST ) 
        {}
*/
        return E_FAIL; 
    }

    if( FAILED(m_pDIJoy->GetDeviceState( sizeof(DIJOYSTATE2), &m_js ) ) )
        return E_FAIL;

    return S_OK;
}

/************************************************************************/
//Method:    DIShutdown()
//Purpose:    Cleanup Objects
/************************************************************************/
void ZDirectInput8::DIShutdown()
{
    if (m_pDI)
    {
        if (m_pDIKeybrd)
        {
            m_pDIKeybrd->Unacquire();
            m_pDIKeybrd->Release();
            m_pDIKeybrd = NULL;
        }

        if (m_pDIMouse)
        {
            m_pDIMouse->Unacquire();
            m_pDIMouse->Release();
            m_pDIMouse = NULL;
        }
        if (m_pDIJoy)
        {
            m_pDIJoy->Unacquire();
            m_pDIJoy->Release();
            m_pDIJoy = NULL;
        }
        m_pDI->Release();
        m_pDI = NULL;
    }
    
}

/************************************************************************/
//Method:    AllMouseUp
//Purpose:    Determine If All Mouse Buttons Are Up
/************************************************************************/
bool ZDirectInput8::AllMouseUp()
{
    if(!m_MouseState.rgbButtons[0] &&
       !m_MouseState.rgbButtons[1] &&
       !m_MouseState.rgbButtons[2] &&
       !m_MouseState.rgbButtons[3] &&
       !m_MouseState.rgbButtons[4] &&
       !m_MouseState.rgbButtons[5] &&
       !m_MouseState.rgbButtons[6] &&
       !m_MouseState.rgbButtons[7])
       return TRUE;

    return FALSE;
}

/************************************************************************/
//Method:    MButtonUp
//Purpose:    Determine If A Specified Mouse Button Is Up
/************************************************************************/
bool ZDirectInput8::MButtonUp(BYTE button)
{
    if(!m_MouseState.rgbButtons[button])
        return TRUE;

    return FALSE;
}

/************************************************************************/
//Method:    EnumAxesCallback
//Purpose:    Joystick Axes Enumeration Callback
/************************************************************************/
bool __stdcall EnumAxesCallback(const DIDEVICEOBJECTINSTANCE *pdidoi, VOID *pContext)
{
    /*
    HWND hDlg = (HWND)pContext;

    DIPROPRANGE diprg; 
    diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow        = DIPH_BYID; 
    diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
    diprg.lMin              = -1000; 
    diprg.lMax              = +1000; 
    
    // Set the range for the axis
    if( FAILED( g_pDI8->m_pDIJoy->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
        return DIENUM_STOP;
*/
    return DIENUM_CONTINUE;
}

/************************************************************************/
//Method:    EnumJoysticksCallback
//Purpose:    Joystick Enumeration Callback
/************************************************************************/
bool __stdcall EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
    ZDirectInput8 * pCtx = (ZDirectInput8*)pContext;
    HRESULT hr;

    hr = pCtx->m_pDI->CreateDevice( pdidInstance->guidInstance, &pCtx->m_pDIJoy, NULL );

    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    return DIENUM_STOP;
}

/************************************************************************/
//Method:    SetHotSpot
//Purpose:    Set the hotspot offset for the cursor
/************************************************************************/
void ZDirectInput8::SetHotSpot(POINT hspot)
{
    m_pHotSpot.x = hspot.x;
    m_pHotSpot.y = hspot.y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::PadButReleased(tushort butID, tulong padID )
{
	return (mJoyButBack[butID]&&PadButUp(butID, padID));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::PadButPushed(tushort butID, tulong padID )
{
	return ((!mJoyButBack[butID])&&PadButDown(butID, padID));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::PadButDown(tushort butID, tulong padID )
{
	return ((m_js.rgbButtons[butID]&0x80)!=0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::PadButUp(tushort butID, tulong padID )
{
	return (!(m_js.rgbButtons[butID]&0x80));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZDirectInput8::PadDeltaX(tulong padID )
{
	if (m_pDIJoy)
		return (m_js.lX-0x7FFF)*(1.0f/0x7FFF);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZDirectInput8::PadDeltaY(tulong padID )
{
	if (m_pDIJoy)
		return (m_js.lY-0x7FFF)*(1.0f/0x7FFF);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZDirectInput8::PadDeltaZ(tulong padID )
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::GetAnyChange(int &aDevice, int &aButton, int &aAxe)
{
	// Kbd
	int i;
	for (i=0;i<255;i++)
	{
		if (KeyReleased(i))
		{
			aDevice = CTRL_KBD;
			aButton = i;
			aAxe = 0;
			return true;
		}
	}
	// Mouse
	for (i=0;i<3;i++)
	{
	    if (MouseButReleased(i))
		{
			aDevice = CTRL_MOUSE;
			aButton = i;
			aAxe = 0;
			return true;
		}
	}
    if (fabs(MouseDeltaX())>10)
	{
		aDevice = CTRL_MOUSE;
		aAxe = (MouseDeltaX()<0)?-1:1;
		return true;
	}
	if (fabs(MouseDeltaY())>10)
	{
		aDevice = CTRL_MOUSE;
		aAxe = (MouseDeltaY()<0)?-2:2;
		return true;
	}
	// PAD
	for (i=0;i<128;i++)
	{
		if (PadButReleased(i))
		{
			aDevice = CTRL_PAD0;
			aButton = i;
			aAxe = 0;
			return true;
		}
	}
	if (PadDeltaX())
	{
		aDevice = CTRL_PAD0;
		aButton = 0;
		aAxe = (PadDeltaX()<0)?-1:1;
		return true;
	}

	if (PadDeltaY())
	{
		aDevice = CTRL_PAD0;
		aButton = 0;
		aAxe = (PadDeltaY()<0)?-2:2;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZDirectInput8::GetValue(int aDevice, int aButton, int aAxe)
{
	switch(aDevice)
	{
	case CTRL_KBD:
		return KeyDown(aButton);
		break;
	case CTRL_MOUSE:
		if (aAxe == 1) return (MouseDeltaX()>0)?MouseDeltaX():0;
		if (aAxe == 2) return (MouseDeltaY()>0)?MouseDeltaY():0;
		if (aAxe == -1) return (MouseDeltaX()<0)?-MouseDeltaX():0;
		if (aAxe == -2) return (MouseDeltaY()<0)?-MouseDeltaY():0;
		return this->MouseButDown(aButton);
		break;
	case CTRL_PAD0:
		if (aAxe == 1) return (PadDeltaX()>0)?PadDeltaX():0;
		if (aAxe == 2) return (PadDeltaY()>0)?PadDeltaY():0;
		if (aAxe == -1) return (PadDeltaX()<0)?-PadDeltaX():0;
		if (aAxe == -2) return (PadDeltaY()<0)?-PadDeltaY():0;
		return PadButDown(aButton);
		break;
	default:
		break;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZDirectInput8::GetValueReleased(int aDevice, int aButton, int aAxe)
{
	switch(aDevice)
	{
	case CTRL_KBD:
		return KeyReleased(aButton);
		break;
	case CTRL_MOUSE:
		if (aAxe == 0) return MouseButReleased(aButton);
		break;
	case CTRL_PAD0:
		if (aAxe == 0) return PadButReleased(aButton);
		break;
	default:
		break;
	}
	return 0;
}

float ZDirectInput8::GetValuePushed(int aDevice, int aButton, int aAxe)
{
	switch(aDevice)
	{
	case CTRL_KBD:
		return KeyPushed(aButton);
		break;
	case CTRL_MOUSE:
		if (aAxe == 0) return MouseButPushed(aButton);
		break;
	case CTRL_PAD0:
		if (aAxe == 0) return PadButPushed(aButton);
		break;
	default:
		break;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZDirectInput8::SetExclusive(bool bExclusive)
{
	mbExclusive = bExclusive;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZDirectInput8::IsExclusive() const
{
	return this->mbExclusive;
}

void ZDirectInput8::MouseXY(int *x,int *y)
{
	/*

	*/
	GetCursorPos(&m_pMousePos);
	ScreenToClient(mhWnd, &m_pMousePos);

	*x = m_pMousePos.x;
	*y = m_pMousePos.y;
}

void ZDirectInput8::MouseXYViewer(int *x,int *y)
{
	*x = m_pMousePos.x;
	*y = m_pMousePos.y;
}
#endif
