///////////////////////////////////////////////////////////////////////////////////////////////////
// Azote Studio Engine
// File Name : ZOccluderBox.h
// Creation : 18/08/2001
// Author : Cedric Guillemet
// Description :
// Reliability : 0/10 
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

#ifndef ZOCCLUDERBOX_H__
#define ZOCCLUDERBOX_H__

#include "../libbase/ZSerializator.h"
#include "ZSpatialEntity.h"

// Includes ///////////////////////////////////////////////////////////////////////////////////////

DECLAREZCLASS(ZOccluderBox);
// Constants //////////////////////////////////////////////////////////////////////////////////////

// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZOccluderBox : public ZSpatialEntity 
{
public:
    DECLARE_SERIALIZABLE
public:
    ZOccluderBox()
	{
	}
	virtual ~ZOccluderBox() {}


	virtual ZSpatialEntity * Clone()
    {
        ZOccluderBox *nInst = (ZOccluderBox*)_New(ZOccluderBox);
		
        return nInst;
    }

	virtual void Load(ZFile */*pFile*/) { }
	virtual void Save(ZFile */*pFile*/) { }

protected:


};

int PreprocessOccluders(const tmatrix &invCameraMat);
unsigned int IsSphereOccluded(const tvector4& aSphere);

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
