///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSerializableClasses.cpp
// Creation : 02/11/2006
// Author : Cedric Guillemet
// Description :
// Reliability : 5/10
///////////////////////////////////////////////////////////////////////////////////////////////////

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZBaseDefs.h"
#include "ZSerializableClasses.h"
#include "ZBaseClass.h"
#include "ZSerializator.h"
#include "ZLogger.h"
#include "ZBaseClass.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned short gSerializableClassesCount;

///////////////////////////////////////////////////////////////////////////////////////////////////

void *ZSerializableClasses::FindInstanceByName(const char *pszName) const
{
    if (classID!=0)
    {
        ZBaseClass *ptr =mFirst;
        while(ptr)
        {
            //ZBaseClass *pui  = ptr;//MAKE_BC_USABLE(ptr);
            const char *pszObjectName = ptr->GetName().c_str();
            if ((pszObjectName)&&(!stricmp(pszObjectName, pszName)) )
                return ptr->QueryInstance();
            //ptr = (ZBaseClass *)NI(ptr);
			ptr = ptr->mNextInstance;
        }
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSerializableClasses * FindClassDefByName(const char *className)
{
    for (unsigned int i=BASIC_TYPE_LIMIT;i<(BASIC_TYPE_LIMIT+gSerializableClassesCount);i++)
    {
        if (gSerializableClasses[i].name)
        if (!strcmp(gSerializableClasses[i].name, className))
            return &gSerializableClasses[i];
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

SerializableField *ZSerializableClasses::FindInEngineField(const char *szFieldName)
{
    for (unsigned int i =0;i<mNbMembers;i++)
    {
        if (mFields[i].name)
            if (!strcmp(mFields[i].name, szFieldName))
                return &mFields[i];
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DumpClasses()
{
	LOG("name                            count  minmem  maxmem  avermem total mem\n");
	LOG("------------------------------------------------------------------------\n");
    for (unsigned int i = 0 ; i < MAX_NUMBER_OF_SERIALIZABLE_CLASSES ; i ++ )
    {
        if (gSerializableClasses[i].mCount!=0)
        {
            uint32_t memMin = -1;
            uint32_t memMax = 0;
            uint32_t memTotal = 0;
            for (ZBaseClass *pc = gSerializableClasses[i].mFirst; pc; pc=pc->mNextInstance)
            {
                tulong mem = pc->GetMemoryUsed();
                if (mem<memMin) memMin = mem;
                if (mem>memMax) memMax = mem;
                memTotal += mem;
            }
            // name - count - minmemory used - max memory used - average mem - total mem
            LOG("%32s %4d %7d %7d %7d %8d\r\n", gSerializableClasses[i].name, gSerializableClasses[i].mCount,
                memMin, memMax, memTotal/gSerializableClasses[i].mCount, memTotal);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DumpInstances()
{

    int i;

    for (i = 0 ; i < MAX_NUMBER_OF_SERIALIZABLE_CLASSES ; i ++ )
    {
        if (gSerializableClasses[i].classID!=0)
        {
            LOG("%s \r\n",gSerializableClasses[i].name);
            ZBaseClass *ptr =gSerializableClasses[i].mFirst;
            while(ptr)
            {
				LOG("   %s(0x%x) Ref : %d\n", ptr->GetName().c_str(), ptr, ptr->GetRefCount());
                ptr = ptr->mNextInstance;
            }
        }
    }

}
