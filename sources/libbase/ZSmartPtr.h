///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZSmartPtr.h
// Creation : 16/06/2007
// Author : Cedric Guillemet
// Description : Reference Counter
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

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#ifndef ZSMART_PTR_H__
#define ZSMART_PTR_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

//#include "PSM_Def.h"

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZRefCounter //: public PSM_BaseClass
{
    private:
        long        mRefCount;

    public:
        // Default constructor.
        ZRefCounter()    { mRefCount = 0; }

        /// Adds one to the internal reference count.
        void AddRef()      { mRefCount++; }

        /// Substracts one from the internal reference count, and
        /// if the count reaches zero, frees the object.
		void  SubRef()
		{
			mRefCount--;
			if (mRefCount<=0)
				delete this;
		}

        long GetCount() { return mRefCount; }
};
#if 0
template <class Type> class h
{
private:
    Type* mPointer;
public:
    h()
    {
        mPointer = NULL;
    }
    ~h()
    {
        Detach();
    }

    h(Type* ptr)
    {
		mPointer = NULL;
        Attach(ptr);
    }

    void Attach(Type* ptr)
    {
		if ((mPointer) && (mPointer != ptr))
			mPointer->SubRef();
        mPointer = ptr;
        if (mPointer)
            mPointer->AddRef();
    }

    void Detach()
    {
        if (mPointer)
        {
            mPointer->SubRef();
        }
        mPointer = NULL;
    }

    void operator = (Type* ptr)
    {
        Attach(ptr);
    }

	h<Type>& operator=(const h<Type>& rVar)
	{

	}
	Type* operator = (const Type *rVar)
	{

		return *this;
	}

template<class Type>
h<Type>& operator=(const h<Type>& rVar)
{

        return *this;

}

    Type* operator->() const
    {
        return mPointer;
    }

	Type*	 ptr() const
	{
		//FASSERT( !mPointer || mPointer->isValid() );
		return mPointer;//const_cast<T*>(mPointer);
	}

	// PJS: Defined to stop gcc confusions with the bool32 cast in STL templates
	//bool	operator<(const FQueryInterface<T>& iOther) const;


			operator Type*() const				{ return const_cast<Type*>(ptr()); }
	Type&		operator*() const				{ return *ptr(); }

};

#endif


template <class Type> class smartptr
{
private:
	Type* mPointer;
public:
	smartptr()
	{
		mPointer = NULL;
	}
	virtual ~smartptr()
	{
		Detach();
	}

	smartptr(Type* ptr)
	{
		mPointer = NULL;
		Attach(ptr);
	}


	smartptr(const smartptr<Type>& ptr)
	{
		mPointer = NULL;
		Attach(ptr);
	}

	void Attach(Type* ptr)
	{
		if (mPointer == ptr)
			return;
		if (mPointer)
			mPointer->SubRef();
		mPointer = ptr;
		if (mPointer)
			mPointer->AddRef();
	}

	void Detach()
	{
		if (mPointer)
		{
			mPointer->SubRef();
		}
		mPointer = NULL;
	}

	void operator = (Type* ptr)
	{
		Attach(ptr);
	}

	Type&		operator*() const
	{
		return *ptr();
	}
	operator Type*() const
	{
		return const_cast<Type*>(ptr());
	}
	Type* operator->() const
	{
		return (Type*)mPointer;
	}

	Type*	 ptr() const
	{
		return mPointer;
	}

	bool operator == (Type* rVal) const
	{
		return (mPointer == rVal);
	}

	bool operator == (const smartptr<Type>& rVal) const
	{
		return (mPointer == rVal.mPointer);
	}

	// new
	smartptr<Type>&	operator=(const smartptr<Type>& rVal)
	{
		Attach( rVal.mPointer );
		return *this;
	}
};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
