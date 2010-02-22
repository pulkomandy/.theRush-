///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZZenExporter.cpp
// Creation : 10/07/2009
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

#include "stdafx.h"

/*
#include "ZAnimation.h"
#include "../LibPlatform/libPlatform.h"
#include "../LibRender/VertexCacheHelper.h"
#include "../LibPlatform/Framework/ZenFWFramework.h"
#include "ZTransform.h"
#include <map>
*/


///////////////////////////////////////////////////////////////////////////////////////////////////

class ZZenExporter : public IExporter
{
public:
    DECLARE_SERIALIZABLE
public:
    ZZenExporter()
    {
		mAnimDicoInd = mIADicoInd = mVADicoInd = mMeshDicoInd = 1;
    }

    virtual ~ZZenExporter() {}
    bool Export(const char *szPath, ZGameResources *pGR);

protected:
	ZFile mFile;
	void SaveTransform(ZTransform *ptr);
	std::map<ZMesh* , ZMesh*> mMeshRep;
	std::map<IVertexArray* , IVertexArray*> mVARep;
	std::map<IIndexArray* , IIndexArray*> mIARep;
	std::map<ZAnimation* , ZAnimation*> mAnimRep;
	
	std::map<ZMesh* , unsigned int> mMeshDico;
	int mMeshDicoInd;
	std::map<IVertexArray* , unsigned int> mVADico;
	int mVADicoInd;
	std::map<IIndexArray* , unsigned int> mIADico;
	int mIADicoInd;
	std::map<ZAnimation* , unsigned int> mAnimDico;
	int mAnimDicoInd;
};


class ZZenImporter : public IImporter
{
public:
    DECLARE_SERIALIZABLE
public:
	ZZenImporter()
	{
		mbFirstSet = false;
	}
    /** Load a mesh from disk */
    virtual bool Import(const char *szPath, const char *pBuf, 
		ZMeshInstance *pFirst, bool bKeepVBOInRam = false, ZGameResources *pGR = NULL);

protected:
	void LoadTransform(ZMeshInstance *pFirst, ZTransform *pParent = NULL , ZGameResources *pGR = NULL);
	ZFile mFile;
	std::map<unsigned int , ZMesh*> mMeshRep;
	std::map<unsigned int , IVertexArray*> mVARep;
	std::map<unsigned int , IIndexArray*> mIARep;
	std::map<unsigned int , ZAnimation*> mAnimRep;
	bool mbFirstSet;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZZenExporter)
END_SERIALIZATION()
BEGIN_SERIALIZATION(ZZenImporter)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

/*
-Encode D:\Projects\Zenith\Bin\ZenithDatas\watergrid.dae  D:\Projects\Zenith\Bin\ZenithDatas\watergrid.zen
-Encode ./Prototype/Teams/WipeOutPulse/Feisar/Feisar.DAE ./Prototype/Teams/WipeOutPulse/Feisar/Feisar.zen
-Encode ./Prototype/Worlds/Earth/Decor/windmill.DAE ./Prototype/Worlds/Earth/Decor/windmill.zen
-View ./Prototype/Teams/WipeOutPulse/Feisar/Feisar.DAE
-View ./Prototype/Worlds/Earth/Decor/windmill.DAE
-View ./Prototype/Worlds/Earth/Decor/windmill.ZEN
-Compress ./Prototype/Worlds/Earth/Decor/windmill.ZEN ./Prototype/Worlds/Earth/Decor/windmill.ZEN
*/


void ZZenExporter::SaveTransform(ZTransform *ptr)
{
	// Spatial Entity

	unsigned char SEType = 0; // transform
	if (ptr->GetSpatialEntity())
	{
		TClassID clsid = ptr->GetSpatialEntity()->GetClassID();
		if (clsid == ClassIDZMeshInstance)
		{
			SEType = 1; // ZMeshInstance
		}
		else
		{ 
			// mmmm.....
		}
	}
	mFile << SEType;
	if (SEType == 1)
	{
		ZMeshInstance *pmi = (ZMeshInstance*)ptr->GetSpatialEntity()->QueryInstance();

		unsigned short nbsu = pmi->GetMesh()->GetNbSubMeshes();
		mFile << nbsu;
		
		// mesh
		
		ZMesh *pMesh = pmi->GetMesh();

		bool bFound = (mMeshRep.find(pMesh) != mMeshRep.end());
		if (!bFound)
			mMeshDico[pMesh] = mMeshDicoInd ++;
		int mID = mMeshDico[pMesh]; 
		mFile << mID;
		
		if (!bFound)
		{
			mMeshRep.insert(std::make_pair(pMesh,pMesh));
			// save it
			uint8 prim = pMesh->GetPrimitive();
			mFile << prim;

			ZBoundingVolume & pbv = pMesh->GetBVolume();
			mFile << pbv;
			uint32 aStart, aCount;
			for (uint i=0;i<nbsu;i++)
			{
				pMesh->GetSubMesh(i, &aStart, &aCount);
				mFile << aStart;
				mFile << aCount;
			}
			// vertex array
			IVertexArray *pva = pMesh->GetVertexBuffer();

			bool bFound = (mVARep.find(pva) != mVARep.end());
			if (!bFound)
				mVADico[pva] = mVADicoInd ++;
			mFile << mVADico[pva];

			if (!bFound)
			{

				uint32 vaformat = pva->GetFormat();
				uint32 vasize = pva->GetVertexSize();
				uint32 vacount = pva->GetVertexCount();
				mFile << vaformat;
				mFile << vasize;
				mFile << vacount;
				mFile.Write(pva->Lock(VAL_READONLY), vacount*vasize);
				pva->Unlock();
				mVARep.insert(std::make_pair(pva, pva));
			}
			// index array
			IIndexArray *pia = pMesh->GetIndexBuffer();
			mID = 0;
			
			if (!pia)
			{
				mFile << mID;
			}
			else
			{
				

				bFound = (mIARep.find(pia) != mIARep.end());
				if (!bFound)
					mIADico[pia] = mIADicoInd ++;
				mFile << mIADico[pia];

				if (!bFound)
				{
					uint32 iacount = pia->GetIndexCount();
					mFile << iacount;
					mFile.Write(pia->Lock(VAL_READONLY), iacount*sizeof(unsigned short));
					pia->Unlock();
					mIARep.insert(std::make_pair(pia, pia));
				}
			}


		}
		// material
		for (uint i=0;i<nbsu;i++)
		{
			ZMaterial *pMat = pmi->GetMaterial(i);
			mFile.Write(pMat->getEffect()->GetName());
			const std::vector<smartptr<ZTexture> >& texl = pMat->getTextureArray();
			unsigned short nbt = texl.size();
			mFile << nbt;
			for (uint j=0;j<nbt;j++)
			{
				mFile.Write(texl[j]->GetName());
			}
			unsigned short nbp = pMat->getNumParams();
			mFile << nbp;
			for (uint j=0;j<nbp;j++)
			{
				FFxSetParam *pms = pMat->getParam(j);
				tvector4 pval;
				pms->getVector(pval);
				mFile << pval;
			}
		}
 
	}


	// transform shits

	mFile << ptr->mLocalMatrix;
    mFile << ptr->mWorldMatrix;
	mFile << ptr->mPivot;

	mFile << ptr->mBaseScale;
	mFile << ptr->mBaseTranslation;
	mFile << ptr->mBaseRotation;

	// Animation

	ZAnimation *pAnim = NULL;// FIXME ptr->GetAnimCtrl()->GetAnimation();

	if (pAnim)
	{
		bool bFound = (mAnimRep.find(pAnim) != mAnimRep.end());
		if (!bFound)
			mAnimDico[pAnim] = mAnimDicoInd ++;
		mFile << mAnimDico[pAnim];

		if (!bFound)
		{
			pAnim->Save(mFile);
		}
	}
	else
	{
		int tempi = 0;
		mFile << tempi;
	}


	// childs
	uint nbChilds = ptr->mChilds.size();
	mFile << nbChilds;
	std::list<smartptr<ZTransform> >::iterator iter = ptr->mChilds.begin();
	for (; iter != ptr->mChilds.end(); ++iter)
	{
		SaveTransform( (*iter) );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZZenExporter::Export(const char *szPath, ZGameResources *pGR)
{
	
	mFile.Open(szPath, ZOPEN_WRITE|ZOPEN_BINARY);

	std::vector<ZTransform*> mTransToSer;

	pGR->FillRootTransformList(&mTransToSer);

	unsigned int nbTr = mTransToSer.size();
	mFile << nbTr;

	// Transforms
	for (uint i =0;i<nbTr; i++)
	{
		ZTransform *ptr = mTransToSer[i];
		
		SaveTransform((ZTransform*)ptr->QueryInstance());//mTransToSer[i]->Save(mFile);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZZenImporter::LoadTransform(ZMeshInstance *pFirst, ZTransform *pParent, ZGameResources *pGR)
{
	ZTransform * ptr = NULL;

	unsigned char ttype;
	mFile >> ttype;
	if (ttype == 1)
	{
		ZMeshInstance *pmi;
		if ((!mbFirstSet)&&(pFirst))
		{
			pmi = pFirst;
			mbFirstSet = true;
		}
		else
			pmi = (ZMeshInstance*)_New(ZMeshInstance);

		ptr = (ZTransform*) pmi->GetTransform();

		// Mesh Instance


		unsigned short nbsu;
		mFile >> nbsu;
		
		// mesh
		unsigned int mID;
		mFile >> mID;
		
		std::map<unsigned int, ZMesh*>::iterator iter = mMeshRep.find(mID);
		if (iter != mMeshRep.end())
		{
			pmi->SetMesh( (*iter).second );
		}
		else
		{
			ZMesh *pMesh = (ZMesh*)_New(ZMesh);
			pmi->SetMesh( pMesh );
			mMeshRep.insert(std::make_pair(mID, pMesh));
			// load it
			uint8 prim;
			mFile >> prim;
			pMesh->SetPrimitive(prim);

			ZBoundingVolume & pbv = pMesh->GetBVolume();
			mFile >> pbv;
			uint32 aStart, aCount;
			for (uint i=0;i<nbsu;i++)
			{
				mFile >> aStart;
				mFile >> aCount;
				pMesh->AddSubMesh(aStart, aCount);
			}
			// vertex array
			mFile >> mID;
			std::map<unsigned int, IVertexArray*>::iterator vaiter = mVARep.find(mID);
			if (vaiter != mVARep.end())
			{
				pMesh->SetVertexBuffer( (*vaiter).second);
			}
			else
			{
				uint32 vaformat;
				uint32 vasize;
				uint32 vacount;
				mFile >> vaformat;
				mFile >> vasize;
				mFile >> vacount;
				IVertexArray *pva = GDD->NewVertexArray();
				pva->Init(vaformat, vacount, true);
				mFile.Read(pva->Lock(VAL_WRITE), vacount*vasize);
				pva->Unlock();
				mVARep.insert(std::make_pair(mID, pva));

				pMesh->SetVertexBuffer( pva );
			}
			// index array
			mFile >> mID;
			if (mID)
			{
				std::map<unsigned int, IIndexArray*>::iterator iaiter = mIARep.find(mID);
				if (iaiter != mIARep.end())
				{
					pMesh->SetIndexBuffer( (*iaiter).second);
				}
				else
				{
					IIndexArray *pia = GDD->NewIndexArray();
					uint32 iacount;
					mFile >> iacount;
					pia->Init(iacount);
					mFile.Read(pia->Lock(VAL_WRITE), iacount*sizeof(unsigned short));
					pia->Unlock();
					mIARep.insert(std::make_pair(mID, pia));

					pMesh->SetIndexBuffer( pia );
				}
			}
		}
		// material
		for (uint i=0;i<nbsu;i++)
		{
			ZMaterial *pMat = pmi->GetMaterial(i);
			tstring effectName;
			mFile.Read(effectName);
			pMat->setEffect(DirectLoad(effectName.c_str()));
			unsigned short nbt;
			mFile >> nbt;
			for (uint j=0;j<nbt;j++)
			{
				tstring texname;
				mFile.Read(texname);
				pMat->addTexture(DirectLoadTex(texname));
			}
			unsigned short nbp;
			mFile >> nbp;
			pMat->connectEffect(true, false);
			for (uint j=0;j<nbp;j++)
			{
				FFxSetParam *pms = pMat->getParam(j);
				tvector4 pval;
				mFile >> pval;
				pms->setVector(pval);
			}
		}

		pmi->SetShadowCaster(true);
	}
	else if (ttype == 0)
	{
		ptr = (ZTransform*)_New(ZTransform);
	}

	// Transform

	mFile >> ptr->mLocalMatrix;
    mFile >> ptr->mWorldMatrix;
	mFile >> ptr->mPivot;

	mFile >> ptr->mBaseScale;
	mFile >> ptr->mBaseTranslation;
	mFile >> ptr->mBaseRotation;


	// Animation

	unsigned int animID;
	mFile >> animID;
/* FIXME
	if (animID)
	{
		std::map<unsigned int, ZAnimation*>::iterator aniter = mAnimRep.find(animID);
		if (aniter != mAnimRep.end())
		{
			ptr->GetAnimCtrl()->SetOwnerTransform(ptr);
			ptr->GetAnimCtrl()->Play( (*aniter).second );
		}
		else
		{
			ZAnimation *pAnim = (ZAnimation*)_New(ZAnimation);
			pAnim->Load(mFile);
			ptr->GetAnimCtrl()->SetOwnerTransform(ptr);
			ptr->GetAnimCtrl()->Play( pAnim );
			mAnimRep.insert(std::make_pair(animID, pAnim));

		}
	}
*/

	if (pParent)
		ptr->SetParent(pParent);

	// childs
	uint nbChilds;
	mFile >> nbChilds;
	for (uint i=0;i<nbChilds;i++)
	{
		LoadTransform( pFirst, ptr, pGR );
	}
	if (!pParent)
	{
		ptr->Update();
		if (pGR)
			pGR->AddResource(ptr);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZZenImporter::Import(const char *szPath, const char *pBuf, 
		ZMeshInstance *pFirst, bool bKeepVBOInRam , ZGameResources *pGR)
{

	mFile.OpenMem((tuchar*)pBuf);

	unsigned int nbTr;
	mFile >> nbTr;

	for (uint i =0;i<nbTr; i++)
	{
		LoadTransform(pFirst, NULL, pGR);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
