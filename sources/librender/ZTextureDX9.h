#ifndef ZTEXTUREDX9_H__
#define ZTEXTUREDX9_H__

#include "ddsDX9.h"

extern IDirect3DDevice9 *GD3D9Device;

class ZTextureDX9: public ZTexture
{
public:

	ZTextureDX9()
	{
		mTexture = NULL;
	}
	virtual ~ZTextureDX9()
	{
		if (mTexture)
			mTexture->Release();
	}

	virtual void LoadDDSFromMemory(unsigned char *ptr, int memSize)
	{
		if (mTexture)
			mTexture->Release();

		HRESULT hr = CreateDDSTextureFromMemory( GD3D9Device, ptr, memSize, &mTexture );
	}

    virtual void Bind(uint8 aLevel)
    {
        GD3D9Device->SetTexture(aLevel, mTexture);
    }
	virtual void * GetPlatformHandle() const { return mTexture; }

	LPDIRECT3DTEXTURE9 mTexture;

	virtual void Reset()
	{
		if (mTexture)
			mTexture->Release();
		mTexture = NULL;
	}
};
#endif