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

#include "GLee.h"
#include "CEGUIBase.h"


#ifdef WIN32
#include "RendererModules/directx9GUIRenderer/d3d9renderer.h"
#endif
#include "RendererModules/OpenGLGUIRenderer/openglrenderer.h"

#include "../libbase/LibBase.h"

#include "IDisplayDevice.h"

CEGUI::Renderer * GetDisplayDeviceCEGuiRendererInterface()
{
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
		return new CEGUI::DirectX9Renderer(GDD->GetD3D9Device(), 512);
	else
#endif
	if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL)
		return new CEGUI::OpenGLRenderer(512);
	return NULL;
}
