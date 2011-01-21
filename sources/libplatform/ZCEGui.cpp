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
#include "libbase/ZFile.h"
#include "libbase/ZProfiler.h"
#include "libbase/ZBaseDefs.h"
#include "libbase/ZPlatformUtils.h"
#include "libbase/ZSerializator.h"
#include "librender/IDisplayDevice.h"

#include "CEGUI.h"
#include "XMLParserModules/TinyXMLParser/CEGUITinyXMLParserModule.h"
#include "CEGUIDefaultResourceProvider.h"
#include "CEGUIWindowManager.h"
#include "CEGUISchemeManager.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIFactoryModule.h"

#ifdef WIN32
#include "RendererModules/direct3d9/ceguidirect3d9renderer.h"
#endif
#include "RendererModules/OpenGLGUIRenderer/openglrenderer.h"


CEGUI::Renderer* myRenderer = NULL;
//CEGUI::LuaScriptModule* script_module = NULL;
CEGUI::DefaultResourceProvider *myResProvider = NULL;
CEGUI::XMLParser *myParser = NULL;
CEGUI::System *mySystem = NULL;
extern "C" {
void  pcre_free(void *)
{
}
}


///////////////////////////////////////////////////////////////////////////////////////////////////

class NoLogger : public CEGUI::Logger
{
   void logEvent (const CEGUI::String&, CEGUI::LoggingLevel)
   {

   }
   void setLogFilename(const CEGUI::String&, bool)
   {      

   }
};

NoLogger nL;

///////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void registerFactory(const CEGUI::String& type_name);
extern "C" CEGUI::uint registerAllFactories(void);

class ZenithCEGuiResProvider : public CEGUI::ResourceProvider
{
public:
	ZenithCEGuiResProvider() {}

	virtual 	~ZenithCEGuiResProvider (void)
	{
	}

	virtual void 	loadRawDataContainer (const CEGUI::String &filename,
		CEGUI::RawDataContainer &output, const CEGUI::String &resourceGroup)
	{

		ZFile file;
		if (file.Open(filename.c_str()))
		{
			int fn = file.GetSize();
			char *ptr = new char [fn+1];
			file.Read(ptr, fn);
			ptr[fn] = 0;
			output.setData((CEGUI::uint8*)ptr);
			output.setSize(fn);
		}

	}

	virtual void 	unloadRawDataContainer (CEGUI::RawDataContainer &data)
	{
		delete [] data.getDataPtr();
		data.setData(NULL);
	}

    virtual size_t getResourceGroupFileNames(std::vector<CEGUI::String>& out_vec,
                                             const CEGUI::String& file_pattern,
                                             const CEGUI::String& resource_group) {};

//const String & 	getDefaultResourceGroup (void) const

//void 	setDefaultResourceGroup (const String &resourceGroup)
};


ZenithCEGuiResProvider gZenithResManager;
CEGUI::Renderer * GetDisplayDeviceCEGuiRendererInterface();
bool GBCEGUIInited = false;

void InitCEGui()
{

	myRenderer = GetDisplayDeviceCEGuiRendererInterface();


	myParser =		new CEGUI::TinyXMLParser();
	mySystem =		CEGUI::System::getSingletonPtr();//&CEGUI::System::create(*myRenderer, &gZenithResManager, myParser);

	GBCEGUIInited = true;

	
}

void TickCEGui(float Ellapsed)
{
	if (!GBCEGUIInited)
		return;

	PROFILER_START(CEGuiTick);
	extern int GGUIStackCount;
	if (GGUIStackCount)
	{
		CEGUI::System::getSingleton().injectTimePulse(Ellapsed);
		CEGUI::System::getSingleton().renderGUI();
	}



	//
	PROFILER_END();
}

void UninitCEGui()
{
	GBCEGUIInited = false;

//	delete mySystem;
//	delete script_module;
	delete myRenderer;

	delete myResProvider;
	delete myParser;
//	LOG("CEGui Uninit OK\r\n");

}

void CEGuiPreResetDevice()
{
	if (!GBCEGUIInited)
		return;
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
		((CEGUI::Direct3D9Renderer*)myRenderer)->preD3DReset();
#endif
	
}

void CEGuiPostResetDevice()
{
	if (!GBCEGUIInited)
		return;
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
		((CEGUI::Direct3D9Renderer*)myRenderer)->postD3DReset();
#endif

}

void InjectChar(unsigned long pWParam)
{
	if (!GBCEGUIInited)
		return;

	CEGUI::System::getSingleton().injectChar((CEGUI::utf32)pWParam);
}
void InjectKeyDown(unsigned long pWParam)
{
	if (!GBCEGUIInited)
		return;

	CEGUI::System::getSingleton().injectKeyDown((CEGUI::utf32)pWParam);
}
void InjectKeyUp(unsigned long pWParam)
{
	if (!GBCEGUIInited)
		return;

	CEGUI::System::getSingleton().injectKeyUp((CEGUI::utf32)pWParam);
}

void InjectMouseMove(int x, int y)
{
	if (!GBCEGUIInited)
		return;

	CEGUI::System::getSingleton().injectMousePosition((float)x, (float)y);
}

void InjectMouseButtons(int BtNb, bool bDown)
{
	if (!GBCEGUIInited)
		return;

	switch (BtNb)
	{
	case 0: //WM_LBUTTONDOWN:
		if (bDown)
 			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
		else
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
 		break;

	case 1: //WM_RBUTTONDOWN:
		if (bDown)
 			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
		else
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
 		break;
	case 2: //WM_MBUTTONDOWN:
		if (bDown)
 			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
		else
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
 		break;
	}
/*
 	case WM_LBUTTONUP:
 		CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
 		break;

 	case WM_RBUTTONDOWN:
 		CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
 		break;

 	case WM_RBUTTONUP:
 		CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
 		break;

 	case WM_MBUTTONDOWN:
 		CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
 		break;

 	case WM_MBUTTONUP:
 		CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
 		break;
		*/
	}
