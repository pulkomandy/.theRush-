///////////////////////////////////////////////////////////////////////////////////////////////////
// Azote Studio Engine
// File Name : ZTrigger.h
// Creation : 17/11/2009
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

#ifndef ZTRIGGER_H__
#define ZTRIGGER_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

DECLAREZCLASS(ZTrigger);
// Constants //////////////////////////////////////////////////////////////////////////////////////

// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZTrigger : public ZSpatialEntity 
{
public:
    DECLARE_SERIALIZABLE
public:
    ZTrigger()
	{
		mTriggerType = TRIGGER_SPHERE;
		mFilterGroup = 1;
		mFilterMask = 0xFFFFFFFF;
		mbActive = true;
		mUserPointer = NULL;

	}
	virtual ~ZTrigger() 
	{
#ifdef _DEBUG
		LOG("Trigger 0x%x Destroyed\n", this);
#endif
	}


	virtual ZSpatialEntity * Clone()
    {
        ZTrigger *nInst = (ZTrigger*)_New(ZTrigger);

		nInst->mFilterGroup = mFilterGroup;
		nInst->mFilterMask = mFilterMask;
		nInst->mTriggerType = mTriggerType;

        return nInst;
    }


	virtual void Load(ZFile *pFile) { }
	virtual void Save(ZFile *pFile) { }

	unsigned int mFilterGroup; // groups it belongs to
	unsigned int mFilterMask; // groups it can trigger with

	enum TRIGGER_TYPE
	{
		TRIGGER_SPHERE,
		TRIGGER_BOX
	};

	TRIGGER_TYPE mTriggerType;

	ZBoundingVolume & GetOriginVolume() { return mOriginBVolume; }
	virtual void SetActive(bool bActive) { mbActive = bActive; }
	virtual bool IsActive() const { return mbActive; }

	void SetUserPointer(void *pUserPointer) { mUserPointer = pUserPointer; };
	void *GetUserPointer() const { return mUserPointer; }

protected:
	ZBoundingVolume mOriginBVolume;
	bool mbActive;
	void *mUserPointer;

};


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
