///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWLoaderNode.cpp
// Creation : 02/05/2009
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

#include "ZenFWLoaderNode.h"
#include "ZenFWDecoderNode.h"
#include "../libbase/ZLogger.h"
#include "..\libplatform\ZSoundFMOD.h"
#include "..\libgame\ZRushTrack.h"
#include "..\libworld\ZMeshInstance.h"

#include <stdio.h>

#ifdef WIN32
#include <io.h>
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWLoaderNode *mGLoaderService;

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWLoaderNode::Run()
{
	ZenFWNode::Run();

	if (!mDataList.empty())
	{
		PROFILER_START(Loader);
		ZBaseClass *mBases[16];
		DataToLoad_t mBuffers[16];
		mProcessingCount ++;
		unsigned int nbEntries = mDataList.ListToArray(mBases, mBuffers, 16);

		// load shits

		for (unsigned int i=0;i<nbEntries;i++)
		{

			LOG("Loading ... (0x%X) %s .", mBases[i], mBases[i]->GetName().c_str());

			mBuffers[i].mBuffer = NULL; // !

			if ( !( (mBases[i]->GetClassID() == ClassIDZRushTrack) && (strstr(mBases[i]->GetName().c_str(),"Default.track")) ) )
			{
				

				FILE *fp = NULL;
				fp = fopen(mBases[i]->GetName().c_str(), "rb");
				if (fp)
				{
	#ifdef WIN32
					int fileno = _fileno(fp);
					int filelen = _filelength(fileno);
	#else
					struct stat bufStat;
					int res = fstat(fileno(fp), &bufStat);
					int filelen = 0;
					if (res == 0)
					{
						filelen = bufStat.st_size;
					}
	#endif
					mBuffers[i].mBuffer = new unsigned char [filelen];
					mBuffers[i].mBufferSize = filelen;
					fread(mBuffers[i].mBuffer, filelen, 1, fp);
					fclose(fp);
					LOG(" OK\n");

					//mBases[i]->SetSizeOnDisk(filelen);
				}
				else
				{
					
					LOG("Error!\n");
				}
			}

		}


		// push to the next thread node
		mGDecoderService->GetDataList().ArrayToList(mBases, mBuffers, nbEntries);
		mProcessingCount --;
		PROFILER_END();
	}
	else
	{
		ZSleep(16); // one frame latency
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZBaseClass* ZenFWLoaderNode::CreateInstance(const char *szName, ZGameResources *pGR)
{
	tstring fname(szName);
	fname.SimplifyPath();

	DataToLoad_t dtlt(fname.c_str(), pGR);

	tstring sext = fname.GetExtension();
	sext.ToUpper();

	if ((sext == ".DDS")||(sext == ".TGA")||(sext == ".AVI"))
	{
		//tstring filen = szName;
		ZTexture *pTex = (ZTexture*)FindInstance(ZTexture, fname.c_str());
		if (pTex)
		{
			return pTex;
		}
		pTex = GDD->NewTexture();//new ZTexture;
		pTex->SetName(fname.c_str());
		mDataList.SingleToList(pTex, dtlt);

		LOG("Defered Loading Tex %s (0x%X)\n", fname.c_str(), pTex);

		return pTex;
	}
	else if (sext == ".DAE")
	{
		ZMeshInstance *pInst = (ZMeshInstance*)_New(ZMeshInstance);
		pInst->SetName(szName);
		//pInst->SetVisible(bAddToScene);
		mDataList.SingleToList(pInst, dtlt);

		LOG("Defered Loading DAE %s (0x%X)\n", fname.c_str(), pInst);
		return pInst;
	}
	else if (sext == ".ZEN")
	{
		ZMeshInstance *pInst = (ZMeshInstance*)_New(ZMeshInstance);
		pInst->SetName(szName);
		mDataList.SingleToList(pInst, dtlt);

		LOG("Defered Loading ZEN %s (0x%X)\n", fname.c_str(), pInst);

		return pInst;
	}
#if 0
	else if (sext == ".PARTICLE")
	{
		tstring filen = szName;
		filen.SimplifyPath();
		IMetaEmitter *pInst = (IMetaEmitter*)FindInstance(ZMetaEmitter, filen.c_str());
		if (pInst)
			return pInst;

		pInst = (IMetaEmitter*)_New(ZMetaEmitter);
		if (pInst)
		{
			pInst->SetName(filen);

			mDataList.SingleToList(pInst, dtlt);
#ifdef DEBUG
			LOG("MetaEmitter (0x%X)\n", pInst);
#endif
			return pInst;
		}
	}
#endif
	else if (sext == ".TRACK")
	{
		ZRushTrack *pInst = (ZRushTrack*)_New(ZRushTrack);
		pInst->SetName(szName);
		mDataList.SingleToList(pInst, dtlt);

		LOG("Defered Loading TRACK %s (0x%X)\n", fname.c_str(), pInst);

		return pInst;
	}

	else if (sext == ".WAV")
	{
		if (GSoundManager)
		{
			tstring filen = szName;
			filen.SimplifyPath();
			ZSoundSample *pInst = (ZSoundSample *)FindInstance(ZSoundSample, filen.c_str());
			if (pInst)
				return pInst;

			const MetaData_t* pmd = mMetaDatas.GetMetaData(GetHash(szName));
			bool b3D = false;
			bool bStream = false;
			if (pmd && (pmd->mType == MetaData_t::AUDIO) )
			{
				b3D = pmd->Audio.is3DSound;
				bStream = pmd->Audio.isStreaming;
			}
			pInst = GSoundManager->NewSoundSample(b3D, bStream);
			if (pInst)
			{
				pInst->SetName(filen);

				mDataList.SingleToList(pInst, dtlt);
#ifdef _DEBUG
				LOG("Wav (0x%X)\n", pInst);
#endif
				return pInst;
			}
		}
	}

        else
        {
        LOG("Extension %s not supported for file : %s\n",sext.c_str(), fname.c_str());
        }
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
