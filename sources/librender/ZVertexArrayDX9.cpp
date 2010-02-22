///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZVertexArrayDX9.cpp
// Creation : 15/06/2007
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
#include <D3D9.h>
//#include "../libplatform/libplatform.h"
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"

extern DWORD GDX9ThreadId;
extern IDirect3DDevice9 *GD3D9Device;

class ZVertexArrayDX9 : public IVertexArray
{
public:
    DECLARE_SERIALIZABLE
public:
    ZVertexArrayDX9()
    {
        mVertexSize = 0;
        mVertexFormat = 0;
        mVertexCount = 0;
		mRamedVBO = NULL;
        mD3DBuffer = NULL;
		mD3D9Declaration = NULL;
		mD3D9DeclarationHWI = NULL;
		mLocked = false;
    }
    virtual ~ZVertexArrayDX9()
    {
		if (mRamedVBO)
			free(mRamedVBO);

		if (GDX9ThreadId != GetCurrentThreadId() ) 
		{
			VRAMService->AddVertexArrayReleaseJob(mD3DBuffer);
			return;
		}

        if (mD3DBuffer)
			mD3DBuffer->Release();

    }
	IDirect3DVertexDeclaration9* BuildDecl(uint32 aFormat)
	{
		IDirect3DVertexDeclaration9 *aDecl;

		// declaration
		std::map<uint32, IDirect3DVertexDeclaration9 *>::iterator iter = mVertexDeclarations.find(aFormat);
		if (iter != mVertexDeclarations.end())
			aDecl = (*iter).second;
		else
		{
			// vertex declaration
			D3DVERTEXELEMENT9 declaration[32]; 
			unsigned int idx = 0, avs = 0, usaged = 0;
			if (aFormat&VAF_XYZ)        
			{ 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT3;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_POSITION;
				declaration[idx].UsageIndex = 0; 
				idx++;
				avs+=12; 
			}

			if (aFormat&VAF_XYZRHW)     
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT , 0 }; 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT4;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_POSITIONT;
				declaration[idx].UsageIndex = 0; 
				avs+=16; 
				idx++;
			}
			if (aFormat&VAF_NORMAL)     
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL , 0 }; 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT3;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_NORMAL;
				declaration[idx].UsageIndex = 0; 
				avs+=12; 
				idx++;
			}
			if (aFormat&VAF_COLOR)      
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_D3DCOLOR,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR    , 0 }; 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_D3DCOLOR;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_COLOR;
				declaration[idx].UsageIndex = 0; 
				avs+=4; 
				idx++;
			}
			if (aFormat&VAF_BINORMAL)   
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL , 0 }; 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT3;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_BINORMAL;
				declaration[idx].UsageIndex = 0; 
				avs+=12; 
				idx++;
			}
			if (aFormat&VAF_BITANGENT)  
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT  , 0 }; 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT3;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TANGENT;
				declaration[idx].UsageIndex = 0; 
				avs+=12; 
				idx++;
			}
			if (aFormat&VAF_TEX0)       
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD , usaged };
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT2;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				avs+=8; 
				usaged++; 
				idx++;
			}
			if (aFormat&VAF_TEX1)       
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD , usaged };
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT2;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				avs+=8; 
				usaged++; 
				idx++;
			}
			if (aFormat&VAF_TEX2)       
			{ 
				//declaration[idx++] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD , usaged };
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT2;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				avs+=8; 
				usaged++; 
				idx++;
			}
			if (aFormat&VAF_TEX3)       
			{ 
				//declaration[idx] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD , usaged }; 
				declaration[idx].Stream = 0;
				declaration[idx].Offset = avs;
				declaration[idx].Type = D3DDECLTYPE_FLOAT2;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				idx++;
				avs+=8; 
				usaged++; 
			}
			if (aFormat&VAF_HWI_MATRIX)       
			{ 
				//declaration[idx] = { 0, avs,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD , usaged }; 
				declaration[idx].Stream = 1;
				declaration[idx].Offset = 0;
				declaration[idx].Type = D3DDECLTYPE_FLOAT4;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				idx++;
				avs+=8; 
				usaged++; 
				declaration[idx].Stream = 1;
				declaration[idx].Offset = 4*sizeof(float);
				declaration[idx].Type = D3DDECLTYPE_FLOAT4;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				idx++;
				avs+=8; 
				usaged++; 
				declaration[idx].Stream = 1;
				declaration[idx].Offset = 8*sizeof(float);
				declaration[idx].Type = D3DDECLTYPE_FLOAT4;
				declaration[idx].Method = D3DDECLMETHOD_DEFAULT;
				declaration[idx].Usage = D3DDECLUSAGE_TEXCOORD;
				declaration[idx].UsageIndex = usaged; 
				idx++;
				avs+=8; 
				usaged++; 
			}
			// end of decl
			declaration[idx].Stream = 0xFF;
			declaration[idx].Offset = 0;
			declaration[idx].Type = D3DDECLTYPE_UNUSED;
			declaration[idx].Method = 0;
			declaration[idx].Usage = 0;
			declaration[idx].UsageIndex = 0; 


			
			GD3D9Device->CreateVertexDeclaration( declaration, &aDecl );
			mVertexDeclarations[aFormat] = aDecl;

			
		}
		return aDecl;
	}

	// reset (clear VRAM pointer but don't destroy ram things)
	virtual void Reset() 
	{
		if (mD3DBuffer)
			mD3DBuffer->Release();
		mD3DBuffer = NULL;
	}

    void Init(uint32 aFormat, int aQty, bool aKeepVBORam, uint32 aFlag)
    {
		bool bRealloc = ((mVertexFormat!=aFormat)||(mVertexCount!=aQty));

		mbKeepVBORam = true;//aKeepVBORam;
		if (GDX9ThreadId != GetCurrentThreadId() ) 
		{

			mVertexFormat = aFormat;
			mVertexCount = aQty;
			mVertexSize = GetVertexSizeFromFormat(aFormat);

			VRAMService->AddVertexArrayJob(NULL, this, true, NULL, mVertexCount, false, mVertexFormat, false);
		}
		else
		{
			mD3D9Declaration = BuildDecl(aFormat);
			// no HWI for pre-transformed vertices
			if (aFormat&2)
				mD3D9DeclarationHWI = NULL;
			else
				mD3D9DeclarationHWI = BuildDecl(aFormat|VAF_HWI_MATRIX);
			

			// buffer 
			mVertexCount = aQty;
			mVertexSize = GetVertexSizeFromFormat(aFormat);


			mVertexFormat = aFormat;

			DWORD usageFlags = D3DUSAGE_WRITEONLY;
			if(aFlag&VACF_DYNAMIC)
				usageFlags = D3DUSAGE_DYNAMIC;

			if (mD3DBuffer)
				mD3DBuffer->Release();
			GD3D9Device->CreateVertexBuffer(aQty*mVertexSize, usageFlags, 0, D3DPOOL_DEFAULT, &mD3DBuffer, NULL );
		}
		if (bRealloc)
		{
			if (mRamedVBO)
				free(mRamedVBO);
			if (mbKeepVBORam)
				mRamedVBO = malloc(aQty*mVertexSize);
		}
    }

    uint32 GetFormat() const { return mVertexFormat; }
    uint32 GetVertexSize() const { return mVertexSize; }
    uint32 GetVertexCount() const { return mVertexCount; }

    void * Lock(IMESH_LOCK aFlag) 
    { 
		if (mLocked) 
			return NULL;

		mLockFlag = aFlag;
		mLocked = true;

		if (mRamedVBO)
			return mRamedVBO;

		// should never come here
		ZASSERT(0,"bad lock usage");

		void *alock;
		mD3DBuffer->Lock(0, mVertexSize*mVertexCount, &alock, (aFlag==VAL_READONLY)?D3DLOCK_READONLY:0); 
        return alock;
    }

    void Unlock() 
    {
		mLocked = false;
		
		if (mbKeepVBORam)
		{
			if (mLockFlag == VAL_READONLY) 
			{
				return;
			}
			
			if (GDX9ThreadId != GetCurrentThreadId() ) 
			{
				VRAMService->AddVertexArrayJob(NULL, this, false, mRamedVBO, mVertexCount, false, mVertexFormat, false);
			}
			else
			{
				void *alock;
				mD3DBuffer->Lock(0, mVertexSize*mVertexCount, &alock, 0);//(aFlag==VAL_READONLY)?D3DLOCK_READONLY:0); 
				memcpy(alock, mRamedVBO, mVertexSize*mVertexCount);
				mD3DBuffer->Unlock();
				static int lockh = 0;
			}
		}
		else
		{
			ZASSERT(0,"bad unlock usage");
			//mD3DBuffer->Unlock();
		}
        
    }

    bool Bind() 
    {
		if (!mD3DBuffer)
			return false;
        GD3D9Device->SetVertexDeclaration(mD3D9Declaration);
        GD3D9Device->SetStreamSource(0, mD3DBuffer, 0, mVertexSize);

		return true;
    }
	void Unbind()
	{
	}

    void BindHWI(IVertexArray *pInstancesStream) 
    {
		ZVertexArrayDX9 *pVAIS = (ZVertexArrayDX9*)pInstancesStream;

        GD3D9Device->SetVertexDeclaration(mD3D9DeclarationHWI);
        GD3D9Device->SetStreamSource(0, mD3DBuffer, 0, mVertexSize);
		GD3D9Device->SetStreamSource(1, pVAIS->mD3DBuffer, 0, pVAIS->mVertexSize);
		
    }

    virtual tulong GetMemoryUsed() const { return (sizeof(ZVertexArrayDX9) + mVertexSize*mVertexCount); }

protected:
    uint32 mVertexSize;
    uint32 mVertexFormat;
    uint32 mVertexCount;
	bool mbKeepVBORam;
	void *mRamedVBO;

	IMESH_LOCK mLockFlag;
	bool mLocked;

    IDirect3DVertexBuffer9 * mD3DBuffer;
    IDirect3DVertexDeclaration9 *mD3D9Declaration, *mD3D9DeclarationHWI;
    static std::map<uint32, IDirect3DVertexDeclaration9 *> mVertexDeclarations;
};

std::map<uint32, IDirect3DVertexDeclaration9 *> ZVertexArrayDX9::mVertexDeclarations;

BEGIN_SERIALIZATION(ZVertexArrayDX9)
END_SERIALIZATION()
