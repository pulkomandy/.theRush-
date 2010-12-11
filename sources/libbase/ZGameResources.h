///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGameResources.h
// Creation : 21/08/2009
// Author : Cedric Guillemet
// Description : Gestion de contenu
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

#ifndef ZGAMERESOURCES_H__
#define ZGAMERESOURCES_H__

#include "../librender/librender.h"
#include "../libworld/libworld.h"
#include "..\libworld\ZTransform.h"
#include "..\libworld\ZPhysicWorldBullet.h"
#include "..\libworld\ZMeshInstance.h"
#include "..\libbase\ZSerializator.h"

DECLAREZCLASS(ZGameResources);

typedef struct gameResEntry_t
{
	gameResEntry_t(const char *_filename, const char *_resname)
	{
		filename = _filename;
		resname = _resname;
	}
	const char *filename;
	const char *resname;
}gameResEntry_t;



class ZGameResources : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE

public:
	ZGameResources()
	{
		mCS = NewCriticalSection();
	}
	virtual ~ZGameResources()
	{
		Clear();
		DestroyCriticalSection(mCS);
	}

	virtual void Clear()
	{
		RemoveAllFromScene();

		LockCriticalSection(mCS);

		gameres_t::iterator iter = mGameResources.begin();
		for (;iter != mGameResources.end(); ++iter)
		{
			ZBaseClass* pObj = (*iter).second;
			if (pObj->GetClassID() != ClassIDZMeshInstance)
				pObj->SubRef();
		}
		mGameResources.clear();
		mUnamedRes.clear();

		UnLockCriticalSection(mCS);
	}
	virtual void AddAllToScene()
	{
		gameres_t::iterator iter = mGameResources.begin();
		for (;iter != mGameResources.end(); ++iter)
		{
			ZBaseClass* pObj = (*iter).second;

			if (pObj->GetClassID() == ClassIDZTransform)
			{
				ZTransform* ptrans = (ZTransform*)pObj->QueryInstance();
				if (!ptrans->GetParent())
				{
					ptrans->AddToScene(GScene);
					ptrans->Update();
				}
			}
		}
		for (unsigned int i=0;i<mUnamedRes.size();i++)
		{
			if (mUnamedRes[i]->GetClassID() == ClassIDZTransform)
			{
				ZTransform* ptrans = (ZTransform*)(ZTransform*)mUnamedRes[i]->QueryInstance();
				if (!ptrans->GetParent())
				{
					ptrans->AddToScene(GScene);
					ptrans->Update();
				}
			}
		}
	}

	virtual void AddAllPhysicToScene(ZPhysicWorldBullet *pPW);

	virtual void RemoveAllFromScene()
	{
		gameres_t::iterator iter = mGameResources.begin();
		for (;iter != mGameResources.end(); ++iter)
		{
			ZBaseClass* pObj = (*iter).second;

			if (pObj->GetClassID() == ClassIDZTransform)
			{
				ZTransform* ptrans = (ZTransform*)pObj->QueryInstance();
				if (!ptrans->GetParent())
					ptrans->RemoveFromScene();
			}
		}
		for (unsigned int i=0;i<mUnamedRes.size();i++)
			((ZTransform*)mUnamedRes[i]->QueryInstance())->RemoveFromScene();
	}

	virtual void AddResource(const char *szFileName)
	{
		LockCriticalSection(mCS);
		gameResEntry_t entry(szFileName, "");
		AddEntry(entry);
		UnLockCriticalSection(mCS);
	}

	virtual void AddResource(ZBaseClass *pObj, const char *szResourceName = NULL)
	{
		LockCriticalSection(mCS);
		
		if (szResourceName)
		{
			mGameResources.insert(std::make_pair(szResourceName, pObj));
			pObj->AddRef();
		}
		else
			mUnamedRes.push_back(pObj);

		UnLockCriticalSection(mCS);
	}
	virtual void AddResource(const char *szFileName, const char *szResourceName)
	{
		LockCriticalSection(mCS);
		gameResEntry_t entry(szFileName, szResourceName);
		AddEntry(entry);
		UnLockCriticalSection(mCS);
	}
	virtual void AddResourceList(gameResEntry_t* entries, int aEntriesCount)
	{
		LockCriticalSection(mCS);
		for (int i=0;i<aEntriesCount;i++)
			AddEntry(entries[i]);
		UnLockCriticalSection(mCS);
	}
	virtual void AddResourceListNULLTerminated(gameResEntry_t* entries)
	{
		LockCriticalSection(mCS);
		int i=0;
		while (entries[i].filename && entries[i].resname)
		{
			AddEntry(entries[i]);
			i++;
		}
		UnLockCriticalSection(mCS);
	}
	virtual ZBaseClass *GetResource(const char *szName)
	{
		ZBaseClass *ret = NULL;
		LockCriticalSection(mCS);
		
		gameres_t::const_iterator iter = mGameResources.find(szName);
		if (iter != mGameResources.end())
			ret = (*iter).second;
		UnLockCriticalSection(mCS);
		return ret;
	}

	virtual ZBaseClass *GetClassByName(const char *szName, TClassID clsID = 0);

	virtual void FillRootTransformList(std::vector<ZTransform*> *pList)
	{
		gameres_t::iterator iter = mGameResources.begin();
		for (;iter != mGameResources.end(); ++iter)
		{
			ZBaseClass* pObj = (*iter).second;

			if (pObj->GetClassID() == ClassIDZTransform)
			{
				ZTransform* ptrans = (ZTransform*)pObj->QueryInstance();
				if (!ptrans->GetParent())
				{
					pList->push_back(ptrans);
				}
			}
		}
		for (unsigned int i=0;i<mUnamedRes.size();i++)
		{
			if (mUnamedRes[i]->GetClassID() == ClassIDZTransform)
			{
				ZTransform* ptrans = (ZTransform*)(ZTransform*)mUnamedRes[i]->QueryInstance();
				if (!ptrans->GetParent())
				{
					pList->push_back(ptrans);
				}
			}
		}
	}

	virtual bool Empty()
	{
		return (mGameResources.empty()&&mUnamedRes.empty());
	}
	virtual void Consolidate()
	{
		gameres_t::iterator iter = mGameResources.begin();
		for (;iter != mGameResources.end(); ++iter)
		{
			ZBaseClass* pObj = (*iter).second;

			if (pObj->GetClassID() == ClassIDZTransform)
			{
				ZTransform *pitr = (ZTransform*)pObj->QueryInstance();
				ZTransform *nitr = pitr->GetRootTransform();
				nitr->SetExternalRef( (*iter).first.c_str() );
				if (pitr != nitr)
				{
					pitr->SetExternalRef("");
					(*iter).second = nitr;
				}
			}
		}
	}

	const std::vector<smartptr<ZBaseClass> >& GetUnamedRes() const { return mUnamedRes; }
protected:
	typedef std::map<tstring, ZBaseClass*> gameres_t;
	gameres_t mGameResources;
	std::vector<smartptr<ZBaseClass> > mUnamedRes;
	ZCriticalSection_t* mCS;

	void AddEntry(const gameResEntry_t& pEntry);
};


template <typename T> class ZDatasProcessor
{
public:
	ZDatasProcessor()
	{
		mOutputThread = NULL;
	}
	virtual ~ZDatasProcessor()
	{
	}

	void SetOutputThread(ZDatasProcessor *pThread) { mOutputThread = pThread; }
	virtual ZBaseClass* CreateInstance(void */*pParam*/) { return NULL; }

	virtual void ProcessDatas() = 0;

	ZDataResource<T>& GetDataList() { return mDataList; }
	virtual unsigned int GetProcessingCount() = 0;
protected:

	ZDataResource<T> mDataList;
	ZDatasProcessor *mOutputThread;
};

#endif
