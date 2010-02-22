///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWVRAMService.cpp
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
//#include "ZenFWFramework.h"
#include "../librender/librender.h"
#include "../librender/GLee.h"
///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWVRAMService *VRAMService;


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::ProcessDatas()
{
	PROFILER_START(VRAM_Upload);
	if ( (!mDataList.empty()) || (!mIndexArrayJobList.empty()) ||
		(!mVertexArrayJobList.empty()) || (!mTextureUpdateJobList.empty()) ||
		(!mVARelaseJobList.empty()) )
	{
		mProcessingCount ++;
		if (!mDataList.empty())
		{
			ZBaseClass *mEntry[16];
			DataToLoad_t mBuffers[16];
			unsigned int nbEntries = mDataList.ListToArray((ZBaseClass **)mEntry, mBuffers, 16);

			// Decode shits
			// decompress if needed


			for (unsigned int nbe = 0;nbe<nbEntries;nbe++)
			{
				if (!mBuffers[nbe].mBuffer) continue;
				/*
#ifdef DEBUG
				char tmps[512];
				if (!mEntry[nbe]->GetName().c_str())
				{
					sprintf_s(tmps, 512, "Uploading ... (0x%X) %s \n", mEntry[nbe], "Unamed");
				}
				else
				{
					sprintf_s(tmps, 512, "Uploading ... (0x%X) %s \n", mEntry[nbe], mEntry[nbe]->GetName().c_str());
				}
				OutputDebugString(tmps);
#endif
				*/

				bool bDeleteMemory = true;

				if (mEntry[nbe]->GetClassID() == ClassIDZTexture)
				{
					ZTexture *ptex = (ZTexture*)mEntry[nbe]->QueryInstance();
					if (ptex->GetType() == TEXTURE_DDS)
					{
						ptex->LoadDDSFromMemory(mBuffers[nbe].mBuffer, mBuffers[nbe].mBufferSize);
					}
					/*
					else if (ptex->GetType() == TEXTURE_2D)
					{
						ptex->LoadRawFromMemory(mBuffers[nbe].mBuffer);
					}
					*/
					else if (ptex->GetType() == TEXTURE_XVID)
					{
						ptex->LoadAVIFromMemory(mBuffers[nbe].mBuffer);
						bDeleteMemory = false;
					}
				}

				if (bDeleteMemory)
					delete [] mBuffers[nbe].mBuffer;
			}
		}

		// Index Array job
		if (!mIndexArrayJobList.empty())
		{
			IIndexArray *mEntry[16];
			IndexArrayJob_t mIAJob[16];
			unsigned int nbEntries = mIndexArrayJobList.ListToArray((ZBaseClass **)mEntry, mIAJob, 16);

			for (uint i=0;i<nbEntries;i++)
			{
				const IndexArrayJob_t& iaj = mIAJob[i];
				IIndexArray *iia = mEntry[i];
				if (iaj.mbNeedInit)
					iia->Init(iaj.mSizeInBytes>>1);

				if (iaj.mpMem)
				{
					memcpy(iia->Lock(VAL_WRITE), iaj.mpMem, iaj.mSizeInBytes);
					iia->Unlock();
				}

				if (iaj.mbFreeMemoryAfterUpload)
					free(iaj.mpMem);

				iia->SubRef();

			}
		}

		// Vertex Array Job
		if (!mVertexArrayJobList.empty())
		{
			IVertexArray *mEntry[16];
			VertexArrayJob_t mVAJob[16];
			unsigned int nbEntries = mVertexArrayJobList.ListToArray((ZBaseClass **)mEntry, mVAJob, 16);

			for (uint i=0;i<nbEntries;i++)
			{
				const VertexArrayJob_t& vaj = mVAJob[i];
				IVertexArray *iva = mEntry[i];
				if (!vaj.mbForParticles)
				{
					if (iva->GetRefCount()> 1)
					{
						if (vaj.mbNeedInit)
							iva->Init(vaj.mVTFormat, vaj.mNbVT, true);

						if (vaj.mpMem)
						{
							void *pVADestbytes = iva->Lock(VAL_WRITE);
							if (pVADestbytes)
								memcpy(pVADestbytes, vaj.mpMem, iva->GetVertexCount()*iva->GetVertexSize());
							iva->Unlock();
						}

						if (vaj.mbComputeBSphere)
						{
							vaj.mpMesh->ComputeBSphere();
						}
					}

					if (vaj.mbFreeMemoryAfterUpload)
						free(vaj.mpMem);
				}
				else
				{
					// for particles
					if (vaj.mNbVT)
					{
						if (iva->GetRefCount()> 1)
						{
							memcpy(iva->Lock(VAL_WRITE), vaj.mpMem, vaj.mNbVT*iva->GetVertexSize());
							iva->Unlock();
						}
						if (vaj.mpMeshInstance->GetRefCount() > 1)
						{
							vaj.mpMeshInstance->GetMesh()->SetSubMesh(0, 0, int(vaj.mNbVT*1.5));
							vaj.mpMeshInstance->SetVisible(true);

						}
					}
					else
					{
						if (vaj.mpMeshInstance->GetRefCount() > 1)
							vaj.mpMeshInstance->SetVisible(false);
					}
					vaj.mpMeshInstance->SubRef();
					vaj.mpMesh->SubRef();


				}
				iva->SubRef();
			}
		}

		// Vertex array free
		if (!mVARelaseJobList.empty())
		{
			ZBaseClass *mEntry[16];
			void* mVA[16];
			unsigned int nbEntries = mVARelaseJobList.ListToArray((ZBaseClass **)mEntry, mVA, 16);

#ifdef WIN32
			if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
			{
				for (uint i=0;i<nbEntries;i++)
					((IDirect3DVertexBuffer9*)mVA[i])->Release();
			}
			else
#endif
			if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL)
			{
				for (uint i=0;i<nbEntries;i++)
					glDeleteBuffers(1, (GLuint*)mVA[i]);
			}
		}

		// Index Array free
		if (!mIARelaseJobList.empty())
		{
			ZBaseClass *mEntry[16];
			void* mIA[16];
			unsigned int nbEntries = mIARelaseJobList.ListToArray((ZBaseClass **)mEntry, mIA, 16);

#ifdef WIN32
			if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
			{
				for (uint i=0;i<nbEntries;i++)
					((IDirect3DIndexBuffer9*)mIA[i])->Release();
			}
			else
#endif
			if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL)
			{
				for (uint i=0;i<nbEntries;i++)
					glDeleteBuffers(1, (GLuint*)mIA[i]);
			}
			
		}
		// shaders
		/*
		if (!mShaderJobList.empty())
		{
			IFx *mEntry[16];
			ShaderArrayJob_t mSHJob[16];
			unsigned int nbEntries = mShaderJobList.ListToArray((ZBaseClass **)mEntry, mSHJob, 16);

			for (uint i =0;i<nbEntries;i++)
			{
				//mEntry[i]->imp>LoadShaderEffect(mSHJob[i].mEffectFileName.c_str());
				DirectLoad(mEntry[i], mSHJob[i].mEffectFileName.c_str());
			}

		}
		*/

		// texture update
		if (!mTextureUpdateJobList.empty())
		{
			ZTexture *mEntry[16];
			TextureUpdateJob_t mtuJob[16];
			unsigned int nbEntries = mTextureUpdateJobList.ListToArray((ZBaseClass **)mEntry, mtuJob, 16);

			for (uint i =0;i<nbEntries;i++)
			{

#ifdef WIN32
				ZTexture *pTex = mtuJob[i].mTexture;
				if (GDD->GetClassID() == ClassIDZDisplayDeviceDX9)
				{
					D3DLOCKED_RECT mLockedRect;
					if ((IDirect3DTexture9 *)pTex->GetPlatformHandle())
					{

						((IDirect3DTexture9 *)pTex->GetPlatformHandle())->LockRect(0, &mLockedRect, NULL, 0);
						memcpy(mLockedRect.pBits,
							mtuJob[i].mpBits,
							mtuJob[i].mHeight * mtuJob[i].mWidth * mtuJob[i].mBpp);
						((IDirect3DTexture9 *)pTex->GetPlatformHandle())->UnlockRect(0);

					}
				}
				else
#endif
				if (GDD->GetClassID() == ClassIDZDisplayDeviceOGL)
				{
				}


			}

		}


		// DONE!!!!
		mProcessingCount --;
	}
	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::AddIndexArrayJob(ZMesh *pMesh, IIndexArray* pIA, bool bInit,
									   void *pBuffer, int iSizeInBytes, bool bDeleteAfterUpload)
{
	pIA->AddRef();

	IndexArrayJob_t entry;
	entry.mbFreeMemoryAfterUpload = bDeleteAfterUpload;
	entry.mbNeedInit = bInit;
	entry.mpMem = pBuffer;
	entry.mSizeInBytes = iSizeInBytes;
	entry.mpMesh = pMesh;

	ZBaseClass *pt = pIA;

	mIndexArrayJobList.ArrayToList(&pt, &entry, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::AddVertexArrayJob(ZMesh *pMesh, IVertexArray* pVA, bool bInit, void *pBuffer,
										int iNbVertices, bool bDeleteAfterUpload,
										unsigned int aVertexFormat, bool bComputeBSphere)
{

	if ((!iNbVertices)&&(!aVertexFormat))
#ifdef _DEBUG
	{
		OutputDebugStringA("Bad vertex job\n");
	}
#else
		return;
#endif

	pVA->AddRef();

	VertexArrayJob_t entry;
	entry.mbForParticles = false;
	entry.mbFreeMemoryAfterUpload = bDeleteAfterUpload;
	entry.mbNeedInit = bInit;
	entry.mpMem = pBuffer;
	entry.mNbVT = iNbVertices;
	entry.mVTFormat = aVertexFormat;
	entry.mbComputeBSphere = bComputeBSphere;
	entry.mpMesh = pMesh;
	ZBaseClass *pt = pVA;



	mVertexArrayJobList.ArrayToList(&pt, &entry, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::AddVertexArrayJobForParticles(ZMeshInstance *pMI, IVertexArray* pVA,
													 void *pBuffer, int iNbVertices)
{
	pVA->AddRef();

	VertexArrayJob_t entry;
	entry.mbForParticles = true;
	entry.mbFreeMemoryAfterUpload = false;
	entry.mbNeedInit = false;
	entry.mpMem = pBuffer;
	entry.mNbVT = iNbVertices;
	entry.mVTFormat = 0;
	entry.mbComputeBSphere = false;
	entry.mpMesh = pMI->GetMesh();
	entry.mpMeshInstance = pMI;
	ZBaseClass *pt = pVA;


	entry.mpMesh->AddRef();
	entry.mpMeshInstance->AddRef();

	mVertexArrayJobList.ArrayToList(&pt, &entry, 1);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::AddVertexArrayReleaseJob(void *pAPITex)
{
	ZBaseClass *pt = NULL;
	void* vttr = pAPITex;
	mVARelaseJobList.ArrayToList(&pt, &vttr, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::AddIndexArrayReleaseJob(void *pAPITex)
{
	ZBaseClass *pt = NULL;
	void* vttr = pAPITex;
	mIARelaseJobList.ArrayToList(&pt, &vttr, 1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZenFWVRAMService::AddShader(IFx *pEffect, const char *szName)
{
	ShaderArrayJob_t sj;
	sj.mEffectFileName = szName;
	sj.mpEffect = pEffect;

	ZBaseClass *pt = pEffect;
	mShaderJobList.ArrayToList(&pt, &sj, 1);

}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::AddTextureUpdate(ZTexture *pTexture, char *pDatas, int sx, int sy, int bpp)
{
	TextureUpdateJob_t tuj;

	tuj.mTexture = pTexture;
	tuj.mpBits = pDatas;
	tuj.mWidth = sx;
	tuj.mHeight = sy;
	tuj.mBpp = bpp;

	mTextureUpdateJobList.ArrayToList((ZBaseClass**)&pTexture, &tuj, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWVRAMService::ClearJobs()
{
	mTextureUpdateJobList.Clear();
	mShaderJobList.Clear();
	mVertexArrayJobList.Clear();
	mIndexArrayJobList.Clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
