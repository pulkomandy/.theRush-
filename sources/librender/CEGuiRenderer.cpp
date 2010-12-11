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
#include <RendererModules/direct3d9/CEGUIdirect3d9renderer.h>
#endif
#include <RendererModules/OpenGL/CEGUIopenglrenderer.h>

#include "../libbase/LibBase.h"

#include "IDisplayDevice.h"

CEGUI::Renderer& GetDisplayDeviceCEGuiRendererInterface()
{
#ifdef WIN32
	if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
		return CEGUI::Direct3D9Renderer::create(GDD->GetD3D9Device());
	else
#endif
	if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL)
		return CEGUI::OpenGLRenderer::create();
}
