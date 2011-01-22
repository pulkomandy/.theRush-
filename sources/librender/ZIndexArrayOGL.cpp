///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZIndexArrayOGL.cpp
// Creation : 27/07/2008
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

#include "GLee.h"
#ifdef MAC_OS
#import <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>

#endif
//#include "../libplatform/libplatform.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"

extern unsigned long GOGLThreadId;

class ZIndexArrayOGL : public IIndexArray
{
public:
    DECLARE_SERIALIZABLE
public:
    ZIndexArrayOGL()
    {
        mIndexCount = 0;
		mIndexBufferSize = 0;
		mIndexBufferName = 0;
		
		mIndexBuffer = NULL;
		mLocked = false;
    }
    virtual ~ZIndexArrayOGL()
    {
		if (GOGLThreadId != GetCurrentThreadId() ) 
		{
			VRAMService->AddIndexArrayReleaseJob(&mIndexBufferName);
			return;
		}

		if( glIsBuffer(mIndexBufferName) )
		{
			glDeleteBuffers(1, &mIndexBufferName);
		}
        if (mIndexBuffer)
			free(mIndexBuffer);
    }

    void Init(int aQty, uint32_t aFlag)
    {
		mFlag = aFlag;
		mIndexCount = aQty;
		if (mIndexBuffer)
			free(mIndexBuffer);

		mIndexBuffer = (unsigned short*)malloc(aQty * 2);
		mIndexBufferSize = aQty * 2;

    }

    uint32_t GetIndexCount() { return mIndexCount; }

    void * Lock(IMESH_LOCK aFlag)
    {
		if (mLocked)
			return NULL;
		mLockFlag = aFlag;
		mLocked = true;

		return mIndexBuffer;
    }

    void Unlock()
    {
		mLocked = false;
		if (mLockFlag == VAL_READONLY) 
			return;
		if (GOGLThreadId != GetCurrentThreadId() )
		{
			VRAMService->AddIndexArrayJob(NULL, this, false, mIndexBuffer, mIndexCount*sizeof(uint16_t), false);
		}
		else
		{
			if (!mIndexBufferName)
				glGenBuffers(1, &mIndexBufferName);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferName);
			switch (mFlag)
			{
			case VAU_STATIC:
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSize, mIndexBuffer, GL_STATIC_DRAW);
				break;
			case VAU_STREAM:
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSize, mIndexBuffer, GL_STREAM_DRAW);
				break;
			case VAU_DYNAMIC:
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSize, mIndexBuffer, GL_DYNAMIC_DRAW);
				break;


			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
    }

    void Bind()
    {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferName);
    }

	virtual void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	virtual tulong GetMemoryUsed() const { return (sizeof(ZIndexArrayOGL) + mIndexBufferSize); }

protected:
    uint32_t mIndexCount;
	uint32_t mFlag;
	uint32_t mIndexBufferSize;
	GLuint	mIndexBufferName;
	unsigned short *mIndexBuffer;
	IMESH_LOCK mLockFlag;
	bool mLocked;
    //IDirect3DIndexBuffer9 * mD3DBuffer;
};

BEGIN_SERIALIZATION(ZIndexArrayOGL)
END_SERIALIZATION()
