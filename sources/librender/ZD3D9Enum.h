 ///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZD3D9Enum.h
// Creation : 18/06/2007
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

#ifndef ZD3D9ENUM_H__
#define ZD3D9ENUM_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include <d3dx9.h>

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZD3D9VideoMode 
{

public:
    tstring        description;
    tulong        width;
    tulong        height;
    tushort        bpp;
    D3DFORMAT    format;
    D3DFORMAT    formatZStencil;
    tulong        refreshRate;
    tulong        idMode;
    bool          antiAlias;
    bool          antiAliasWindowed;

public:
    ZD3D9VideoMode();
    virtual ~ZD3D9VideoMode() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZD3D9Adapters 
{

public:
    // Adapter list
    tstring                                description;
    D3DADAPTER_IDENTIFIER9            adapters;
    D3DCAPS9                             adaptersCaps;

    // Video Modes
    tarray<ZD3D9VideoMode*>        videoModes;

    // Other Caps
    bool        textureCompression;
    tushort    textureBpp;
    bool        tnl;
    bool        hwVertexShader;
    bool        hwPixelShader;

public:
    ZD3D9Adapters();
    virtual ~ZD3D9Adapters() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZD3D9Enum 
{

    protected:
        LPDIRECT3D9 d3d;

    public:
        // Adapters
        tulong                            nbAdapter;
        tarray<ZD3D9Adapters*>  adapters;

        // Video Modes
        ZD3D9VideoMode                initialMode;
                                            
    public:
        ZD3D9Enum();
        virtual ~ZD3D9Enum();

    public:
        bool Init( LPDIRECT3D9 d3d );

    public:
        bool EnumAdapter();
        bool EnumVideoModes();
        bool EnumCaps();
        bool GetValidZStencil(ZD3D9VideoMode * videoMode);
		void ClearAdapters();

    public:
        bool FindMode(ZD3D9VideoMode ** mode, tushort adapterID, tulong width, tulong height, tushort mode32Bits);

		typedef struct resolution_t
		{
			resolution_t(int w, int h)
			{
				width = w;
				height = h;
			}
			int width, height;
		} resolution_t;
		std::vector<resolution_t> mResolutions;
		static int ZD3D9Enum::sortResolutions(const void *a, const void *b);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
