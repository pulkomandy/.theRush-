///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZD3D9Enum.cpp
// Creation : 18/06/2007
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

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZD3D9Enum.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

ZD3D9VideoMode::ZD3D9VideoMode()
{
    width                        = 0;
    height                    = 0;
    bpp                        = 0;
    format                    = D3DFMT_UNKNOWN;
    formatZStencil            = D3DFMT_UNKNOWN;
    refreshRate                = 0;
    idMode                    = 0;
    antiAlias                = false;
    antiAliasWindowed        = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

ZD3D9Adapters::ZD3D9Adapters()
{
    textureCompression = false;
    textureBpp             = 0;
    tnl                     = false;
    hwVertexShader         = false;
    hwPixelShader          = false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZD3D9Enum::ZD3D9Enum()
{
    d3d    = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZD3D9Enum::Init( LPDIRECT3D9 p_d3d )
{

    d3d = p_d3d;

    EnumAdapter();
    EnumVideoModes();
    EnumCaps();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZD3D9Enum::~ZD3D9Enum()
{
	ClearAdapters();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZD3D9Enum::ClearAdapters()
{
    tarray<ZD3D9Adapters*>::iterator iterAdapt;
    for( iterAdapt = adapters.begin() ; iterAdapt != adapters.end() ; ++iterAdapt)
    {
        tarray<ZD3D9VideoMode*>::iterator iterMode;
        for( iterMode = (*iterAdapt)->videoModes.begin() ; iterMode != (*iterAdapt)->videoModes.end() ; ++iterMode)
        {
            if ((*iterMode))
				delete (*iterMode);
        }                
        if ((*iterAdapt))
			delete (*iterAdapt);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZD3D9Enum::EnumAdapter()
{
    tulong i;

    nbAdapter        = d3d->GetAdapterCount();
    for( i=0 ; i<nbAdapter ; i++)
    {
        ZD3D9Adapters * newAdapter = new ZD3D9Adapters;
        d3d->GetAdapterIdentifier(i, 0, &newAdapter->adapters );
        d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &newAdapter->adaptersCaps );
        adapters.push_back(newAdapter);
    }


    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int ZD3D9Enum::sortResolutions(const void *a, const void *b)
{
	resolution_t * p1 = (resolution_t*)a;
	resolution_t * p2 = (resolution_t*)b;

	if (p1->width<p2->width) return -1;
	if (p1->width>p2->width) return 1;
	if (p1->height<p2->height) return -1;
	if (p1->height>p2->height) return 1;
	return 0;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZD3D9Enum::EnumVideoModes()
{
    tulong i,j;
    D3DDISPLAYMODE curMode;
    tchar  descBuffer[50];

    // Get the desktop tvoid Initial mode
    d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &curMode );

    initialMode.width           = (tulong)curMode.Width;
    initialMode.height        = (tulong)curMode.Height;
    initialMode.refreshRate    = curMode.RefreshRate;
    initialMode.format        = curMode.Format;

    // Anti Aliasing Available ?
    if(d3d->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, initialMode.format, false, D3DMULTISAMPLE_2_SAMPLES, NULL )==0)
    {
        initialMode.antiAlias = true;
    }
    else
    {
        initialMode.antiAlias = false;
    }
    
    if(d3d->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, initialMode.format, true, D3DMULTISAMPLE_2_SAMPLES, NULL )==0)
    {
        initialMode.antiAliasWindowed = true;
    }
    else
    {
        initialMode.antiAliasWindowed = false;
    }

    // Build Description for tvoid Initial mode
    if((curMode.Format == D3DFMT_X8R8G8B8) || ( curMode.Format == D3DFMT_A8R8G8B8 ))
    {
        sprintf_s(descBuffer, 50, "%dx%dx32 - %dHz", curMode.Width, curMode.Height, curMode.RefreshRate);
        initialMode.bpp = 32;
    }
    else
    {
         sprintf_s(descBuffer, 50, "%dx%dx16 - %dHz", curMode.Width, curMode.Height, curMode.RefreshRate);
         initialMode.bpp = 16;
    }

    GetValidZStencil(&initialMode);

    initialMode.description = descBuffer;

    // Get video mode List
    tarray<ZD3D9Adapters*>::iterator iter;

    i = 0;

    D3DFORMAT modeToEnum[] = 
    {
        D3DFMT_A8R8G8B8, 
        D3DFMT_X8R8G8B8,
        D3DFMT_R5G6B5,
        D3DFMT_A2B10G10R10, 
        D3DFMT_X1R5G5B5, 
        D3DFMT_A1R5G5B5,
        D3DFMT_R5G6B5
    };

    tulong nbModeToEnum = 2;

    for( iter = adapters.begin() ; iter != adapters.end() ; ++iter)
    {
        for(tulong x=0;x<nbModeToEnum; x++)
        {
            tulong nbVideoModes = d3d->GetAdapterModeCount(i, modeToEnum[x]);
            ZD3D9VideoMode * newMode;

            for( j=0 ; j<nbVideoModes ; j++)
            {
                bool keepMode = true;

                newMode = new ZD3D9VideoMode;
                d3d->EnumAdapterModes(i, modeToEnum[x], j, &curMode);

                newMode->width            = (tulong)curMode.Width;
                newMode->height            = (tulong)curMode.Height;
                newMode->refreshRate    = curMode.RefreshRate;
                newMode->format            = curMode.Format;
                newMode->idMode            = j;

                // Build Description for current mode
                if((curMode.Format == D3DFMT_X8R8G8B8) || ( curMode.Format == D3DFMT_A8R8G8B8 ))
                {
					mResolutions.push_back(resolution_t(curMode.Width, curMode.Height));
                    //sprintf(descBuffer,"%dx%dx32 - %dHz", curMode.Width, curMode.Height, curMode.RefreshRate);
                    newMode->bpp = 32;
                }
                else{
                    if((curMode.Format == D3DFMT_R5G6B5) || ( curMode.Format == D3DFMT_X1R5G5B5 ) || ( curMode.Format == D3DFMT_A1R5G5B5 ) ||
                        (curMode.Format == D3DFMT_A4R4G4B4) || ( curMode.Format == D3DFMT_A8R3G3B2 ) || ( curMode.Format == D3DFMT_X4R4G4B4 ) ||
                        (curMode.Format == D3DFMT_A8P8) || ( curMode.Format == D3DFMT_A8L8 )
                      )
                    {
						
                        //sprintf(descBuffer,"%dx%dx16 - %dHz", curMode.Width, curMode.Height, curMode.RefreshRate);
                        newMode->bpp = 16;
                    }
                    else
                    {
                        keepMode = false;
                    }
                }
                newMode->description = descBuffer;


                if(!GetValidZStencil(newMode))
                {
                    keepMode = false;
                }

                tulong quality;
                // Anti Aliasing Available ?
                if(d3d->CheckDeviceMultiSampleType(i, D3DDEVTYPE_HAL, newMode->format, false, D3DMULTISAMPLE_2_SAMPLES, &quality )==0)
                {
                    if(d3d->CheckDeviceMultiSampleType(i, D3DDEVTYPE_HAL, newMode->formatZStencil, false, D3DMULTISAMPLE_2_SAMPLES, &quality )==0)
                    {
                        newMode->antiAlias = true;
                    }
                }
                else
                {
                    newMode->antiAlias = false;
                }
                
                if(d3d->CheckDeviceMultiSampleType(i, D3DDEVTYPE_HAL, newMode->format, true, D3DMULTISAMPLE_2_SAMPLES, &quality  )==0)
                {
                    if(d3d->CheckDeviceMultiSampleType(i, D3DDEVTYPE_HAL, newMode->formatZStencil, true, D3DMULTISAMPLE_2_SAMPLES, &quality )==0)
                    {
                        newMode->antiAlias = true;
                    }
                }
                else
                {
                    newMode->antiAliasWindowed = false;
                }

                if(keepMode)
                {
                    (*iter)->videoModes.push_back(newMode);
                }
                else
                {
                    if (newMode)
						delete newMode;
					newMode = NULL;
                }
            }
        }
        i++;
    }
	qsort(&mResolutions[0], mResolutions.size(), sizeof(resolution_t), sortResolutions);

    return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZD3D9Enum::GetValidZStencil(ZD3D9VideoMode * videoMode)
{
    D3DFORMAT const k32BitZStencilFormats[] = 
    {
        D3DFMT_D32,
        D3DFMT_D24S8,
        D3DFMT_D24X8,
        D3DFMT_D24X4S4,
        D3DFMT_UNKNOWN              
    };

    D3DFORMAT const k16BitZStencilFormats[] = 
    {
        D3DFMT_D16,      
        D3DFMT_D15S1,
        D3DFMT_D16_LOCKABLE,
        D3DFMT_UNKNOWN              
    };

//    D3DFORMAT const *pZStencil = (video == 16) ? k16BitZStencilFormats : k32BitZStencilFormats;


    tulong i;
    if(videoMode->bpp == 16)
    {
        for(i = 0; (k16BitZStencilFormats[i] != D3DFMT_UNKNOWN); ++i)
        {
            if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, k16BitZStencilFormats[i]) == 0)
            {
                if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, k16BitZStencilFormats[i])==0)
                {
                    videoMode->formatZStencil = k16BitZStencilFormats[i];
                    return true;
                }
            }
        }

        // No 16 bits mode found search for 32 bits
        for(i = 0; (k32BitZStencilFormats[i] != D3DFMT_UNKNOWN); ++i)
        {
            if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, k32BitZStencilFormats[i]) == 0)
            {
                if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, k32BitZStencilFormats[i])==0)
                {
                    videoMode->formatZStencil = k32BitZStencilFormats[i];
                    return true;
                }
            }
        }
        }
    else
    {
        for(i = 0; (k32BitZStencilFormats[i] != D3DFMT_UNKNOWN); ++i)
        {
            if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, k32BitZStencilFormats[i]) == 0)
            {
                if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, k32BitZStencilFormats[i])==0)
                {
                    videoMode->formatZStencil = k32BitZStencilFormats[i];
                    return true;
                }
            }
        }
        // No 32 bits mode found search for 16 bits
        for(i = 0; (k16BitZStencilFormats[i] != D3DFMT_UNKNOWN); ++i)
        {
            if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, k16BitZStencilFormats[i]) == 0)
            {
                if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, k16BitZStencilFormats[i])==0)
                {
                    videoMode->formatZStencil = k16BitZStencilFormats[i];
                    return true;
                }
            }
        }
    }
    return false;
/*
    D3DFORMAT const *pColor    = (colorPrecision    == 16) ? k16BitColorFormats    : k32BitColorFormats;
    dxerr            error1, error2;
    int              i, j;
    

    for (i = 0; (pZStencil[i] != D3DFMT_UNKNOWN); ++i)
    {
        for (j = 0; (pColor[j] != D3DFMT_UNKNOWN); ++j)
        {
            error1 = mpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
                                             pColor[j], D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, pZStencil[i]);            
            error2 = mpD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
                                                   pColor[j], pColor[j], pZStencil[i]);
            // check if this combination works, if so we are done
            if ((error1 == 0) && (error2 == 0))
                return true;
        }
    }

    if(videoMode->bpp == 16)
    {

      if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D15S1)==0)
            {
                videoMode->formatZStencil = D3DFMT_D15S1;
                return true;
            }
        }
      
      if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D16)==0)
            {
                videoMode->formatZStencil = D3DFMT_D16;
                return true;
            }
        }
        
        if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16_LOCKABLE) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D16_LOCKABLE)==0)
            {
                videoMode->formatZStencil = D3DFMT_D16_LOCKABLE;
                return true;
            }
        }
        
        return false;
    }
    else
    {
      if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D32)==0)
            {
                videoMode->formatZStencil = D3DFMT_D32;
                return true;
            }
        }
      
      if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D24S8)==0)
            {
                videoMode->formatZStencil = D3DFMT_D24S8;
                return true;
            }
        }
        
        if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D32)==0)
            {
                videoMode->formatZStencil = D3DFMT_D32;
                return true;
            }
        }

        if(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4) == 0)
        {
            if(d3d->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, videoMode->format, videoMode->format, D3DFMT_D24X4S4)==0)
            {
                videoMode->formatZStencil = D3DFMT_D24X4S4;
                return true;
            }
        }
        
        return false;

    }
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZD3D9Enum::EnumCaps()
{
    HRESULT                        hr;
    ZD3D9Adapters *        curAdapter;

    tarray<ZD3D9Adapters*>::iterator iter;

    tulong i =0;
    for( iter = adapters.begin() ; iter != adapters.end() ; ++iter)
    {
        curAdapter = (*iter);

        // Texture Compression avaible ?
        curAdapter->textureCompression = false;

        hr = d3d->CheckDeviceFormat( i, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT3);
        if(hr == D3D_OK)
        {
            hr = d3d->CheckDeviceFormat( i, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT5);
            if(hr == D3D_OK)
            {
                curAdapter->textureCompression = true;
            }
        }

        // 32 Bits texture avaible ?
        curAdapter->textureBpp = 16;

        hr = d3d->CheckDeviceFormat( i, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, 0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8);
        if(hr == D3D_OK)
        {
            curAdapter->textureBpp = 32;
        }
        // TnL avaible ?
        D3DCAPS9 caps;
        hr =  d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &caps );

        if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            curAdapter->tnl = true;
        }
        else
        {
            curAdapter->tnl = false;
        }
    
        // HW Vertex and Pixel Shader .

        if( caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
        {
            curAdapter->hwVertexShader = false;
        }
        else
        {
            curAdapter->hwVertexShader = true;
        }

        if( caps.PixelShaderVersion < D3DPS_VERSION(1,1) )
        {
            curAdapter->hwPixelShader = false;
        }
        else
        {
            curAdapter->hwPixelShader = true;
        }

        i++;
    }

    return true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZD3D9Enum::FindMode(ZD3D9VideoMode ** mode, tushort adapter, tulong width, tulong height, tushort bpp)
{
    float curRefreshRate = 0;
    bool  found = false;


    tarray<ZD3D9Adapters*>::iterator iter;

    for( iter = adapters.begin() ; iter != adapters.end() ; ++iter)
    {
        ZD3D9Adapters * curAdapters = (*iter);

        tarray<ZD3D9VideoMode*>::iterator iter2;
        for( iter2 = curAdapters->videoModes.begin() ; iter2 != curAdapters->videoModes.end() ; ++iter2)
        {
            ZD3D9VideoMode * curVidMode = (*iter2);
            if( (curVidMode->width == width) && (curVidMode->height == height) && (curVidMode->bpp == bpp))
            {
                if((curVidMode->refreshRate >= curRefreshRate) || (curVidMode->refreshRate == 0))
                {
                    *mode = curVidMode;
                    curRefreshRate = (float) curVidMode->refreshRate;
                    found = true;
                }
            }
        }
    }

    if(found)
    {
        return true;
    }

    return false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
