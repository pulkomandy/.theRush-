///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZFunctionSlot.h
// Creation : 12/04/2009
// Author : Cedric Guillemet
// Description : Shameless taken from CEGUI
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

#ifndef ZFUNCTIONSLOT_H__
#define ZFUNCTIONSLOT_H__

#include "targetver.h"
#undef __STRICT_ANSI__
#include <stdlib.h>

class ZSlotFunctorBase
{
public:
    virtual ~ZSlotFunctorBase() {};
    virtual void operator()() = 0;
};


template<typename T>
class ZMemberFunctionSlot  : public ZSlotFunctorBase
{
public:
    //! Member function slot type.
    typedef void(T::*MemberFunctionType)();

    ZMemberFunctionSlot(MemberFunctionType func, T* obj) :
        d_function(func),
        d_object(obj)
    {}

    virtual void operator()()
    {
        (d_object->*d_function)();
    }

private:
    MemberFunctionType d_function;
    T* d_object;
};


class ZSubscriberSlot
{
public:
    template<typename T>
    ZSubscriberSlot(void (T::*function)(), T* obj) :
        d_functor_impl(new ZMemberFunctionSlot<T>(function, obj))
    {}

	ZSubscriberSlot() { d_functor_impl = NULL; }

    void operator()() const
    {
        return (*d_functor_impl)();
    }

	ZSlotFunctorBase* d_functor_impl;
};

#endif
