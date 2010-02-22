#include "stdafx.h"
#ifdef WIN32
#include "CEGUI.h"


inline std::string ScancodeToString(int scancode)
{
   std::string value;
   BYTE keyboardState[256];
   unsigned char buffer[2];
   buffer[0] = buffer[1] = '\0';

   // Retrieve the keyboard layout in order to perform the necessary convertions
   HKL hklKeyboardLayout = GetKeyboardLayout(0); // 0 means current thread
   // This seemingly cannot fail
   // If this value is cached then the application must respond to WM_INPUTLANGCHANGE


   // Retrieve the keyboard state
   //if( FAILED( g_pKeyboard->GetDeviceState(sizeof(keyboardState), keyboardState) ) )
   //   return "";

   // Retrieve the keyboard state
   // Handles CAPS-lock and SHIFT, which g_pKeyboard->GetDeviceState() does not
   if (GetKeyboardState(keyboardState) == FALSE)
      return "";


   /* 0. Convert virtual-key code into a scan code
       1. Convert scan code into a virtual-key code
         Does not distinguish between left- and right-hand keys.
       2. Convert virtual-key code into an unshifted character value
         in the low order word of the return value. Dead keys (diacritics)
        are indicated by setting the top bit of the return value.
       3. Windows NT/2000/XP: Convert scan code into a virtual-key
         Distinguishes between left- and right-hand keys.*/
   UINT virtualKey = MapVirtualKeyEx(scancode, 3, hklKeyboardLayout);
   if (virtualKey == 0) // No translation possible
      return "";


  /* Parameter 5:
       0. No menu is active
       1. A menu is active
     Return values:
       0. No translation available
       1. A translation exists
       2. Dead-key could not be combined with character   */
  if (ToAsciiEx(virtualKey, scancode, keyboardState, (LPWORD) buffer, 0, hklKeyboardLayout) == 1)
     value = buffer[0];
  return value;
}

inline CEGUI::uint SDLKeyToCEGUIKey(SDLKey key)
{
	using namespace CEGUI;
	switch (key)
	{
	case SDLK_BACKSPACE:    return Key::Backspace;
	case SDLK_TAB:          return Key::Tab;
	case SDLK_RETURN:       return Key::Return;
	case SDLK_PAUSE:        return Key::Pause;
	case SDLK_ESCAPE:       return Key::Escape;
	case SDLK_SPACE:        return Key::Space;
	case SDLK_COMMA:        return Key::Comma;
	case SDLK_MINUS:        return Key::Minus;
	case SDLK_PERIOD:       return Key::Period;
	case SDLK_SLASH:        return Key::Slash;
	case SDLK_0:            return Key::Zero;
	case SDLK_1:            return Key::One;
	case SDLK_2:            return Key::Two;
	case SDLK_3:            return Key::Three;
	case SDLK_4:            return Key::Four;
	case SDLK_5:            return Key::Five;
	case SDLK_6:            return Key::Six;
	case SDLK_7:            return Key::Seven;
	case SDLK_8:            return Key::Eight;
	case SDLK_9:            return Key::Nine;
	case SDLK_COLON:        return Key::Colon;
	case SDLK_SEMICOLON:    return Key::Semicolon;
	case SDLK_EQUALS:       return Key::Equals;
	case SDLK_LEFTBRACKET:  return Key::LeftBracket;
	case SDLK_BACKSLASH:    return Key::Backslash;
	case SDLK_RIGHTBRACKET: return Key::RightBracket;
	case SDLK_a:            return Key::A;
	case SDLK_b:            return Key::B;
	case SDLK_c:            return Key::C;
	case SDLK_d:            return Key::D;
	case SDLK_e:            return Key::E;
	case SDLK_f:            return Key::F;
	case SDLK_g:            return Key::G;
	case SDLK_h:            return Key::H;
	case SDLK_i:            return Key::I;
	case SDLK_j:            return Key::J;
	case SDLK_k:            return Key::K;
	case SDLK_l:            return Key::L;
	case SDLK_m:            return Key::M;
	case SDLK_n:            return Key::N;
	case SDLK_o:            return Key::O;
	case SDLK_p:            return Key::P;
	case SDLK_q:            return Key::Q;
	case SDLK_r:            return Key::R;
	case SDLK_s:            return Key::S;
	case SDLK_t:            return Key::T;
	case SDLK_u:            return Key::U;
	case SDLK_v:            return Key::V;
	case SDLK_w:            return Key::W;
	case SDLK_x:            return Key::X;
	case SDLK_y:            return Key::Y;
	case SDLK_z:            return Key::Z;
	case SDLK_DELETE:       return Key::Delete;
	case SDLK_KP0:          return Key::Numpad0;
	case SDLK_KP1:          return Key::Numpad1;
	case SDLK_KP2:          return Key::Numpad2;
	case SDLK_KP3:          return Key::Numpad3;
	case SDLK_KP4:          return Key::Numpad4;
	case SDLK_KP5:          return Key::Numpad5;
	case SDLK_KP6:          return Key::Numpad6;
	case SDLK_KP7:          return Key::Numpad7;
	case SDLK_KP8:          return Key::Numpad8;
	case SDLK_KP9:          return Key::Numpad9;
	case SDLK_KP_PERIOD:    return Key::Decimal;
	case SDLK_KP_DIVIDE:    return Key::Divide;
	case SDLK_KP_MULTIPLY:  return Key::Multiply;
	case SDLK_KP_MINUS:     return Key::Subtract;
	case SDLK_KP_PLUS:      return Key::Add;
	case SDLK_KP_ENTER:     return Key::NumpadEnter;
	case SDLK_KP_EQUALS:    return Key::NumpadEquals;
	case SDLK_UP:           return Key::ArrowUp;
	case SDLK_DOWN:         return Key::ArrowDown;
	case SDLK_RIGHT:        return Key::ArrowRight;
	case SDLK_LEFT:         return Key::ArrowLeft;
	case SDLK_INSERT:       return Key::Insert;
	case SDLK_HOME:         return Key::Home;
	case SDLK_END:          return Key::End;
	case SDLK_PAGEUP:       return Key::PageUp;
	case SDLK_PAGEDOWN:     return Key::PageDown;
	case SDLK_F1:           return Key::F1;
	case SDLK_F2:           return Key::F2;
	case SDLK_F3:           return Key::F3;
	case SDLK_F4:           return Key::F4;
	case SDLK_F5:           return Key::F5;
	case SDLK_F6:           return Key::F6;
	case SDLK_F7:           return Key::F7;
	case SDLK_F8:           return Key::F8;
	case SDLK_F9:           return Key::F9;
	case SDLK_F10:          return Key::F10;
	case SDLK_F11:          return Key::F11;
	case SDLK_F12:          return Key::F12;
	case SDLK_F13:          return Key::F13;
	case SDLK_F14:          return Key::F14;
	case SDLK_F15:          return Key::F15;
	case SDLK_NUMLOCK:      return Key::NumLock;
	case SDLK_SCROLLOCK:    return Key::ScrollLock;
	case SDLK_RSHIFT:       return Key::RightShift;
	case SDLK_LSHIFT:       return Key::LeftShift;
	case SDLK_RCTRL:        return Key::RightControl;
	case SDLK_LCTRL:        return Key::LeftControl;
	case SDLK_RALT:         return Key::RightAlt;
	case SDLK_LALT:         return Key::LeftAlt;
	case SDLK_LSUPER:       return Key::LeftWindows;
	case SDLK_RSUPER:       return Key::RightWindows;
	case SDLK_SYSREQ:       return Key::SysRq;
	case SDLK_MENU:         return Key::AppMenu;
	case SDLK_POWER:        return Key::Power;
	default:                return 0;
	}
	return 0;
}
#endif
void ZenFWSDLWindow::Init()
{
	if( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_JOYSTICK  ) < 0 || !SDL_GetVideoInfo() )
		return ;
	INITPARAMS RendererInitParams;
	GConfig->BuildInitParams(&RendererInitParams);
	SDL_SetVideoMode( RendererInitParams.width, RendererInitParams.height, SDL_GetVideoInfo()->vfmt->BitsPerPixel, 0 );
	SDL_WM_SetCaption( ".the rush//build "__DATE__" // "__TIME__, 0 ); 

#ifdef XXWIN32
	mInputDevice = (IInput*)_New(ZDirectInput8);
#else
	mInputDevice = (IInput*)_New(ZInputSDL);
#endif
	mInputDevice->Create();

}
void ZenFWSDLWindow::Run() 
{ 
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_VIDEORESIZE:
			LOG("SDL_VIDEORESIZE\n");
			break;
			case SDL_ACTIVEEVENT:
				{

						SDL_ActiveEvent* activeevent = (SDL_ActiveEvent*)&event;
					static int mPreviousGain = -1;
					if (mPreviousGain != activeevent->gain)
					{
						LOG("SDL_ACTIVEEVENT with gain = %d\n", activeevent->gain);
						if (activeevent->gain)
						{
							GetInputDevice()->SetExclusive( (ZGetCursorVisibility() == 0) );
						}
						else
						{
							
							GetInputDevice()->SetExclusive(false);
							//SDL_IconifyWindow();
							{
								//SDL_SysWMinfo info;
								SDL_Surface *sdl_screen;
								int retval = 0;

								/* See if the display is in fullscreen mode */
								sdl_screen = SDL_GetVideoSurface();
								if (sdl_screen->flags & SDL_FULLSCREEN) 
								{
									SDL_WM_IconifyWindow();
								}
							}
							

						}
						mPreviousGain = activeevent->gain;
					}
				}
			break;

		case SDL_KEYDOWN:
#ifdef WIN32
#if 0
			{
				// to tell CEGUI that a key was pressed, we inject the scancode.
				//InjectKeyDown(event.syswm.msg->wParam);

				//if (event.syswm.msg->msg == WM_CHAR)
				//unsigned short (event.syswm.msg->msg>>16)
				unsigned long wParam = event.syswm.msg->msg;//->msg;
				unsigned long lParam = wParam;
				if (wParam == 0xd)
					//wParam = 0x1c;
					InjectKeyDown(0x1c);
				else
				{
					// conversion for few keys
					unsigned char kcc = (lParam>>24)&0xFF;
					switch(kcc)
					{
					case 0x4b: 
						kcc = 0xCB; 
						break;
					case 0x4d: 
						kcc = 0xCD; 
						break;
					case 0x53: kcc = 0xD3;
						break;
					default: break;

					}

					InjectKeyDown(kcc);
				}
				//InjectChar(kcc);
			}
#endif
			{
			unsigned int scanc = SDLKeyToCEGUIKey(event.key.keysym.sym);
			std::string val = ScancodeToString(scanc);
			if (val.empty() || (val[0]<0x20))
			{
				InjectKeyDown(scanc);
				//InjectChar(scanc);
			}
			else
			{
				InjectChar(val[0]);
				InjectKeyDown(val[0]);
			}
			}
#else
			// to tell CEGUI that a key was pressed, we inject the scancode.
			InjectKeyDown(event.key.keysym.scancode); 
			if (event.key.keysym.unicode != 0)
			{
				InjectChar(event.key.keysym.unicode);
			}
#endif
			break;

			// key up
		case SDL_KEYUP:
#ifdef WIN32
			// like before we inject the scancode directly.
			//WM_KEYUP
			//InjectKeyUp(event.syswm.msg->wParam);
		{
			unsigned int scanc = SDLKeyToCEGUIKey(event.key.keysym.sym);
			std::string val = ScancodeToString(scanc);
			if (val.empty())
			{
				InjectKeyUp(scanc);
			}
			else
			{
				InjectKeyUp(val[0]);
			}
		}
#else
			// like before we inject the scancode directly.
			InjectKeyUp(event.key.keysym.scancode);

#endif
			break;


#if 0
		case SDL_KEYDOWN:
			{
				SDL_KeyboardEvent* kbdevent = (SDL_KeyboardEvent*)&event;
				SDL_keysym * keysym = &kbdevent->keysym;

				//InjectChar((unsigned long )keysym->unicode);
				InjectChar((unsigned long )SDL_GetKeyName(keysym->sym));
			}
			break;
			/*
			case SDL_KEYUP:
			{
			SDL_KeyboardEvent* kbdevent = (SDL_KeyboardEvent*)&event;
			SDL_keysym * keysym = &kbdevent->keysym;

			//InjectChar((unsigned long )keysym->unicode);
			InjectKeyUp((unsigned long )keysym->sym);
			}
			break;
			*/
#endif
		case SDL_QUIT:
			ZManager->AskToQuit();
			break;
		default:
			break;
		}
	} 

	//mInputDevice->Tick();

	// ALT-F4
	if (GetInputDevice()->KeyDown(TKEY_ALT)&&GetInputDevice()->KeyDown(TKEY_F4))
	{
		ZManager->AskToQuit();
	}
}
