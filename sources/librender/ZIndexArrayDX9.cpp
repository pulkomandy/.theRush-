///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZIndexArrayDX9.cpp
// Creation : 19/06/2007
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

#include "stdafx.h"
#include <D3D9.h>
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"

extern DWORD GDX9ThreadId;
extern IDirect3DDevice9 *GD3D9Device;

class ZIndexArrayDX9 : public IIndexArray
{
public:
    DECLARE_SERIALIZABLE
public:
    ZIndexArrayDX9()
    {
        mIndexCount = 0;
        mD3DBuffer = NULL;
		mLocked = false;
		mRamBuffer = NULL;
    }
    virtual ~ZIndexArrayDX9()
    {
		if (mRamBuffer)
			free(mRamBuffer);

		if (GDX9ThreadId != GetCurrentThreadId() ) 
		{
			VRAMService->AddIndexArrayReleaseJob(mD3DBuffer);
			return;
		}
        if (mD3DBuffer)
			mD3DBuffer->Release();

    }

    void Init(int aQty, uint32 aFlag)
    {
		bool bRealloc = (mIndexCount!=aQty);

        mIndexCount = aQty;
        DWORD usageFlags = D3DUSAGE_WRITEONLY;
        if(aFlag&VACF_DYNAMIC)
            usageFlags |= D3DUSAGE_DYNAMIC;

		if (bRealloc)
		{
			if (mRamBuffer)
				free(mRamBuffer);
			mRamBuffer = (unsigned short *)malloc(aQty * sizeof(unsigned short));
		}
		if (GDX9ThreadId != GetCurrentThreadId() ) 
		{
			VRAMService->AddIndexArrayJob(NULL, this, true, NULL, mIndexCount*sizeof(uint16), false);
		}
		else
		{
			GD3D9Device->CreateIndexBuffer(aQty*sizeof(uint16), usageFlags, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mD3DBuffer, NULL );
		}
    }

    uint32 GetIndexCount() { return mIndexCount; }

	virtual void Reset() 
	{
		if (mD3DBuffer)
			mD3DBuffer->Release();
		mD3DBuffer = NULL;
	}

    void * Lock(IMESH_LOCK aFlag) 
    { 
		if (mLocked) 
			return NULL;
		mLockFlag = aFlag;
		mLocked = true;

		//if (mLockFlag == VAL_READONLY)
		return mRamBuffer;

    }
    void Unlock() 
    {
		mLocked = false;
		if (mLockFlag == VAL_READONLY)
			return;
		else
		{
			if (GDX9ThreadId != GetCurrentThreadId() ) 
			{
				VRAMService->AddIndexArrayJob(NULL, this, false, mRamBuffer, mIndexCount*sizeof(uint16), false);
			}
			else
			{
				void *alock;
				mD3DBuffer->Lock(0, sizeof(uint16)*mIndexCount, &alock, 0); 
				memcpy( alock, mRamBuffer, mIndexCount*sizeof(unsigned short));
				mD3DBuffer->Unlock();
			}
		}
    }

    void Bind()
    {
        GD3D9Device->SetIndices(mD3DBuffer);
    }

	void Unbind()
	{
		GD3D9Device->SetIndices(NULL);
	}

	virtual tulong GetMemoryUsed() const { return (sizeof(ZIndexArrayDX9) + mIndexCount*sizeof(uint16)); }

protected:
    uint32 mIndexCount;

    IDirect3DIndexBuffer9 * mD3DBuffer;

	IMESH_LOCK mLockFlag;
	bool mLocked;
	void *mRamBuffer;
};

BEGIN_SERIALIZATION(ZIndexArrayDX9)
END_SERIALIZATION()
