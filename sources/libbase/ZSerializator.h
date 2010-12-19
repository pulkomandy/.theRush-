///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSerialisator.h
// Creation : 13/06/2007
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

#ifndef ZSERIALIZATOR_H__
#define ZSERIALIZATOR_H__

#include "ZSerializableClasses.h"
#include "ZBaseClass.h"
#include <memory.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: definir templates a serializer: templates d'agregats & de pointeurs
// TODO: pointeurs array, verifier que ca marche quand chaque instance provient d'une classe differente
// TODO: faire 1 bonne grosse classe de test avec #define pour reading/writing avec classes differentes

// NOTE: les tableaux static de pointers vers classes sont interdits?

// define the number of basic types limit
// the classID below that limit are used of structs (vertex, matrix, color, string,...)
// basic type allow reflections but serialisation are faster
// the serialisation process treats them as bytes array
// basic types cannot be derived, have no pointers, no templates agregats

#define BASIC_TYPE_LIMIT 100
// below the following value are the atomic types
#define BASIC_ATOMIC_TYPE  20

// sets the maximum of classes descriptions/ID
#define MAX_NUMBER_OF_SERIALIZABLE_CLASSES 1000

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)

#define DECLAREZCLASS(x) extern TClassID ClassID##x


/////////////////////////////////////////</////////////////////////////
// debug

// dictionnary memory overhead
extern unsigned int gDictionnarySize;

struct SerializableClassesInstanciation;
struct SerializableField;
struct luaL_Reg;
//////////////////////////////////////////////////////////////////////
// serializable classes declaration
// list registered class/structs
// allow new creation from a class ID


extern ZSerializableClasses gSerializableClasses[];
/*
template<typename T> T* makeNew(TClassID clsID)
{
	return ((T*)gSerializableClasses[clsID].New());//new T;
}
*/
inline void _AddInstance(ZBaseClass *pObj)
{
	gSerializableClasses[pObj->GetClassID()].AddInstance(pObj);
}
inline void* _CreateNew(TClassID clsID)
{
	return (void*)gSerializableClasses[clsID].AddInstance(gSerializableClasses[clsID].New())->QueryInstance();
}
#define _New(x) _CreateNew(ClassID##x)

//#define NewClass(x,y) makeNew<x>(ClassID##y)
//#define _New(x) (void*)gSerializableClasses[ClassID##x].New()

//#define NewClass(x) gSerializableClasses[ClassID##x].New()
#define FirstInstanceOf(x) (void*)(gSerializableClasses[ClassID##x].mFirst?gSerializableClasses[ClassID##x].mFirst->QueryInstance():NULL)
#define LastInstanceOf(x) (void*)(gSerializableClasses[ClassID##x].mLast?gSerializableClasses[ClassID##x].mLast->QueryInstance():NULL)
#define FindInstance(x,y) (void*)gSerializableClasses[ClassID##x].FindInstanceByName(y)
#define GetInstancesCount(x) gSerializableClasses[ClassID##x].mCount
#define NI(x) (x->mNextInstance?x->mNextInstance->QueryInstance():NULL)
//////////////////////////////////////////////////////////////////////
// various macros used to get offset of member/infos fields/ size of members/offset of classID/....
// compute a member's offset from a structure

#define OFFSETOF(s,m)                       (size_t(&(((s*)1)->m))-1)
#define OFFSETOFFIELDS(s,m)                 (size_t((((s*)1)->m->mInfosSerializableFields)))
#define OFFSETOFFIELDSAGREGAT(s,m)          (size_t((((s*)1)->m.mInfosSerializableFields)))
#define OFFSETOFFIELDSAGREGATARRAY(s,m)     (size_t((((s*)1)->m[0].mInfosSerializableFields)))
#define SIZEOF(s,m)                         (sizeof((((s*)1)->m)))
#define SIZEOFARRAY(s,m)                    (sizeof((((s*)1)->m[0])))
#define CLASSID(s,m)                        (((((s*)1)->m->mClassID)))
#define CLASSIDAGREGAT(s,m)                 (((((s*)1)->m.mClassID)))
#define CLASSIDAGREGATARRAY(s,m)            (((((s*)1)->m[0].mClassID)))

//////////////////////////////////////////////////////////////////////
// macro type of serialized members

enum SerializableType
{
    SerializableBase,
    SerializableAgregat,
    SerializableBaseClass,
    SerializablePointer,
    SerializableTListBasic,
    SerializableTArrayBasic,
    SerializableTList,
    SerializableTArray,
    SerializableEnd = 0xFFFF //set it as a short
};

// types for base
enum SerBaseType
{
    SB_UNDEFINED,
    SB_I8,
    SB_UI8,
    SB_I16,
    SB_UI16,
    SB_I32,
    SB_UI32,
    SB_FLOAT,
    SB_DOUBLE,
    SB_BOOL,
    SB_STRING,
    SB_WSTRING,
    SB_MATRIX,
    SB_TCOLOR,
    SB_TVECTOR3,
    SB_TVECTOR4,
    SB_TVECTOR2,
    SB_QUATERNION,
    SB_END = 0xFFFF
};
//////////////////////////////////////////////////////////////////////
// description field.
// one field per class/struct member

#define ADD_MEMBER_FIELD_BASIC name = pName;\
                            gSerializableClasses[ParentClassID].mNbMembers++;

#ifdef _DEBUG
#define ADD_MEMBER_FIELD() ADD_MEMBER_FIELD_BASIC\
                            gDictionnarySize += int(sizeof(SerializableField) + strlen(name) +1);

#else
#define ADD_MEMBER_FIELD() ADD_MEMBER_FIELD_BASIC
#endif

typedef struct SerializableField
{
    SerializableField() {}

    //templates of basic types
    SerializableField(TClassID ParentClassID, const char *pName, unsigned int iSize, unsigned int iOffset, SerBaseType eBaseType, SerializableType eType)
    {
        ADD_MEMBER_FIELD();
        size_fieldsOffset = iSize;
        offset = iOffset;
        type = eType;
        classID = (TClassID)eBaseType;
        count = 0;
        mEnumStr = NULL;
    }

    SerializableField(TClassID ParentClassID, const char *pName, unsigned int iSize, unsigned int iOffset, SerializableType eType)
    {
        ADD_MEMBER_FIELD();
        size_fieldsOffset = iSize;
        offset = iOffset;
        type = eType;
        if (eType == SerializableBase)
            classID = SB_UNDEFINED;
        count = 1;
        mEnumStr = NULL;
    }

    SerializableField(TClassID ParentClassID, const char *pName, unsigned int iSize, unsigned int iOffset, unsigned int iCount, SerBaseType eBaseType, const char* pEnumStr)
    {
        ADD_MEMBER_FIELD();

        size_fieldsOffset = iSize;
        offset = iOffset;
        type = SerializableBase;
        classID = (TClassID)eBaseType;
        count = iCount;
        mEnumStr = pEnumStr;
    }

    SerializableField(TClassID ParentClassID, const char *pName, unsigned int iOffset, unsigned int iFieldsOffset, TClassID iClassID, unsigned int iCount, SerializableType eType = SerializablePointer) // for pointers;
    {
        ADD_MEMBER_FIELD();

        offset = iOffset;
        size_fieldsOffset = iFieldsOffset;
        type = eType;
        classID = iClassID;
        count = iCount;
        mEnumStr = NULL;
    }

    SerializableField(TClassID ParentClassID, const char *pName, unsigned int iSize, unsigned int iOffset, unsigned int iCount, SerializableType eType)
    {
        ADD_MEMBER_FIELD();

        size_fieldsOffset = iSize;
        offset = iOffset;
        type = eType;
        count = iCount;
        if (eType == SerializableBase)
            classID = SB_UNDEFINED;
        mEnumStr = NULL;
    }

    const char *name;
    int size_fieldsOffset; // can be a pointer to a SerializableField array for class agregat in another class-> so is int, but serialized as int32
    uint32_t offset;
    uint32_t count;
    TClassID classID;                // classID is a SerBaseType for basic type
    SerializableType type;
    const char *mEnumStr;
    // deserialization infos, not used for in-engine infos. only used in deserialization of dictionnary.
    SerializableField *pInEngineField; // NULL means no more present
} SerializableField;

//////////////////////////////////////////////////////////////////////
// Magic ID. every SERIALIZEd file will begin this this 4 byte chunk
// used to not serialize in a non-serialized-out file

static const unsigned int MAGICID='L';//33T';
extern unsigned int gCLASS_ID_Counter;
//////////////////////////////////////////////////////////////////////
// class serialisation bootstrap. those are the methods used when asking a class to be serialized


/*
#define SERIALIZE_XML_OPREAD(x) void x::ReadXMLFrom(ZFileSerializer &file) \
{\
    TiXmlDocument xmldoc; \
    char *xmlDoc = new char [file.GetSize()]; \
    file.Read(xmlDoc,file.GetSize()); \
    xmldoc.Parse((const char*)xmlDoc); \
    file.SerializeXMLIn(mInfosSerializableFields, this, mInfosSerializableName, &xmldoc); \
    delete [] xmlDoc; \
    DoAfterRead(mClassID, this); \
}
*/
//////////////////////////////////////////////////////////////////////
// class/struct description macros



#define DECLARE_SERIALIZABLE_BASE public:  \
                             static SerializableField *mInfosSerializableFields; \
                             static const char mInfosSerializableName[]; \
                             static ZBaseClass* CreateNew(SerializableField **pFields, int aCount); \
                             static TClassID mClassID; \
                             virtual TClassID GetClassID() { return mClassID; } \
                             virtual const char* GetZClassName() { return mInfosSerializableName; }



#define DECLARE_BASIC_SERIALIZABLE DECLARE_SERIALIZABLE_BASE \
                                 SerializableField* getInfosFields() { return mInfosSerializableFields; }
extern ZBaseClass *GALLOCATING_PTR;
extern ZCriticalSection_t *GInstancesCriticalSection;
extern TClassID GALLOCATING_CLASSID;
#define DECLARE_SERIALIZABLE DECLARE_SERIALIZABLE_BASE \
                             virtual SerializableField* getInfosFields() { return mInfosSerializableFields; } \
								virtual void *QueryInstance() { return this; }\
								virtual void *GetNext() { return this->mNextInstance; }


#define BEGIN_BASIC_SERIALIZATION(x) const char x::mInfosSerializableName[]={QUOTE(x)}; \
                            namespace SERIALIZATION_NAMESPACE##x {\
                            extern ::SerializableField mInfosSerializableFieldsTemp[]; }\
                            TClassID x::mClassID = TClassID( BASIC_ATOMIC_TYPE + gCLASS_ID_Counter++); \
                            SerializableClassesInstanciation x##y(x::mClassID, x::CreateNew,x::mInfosSerializableName,sizeof(x), NULL, NULL, SERIALIZATION_NAMESPACE##x::mInfosSerializableFieldsTemp); \
                            ZBaseClass* x::CreateNew(SerializableField **pFields, int aCount=1) { (*pFields) = mInfosSerializableFields; if (aCount==1) return new x; else return new x[aCount];} \
                            SerializableField *x::mInfosSerializableFields=&SERIALIZATION_NAMESPACE##x::mInfosSerializableFieldsTemp[0];\
                            namespace SERIALIZATION_NAMESPACE##x { typedef ::x CURRENTCLASS; \
                            ::SerializableField mInfosSerializableFieldsTemp[]={


#define BEGIN_SERIALIZATION(x) const char x::mInfosSerializableName[]={QUOTE(x)}; \
                            namespace SERIALIZATION_NAMESPACE##x {\
                            extern ::SerializableField mInfosSerializableFieldsTemp[]; }\
                            TClassID ClassID##x; TClassID x::mClassID = ClassID##x = TClassID(BASIC_TYPE_LIMIT + gCLASS_ID_Counter++); \
                            SerializableClassesInstanciation x##y((x::mClassID),x::CreateNew,x::mInfosSerializableName,sizeof(x), SERIALIZATION_NAMESPACE##x::mInfosSerializableFieldsTemp); \
                            ZBaseClass* x::CreateNew(SerializableField **pFields, int aCount) { (*pFields) = mInfosSerializableFields; if (aCount==1) return new x; else return new x[aCount]; } \
                            SerializableField *x::mInfosSerializableFields=SERIALIZATION_NAMESPACE##x::mInfosSerializableFieldsTemp;\
                            namespace SERIALIZATION_NAMESPACE##x { typedef ::x CURRENTCLASS; \
                            ::SerializableField mInfosSerializableFieldsTemp[]={


#define BEGIN_SERIALIZATION_PURE(x) const char x::mInfosSerializableName[]={QUOTE(x)}; \
                            namespace SERIALIZATION_NAMESPACE##x {\
                            extern ::SerializableField mInfosSerializableFieldsTemp[]; }\
                            TClassID x::mClassID = (BASIC_TYPE_LIMIT + gCLASS_ID_Counter++); \
                            SerializableClassesInstanciation x##y((x::mClassID),x::CreateNew,x::mInfosSerializableName,sizeof(x), SERIALIZATION_NAMESPACE##x::mInfosSerializableFieldsTemp); \
                            ZBaseClass* x::CreateNew(SerializableField **pFields, int aCount) { return NULL; } \
                            SerializableField *x::mInfosSerializableFields=SERIALIZATION_NAMESPACE##x::mInfosSerializableFieldsTemp;\
                            namespace SERIALIZATION_NAMESPACE##x { typedef ::x CURRENTCLASS; \
static ::SerializableField mInfosSerializableFieldsTemp[]={


//#define SERIALIZE_BASE(CURRENTCLASS,x) SerializableField(QUOTE(x),SIZEOF(CURRENTCLASS,x),OFFSETOF(CURRENTCLASS,x),SerializableBase),
//#define SERIALIZE_ARRAYBASE(CURRENTCLASS,x,C) SerializableField(QUOTE(x),SIZEOFARRAY(CURRENTCLASS,x),OFFSETOF(CURRENTCLASS,x),C,SerializableArrayBase),
#define SERIALIZE_POINTER(x) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),OFFSETOF(CURRENTCLASS,x),OFFSETOFFIELDS(CURRENTCLASS,x),CLASSID(CURRENTCLASS,x),1),

#define SERIALIZE_2POINTER(x,y) SERIALIZE_POINTER(x) SERIALIZE_POINTER(y)
#define SERIALIZE_3POINTER(x,y,z) SERIALIZE_2POINTER(x,y) SERIALIZE_POINTER(z)
#define SERIALIZE_4POINTER(x,y,z,w) SERIALIZE_2POINTER(x,y) SERIALIZE_2POINTER(z,w)

#define SERIALIZE_AGREGAT(x) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),OFFSETOF(CURRENTCLASS,x),OFFSETOFFIELDSAGREGAT(CURRENTCLASS,x),CLASSIDAGREGAT(CURRENTCLASS,x), 1, SerializableAgregat),
#define SERIALIZE_AGREGAT_ARRAY(x,c) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),OFFSETOF(CURRENTCLASS,x),OFFSETOFFIELDSAGREGATARRAY(CURRENTCLASS,x),CLASSIDAGREGATARRAY(CURRENTCLASS,x), c, SerializableAgregat),
#define SERIALIZE_BASECLASS(x) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),(((unsigned int)static_cast <x*>((CURRENTCLASS*)1))-1) , (unsigned int)x::mInfosSerializableFields, x::mClassID, 1, SerializableBaseClass),

#define SERIALIZE_MATRIX(x) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),SIZEOF(CURRENTCLASS,x),OFFSETOF(CURRENTCLASS,x), 1, SB_MATRIX, ""),

#define SERIALIZE_TYPED(x,type)              ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),SIZEOF(CURRENTCLASS,x),OFFSETOF(CURRENTCLASS,x), 1, type, ""),
#define SERIALIZE_TYPED_ENUM(x,type,enumStr) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),SIZEOF(CURRENTCLASS,x),OFFSETOF(CURRENTCLASS,x), 1, type, enumStr),
#define SERIALIZE_TYPED_ARRAY(x,type,count)  ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),SIZEOFARRAY(CURRENTCLASS,x),OFFSETOF(CURRENTCLASS,x), count, type, ""),

#define SERIALIZE_TSTRING(x) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),sizeof(tstring),OFFSETOF(CURRENTCLASS,x), 1, SB_STRING,""),
#define SERIALIZE_TSTRING_ARRAY(x,y) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x),sizeof(tstring),OFFSETOF(CURRENTCLASS,x), y, SB_STRING,""),

#define SERIALIZE_ENUM8(x,y) SERIALIZE_TYPED_ENUM(x,SB_I8,y)
#define SERIALIZE_I8(x)        SERIALIZE_TYPED(x,SB_I8)
#define SERIALIZE_UI8(x)    SERIALIZE_TYPED(x,SB_UI8)
#define SERIALIZE_I16(x)    SERIALIZE_TYPED(x,SB_I16)
#define SERIALIZE_UI16(x)    SERIALIZE_TYPED(x,SB_UI16)
#define SERIALIZE_I32(x)    SERIALIZE_TYPED(x,SB_I32)
#define SERIALIZE_UI32(x)    SERIALIZE_TYPED(x,SB_UI32)
#define SERIALIZE_FLOAT(x)    SERIALIZE_TYPED(x,SB_FLOAT)
#define SERIALIZE_DOUBLE(x) SERIALIZE_TYPED(x,SB_DOUBLE)
#define SERIALIZE_BOOL(x)    SERIALIZE_TYPED(x,SB_BOOL)
#define SERIALIZE_UI32_FLAGS(x,y)    SERIALIZE_TYPED_ENUM(x,SB_UI32,y)


#define SERIALIZE_I8_ARRAY(x,c)        SERIALIZE_TYPED_ARRAY(x,SB_I8,c)
#define SERIALIZE_UI8_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_UI8,c)
#define SERIALIZE_I16_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_I16,c)
#define SERIALIZE_UI16_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_UI16,c)
#define SERIALIZE_I32_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_I32,c)
#define SERIALIZE_UI32_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_UI32,c)
#define SERIALIZE_FLOAT_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_FLOAT,c)
#define SERIALIZE_DOUBLE_ARRAY(x,c) SERIALIZE_TYPED_ARRAY(x,SB_DOUBLE,c)
#define SERIALIZE_BOOL_ARRAY(x,c)    SERIALIZE_TYPED_ARRAY(x,SB_BOOL,c)

#define SERIALIZE_2I8(x,y)        SERIALIZE_TYPED(x,SB_I8)        SERIALIZE_TYPED(y,SB_I8)
#define SERIALIZE_2UI8(x,y)        SERIALIZE_TYPED(x,SB_UI8)        SERIALIZE_TYPED(y,SB_UI8)
#define SERIALIZE_2I16(x,y)        SERIALIZE_TYPED(x,SB_I16)        SERIALIZE_TYPED(y,SB_I16)
#define SERIALIZE_2UI16(x,y)    SERIALIZE_TYPED(x,SB_UI16)        SERIALIZE_TYPED(y,SB_UI16)
#define SERIALIZE_2I32(x,y)        SERIALIZE_TYPED(x,SB_I32)        SERIALIZE_TYPED(y,SB_I32)
#define SERIALIZE_2UI32(x,y)    SERIALIZE_TYPED(x,SB_UI32)        SERIALIZE_TYPED(y,SB_UI32)
#define SERIALIZE_2FLOAT(x,y)    SERIALIZE_TYPED(x,SB_FLOAT)        SERIALIZE_TYPED(y,SB_FLOAT)
#define SERIALIZE_2DOUBLE(x,y)    SERIALIZE_TYPED(x,SB_DOUBLE)    SERIALIZE_TYPED(y,SB_DOUBLE)
#define SERIALIZE_2BOOL(x,y)    SERIALIZE_TYPED(x,SB_BOOL)        SERIALIZE_TYPED(y,SB_BOOL)

#define SERIALIZE_3I8(x,y,z)        SERIALIZE_2I8(x,y)            SERIALIZE_TYPED(z,SB_I8)
#define SERIALIZE_3UI8(x,y,z)        SERIALIZE_2UI8(x,y)            SERIALIZE_TYPED(z,SB_UI8)
#define SERIALIZE_3I16(x,y,z)        SERIALIZE_2I16(x,y)            SERIALIZE_TYPED(z,SB_I16)
#define SERIALIZE_3UI16(x,y,z)        SERIALIZE_2UI16(x,y)        SERIALIZE_TYPED(z,SB_UI16)
#define SERIALIZE_3I32(x,y,z)        SERIALIZE_2I32(x,y)            SERIALIZE_TYPED(z,SB_I32)
#define SERIALIZE_3UI32(x,y,z)        SERIALIZE_2UI32(x,y)        SERIALIZE_TYPED(z,SB_UI32)
#define SERIALIZE_3FLOAT(x,y,z)        SERIALIZE_2FLOAT(x,y)        SERIALIZE_TYPED(z,SB_FLOAT)
#define SERIALIZE_3DOUBLE(x,y,z)    SERIALIZE_2DOUBLE(x,y)        SERIALIZE_TYPED(z,SB_DOUBLE)
#define SERIALIZE_3BOOL(x,y,z)        SERIALIZE_2BOOL(x,y)        SERIALIZE_TYPED(z,SB_BOOL)

#define SERIALIZE_4I8(x,y,z,w)        SERIALIZE_3I8(x,y,z)        SERIALIZE_TYPED(w,SB_I8)
#define SERIALIZE_4UI8(x,y,z,w)        SERIALIZE_3UI8(x,y,z)        SERIALIZE_TYPED(w,SB_UI8)
#define SERIALIZE_4I16(x,y,z,w)        SERIALIZE_3I16(x,y,z)        SERIALIZE_TYPED(w,SB_I16)
#define SERIALIZE_4UI16(x,y,z,w)    SERIALIZE_3UI16(x,y,z)        SERIALIZE_TYPED(w,SB_UI16)
#define SERIALIZE_4I32(x,y,z,w)        SERIALIZE_3I32(x,y,z)        SERIALIZE_TYPED(w,SB_I32)
#define SERIALIZE_4UI32(x,y,z,w)    SERIALIZE_3UI32(x,y,z)        SERIALIZE_TYPED(w,SB_UI32)
#define SERIALIZE_4FLOAT(x,y,z,w)    SERIALIZE_3FLOAT(x,y,z)        SERIALIZE_TYPED(w,SB_FLOAT)
#define SERIALIZE_4DOUBLE(x,y,z,w)    SERIALIZE_3DOUBLE(x,y,z)    SERIALIZE_TYPED(w,SB_DOUBLE)
#define SERIALIZE_4BOOL(x,y,z,w)    SERIALIZE_3BOOL(x,y,z)        SERIALIZE_TYPED(w,SB_BOOL)

#define SERIALIZE_END SerializableField(0,"",0,0,SerializableEnd)
#define END_SERIALIZATION() SERIALIZE_END }; }

#define SERIALIZE_TBASIC_ARRAY(x,y) ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x), sizeof(((((CURRENTCLASS*)1)->x.tab[0]))), OFFSETOF(CURRENTCLASS,x), y, SerializableTArrayBasic),
#define SERIALIZE_TBASIC_LIST(x,y)  ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x), sizeof(((((CURRENTCLASS*)1)->x.first->data))), OFFSETOF(CURRENTCLASS,x), y, SerializableTListBasic),
//sizeof(((((CURRENTCLASS*)1)->x)))-12

#define SERIALIZE_TARRAY_I8(x)        SERIALIZE_TBASIC_ARRAY(x, SB_I8)
#define SERIALIZE_TARRAY_UI8(x)        SERIALIZE_TBASIC_ARRAY(x, SB_UI8)
#define SERIALIZE_TARRAY_I16(x)        SERIALIZE_TBASIC_ARRAY(x, SB_I16)
#define SERIALIZE_TARRAY_UI16(x)    SERIALIZE_TBASIC_ARRAY(x, SB_UI16)
#define SERIALIZE_TARRAY_I32(x)        SERIALIZE_TBASIC_ARRAY(x, SB_I32)
#define SERIALIZE_TARRAY_UI32(x)    SERIALIZE_TBASIC_ARRAY(x, SB_UI32)
#define SERIALIZE_TARRAY_FLOAT(x)    SERIALIZE_TBASIC_ARRAY(x, SB_FLOAT)
#define SERIALIZE_TARRAY_DOUBLE(x)    SERIALIZE_TBASIC_ARRAY(x, SB_DOUBLE)
#define SERIALIZE_TARRAY_BOOL(x)    SERIALIZE_TBASIC_ARRAY(x, SB_BOOL)

#define SERIALIZE_TLIST_I8(x)        SERIALIZE_TBASIC_LIST(x, SB_I8)
#define SERIALIZE_TLIST_UI8(x)        SERIALIZE_TBASIC_LIST(x, SB_UI8)
#define SERIALIZE_TLIST_I16(x)        SERIALIZE_TBASIC_LIST(x, SB_I16)
#define SERIALIZE_TLIST_UI16(x)        SERIALIZE_TBASIC_LIST(x, SB_UI16)
#define SERIALIZE_TLIST_I32(x)        SERIALIZE_TBASIC_LIST(x, SB_I32)
#define SERIALIZE_TLIST_UI32(x)        SERIALIZE_TBASIC_LIST(x, SB_UI32)
#define SERIALIZE_TLIST_FLOAT(x)    SERIALIZE_TBASIC_LIST(x, SB_FLOAT)
#define SERIALIZE_TLIST_DOUBLE(x)    SERIALIZE_TBASIC_LIST(x, SB_DOUBLE)
#define SERIALIZE_TLIST_BOOL(x)        SERIALIZE_TBASIC_LIST(x, SB_BOOL)

// templates of serializable class
#define SERIALIZE_TARRAY(x)            ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x), size_t(&(((((CURRENTCLASS*)1)->x.tab[0].mInfosSerializableFields)))),      OFFSETOF(CURRENTCLASS,x), SerBaseType(((((CURRENTCLASS*)1)->x.tab[0].mClassID))),     SerializableTArray),
#define SERIALIZE_TLIST(x)            ::SerializableField(CURRENTCLASS::mClassID, QUOTE(x), size_t(&(((((CURRENTCLASS*)1)->x.first->data.mInfosSerializableFields)))), OFFSETOF(CURRENTCLASS,x), SerBaseType(((((CURRENTCLASS*)1)->x.first->data.mClassID))),SerializableTList),
/*
#define SERIALIZE_TARRAYPTR // array de pointer
#define SERIALIZE_TLISTPTR

#define SERIALIZE_TBASICARRAY // array de types de bases (float, int,...)
#define SERIALIZE_TBASICLIST

#define SERIALIZE_TARRAY    // templates
#define SERIALIZE_TLIST
*/

//////////////////////////////////////////////////////////////////////
// shorts macro names version
// for lazy and/or memory limited persons like me ;)

//////////////////////////////////////////////////////////////////////
// classes instancing

typedef struct SerializableClassesInstanciation
{
    SerializableClassesInstanciation(TClassID iClassId,
        ZBaseClass *(* iCreateNew)(SerializableField **pFields, int aCount),
        const char *pName,
        size_t sClassSize,
        SerializableField *pFields );

} SerializableClassesInstanciation;



//////////////////////////////////////////////////////////////////////
// pointer serialisation, and keep-track

typedef struct SerializedPointer
{
    SerializedPointer():ptr(NULL),classID(0),newPtr(NULL) { }
    SerializedPointer(void *_ptr, TClassID _classID)
    {
        ptr = _ptr;
        classID = _classID;
    }
    SerializedPointer(void *_ptr, TClassID _classID, void *_newPtr)
    {
        ptr = _ptr;
        classID = _classID;
        newPtr = _newPtr;
    }

    void *ptr;
    TClassID classID;
    void *newPtr;//ptr used for reading
} SerializedPointer;

//////////////////////////////////////////////////////////////////////
// debug/runtime class desc/object member dumping

void DumpSerializationFields(SerializableField *pFields, const char *pName, int depth);
void DumpObjectValuesFn(SerializableField *pFields, const char *className, void *ptr, int depth = -1);

// use those defines:

// dump class description
// parameter: class ex: DumpSerializationInfo(vertex)
#define DumpSerializationInfo(x) DumpSerializationFields(x::mInfosSerializableFields,x::mInfosSerializableName,-1)

// dump object
// parameter: pointer to class instance
#define DumpObjectValues(x) DumpObjectValuesFn(x->mInfosSerializableFields, x->mInfosSerializableName, x, -1)



//////////////////////////////////////////////////////////////////////
// RTTI

bool IsKindOfFunc(TClassID idSrc, TClassID idDst, SerializableField *fieldDst);


// return true if instance x is kind of y
// \parameter pointer to class instance
// \parameter class name without quotes
// both class must be serialisable

//#define IsKindOf(x,y) IsKindOfFunc(x->mClassID, y::mClassID, x->mInfosSerializableFields)

//////////////////////////////////////////////////////////////////////
// Interfaces for users

const char* getSerializableClassName(TClassID aClassID);
SerializableField* getSerializableClassFields(TClassID aClassID);
luaL_Reg **getSerializableClassLuaMethods(TClassID aClassID);
void setSerializableClassLuaMethods(TClassID aClassID, const luaL_Reg *pMet);
const luaL_Reg * getSerializableLuaMethods(TClassID aClassID);
TClassID getClassIDFromClassName(const char *pszClassName);

class TiXmlDocument;
tstring SerializeXMLOut(SerializableField *pFields,void *ptr, const char *className);
void SerializeXMLIn(SerializableField *pFields,void *ptr, const char *className, TiXmlDocument *pDoc);

bool XMLizeOut(ZBaseClass *pClass, const char *szFileName);
bool XMLizeIn(ZBaseClass *pClass, const char *szFileName);

bool XMLizeOut(std::vector<ZBaseClass *>& pClass, const char *szFileName);
bool XMLizeIn(std::vector<ZBaseClass *>& pClass, const char *szFileName);

#endif

