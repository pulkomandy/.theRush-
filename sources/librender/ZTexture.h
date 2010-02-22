///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTexture.h
// Creation : 23/06/2007
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

#ifndef ZTEXTURE_H__
#define ZTEXTURE_H__

enum TEXTURE_TYPE
{
    TEXTURE_UNDEFINED,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBE,
    TEXTURE_XVID,
	TEXTURE_DDS
};

DECLAREZCLASS(ZTexture);
DECLAREZCLASS(ZTextureOGL);
DECLAREZCLASS(ZTextureDX9);

class ZTexture : public virtual ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
    ZTexture() 
    {
        mWidth = mHeight = mBPP = 0;
        mType = TEXTURE_UNDEFINED;
        mFrameUpdating = false;
		mEnabledDecoding = true;
		mLastFrameTime = 0;

    }
    virtual ~ZTexture() 
    {
#ifdef _DEBUG
		LOG("Texture 0x%x (%s) destroyed.\n", this, this->GetName().c_str());
#endif
		for (int i=0;i< mFrameTexturesToUpdateIndex; i++)
		{
			if (ZTexture::mFrameTexturesToUpdate[i] == this)
			{
				ZTexture::mFrameTexturesToUpdate[i] = NULL;
				return;
			}
		}
    }
    virtual bool LoadFile(const char *szFileName) { mName = szFileName; return true;}
	virtual void LoadDDSFromMemory(unsigned char *, int ) {}

	virtual void LoadAVIFromMemory(unsigned char *) {}

    // update
    virtual bool NeedUpdate() { return false; }
    virtual void Update() {}
    virtual void SetFrameUpdate(bool bUpdate) 
    { 
        if ((!mFrameUpdating) && (bUpdate))
        {
            mFrameTexturesToUpdate[mFrameTexturesToUpdateIndex++] = this;
            mFrameUpdating = true; 
        }
        else
            mFrameUpdating = false; 
    }

    virtual void *Lock(uint16 aSurfaceNb) { return NULL; }
    virtual void Unlock(uint16 aSurfaceNb) {}
    virtual void Bind(uint8 aLevel) {}


    virtual uint32 GetWidth() { return mWidth; }
    virtual uint32 GetHeight() { return mHeight; }
    virtual uint32 GetBPP() { return mBPP; }

    virtual uint8 GetType() { return mType; }
	virtual void SetType(uint8 atype) { mType = atype; }

    virtual void * GetPlatformHandle() const { return NULL; }
    virtual tulong GetMemoryUsed() const { return sizeof(ZTexture); }
	virtual void EnableDecoding(bool bEnable) { mEnabledDecoding = bEnable; }
	virtual bool CanDecode() 
	{ 
		return mEnabledDecoding; 
	}
	virtual void ForceTextureHandle(void*pHandle) {}

	virtual void SetLastFrameTime(float aTime) { mLastFrameTime = aTime; }
	virtual float GetLastFrameTime() { return mLastFrameTime; }
	virtual void ReinitStreaming() {}

	virtual void SetLoop(bool bLoop) { }
	virtual void Reset() {}


protected:
    // user must free returned buffer
    int mWidth, mHeight, mBPP;
	int mNbMipmaps;
    uint8 mType;
	bool mEnabledDecoding;
    // update
    bool mFrameUpdating;
    static ZTexture *mFrameTexturesToUpdate[2048];
    static int mFrameTexturesToUpdateIndex;
    friend void UpdateAllNeededTextures();
	float mLastFrameTime;
	friend void ClearUpdateTexturesList();
};

ZTexture* DirectLoadTex(const char *szName);

#endif

