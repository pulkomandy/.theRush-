///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZIndexArray.cpp
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


#include <stdint.h>

#include "libbase/ZSerializator.h"
#include "IStreamArrays.h"


class ZIndexArray : public IIndexArray
{
public:
    DECLARE_SERIALIZABLE
public:
    ZIndexArray()
    {
        mIndexCount = 0;
		mRamBuf = NULL;
    }
    virtual ~ZIndexArray()
    {
		if (mRamBuf)
			free(mRamBuf);
    }

    void Init(int aQty, uint32_t aFlag)
    {
		if (mRamBuf)
			free(mRamBuf);

		mRamBuf = (uint16_t*)malloc(aQty * sizeof(uint16_t));
		mIndexCount = aQty;
    }

    uint32_t GetIndexCount() { return mIndexCount; }

    void * Lock(IMESH_LOCK aFlag)
    {
        return mRamBuf;
    }
    void Unlock()
    {
    }

    void Bind()
    {
    }

	virtual void Unbind() {}

	virtual tulong GetMemoryUsed() const { return (sizeof(ZIndexArray) + mIndexCount*sizeof(uint16_t)); }

protected:
    uint32_t mIndexCount;
	void *mRamBuf;
};

BEGIN_SERIALIZATION(ZIndexArray)
END_SERIALIZATION()
