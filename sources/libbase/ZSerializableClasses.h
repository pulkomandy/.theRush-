///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSerializableClasses.h
// Creation : 02/11/2006
// Author : Cedric Guillemet
// Description :
// Reliability : 5/10
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

#ifndef ZSERIALIZABLE_CLASS_H__
#define ZSERIALIZABLE_CLASS_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZPlatformUtils.h"
#include "ZBaseClass.h"
#include "ZBaseDefs.h"


// Class //////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
        extern unsigned int gDictionnarySize;
#endif

extern ZCriticalSection_t* GInstancesCriticalSection;

#define MAKE_BC_USABLE(x) (ZBaseClass *)((char*)x+sizeof(void*))

typedef struct ZSerializableClasses
{
    ZSerializableClasses()
    {
        mNbMembers = 0;
        mFirst = NULL;
        mLast = NULL;
        mCount = 0;

    }

    ZSerializableClasses(TClassID iClassId, ZBaseClass *(* iCreateNew)(SerializableField **pFields, int aCount), const char *pName,
        size_t sClassSize,
        SerializableField *pFields)
    {
        classID = iClassId;
        CreateNew = iCreateNew;
        name = pName;
        mClassSize = sClassSize;
        mFields = pFields;
        mNbMembers = 0;
/*
        mLuaMethods = NULL;
*/
        mFirst = NULL;
        mLast = NULL;
        mCount = 0;

#ifdef _DEBUG
        gDictionnarySize += int(sizeof(ZSerializableClasses) + strlen(name) + 1);
#endif

    }

    const char *name;
    TClassID classID;
    size_t    mClassSize;
    unsigned short mNbMembers;
    SerializableField *mFields;
    ZBaseClass *(* CreateNew)(SerializableField **pFields, int aCount);

    SerializableField *FindInEngineField(const char *szFieldName);

    // deserialization corresponding class. not used for inengine stuffs
    ZSerializableClasses *pInEngineClass; // NULL means no more present

    ZBaseClass *New()
    {
        return CreateNew(&mFields, 1);
    }
/*
    const luaL_Reg *mLuaMethods;
*/

    // instance management
    ZBaseClass *mFirst;
    ZBaseClass *mLast;
    unsigned int mCount;
    ZBaseClass * AddInstance(ZBaseClass *pInstance)
    {
		if (!pInstance) return NULL;
		LockCriticalSection(GInstancesCriticalSection);

        if (!mFirst)
        {
            mLast = mFirst = pInstance;
        }
        else
        {
			mLast->SetNextInstance(pInstance);
			pInstance->SetPrevInstance(mLast);

            mLast = pInstance;
        }
		pInstance->SetRootClassID(pInstance->GetClassID());

		mCount++;
		UnLockCriticalSection(GInstancesCriticalSection);
		return pInstance;
    }

    void RemoveInstance(ZBaseClass *pInstance)
    {
		if (!mCount)
			return;
		LockCriticalSection(GInstancesCriticalSection);

        if (pInstance == mFirst)
        {
            mFirst = pInstance->mNextInstance;
        }

        if (pInstance == mLast)
        {
            mLast = pInstance->mPrevInstance;
        }

        if (pInstance->mNextInstance)
        {
            pInstance->mNextInstance->mPrevInstance = pInstance->mPrevInstance;
        }

        if (pInstance->mPrevInstance)
        {
            pInstance->mPrevInstance->mNextInstance = pInstance->mNextInstance;
        }
		UnLockCriticalSection(GInstancesCriticalSection);
        mCount--;
    }

    void *FindInstanceByName(const char *pszName) const;

} ZSerializableClasses;

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSerializableClasses * FindClassDefByName(const char *className);

///////////////////////////////////////////////////////////////////////////////////////////////////
// infos

void DumpInstances(TClassID aClass);

///////////////////////////////////////////////////////////////////////////////////////////////////

void SaveInstancesToDisk_PCFormat(const char *szFileName);

///////////////////////////////////////////////////////////////////////////////////////////////////

void DumpClasses();

///////////////////////////////////////////////////////////////////////////////////////////////////

void DumpInstances();

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
