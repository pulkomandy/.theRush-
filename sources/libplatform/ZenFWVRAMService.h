///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWVRAMService.h
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



#ifndef ZENFWVRAMSERVICE_H__
#define ZENFWVRAMSERVICE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZenFWManager.h"
#include "..\libbase\ZGameResources.h"

class ZenFWVRAMService;
extern ZenFWVRAMService *VRAMService;

///////////////////////////////////////////////////////////////////////////////////////////////////


class ZenFWVRAMService : public ZDatasProcessor<DataToLoad_t>
{
public:
	ZenFWVRAMService() { mProcessingCount = 0; VRAMService = this;}
	virtual ~ZenFWVRAMService()	{}

	virtual void ProcessDatas();
	virtual unsigned int  GetProcessingCount() { return mProcessingCount;}
	ZDataResource<DataToLoad_t>& GetDataList() { return mDataList; }


	virtual void AddIndexArrayJob(ZMesh *pMesh, IIndexArray* pIA, bool bInit, void *pBuffer, int iSizeInBytes, bool bDeleteAfterUpload);
	virtual void AddVertexArrayJob(ZMesh *pMesh, IVertexArray* pVA, bool bInit, void *pBuffer, int iNbVertices, bool bDeleteAfterUpload, unsigned int aVertexFormat, bool bComputeBSphere);
	virtual void AddVertexArrayJobForParticles(ZMeshInstance *pMI, IVertexArray* pVA, void *pBuffer, int iNbVertices);
	virtual void AddVertexArrayReleaseJob(void *pAPITex);
	virtual void AddIndexArrayReleaseJob(void *pAPITex);
	//virtual void AddShader(IFx *pEffect, const char *szName);
	virtual void AddTextureUpdate(ZTexture *pTexture, char *pDatas, int sx, int sy, int bpp);

	virtual void ClearJobs();

	virtual bool ServiceHasWorkToDo()
	{
		bool bRet = (!mIndexArrayJobList.empty());
		bRet |= (!mVertexArrayJobList.empty());
		bRet |= (!mShaderJobList.empty());
		bRet |= (!mTextureUpdateJobList.empty());
		return bRet;
	}
protected:
	unsigned int mProcessingCount;
	typedef struct IndexArrayJob_t
	{
		void *mpMem;
		bool mbFreeMemoryAfterUpload;
		bool mbNeedInit;
		unsigned int mSizeInBytes;
		ZMesh *mpMesh;
	} IndexArrayJob_t;

	typedef struct VertexArrayJob_t
	{
		bool mbForParticles;
		void *mpMem;
		bool mbFreeMemoryAfterUpload;
		bool mbNeedInit;
		unsigned int mNbVT;
		unsigned int mVTFormat;
		ZMesh *mpMesh;
		ZMeshInstance *mpMeshInstance;
		bool mbComputeBSphere;
	} VertexArrayJob_t;

	typedef struct ShaderArrayJob_t
	{
		ZFx *mpEffect;
		tstring mEffectFileName; // to replace with full loaded text
	} ShaderArrayJob_t;

	typedef struct TextureUpdateJob_t
	{
		ZTexture *mTexture;
		char *mpBits;
		int mWidth;
		int mHeight;
		int mBpp;
	} TextureUpdateJob_t;

	ZDataResource<IndexArrayJob_t> mIndexArrayJobList;
	ZDataResource<VertexArrayJob_t> mVertexArrayJobList;
	ZDataResource<ShaderArrayJob_t> mShaderJobList;
	ZDataResource<TextureUpdateJob_t> mTextureUpdateJobList;
	ZDataResource<void*> mVARelaseJobList;
	ZDataResource<void*> mIARelaseJobList;
};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif
