///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWDecoderNode.cpp
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

#include "stdafx.h"
#include <math.h>
#include "zlib.h"
#include "../libworld/libworld.h"
#include "../libgame/libgame.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWDecoderNode *mGDecoderService;

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWDecoderNode::Run()
{
	ZenFWNode::Run();

	if (!mDataList.empty())
	{
		PROFILER_START(Decoder);
		//float timeRef = gTimer.GetTime();
		mProcessingCount ++;
		//do
		{
		ZBaseClass *mBases[16];
		DataToLoad_t mBuffers[16];
		unsigned int nbEntries = mDataList.ListToArray(mBases, mBuffers, 1);

		// Decode shits


		for (unsigned int i=0;i<nbEntries;i++)
		{
			/*
#ifdef DEBUG
			char tmps[512];
			sprintf_s(tmps, 512, "Decoding ... (0x%X) %s \n", mBases[i], mBases[i]->GetName().c_str());
			OutputDebugString(tmps);
#endif
			*/
			// decompress if needed
			unsigned char zenheader[]={'Z','E','N','C'};
			if ((mBuffers[i].mBuffer)&&(!memcmp(mBuffers[i].mBuffer, zenheader, 4)))
			{

				unsigned long destLen = ((unsigned int*)mBuffers[i].mBuffer)[1];
				unsigned long compressedLen = mBuffers[i].mBufferSize-8;

				unsigned char *uncompressedBuffer = new unsigned char[destLen];

				int res = uncompress OF((uncompressedBuffer,
					&destLen,
					mBuffers[i].mBuffer+8,
					compressedLen));
				if (res == Z_OK)
				{
					// parse header
					delete [] mBuffers[i].mBuffer;
					mBuffers[i].mBuffer = uncompressedBuffer;
					//mBases[i]->SetSizeOnDisk(destLen);
				}
			}
			// type of file
			tstring sext = mBases[i]->GetName().GetExtension();
			sext.ToUpper();
			if (mBases[i]->GetClassID() == ClassIDZTexture)
			{
				if (sext == ".DDS")
				{
					((ZTexture*)mBases[i]->QueryInstance())->SetType(TEXTURE_DDS);
				}
				/*
				else if (sext == ".TGA")
				{
					((ZTexture*)mBases[i])->SetType(TEXTURE_2D);
					uint8 *data = ((ZTexture*)mBases[i])->DecodeTGAFromMemory(mBuffers[i].mBuffer);
					delete [] mBuffers[i].mBuffer;
					mBuffers[i].mBuffer = data;
				}
				*/
				else if (sext == ".AVI")
				{
					((ZTexture*)mBases[i]->QueryInstance())->SetType(TEXTURE_XVID);
					/*
					uint8 *data = ((ZTexture*)mBases[i])->DecodeTGAFromMemory(mBuffers[i]+4);
					delete [] mBuffers[i];
					mBuffers[i] = data;
					*/
				}
			}
			else if (mBases[i]->GetClassID() == ClassIDZMeshInstance)
			{
				if (sext == ".DAE")
				{
					tstring fname(mBases[i]->GetName());

					smartptr<IImporter> mColladaImport = (IImporter*)_New(ZImporterCollada);

					mColladaImport->Import(fname.GetPath(), (const char*)mBuffers[i].mBuffer, (ZMeshInstance*)mBases[i]->QueryInstance(), mBuffers[i].mResource, mBuffers[i].mResource);
					ZMeshInstance *pInst = (ZMeshInstance*)mBases[i]->QueryInstance();

					delete [] mBuffers[i].mBuffer;
					mBuffers[i].mBuffer = NULL;
				}
				else if (sext == ".ZEN")
				{
					tstring fname(mBases[i]->GetName());

					smartptr<IImporter> mZenImport = (IImporter*)_New(ZZenImporter);

					mZenImport->Import(fname.GetPath(), (const char*)mBuffers[i].mBuffer, (ZMeshInstance*)mBases[i]->QueryInstance(), mBuffers[i].mResource, mBuffers[i].mResource);
					ZMeshInstance *pInst = (ZMeshInstance*)mBases[i]->QueryInstance();

					delete [] mBuffers[i].mBuffer;
					mBuffers[i].mBuffer = NULL;
				}
			}

			else if (mBases[i]->GetClassID() == ClassIDZRushTrack)
			{
				ZFile mFile;
				mFile.OpenMem(mBuffers[i].mBuffer, mBuffers[i].mBufferSize);
				GTrack->Load(mFile);
			}
			else if (mBases[i]->GetClassID() == ClassIDZSoundSample)
			{
				((ZSoundSample*)mBases[i])->Preload( mBuffers[i].mBuffer, mBuffers[i].mBufferSize );
				delete [] mBuffers[i].mBuffer;
				mBuffers[i].mBuffer = NULL;
			}
#if 0			
			else if (mBases[i]->GetClassID() == ClassIDZMetaEmitter)
			{
				ZFile mFile;
				mFile.OpenMem(mBuffers[i].mBuffer, mBases[i]->GetSizeOnDisk());
				((IMetaEmitter*)mBases[i])->Load(&mFile);

				delete [] mBuffers[i].mBuffer;
				mBuffers[i].mBuffer = NULL;
			}
#endif

		}

		// push to the next node

		if (VRAMService)
			VRAMService->GetDataList().ArrayToList(mBases, mBuffers, nbEntries);
		/*
		if (mOutputThread)
			mOutputThread->GetDataList().ArrayToList(mBases, (void**)mBuffers, nbEntries);
*/
		}
		//while ((gTimer.GetTime() - timeRef)<0.010);
		mProcessingCount --;

		PROFILER_END();
		//Sleep(1);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
