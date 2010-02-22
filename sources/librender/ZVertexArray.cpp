///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZVertexArray.cpp
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

#include "stdafx.h"


unsigned int GetVertexSizeFromFormat(uint32 aFormat)
{
	uint mVertexSize = 0;
	if (aFormat&VAF_XYZ) mVertexSize+=12;
	if (aFormat&VAF_XYZRHW) mVertexSize+=16;
	if (aFormat&VAF_NORMAL) mVertexSize+=12;
	if (aFormat&VAF_COLOR) mVertexSize+=4;
	if (aFormat&VAF_BINORMAL) mVertexSize+=12;
	if (aFormat&VAF_BITANGENT) mVertexSize+=12;
	if (aFormat&VAF_TEX0) mVertexSize+=8;
	if (aFormat&VAF_TEX1) mVertexSize+=8;
	if (aFormat&VAF_TEX2) mVertexSize+=8;
	if (aFormat&VAF_TEX3) mVertexSize+=8;
	if (aFormat&VAF_HWI_MATRIX) mVertexSize+=48;
	return mVertexSize;
}

class ZVertexArray : public IVertexArray
{
public:
    DECLARE_SERIALIZABLE
public:
    ZVertexArray()
    {
        mVertexSize = 0;
        mVertexFormat = 0;
        mVertexCount = 0;
		mRamedVBO = NULL;
    }
    virtual ~ZVertexArray()
    {
		if (mRamedVBO)
			free(mRamedVBO);
    }

    void Init(uint32 aFormat, int aQty, bool, uint32 aFlag)
    {
		// buffer
        mVertexCount = aQty;


        mVertexFormat = aFormat;
		mVertexSize = GetVertexSizeFromFormat(aFormat);

		if (mRamedVBO)
			free(mRamedVBO);
		//if (mbKeepVBORam)
		mRamedVBO = malloc(aQty*mVertexSize);
    }

    uint32 GetFormat() const { return mVertexFormat; }
    uint32 GetVertexSize() const { return mVertexSize; }
    uint32 GetVertexCount() const { return mVertexCount; }

    void * Lock(IMESH_LOCK aFlag)
    {
        return mRamedVBO;
    }
    void Unlock()
    {
    }


    bool Bind()
    {
		return true;
    }

	void Unbind()
	{
	}

	virtual void BindHWI(IVertexArray *pInstancesStream)
	{
	}

	virtual tulong GetMemoryUsed() const { return (sizeof(ZVertexArray) + mVertexSize*mVertexCount); }

protected:
    uint32 mVertexSize;
    uint32 mVertexFormat;
    uint32 mVertexCount;
	void *mRamedVBO;
};

BEGIN_SERIALIZATION(ZVertexArray)
END_SERIALIZATION()
