///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZBaseClass.h
// Creation : 02/11/2006
// Author : Cedric Guillemet
// Description :
// Reliability : 
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

#ifndef ZBASECLASS_H__
#define ZBASECLASS_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////


//typedef unsigned short TClassID; //65535 classes/types shoud be enough
struct SerializableField;


// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZBaseClass
{
public:
    ZBaseClass();
    virtual ~ZBaseClass();

    virtual const tstring& GetName() const 
	{ 
		return mName; 
	}
    virtual void SetName(const tstring& aName) { mName = aName;}
    static const tstring mUnnamed;
    virtual TClassID GetClassID() = 0;

    virtual tulong GetMemoryUsed() const { return 0;}

    /// Adds one to the internal reference count.
    void AddRef()      
	{ 
		mRefCount++; 
	}

    /// Substracts one from the internal reference count, and 
    /// if the count reaches zero, frees the object.
    void  SubRef() 
	{ 
		mRefCount--; 
		if (mRefCount<=0)
			delete this;
	}

    tlong GetRefCount() const 
	{ 
		return mRefCount; 
	}

    // dump instance info to LOG
    void DumpInstance();
    virtual SerializableField* getInfosFields() = 0;
    virtual const char* GetZClassName() = 0;


	virtual void SetNextInstance(ZBaseClass *pNext) 
	{ 
		mNextInstance = pNext; 
	}
	virtual void SetPrevInstance(ZBaseClass *pPrev) 
	{ 
		mPrevInstance = pPrev; 
	}
	virtual ZBaseClass *GetNextInstance() 
	{ 
		return mNextInstance; 
	}
	virtual ZBaseClass *GetPrevInstance() 
	{ 
		return mPrevInstance; 
	}

	virtual void *QueryInstance() { return NULL; }
	virtual void *GetNext() { return this->mNextInstance; }
	ZBaseClass* mNextInstance;
    ZBaseClass* mPrevInstance;
	void SetRootClassID(TClassID clsID) 
	{ 
		mZBClassID = clsID; 
	}


protected:

    tstring			mName;
    tlong			mRefCount;
	TClassID		mZBClassID;

};
// Inlines ////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
