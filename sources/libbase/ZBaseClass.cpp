///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : PSM_BaseClass.h
// Creation : 02/11/2006
// Author : Cedric Guillemet
// Description :
// Reliability : 5/10
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
// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ZBaseClass.h"

const tstring ZBaseClass::mUnnamed("Unnamed");

ZBaseClass::ZBaseClass()
{
    mNextInstance = NULL;
    mPrevInstance = NULL;
	mRefCount = 0;
}

ZBaseClass::~ZBaseClass()
{
	gSerializableClasses[mZBClassID].RemoveInstance(this);
}

void ZBaseClass::DumpInstance()
{
    DumpObjectValuesFn(this->getInfosFields(), this->GetZClassName(), this, -1);
}
