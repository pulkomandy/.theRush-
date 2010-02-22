///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZVertexArrayOGL.cpp
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

#include "stdafx.h"
#include "GLee.h"
#ifdef MAC_OS
#import <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>

#endif
//#include "../libplatform/libplatform.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

extern unsigned long GOGLThreadId;

class ZVertexArrayOGL : public IVertexArray
{
public:
    DECLARE_SERIALIZABLE
public:
    ZVertexArrayOGL()
    {
        mVertexSize = 0;
        mFormat = 0;
        mVertexCount = 0;
		mRAMBuffer = NULL;
		mVBOBufferName = 0;
		mLocked = false;
    }

    virtual ~ZVertexArrayOGL()
    {
		if (mRAMBuffer)
			free(mRAMBuffer);

		if (GOGLThreadId != GetCurrentThreadId() ) 
		{
			VRAMService->AddVertexArrayReleaseJob(&mVBOBufferName);
			return;
		}

		if( glIsBuffer(mVBOBufferName) )
		{
			glDeleteBuffers(1, &mVBOBufferName);
		}
    }

    void Init(uint32 aFormat, int aQty, bool aKeepVBORam, uint32 aFlag)
    {

		bool bRealloc = ((mFormat!=aFormat)||(mVertexCount!=aQty));
		mFlag = aFlag;
		mbKeepVBORam = true;//aKeepVBORam;
		if (GOGLThreadId != GetCurrentThreadId() ) 
		{

			mFormat = aFormat;
			mVertexCount = aQty;
			mVertexSize = GetVertexSizeFromFormat(aFormat);
			mVertexBufferSize = mVertexSize * mVertexCount;



			VRAMService->AddVertexArrayJob(NULL, this, true, NULL, mVertexCount, false, mFormat, false);
		}
		else
		{
			// buffer 
			mVertexCount = aQty;
			mVertexSize = GetVertexSizeFromFormat(aFormat);
			mVertexBufferSize = mVertexSize * mVertexCount;
			mFormat = aFormat;
			int mStride = 0;
			if( aFormat & VAF_XYZ && aFormat & VAF_XYZRHW )
			{
				aFormat ^= VAF_XYZRHW;
			}

			if( aFormat&VAF_XYZ )
			{
				mVertexOffset = mStride;
				mStride += 3*sizeof(float);
			}

			if( aFormat&VAF_XYZRHW )
			{
				mVertexOffset = mStride;
				mStride += 4*sizeof(float);
			}

			if( aFormat&VAF_NORMAL )
			{
				mNormalOffset = mStride;
				mStride += 3*sizeof(float);
			}
			if( aFormat&VAF_COLOR )
			{
				mColorOffset = mStride;
				mStride += 4*sizeof(unsigned char);
			}
			if( aFormat&VAF_BINORMAL )
			{
				mBiNormalOffset = mStride;
				mStride += 2*sizeof(float);
			}
			if( aFormat&VAF_BITANGENT )
			{
				mBiTangentOffset = mStride;
				mStride += 2*sizeof(float);
			}
			if( aFormat&VAF_TEX0 )
			{
				mUV0Offset	= mStride;
				mStride += 2*sizeof(float);
			}
			if( aFormat&VAF_TEX1 )
			{
				mUV1Offset	= mStride;
				mStride += 2*sizeof(float);
			}
			if( aFormat&VAF_TEX2 )
			{
				mUV2Offset	= mStride;
				mStride += 2*sizeof(float);
			}
			if( aFormat&VAF_TEX3 )
			{
				mUV3Offset	= mStride;
				mStride += 2*sizeof(float);
			}

			if (mVBOBufferName)
				glDeleteBuffers(1, &mVBOBufferName);
			mVBOBufferName = 0;
		}
		if (bRealloc)
		{
			if (mRAMBuffer)
				free(mRAMBuffer);
			if (mbKeepVBORam)
				mRAMBuffer = malloc(aQty*mVertexSize);
		}
    }

    uint32 GetFormat() const { return mFormat; }
    uint32 GetVertexSize() const { return mVertexSize; }
    uint32 GetVertexCount() const { return mVertexCount; }

    void * Lock(IMESH_LOCK aFlag) 
    { 
		if (mLocked) 
			return NULL;
		mLockFlag = aFlag;
		mLocked = true;
		
		return mRAMBuffer;
    }

    void Unlock() 
    {
		mLocked = false;

		if (mLockFlag == VAL_READONLY) 
			return;
		
		if (GOGLThreadId != GetCurrentThreadId() ) 
		{
			VRAMService->AddVertexArrayJob(NULL, this, false, mRAMBuffer, mVertexCount, false, mFormat, false);
		}
		else
		{
			if (!mVBOBufferName)
				glGenBuffers(1, &mVBOBufferName);

			glBindBuffer(GL_ARRAY_BUFFER, mVBOBufferName);

			switch (mFlag)
			{
			case VAU_STATIC:
				glBufferData(GL_ARRAY_BUFFER, mVertexBufferSize, mRAMBuffer, GL_STATIC_DRAW);        
				break;
			case VAU_STREAM:
				glBufferData(GL_ARRAY_BUFFER, mVertexBufferSize, mRAMBuffer, GL_STREAM_DRAW);        
				break;
			case VAU_DYNAMIC:
				glBufferData(GL_ARRAY_BUFFER, mVertexBufferSize, mRAMBuffer, GL_DYNAMIC_DRAW);        
				break;
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
    }

    bool Bind() 
    {
		if (!mVBOBufferName)
			return false;
		glBindBuffer(GL_ARRAY_BUFFER, mVBOBufferName);

		if( mFormat&VAF_XYZ )
		{
			glVertexPointer(3, GL_FLOAT, mVertexSize, BUFFER_OFFSET(mVertexOffset));
			glEnableClientState(GL_VERTEX_ARRAY);
		}

		if( mFormat&VAF_XYZRHW )
		{
		}

		if( mFormat&VAF_NORMAL )
		{
			glNormalPointer(GL_FLOAT, mVertexSize, BUFFER_OFFSET(mNormalOffset));
			glEnableClientState(GL_NORMAL_ARRAY);
		}
		if( mFormat&VAF_COLOR )
		{
			glColorPointer(4, GL_UNSIGNED_BYTE, mVertexSize, BUFFER_OFFSET(mColorOffset));
			glEnableClientState(GL_COLOR_ARRAY);
		}
		if( mFormat&VAF_BINORMAL )
		{
		}
		if( mFormat&VAF_BITANGENT )
		{
		}
		if( mFormat&VAF_TEX0 )
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glActiveTextureARB	(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, mVertexSize, BUFFER_OFFSET(mUV0Offset));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if( mFormat&VAF_TEX1 )
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glActiveTextureARB	(GL_TEXTURE1_ARB);
			glTexCoordPointer(2, GL_FLOAT, mVertexSize, BUFFER_OFFSET(mUV1Offset));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if( mFormat&VAF_TEX2 )
		{
		}
		if( mFormat&VAF_TEX3 )
		{
		}

		/*
        ZDisplayDeviceOGL::mD3DDevice->SetVertexDeclaration(mD3D9Declaration);
        ZDisplayDeviceOGL::mD3DDevice->SetStreamSource(0, mD3DBuffer, 0, mVertexSize);
		*/
		return true;
    }

	virtual void BindHWI(IVertexArray *pInstancesStream)
	{
	}

	virtual void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if( mFormat&VAF_XYZ )
		{
                glVertexPointer(3, GL_FLOAT, 0,0);
		glDisableClientState(GL_VERTEX_ARRAY);
		}

		if( mFormat&VAF_XYZRHW )
		{
		}
		if( mFormat&VAF_NORMAL )
		{
                glNormalPointer(GL_FLOAT, 0, 0);
		glDisableClientState(GL_NORMAL_ARRAY);
                
		}
		if( mFormat&VAF_COLOR )
		{
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
		glDisableClientState(GL_COLOR_ARRAY);
                
		}
		if( mFormat&VAF_BINORMAL )
		{
		}
		if( mFormat&VAF_BITANGENT )
		{
		}
		if( mFormat&VAF_TEX0 )
		{
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glActiveTextureARB	(GL_TEXTURE0_ARB);
                        glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                       
		}
		if( mFormat&VAF_TEX1 )
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glActiveTextureARB	(GL_TEXTURE1_ARB);
                        glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                        

		}
		/*
		if( mFormat&VAF_TEX2 )
		{
		}
		if( mFormat&VAF_TEX3 )
		{
		}*/



                //glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glClientActiveTextureARB(GL_TEXTURE0_ARB);
		//glActiveTextureARB	(GL_TEXTURE0_ARB);
	}

    virtual tulong GetMemoryUsed() const { return (sizeof(ZVertexArrayOGL) + mVertexSize*mVertexCount); }

protected:
    uint32 mVertexSize;
    uint32 mFormat;
    uint32 mVertexCount;
	uint32 mFlag;
	bool mbKeepVBORam;

	unsigned long	mVertexBufferSize;
	int				mVertexOffset;
	int				mNormalOffset;
	int				mColorOffset;
	int				mBiNormalOffset;
	int				mBiTangentOffset;
	int				mUV0Offset;
	int				mUV1Offset;
	int				mUV2Offset;
	int				mUV3Offset;
	void			*mRAMBuffer;
	GLuint			mVBOBufferName;
	bool			mLocked;
	IMESH_LOCK		mLockFlag;
};

BEGIN_SERIALIZATION(ZVertexArrayOGL)
END_SERIALIZATION()
