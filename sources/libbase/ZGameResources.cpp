///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGameResources.cpp
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

#include "stdafx.h"
//#include "../libplatform/ZenFWNode.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
#include "../libplatform/ZSoundFMOD.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZGameResources)
END_SERIALIZATION()

ZBaseClass *ZGameResources::GetClassByName(const char *szName, TClassID clsID)
{
	ZBaseClass *ret = NULL;
	LockCriticalSection(mCS);
	
	if (!clsID)
	{
		gameres_t::const_iterator iter = mGameResources.begin();
		for (; iter != mGameResources.end(); ++iter)
		{
			const char *pbcn = (*iter).second->GetName().c_str();
			if ( !strcmp(pbcn, szName))
                        {
				ret = (*iter).second;
                                break;
                        }
		}
if (!ret)
{
		std::vector<smartptr<ZBaseClass> >::iterator iti = mUnamedRes.begin();
		for (; iti != mUnamedRes.end(); ++iti)
		{
			const char *pbcn = (*iti)->GetName().c_str();
			if ( !strcmp(pbcn, szName))
                        {
				ret = (*iti);
                                break;
                        }
		}
}
	}
	else
	{
		gameres_t::const_iterator iter = mGameResources.begin();
		for (; iter != mGameResources.end(); ++iter)
		{
			const char *pbcn = (*iter).second->GetName().c_str();
			if ( ( clsID == (*iter).second->GetClassID() ) && ( !strcmp(pbcn, szName)) )
			{
                                ret =  (*iter).second;
                                break;
                        }
		}
if (!ret)
{
		std::vector<smartptr<ZBaseClass> >::iterator iti = mUnamedRes.begin();
		for (; iti != mUnamedRes.end(); ++iti)
		{
			ZBaseClass *pbc = (*iti);
			const char *pbcn = pbc->GetName().c_str();
			TClassID pbcclsid = pbc->GetClassID();
			
			if ( ( clsID ==  pbcclsid) && ( !strcmp(pbcn, szName)) )
                        {
				ret = pbc;
                                break;
                        }
		}
}
	}
	UnLockCriticalSection(mCS);
	return ret;
}


void ZGameResources::AddEntry(const gameResEntry_t& pEntry)
{

	tstring tu = pEntry.filename;
	tu.ToUpper();
	if (strstr(tu.c_str(),".DAE"))
	{
		ZBaseClass *pObj = mGLoaderService->CreateInstance(pEntry.filename, this);
		ZTransform* pTr = ((ZMeshInstance*)pObj->QueryInstance())->GetTransform()->GetRootTransform();
		//pTr->AddRef();
		mGameResources.insert(std::make_pair(pEntry.resname, pObj));
		
	}
#if 0
	else if (strstr(tu.c_str(),".FX"))
	{
		/*
		ZBaseClass *pObj = LoadShaderEffect(gameEntries[i].filename);
		*/
		ZBaseClass *pObj = DirectLoad(gameEntries[i].filename);
		pObj->AddRef();
		mGameResources.insert(std::make_pair(gameEntries[i].resname, pObj));
	}
#endif
	else if (strstr(tu.c_str(),".DDS") /*|| strstr(tu.c_str(),".TGA") || strstr(tu.c_str(),".JPG") || strstr(tu.c_str(),".PNG")*/)
	{
		ZTexture *ntex = DirectLoadTex(pEntry.filename);
		ntex->AddRef();
		mGameResources.insert(std::make_pair(pEntry.resname, ntex));

	}
	else if (strstr(tu.c_str(),".WAV"))
	{
		if(GSoundManager)
		{
			ZBaseClass *pObj = mGLoaderService->CreateInstance(pEntry.filename);// GetSound()->Preload(gameEntries[i].filename);
			pObj->AddRef();
			mGameResources.insert(std::make_pair(pEntry.resname, pObj));//, bool aStreamed = false, bool b3D = false)
		}
	}
#if 0
	else if (strstr(tu.c_str(),".PARTICLE"))
	{
		ZBaseClass *pme = mGLoaderService->CreateInstance(pEntry.filename);// GetSound()->Preload(gameEntries[i].filename);

		pme->AddRef();
		mGameResources.insert(std::make_pair(pEntry.resname, pme));//, bool aStreamed = false, bool b3D = false)

	}
#endif
}


void ZGameResources::AddAllPhysicToScene(ZPhysicWorldBullet *pPW)
{
	gameres_t::iterator iter = mGameResources.begin();
	for (;iter != mGameResources.end(); ++iter)
	{
		ZBaseClass* pObj = (*iter).second;

		if (pObj->GetClassID() == ClassIDZTransform)
		{

			ZTransform* ptrans = (ZTransform*)pObj->QueryInstance();
			if (ptrans->GetSpatialEntity() && (ptrans->GetSpatialEntity()->GetClassID() == ClassIDZMeshInstance) )
			{
				ZMeshInstance *pmi = (ZMeshInstance *)ptrans->GetSpatialEntity()->QueryInstance();
				if (pmi->GetMesh()->GetPhysicTriMesh())
				{
					pmi->SetPhysicTriMeshInstance(pPW->AddPhysicModelToWorld(pmi->GetMesh()->GetPhysicTriMesh(), pPW, 1, 255, (tmatrix*)&ptrans->GetWorldMatrix()));
				}
			}
		}
	}
	for (unsigned int i=0;i<mUnamedRes.size();i++)
	{
		if (mUnamedRes[i]->GetClassID() == ClassIDZTransform)
		{
			ZTransform* ptrans = (ZTransform*)(ZTransform*)mUnamedRes[i]->QueryInstance();
			if (ptrans->GetSpatialEntity() && (ptrans->GetSpatialEntity()->GetClassID() == ClassIDZMeshInstance) )
			{
				ZMeshInstance *pmi = (ZMeshInstance *)ptrans->GetSpatialEntity()->QueryInstance();
				if (pmi->GetMesh()->GetPhysicTriMesh())
				{
					pmi->SetPhysicTriMeshInstance(pPW->AddPhysicModelToWorld(pmi->GetMesh()->GetPhysicTriMesh(), pPW, 1, 255, (tmatrix*)&ptrans->GetWorldMatrix()));
				}
			}
		}
	}
}