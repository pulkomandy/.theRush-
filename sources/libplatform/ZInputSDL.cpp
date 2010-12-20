///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZInputSDL.cpp
// Creation : 08/06/2009
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

#include "IInput.h"

#include <math.h>

#ifdef LINUX
#define __int64 long
#endif

#include <SDL/SDL.h>

#define KEYDOWN(name, key) (name[key] & 0xFF)
#define KEYUP(name, key) (!(name[key] & 0xFF))

TKEYVAL  TKEY_LEFT;
TKEYVAL  TKEY_RIGHT;
TKEYVAL  TKEY_UP;
TKEYVAL  TKEY_DOWN;
TKEYVAL  TKEY_SPACE;
TKEYVAL  TKEY_CONTROL;
TKEYVAL  TKEY_BACKQUOTE;
TKEYVAL  TKEY_ALT;
TKEYVAL 	TKEY_a;
TKEYVAL 	TKEY_b;
TKEYVAL 	TKEY_c;
TKEYVAL 	TKEY_d;
TKEYVAL 	TKEY_e;
TKEYVAL 	TKEY_f;
TKEYVAL 	TKEY_g;
TKEYVAL 	TKEY_h;
TKEYVAL 	TKEY_i;
TKEYVAL 	TKEY_j;
TKEYVAL 	TKEY_k;
TKEYVAL 	TKEY_l;
TKEYVAL 	TKEY_m;
TKEYVAL 	TKEY_n;
TKEYVAL 	TKEY_o;
TKEYVAL 	TKEY_p;
TKEYVAL 	TKEY_q;
TKEYVAL 	TKEY_r;
TKEYVAL 	TKEY_s;
TKEYVAL 	TKEY_t;
TKEYVAL 	TKEY_u;
TKEYVAL 	TKEY_v;
TKEYVAL 	TKEY_w;
TKEYVAL 	TKEY_x;
TKEYVAL 	TKEY_y;
TKEYVAL 	TKEY_z;
TKEYVAL 	TKEY_DELETE;		
TKEYVAL  TKEY_ESC;
TKEYVAL  TKEY_TAB;
TKEYVAL  TKEY_F1;
TKEYVAL  TKEY_F2; 
TKEYVAL  TKEY_F3;
TKEYVAL  TKEY_F4;
TKEYVAL  TKEY_F5; 
TKEYVAL  TKEY_F6;
TKEYVAL  TKEY_F7; 
TKEYVAL  TKEY_F8;
TKEYVAL  TKEY_F9;
TKEYVAL TKEY_F10;

typedef struct ZPOINT
{
	int x,y;
} ZPOINT;

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZInputSDL : IInput
{
public:
	DECLARE_SERIALIZABLE
public:
	ZInputSDL();
	virtual ~ZInputSDL();

	virtual bool Create(bool exclusive = false);


	virtual void HandleKeyboard();
	virtual void HandleMouse();
	virtual void HandlePad();

	//virtual void HandlePad() = 0;
	// must be called when a keyboard/mouse/has been removed/plugged
	//virtual void HandleDevicesChanged() = 0;

	// keyboard polling
	virtual bool KeyReleased(unsigned short keyID, tulong keyboardID = 0) 
	{ 
		return (mKBKeyReleased[keyID]!=0); 
	}// (KEYUP(mKBbufferFront, keyID)&&KEYDOWN(mKBbufferBack, keyID)); }
	virtual bool KeyDown(unsigned short keyID, tulong keyboardID = 0) { return (KEYDOWN(mKBbufferFront, keyID)!=0); }
	virtual bool KeyUp(unsigned short keyID, tulong keyboardID = 0) { return KEYUP(mKBbufferFront, keyID); }

	// Mouse state polling shortcut
	virtual bool MouseButReleased(tushort butID, tulong mouseID = 0) { return false; }
	virtual bool MouseButPushed(tushort butID, tulong mouseID = 0) { return false; }
	virtual bool MouseButDown(tushort butID, tulong mouseID = 0) { return (!m_bMouseUp[butID]); }
	virtual bool MouseButUp(tushort butID, tulong mouseID = 0) { return (m_bMouseUp[butID]!=0); }
	virtual void  MouseXY(int *X, int *Y);
	virtual void  MouseXYViewer(int *X, int *Y);

	virtual float MouseDeltaX(tulong mouseID = 0) { return (float)m_pMouseDelta.x; }
	virtual float MouseDeltaY(tulong mouseID = 0) { return (float)m_pMouseDelta.y; }
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

	virtual tulong GetMemoryUsed() const { return sizeof(ZInputSDL); }

	virtual void SetExclusive(bool bExclusive)
	{
		if (mbExclusive == bExclusive)
			return;

		mbExclusive = bExclusive;
		if (mbExclusive)
		{
			SDL_ShowCursor(SDL_DISABLE);
#ifdef WIN32
			SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse(100, 100);
			SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
#endif
		}
		else
			SDL_ShowCursor(SDL_ENABLE);
	}

	virtual bool IsExclusive() const
	{
		return mbExclusive;
	}

protected:
	unsigned char mKBbufferFront[1024];
	unsigned char mKBbufferBack[1024];
	unsigned char mKBKeyReleased[1024];
	unsigned char mKBKeyPushed[1024];

	unsigned char mJoyButBack[128];

	//8 Element Array, TRUE if Button [n] is up
	bool m_bMouseUp[8];

	//Mouse Position
	ZPOINT m_pMousePos;
	ZPOINT m_pMouseDelta;
	bool mbExclusive;

	void InputKeyboardEvent(SDL_KeyboardEvent* event);
};
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZInputSDL)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////
int scan2ascii(unsigned long scancode, unsigned short* result)
{
#ifdef WIN32
   static HKL layout=GetKeyboardLayout(0);
   static unsigned char State[256];

   if (GetKeyboardState(State)==FALSE)
      return 0;
   unsigned int vk=MapVirtualKeyEx(scancode,1,layout);
   return ToAsciiEx(vk,scancode,State,result,0,layout);
#endif
#ifdef LINUX
   return 0;
#endif
}





/* Les joysticks */
SDL_Joystick * tableJoysticks[4];

/* Gestion du clavier */
void ZInputSDL::InputKeyboardEvent(SDL_KeyboardEvent* event)
{
	/* type et state contiennent en fait la même information */
	/* On utilise type */
	SDL_keysym * keysym = &event->keysym;



	if(event->type == SDL_KEYDOWN)
	{
		/*
		if(((keysym->unicode & 0xFF80) == 0) && (keysym->unicode > 32))
		{
		printf("Touche enfoncee : %c\n",event->keysym.unicode);
		}
		else
		{
		printf("Touche enfoncee non traduite\n");
		}

		printf("Nom SDL de la touche : %s\n",SDL_GetKeyName(keysym->sym));
		*/
		mKBbufferFront[keysym->scancode] = 0x80;
	}
	else
	{
		//printf("Touche relevee : %s\n",SDL_GetKeyName(keysym->sym));
		mKBbufferFront[keysym->scancode] = 0;
	}

}

/* Gestion des mouvements de la souris */
void InputMouseMotionEvent(SDL_MouseMotionEvent* event)
{
	printf("Mouvement de la souris (%i,%i) . Coordonnees dans la fenetres (%i,%i)",
		event->xrel, event->yrel,
		event->x   , event->y);

	if(event->state & SDL_BUTTON(1))
	{
		printf(" - B1");
	}
	if(event->state & SDL_BUTTON(2))
	{
		printf(" - B2");
	}
	if(event->state & SDL_BUTTON(3))
	{
		printf(" - B3");
	}
	printf("\n");
}

/* Gestion des boutons de la souris */
void InputMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	if(event->type == SDL_MOUSEBUTTONDOWN)
	{
		printf("Bouton %i appuye",event->button);
	}
	else
	{
		printf("Bouton %i relache",event->button);
	}
	printf(" aux coordonnees (%i,%i)\n",event->x,event->y);
}

/* Initialisation des joysticks */
void InputInitJoysticks()
{
	/* Etat initial. Le hot-plug est specifique OS */
	int numJoystick = SDL_NumJoysticks();
	int i;

	printf("Nombre de joysticks detectes : %i\n",numJoystick);

	if(numJoystick>4)
	{
		printf("Seuls les quatre premiers joysticks seront pris en compte\n");
		numJoystick = 4;
	}

	for(i=0 ; i<numJoystick ; i++)
	{
		printf("Nom du joystick %i : %s\n", i, SDL_JoystickName(i));
		tableJoysticks[i] = SDL_JoystickOpen(i);
		if(tableJoysticks[i])
		{
			printf("Nombre d'axes : %d\n",     SDL_JoystickNumAxes   (tableJoysticks[i]));
			printf("Nombre de boutons : %d\n", SDL_JoystickNumButtons(tableJoysticks[i]));
			printf("... joystick ouvert\n");
		}
		else
		{
			printf("... probleme d'ouverture avec ce joystick\n");
		}
	}
	for(i=numJoystick ; i<4 ; i++)
	{
		tableJoysticks[i] = NULL;
	}
}

/* Ferme correctement les joysticks */
void InputCloseJoysticks()
{
	int i;
	for(i=0; i<4; i++)
	{
		if(tableJoysticks[i])
		{
			SDL_JoystickClose(tableJoysticks[i]);
			printf("Joystick %i ferme\n",i);
		}
	}
}

/* Gestion des axes d'un joystick */
void InputJoystickAxisEvent(SDL_JoyAxisEvent * event)
{
	printf("Axe %i du joystick %i : %i\n",event->axis,event->which,event->value);
}

/* Gestion des buttons d'un joystick */
void InputJoystickButtonEvent(SDL_JoyButtonEvent* event)
{
	printf("Bouton %i du joystick %i : ",event->button,event->which);
	if(event->state == SDL_PRESSED)
	{
		printf("appuye\n");
	}
	else
	{
		printf("relache\n");
	}
}

void Quitter()
{
	InputCloseJoysticks();
	SDL_Quit();
}





















///////////////////////////////////////////////////////////////////////////////////////////////////

ZInputSDL::ZInputSDL()
{
	m_pMouseDelta.x = m_pMouseDelta.y = m_pMousePos.x = m_pMousePos.y = 0;
	SetExclusive(true);
	memset(&m_bMouseUp[0],0xFF, sizeof(bool)*8);

	memset(mKBbufferBack, 0, 1024);
	memset(mKBbufferFront, 0, 1024);
	memset(mKBKeyReleased, 0, 1024);
	memset(mKBKeyPushed, 0, 1024);
	


	TKEY_LEFT =			SDLK_LEFT;
	TKEY_RIGHT =		SDLK_RIGHT;
	TKEY_UP =			SDLK_UP;
	TKEY_DOWN =			SDLK_DOWN;
	TKEY_SPACE =		SDLK_SPACE;
	TKEY_CONTROL =		SDLK_LCTRL;
	TKEY_BACKQUOTE =	SDLK_BACKQUOTE;
	TKEY_ALT =			SDLK_LALT;
	TKEY_a =			97;
	TKEY_b =			98;
	TKEY_c =			99;
	TKEY_d =			100;
	TKEY_e =			101;
	TKEY_f =			102;
	TKEY_g =			103;
	TKEY_h =			104;
	TKEY_i =			105;
	TKEY_j =			106;
	TKEY_k =			107;
	TKEY_l =			108;
	TKEY_m =			109;
	TKEY_n =			110;
	TKEY_o =			111;
	TKEY_p =			112;
	TKEY_q =			113;
	TKEY_r =			114;
	TKEY_s =			115;
	TKEY_t =			116;
	TKEY_u =			117;
	TKEY_v =			118;
	TKEY_w =			119;
	TKEY_x =			120;
	TKEY_y =			121;
	TKEY_z =			122;
	TKEY_DELETE = 		127;
	TKEY_ESC =			SDLK_ESCAPE;
	TKEY_TAB =			SDLK_TAB;
	TKEY_F1 =			SDLK_F1; 
	TKEY_F2 =			SDLK_F2;
	TKEY_F3 =			SDLK_F3;
	TKEY_F4 =			SDLK_F4;
	TKEY_F5 =			SDLK_F5;
	TKEY_F6 =			SDLK_F6;
	TKEY_F7 =			SDLK_F7;
	TKEY_F8 =			SDLK_F8;
	TKEY_F9 =			SDLK_F9;
	TKEY_F10 =			SDLK_F10;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZInputSDL::~ZInputSDL()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ZInputSDL::Create(bool exclusive)
{
#if 0
	/* Initialisation de la partie Vidéo de SDL */
	if (SDL_Init (SDL_INIT_JOYSTICK|SDL_INIT_VIDEO) < 0)
		/* En cas d'échec, sortie */
		exit (1);

	/* Lorsque le programme se termine, il ferme SDL correctement */
	atexit(Quitter);

	//SDL_SetVideoMode(320, 240, 0, SDL_ANYFORMAT);
#endif
	extern IInput *GInputDevice;
	GInputDevice = this;
	SDL_EnableUNICODE(0);
	memset(mKBbufferFront,0, sizeof(char)*1024);
	memset(mKBbufferBack,0, sizeof(char)*1024);
	InputInitJoysticks(/*tableJoysticks*/);
	return true;
}


void ZInputSDL::HandleKeyboard()
{

}

void ZInputSDL::HandleMouse()
{

}

void ZInputSDL::HandlePad()
{

}

void  ZInputSDL::MouseXY(int *X, int *Y)
{
	SDL_GetMouseState(X, Y);
}
void  ZInputSDL::MouseXYViewer(int *X, int *Y)
{
	*X = m_pMousePos.x;
	*Y = m_pMousePos.y;
}

void ZInputSDL::Tick()
{
	//PROFILER_START(SDLInputTick);
	ZPOINT tempmousepos;
	char buts = SDL_GetMouseState(&tempmousepos.x, &tempmousepos.y);
	m_bMouseUp[1] = true;
	m_bMouseUp[2] = true;
	if(buts)
	{
		m_bMouseUp[1] = !(SDL_BUTTON(1));
		m_bMouseUp[2] = !(SDL_BUTTON(2));
	}
#ifndef WIN32	
	static int mPreviousX=999, mPreviousY=999;
if (mPreviousX == 999)
{
mPreviousX = m_pMousePos.x;
mPreviousY = m_pMousePos.y;
}

int prevx = m_pMousePos.x;
int prevy = m_pMousePos.y;

	m_pMousePos.x -= mPreviousX;
	m_pMousePos.y -= mPreviousY;
	
mPreviousX = prevx;
mPreviousY = prevy;

#else
	if (mbExclusive)
	{

		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse(100, 100);
		SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
	}
	tempmousepos.x -= 100;
	tempmousepos.y -= 100;

#endif

	m_pMouseDelta.x = tempmousepos.x - m_pMousePos.x;
	m_pMouseDelta.y = tempmousepos.y - m_pMousePos.y;
	m_pMousePos = tempmousepos;

	

	memcpy(mKBbufferBack, mKBbufferFront, 1024);
	memset(mKBbufferFront, 0, 1024);
	memset(mKBKeyReleased, 0, 1024);
	memset(mKBKeyPushed, 0, 1024);
	
	int numkeys = 0;
	Uint8 *keystate = SDL_GetKeyState(&numkeys);

	for (int i=0;i<numkeys;i++)
	{
		if (keystate[i])
		{
			mKBbufferFront[i] = 0x80;
			if (!mKBbufferBack[i])
			{
				mKBKeyPushed[i] = 1;
				//LOG("Key released\n");
			}
		}
		else
		{
			if (mKBbufferBack[i])
			{
				mKBKeyReleased[i] = 1;
				//LOG("Key released\n");
			}
		}

	}



	//PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZInputSDL::PadButReleased(tushort butID, tulong padID )
{
	return (mJoyButBack[butID]&&PadButUp(butID, padID));
}
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZInputSDL::PadButPushed(tushort butID, tulong padID )
{
	// FIXME
	return (mJoyButBack[butID]&&PadButUp(butID, padID));
}
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZInputSDL::PadButDown(tushort butID, tulong padID )
{
	return false;//((m_js.rgbButtons[butID]&0x80)!=0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZInputSDL::PadButUp(tushort butID, tulong padID )
{
	return false;//(!(m_js.rgbButtons[butID]&0x80));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZInputSDL::PadDeltaX(tulong padID )
{
	/*
	if (m_pDIJoy)
	return (m_js.lX-0x7FFF)*(1.0f/0x7FFF);
	*/
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZInputSDL::PadDeltaY(tulong padID )
{
	/*
	if (m_pDIJoy)
	return (m_js.lY-0x7FFF)*(1.0f/0x7FFF);
	*/
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZInputSDL::PadDeltaZ(tulong padID )
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZInputSDL::GetAnyChange(int &aDevice, int &aButton, int &aAxe)
{
	// Kbd
	int i;
	for (i=0;i<1024;i++)
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

float ZInputSDL::GetValue(int aDevice, int aButton, int aAxe)
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

float ZInputSDL::GetValueReleased(int aDevice, int aButton, int aAxe)
{
	switch(aDevice)
	{
	case CTRL_KBD:
		if ( mKBKeyReleased[aButton] )
		{
			//LOG("KBD released\n");
			return 1.f;
		}
		/*
		else
		{
			LOG("GN22222\né");
		}
		*/

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
	return 0.f;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

float ZInputSDL::GetValuePushed(int aDevice, int aButton, int aAxe)
{
	switch(aDevice)
	{
	case CTRL_KBD:
		if ( mKBKeyPushed[aButton] )
		{
			//LOG("KBD released\n");
			return 1.f;
		}
		/*
		else
		{
			LOG("GN22222\né");
		}
		*/

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
	return 0.f;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
