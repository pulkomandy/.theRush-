///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSerialisator.cpp
// Creation : 13/06/2007
// Author : Cedric Guillemet
// Description :
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
#include "stdafx.h"
#include "ZSerializator.h"
#include <list>
#include "ZLogger.h"
#include "ZBaseMaths.h"
#include "tinyxml.h"
#ifdef WIN32
#include <io.h>
#endif

unsigned int gCLASS_ID_Counter = 0;

void* BuildSerializableClass(TClassID classID, SerializableField **pFields, int aCount=1);

#ifdef _DEBUG
unsigned int gDictionnarySize = 0;
#endif

ZSerializableClasses gSerializableClasses[MAX_NUMBER_OF_SERIALIZABLE_CLASSES];
bool bFirstSerializableClassesSet = false;
unsigned short gSerializableClassesCount = 0;
ZCriticalSection_t* GInstancesCriticalSection;

class CInstancesCriticalSection
{
public:
	CInstancesCriticalSection()
	{
		GInstancesCriticalSection = NewCriticalSection();
		PROFILER_ENABLE;
	}
	~CInstancesCriticalSection()
	{
		//DestroyCriticalSection(GInstancesCriticalSection);
	}
};

ZTimer gTimer;

void Zexit(int xt)
{
	exit(xt);
}

static CInstancesCriticalSection MakeCInstancesCriticalSection;
SerializableClassesInstanciation::SerializableClassesInstanciation(TClassID iClassId,
                                                                   ZBaseClass *(* iCreateNew)(SerializableField **pFields, int aCount),
                                                                   const char *pName,
                                                                   size_t sClassSize,
                                                                   SerializableField *pFields)
{
	void *test =&MakeCInstancesCriticalSection;
    // clean it
    if (!bFirstSerializableClassesSet)
    {
        memset(gSerializableClasses,0,sizeof(ZSerializableClasses) * MAX_NUMBER_OF_SERIALIZABLE_CLASSES);
        bFirstSerializableClassesSet = true;
    }

    // check ID isn't already used
    if (gSerializableClasses[iClassId].classID != 0)
    {
        // big big error
#ifdef PSM_WIN32
        char msgErr[512];
        sprintf(msgErr,"ID %d already used between classes %s and %s.",iClassId, gSerializableClasses[iClassId].name, pName);
        MessageBox(NULL, msgErr, "Class ID errors", MB_OK);
        Zexit(-1);
#endif
    }
    // add it
    gSerializableClasses[iClassId] = ZSerializableClasses(iClassId, iCreateNew, pName, sClassSize, pFields);
    gSerializableClassesCount ++;

	
}

void* BuildSerializableClass(TClassID classID, SerializableField **pFields, int aCount)
{
	void *test =&MakeCInstancesCriticalSection;

    if (gSerializableClasses[classID].classID == classID)
        return gSerializableClasses[classID].CreateNew(pFields, aCount);
    (*pFields) = NULL;
    return NULL;
}


//////////////////////////////////////////////////////////////////////
// debug/runtime dump infos

const char *GetSerialisationName(SerializableType eType)
{
    switch(eType)
    {
        case SerializableBase:       return "Base";        break;
        case SerializablePointer:    return "Pointer";     break;
        case SerializableEnd:        return "End";         break;
        case SerializableBaseClass:  return "BaseClass";   break;
        case SerializableAgregat:    return "Agregat";     break;
    }
    return "";
}

const char *getTabDepth(int depth)
{
    static const char gTabDepth[]={"\t\t\t\t\t\t\t\t\t\t"};
    if (depth>10)
        depth = 10;

    return &gTabDepth[10-depth];
}


void DumpSerializationFields(SerializableField *pFields, const char *pName, int depth)
{
	
    depth++;
    printf("%sSerialization of \"%s\" class/struct\n",getTabDepth(depth),pName);
    while (pFields->type != SerializableEnd)
    {
        if (pFields->type == SerializableBaseClass)
        {
            printf("%s+ %s (SuperClass) ofs %d\n",getTabDepth(depth),pFields->name,(int)pFields->offset);
            DumpSerializationFields((SerializableField *)pFields->size_fieldsOffset, pFields->name, depth);
        }
        else
        {
            if (pFields->count >1)
                printf("%s%s (Array of %d elements) %d bytes each ofs %d\n",getTabDepth(depth),pFields->name,(int)pFields->count,pFields->size_fieldsOffset,(int)pFields->offset);
            else
                printf("%s%s (%s) %d bytes ofs %d\n",getTabDepth(depth),pFields->name,GetSerialisationName(pFields->type),pFields->size_fieldsOffset,(int)pFields->offset);
        }

        pFields++;
    }
}

void DumpObjectValuesFn(SerializableField *pFields, const char *className, void *ptr, int depth)
{
    depth++;
    if (depth == 0)
        LOG("%sDumping %s(0x%p) \r\n",getTabDepth(depth),className,ptr);

    while (pFields->type != SerializableEnd)
    {
        if (pFields->type == SerializableBaseClass)
        {
            LOG("%s+ %s \r\n",getTabDepth(depth),pFields->name);
            DumpObjectValuesFn((SerializableField *)pFields->size_fieldsOffset, pFields->name, (void*)((((char*)ptr)+pFields->offset)), depth);
        }
        else
        {
            // agregated class?
            if (pFields->type == SerializableAgregat)
            {
                if (pFields->count>1)
                {
                    for (unsigned int i= 0;i<pFields->count;i++)
                    {
                        LOG("%s%s[%d]\r\n",getTabDepth(depth),pFields->name,i);
                        DumpObjectValuesFn((SerializableField *)pFields->size_fieldsOffset,
                            pFields->name,
                            (void*)((((char*)ptr)+pFields->offset+(i*gSerializableClasses[pFields->classID].mClassSize))),
                            depth);
                    }
                }
                else
                {
                    LOG("%s%s\r\n",getTabDepth(depth),pFields->name);
                    DumpObjectValuesFn((SerializableField *)pFields->size_fieldsOffset, pFields->name, (void*)((((char*)ptr)+pFields->offset)), depth);
                }
            }
            else
            {

                if (pFields->count>1)
                {
                    LOG("%s%s[%d] = {",getTabDepth(depth),pFields->name,pFields->count);
                }
                else
                    LOG("%s%s = ",getTabDepth(depth),pFields->name);

                for (unsigned int i= 0;i<pFields->count;i++)
                {
                    char *ptrOfset = (((char*)ptr)+pFields->offset+(i*pFields->size_fieldsOffset));
                    // base
                    switch (pFields->classID)
                    {
                    case SB_UNDEFINED:    LOG("(undefined type)");                    break;
                    case SB_I8:            LOG("%d",*(char*)ptrOfset );                break;
                    case SB_UI8:        LOG("%d",*(unsigned char*) ptrOfset);    break;
                    case SB_I16:        LOG("%d",*(short*) ptrOfset);            break;
                    case SB_UI16:        LOG("%d",*(unsigned short*) ptrOfset);    break;
                    case SB_I32:        LOG("%d",*(long*) ptrOfset);                break;
                    case SB_UI32:        LOG("%d",*(unsigned long*) ptrOfset);    break;
                    case SB_FLOAT:        LOG("%f",*(float*) ptrOfset);            break;
                    case SB_DOUBLE:        LOG("%f",*(double*) ptrOfset);            break;
                    case SB_STRING:        LOG("%s",(char*) ptrOfset);                break;
                    case SB_BOOL:        LOG("%s",*(bool*) ptrOfset?"true":"false");                break;
                    case SB_MATRIX: LOG("%5.4f %5.4f %5.4f %5.4f\r\n%s               %5.4f %5.4f %5.4f %5.4f\r\n%s               %5.4f %5.4f %5.4f %5.4f\r\n%s               %5.4f %5.4f %5.4f %5.4f\r\n",
                                                            ((tmatrix*)ptrOfset)->m16[0], ((tmatrix*)ptrOfset)->m16[1], ((tmatrix*)ptrOfset)->m16[2], ((tmatrix*)ptrOfset)->m16[3],
                                        getTabDepth(depth), ((tmatrix*)ptrOfset)->m16[4], ((tmatrix*)ptrOfset)->m16[5], ((tmatrix*)ptrOfset)->m16[6], ((tmatrix*)ptrOfset)->m16[7],
                                        getTabDepth(depth), ((tmatrix*)ptrOfset)->m16[8], ((tmatrix*)ptrOfset)->m16[9], ((tmatrix*)ptrOfset)->m16[10], ((tmatrix*)ptrOfset)->m16[11],
                                        getTabDepth(depth), ((tmatrix*)ptrOfset)->m16[12], ((tmatrix*)ptrOfset)->m16[13], ((tmatrix*)ptrOfset)->m16[14], ((tmatrix*)ptrOfset)->m16[15] ); break;
                    case SB_WSTRING:
                        //printf("%s%s = %d \n",getTabDepth(depth),pFields->name,*(int*) (((char*)ptr)+pFields->offset));
                        break;
                    }
                    if ((i!=(pFields->count-1))&&(pFields->count>1)) printf(", ");

                }
                if (pFields->count>1)
                    LOG("}\r\n");
                else
                    LOG("\r\n");
            }
        }

        pFields++;
    }
}

//////////////////////////////////////////////////////////////////////
// RTTI

// return true if instance x is kind of y
// true si idsrc.id == idsrc.id or if iddst is a base of idsrc
bool IsKindOfFunc(TClassID idSrc, TClassID idDst, SerializableField *pFields)
{
    if (idSrc == idDst) return true;

    while (pFields->type != SerializableEnd)
    {
        if (pFields->type == SerializableBaseClass)
        {
            bool bret = IsKindOfFunc(pFields->classID, idDst, (SerializableField *)pFields->size_fieldsOffset);
            if (bret) return true;
        }

        pFields++;
    }

    return false;
}

// returns class name from class ID

const char* getSerializableClassName(TClassID aClassID)
{
    return gSerializableClasses[BASIC_TYPE_LIMIT+aClassID].name;
}

SerializableField* getSerializableClassFields(TClassID aClassID)
{
    return gSerializableClasses[BASIC_TYPE_LIMIT+aClassID].mFields;
}

void setSerializableClassLuaMethods(TClassID aClassID, const luaL_Reg *pMet)
{
    /*
    gSerializableClasses[aClassID].mLuaMethods = pMet;
    */
}

const luaL_Reg * getSerializableLuaMethods(TClassID aClassID)
{
    /*
    return gSerializableClasses[BASIC_TYPE_LIMIT+aClassID].mLuaMethods;
    */
    return NULL;
}

TClassID getClassIDFromClassName(const char *pszClassName)
{
    int i;
    for (i = 0 ; i < MAX_NUMBER_OF_SERIALIZABLE_CLASSES ; i ++ )
    {
        if (gSerializableClasses[i].classID!=0)
        {
            if (!strcmp(gSerializableClasses[i].name, pszClassName))
                return gSerializableClasses[i].classID;
        }
    }
	
    return 0;
}

// objet Creation for ZCriticalSection_t* GInstancesCriticalSection;




tstring SerializeXMLOut(SerializableField *pFields,void *ptr, const char *className)
{
	tstring ret;
    ret.Printf("<%s \r\n",className);
    while (pFields->type != SerializableEnd)
    {
        switch (pFields->type)
        {
        // templates
        case SerializableTListBasic:
        case SerializableTList:
            {/*
                tlist<char>* tabbr = (tlist<char>*)( ((char*)ptr)+pFields->offset);
                unsigned long tabCount;
//                tlist<char>::Node *pNode = tabbr->first;

                // read count
                Read(&tabCount, sizeof(unsigned long));
                tabbr->count = tabCount;

                void *Previous = NULL;

                if (pFields->classID < BASIC_TYPE_LIMIT)
                {
                    unsigned long realsize = (pFields->classID < BASIC_ATOMIC_TYPE)?pFields->size_fieldsOffset : gSerializableClasses[pFields->classID].mClassSize;
                    for (unsigned long i = 0 ; i < tabCount ; i++ )
                    {
                        char *nodeDatas = new char [realsize + sizeof(void*)*2]; // 2 sizeof(void*) for prev&next pointers
                        if (i==0) tabbr->first = (tlist<char>::Node*)nodeDatas;

                        *((void**)nodeDatas) = NULL;
                        *((void**)((char*)nodeDatas+sizeof(void*))) = Previous;

                        if (Previous) *((void**)Previous) = nodeDatas;
                        Read((((char*)nodeDatas)+sizeof(void*)*2), realsize);

                        Previous = nodeDatas;
                        tabbr->last = (tlist<char>::Node*)nodeDatas;
                    }
                }
                else
                {
                    for (unsigned long i = 0 ; i < tabCount ; i++ )
                    {
                        // call constructor: /!\ constructor is not called!!!
                        //SerializableField *pnFields;
                        //tabbr->tab = (char*)BuildSerializableClass(pFields->classID, &pnFields, tabCount);
                        unsigned long realsize = gSerializableClasses[pFields->classID].mClassSize;
                        char *nodeDatas = new char [realsize + sizeof(void*)*2]; // 2 sizeof(void*) for prev&next pointers
                        if (i==0) tabbr->first = (tlist<char>::Node*)nodeDatas;

                        *((void**)nodeDatas) = NULL;
                        *((void**)((char*)nodeDatas+sizeof(void*))) = Previous;

                        if (Previous) *((void**)Previous) = nodeDatas;
                        void *ptrDest = ((void*)(((char*)nodeDatas)+sizeof(void*)*2));
                        //Read(pNodeData, realsize);
                        SerializedPointer curptr;
                        SerializeInPointer(&curptr);
                        AddPointerToSerialized(&curptr, ptrDest);

                        SerializeIn((SerializableField*)pFields->size_fieldsOffset, ptrDest);

                        Previous = nodeDatas;
                        tabbr->last = (tlist<char>::Node*)nodeDatas;
                    }
                }
                */
            }
            break;
        case SerializableTArrayBasic:
        case SerializableTArray:
            {
				/*
                tarray<char>* tabbr = (tarray<char>*)( ((char*)ptr)+pFields->offset);
                unsigned long tabCount;

                // write count
                //Read(&tabCount, sizeof(unsigned long));
                tabbr->count = tabCount;
                tabbr->maxSize = tabCount;

                if (pFields->classID)// < BASIC_TYPE_LIMIT)
                {
                    unsigned long realsize = (pFields->classID < BASIC_ATOMIC_TYPE)?pFields->size_fieldsOffset : gSerializableClasses[pFields->classID].mClassSize;

                    //tabbr->tab = new char[realsize * tabCount];
                    //Read(tabbr->tab, realsize * tabCount);
                }
                else
                {
                    SerializableField *pnFields;
                    tabbr->tab = (char*)BuildSerializableClass(pFields->classID, &pnFields, tabCount);
                    for (unsigned long i = 0 ; i < tabCount ; i++ )
                    {
                        char *ptrDest = ((char*)tabbr->tab)+(gSerializableClasses[pFields->classID].mClassSize * i);

                        SerializedPointer curptr;
                        SerializeInPointer(&curptr);
                        AddPointerToSerialized(&curptr, ptrDest);

                        // test known simple agregat (vector, matrix, color, string) for specialised/fast writing
                        SerializeIn((SerializableField*)pFields->size_fieldsOffset, ptrDest);
                    }
                }
				*/
                
            }
            break;
        case SerializableBase:
            {
                for (unsigned int i= 0;i<pFields->count;i++)
                {
					tstring val,valval;
                    val.Printf("%s",pFields->name);
                    char *ptrOfset = (((char*)ptr)+pFields->offset+(i*pFields->size_fieldsOffset));
                    // base
                    switch (pFields->classID)
                    {
                    case SB_UNDEFINED:  valval.Printf("=\"\"\r\n");                                 break;
                    case SB_I8:         valval.Printf("=\"%d\"\r\n",*(char*)ptrOfset );             break;
                    case SB_UI8:        valval.Printf("=\"%d\"\r\n",*(unsigned char*) ptrOfset);    break;
                    case SB_I16:        valval.Printf("=\"%d\"\r\n",*(short*) ptrOfset);            break;
                    case SB_UI16:       valval.Printf("=\"%d\"\r\n",*(unsigned short*) ptrOfset);   break;
                    case SB_I32:        valval.Printf("=\"%d\"\r\n",*(long*) ptrOfset);             break;
                    case SB_UI32:       valval.Printf("=\"%d\"\r\n",*(unsigned long*) ptrOfset);    break;
                    case SB_FLOAT:      valval.Printf("=\"%f\"\r\n",*(float*) ptrOfset);            break;
                    case SB_DOUBLE:     valval.Printf("=\"%f\"\r\n",*(double*) ptrOfset);           break;
                    case SB_BOOL:       valval.Printf("=\"%d\"\r\n",(*(bool*) ptrOfset)?1:0);       break;
                    case SB_STRING:     valval.Printf("=\"%s\"\r\n",(char*) ptrOfset);              break;
                    case SB_WSTRING:    //printf("%s%s = %d \n",getTabDepth(depth),pFields->name,*(int*) (((char*)ptr)+pFields->offset));
                        break;
                    }
					ret += val;
					ret += valval;
                }
            }
            break;
        case SerializableBaseClass:
        case SerializableAgregat:
            {/*
                // test basic type
                if (pFields->classID < BASIC_TYPE_LIMIT)
                {
                    char *ptrDest = ((char*)ptr)+pFields->offset;

                    //retrieve its previous pointer
                    SerializedPointer curptr;
                    SerializeInPointer(&curptr);
                    AddPointerToSerialized(&curptr, ptrDest);
                    // read it
                    Read(ptrDest, gSerializableClasses[pFields->classID].mClassSize * pFields->count);
                }
                else
                {
                    for (unsigned int i=0;i<pFields->count;i++)
                    {
                        char *ptrDest = ((char*)ptr)+pFields->offset + (gSerializableClasses[pFields->classID].mClassSize*i);
                        //retrieve its previous pointer
                        SerializedPointer curptr;
                        SerializeInPointer(&curptr);
                        AddPointerToSerialized(&curptr, ptrDest);
                        // do the serialization
                        SerializeIn((SerializableField*)pFields->size_fieldsOffset, ptrDest );
                        DoAfterRead(pFields->classID, ptrDest );
                    }
                }
                */
            }
            break;
        case SerializablePointer:
            {/*
                SerializedPointer curptr;
                SerializeInPointer(&curptr);

                void **toSerPtr = ((void**)(((unsigned int)ptr) + pFields->offset) );

                if (!curptr.ptr)
                {
                    // NULL pointer
                    (*toSerPtr) = NULL;
                }
                else
                {
                    SerializedPointer* seekPtr = IsPointerAllReadySerialized(curptr.ptr);
                    if (seekPtr)
                    {
                        // allready read, just assign pointer value
                        (*toSerPtr) = seekPtr->newPtr;
                    }
                    else
                    {
                        // not read, create new class and read it
                        SerializableField *nFields = NULL;
                        (*toSerPtr) = BuildSerializableClass(curptr.classID, &nFields);
                        AddPointerToSerialized(&curptr, (*toSerPtr));
                        SerializeIn(nFields, (*toSerPtr));
                    }
                }
                */
            }
            break;
        default:break;
        }
        pFields++;
    }
	ret += " />\r\n";

	return ret;
}

void SerializeXMLIn(SerializableField *pFields,void *ptr, const char *className, TiXmlDocument *pDoc)
{

//    TiXmlNode* node;
    TiXmlNode* root=pDoc->FirstChild( className );
	if (!root) 
		return;
    TiXmlElement* itemElement = root->ToElement();

    if (itemElement)
    {

        while (pFields->type != SerializableEnd)
        {
            switch (pFields->type)
            {
                // templates
            case SerializableTListBasic:
            case SerializableTList:
                {
                }
                break;
            case SerializableTArrayBasic:
            case SerializableTArray:
                {
                }
                break;
            case SerializableBase:
                {
                    for (unsigned int i= 0;i<pFields->count;i++)
                    {
                        char *smarg=(char*)itemElement->Attribute(pFields->name);

                        if (!smarg) continue;
                        char *ptrOfset = (((char*)ptr)+pFields->offset+(i*pFields->size_fieldsOffset));
                        // base
                        switch (pFields->classID)
                        {
                        case SB_UNDEFINED:  break;
                        case SB_I8:         sscanf(smarg,"%d",(char*)ptrOfset );             break;
                        case SB_UI8:        sscanf(smarg,"%d",(unsigned char*) ptrOfset);    break;
                        case SB_I16:        sscanf(smarg,"%d",(short*) ptrOfset);            break;
                        case SB_UI16:       sscanf(smarg,"%d",(unsigned short*) ptrOfset);   break;
                        case SB_I32:        sscanf(smarg,"%d",(long*) ptrOfset);             break;
                        case SB_UI32:       sscanf(smarg,"%d",(unsigned long*) ptrOfset);    break;
                        case SB_FLOAT:      sscanf(smarg,"%f",(float*) ptrOfset);            break;
                        case SB_DOUBLE:     sscanf(smarg,"%lf",(double*) ptrOfset);          break;
                        case SB_BOOL:       (*(bool*) ptrOfset) = (smarg[0]!='0');           break;
                        case SB_STRING:     /*sscanf("%s\"\r\n",(char*) ptrOfset);*/         break;
                        case SB_WSTRING:    //printf("%s%s = %d \n",getTabDepth(depth),pFields->name,*(int*) (((char*)ptr)+pFields->offset));
                            break;
                        }
                    }
                }
                break;
            case SerializableBaseClass:
            case SerializableAgregat:
                {
                }
                break;
            case SerializablePointer:
                {
                }
                break;
            default:break;
            }
            pFields++;
        }
    }

}


bool XMLizeOut(ZBaseClass *pClass, const char *szFileName)
{
	tstring val = SerializeXMLOut(pClass->getInfosFields(), pClass, pClass->GetZClassName());
	FILE *fp = fopen(szFileName,"wb");
	if (fp)
	{
		fwrite(val.c_str(), val.Length(), 1, fp);
		fclose(fp);
		return true;
	}
	return false;
}

bool XMLizeIn(ZBaseClass *pClass, const char *szFileName)
{
	TiXmlDocument xmlDoc;

	FILE *fp = fopen(szFileName, "rb");
	if (fp)
	{
#ifdef WIN32
		int fileno = _fileno(fp);
		int filelen = _filelength(fileno);
#else
		struct stat bufStat;
		int res = fstat(fileno(fp), &bufStat);
		int filelen = 0;
		if (res == 0)
		{
			filelen = bufStat.st_size;
		}
#endif
		char *ptxt = new char [filelen+1];
		ptxt[filelen] = 0;
		fread(ptxt, filelen, 1, fp);
		if (xmlDoc.Parse(ptxt))
		{
			SerializeXMLIn(pClass->getInfosFields(), pClass, pClass->GetZClassName(), &xmlDoc);

			return true;
		}
	}

	return false;
}



bool XMLizeOut(std::vector<ZBaseClass *>& pClass, const char *szFileName)
{
	tstring val = "";
	for (int i=0;i<pClass.size();i++)
		val += SerializeXMLOut(pClass[i]->getInfosFields(), pClass[i], pClass[i]->GetZClassName());

	FILE *fp = fopen(szFileName,"wb");
	if (fp)
	{
		fwrite(val.c_str(), val.Length(), 1, fp);
		fclose(fp);
		return true;
	}
	return false;
}

bool XMLizeIn(std::vector<ZBaseClass *>& pClass, const char *szFileName)
{
	TiXmlDocument xmlDoc;

	FILE *fp = fopen(szFileName, "rb");
	if (fp)
	{
#ifdef WIN32
		int fileno = _fileno(fp);
		int filelen = _filelength(fileno);
#else
		struct stat bufStat;
		int res = fstat(fileno(fp), &bufStat);
		int filelen = 0;
		if (res == 0)
		{
			filelen = bufStat.st_size;
		}
#endif
		char *ptxt = new char [filelen+1];
		ptxt[filelen] = 0;
		fread(ptxt, filelen, 1, fp);
		if (xmlDoc.Parse(ptxt))
		{
			for (int i=0;i<pClass.size();i++)
				SerializeXMLIn(pClass[i]->getInfosFields(), pClass[i], pClass[i]->GetZClassName(), &xmlDoc);

			return true;
		}
	}

	return false;
}