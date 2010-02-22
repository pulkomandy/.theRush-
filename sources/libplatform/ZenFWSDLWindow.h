#ifndef ZENFWSDLWINDOW_H__
#define ZENFWSDLWINDOW_H__

#if (defined(WIN32) || defined(MAC_OS))
#include "SDL.h"
#include "SDL_syswm.h"
#else
#include <SDL/SDL.h>
#endif
void InjectChar(unsigned long pWParam);
void InjectKeyDown(unsigned long pWParam);
void InjectKeyUp(unsigned long pWParam);

class ZenFWSDLWindow : public ZenFWNode
{
public:
	ZenFWSDLWindow() : ZenFWNode("ZenFWSDLWindow")
	{
		
	}
	virtual ~ZenFWSDLWindow()
	{
		SDL_Quit();
	}

	void Init();
	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 11; }
	virtual void Run();
protected:
	SDL_Event event; 
	
	
	smartptr<IInput> mInputDevice;
};

#endif
