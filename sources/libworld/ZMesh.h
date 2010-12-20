///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZMesh.h
// Creation : 20/06/2007
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


#ifndef ZMESH_H__
#define ZMESH_H__

#include "..\libbase\ZBVolume.h"
#include "..\librender\IStreamArrays.h"
#include "..\librender\IDisplayDevice.h"

class ZPhysicTriMesh;

DECLAREZCLASS(ZMesh);

extern IIndexArray *mPreviousIA;
extern IVertexArray *mPreviousVA;

#include "ZPhysicTriMesh.h"

class ZMesh : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
    ZMesh() 
    {
        mPrimitive = PRIM_UNDEFINED;
    }

    ~ZMesh() 
	{
		/*
#ifdef _DEBUG
		if (mIndexArray.ptr())
		{
			ZASSERT( (mIndexArray->GetRefCount() < 2) , "~ZMesh : Too much references on mIndexArray");
		}
#endif
		ZASSERT( (mVertexArray->GetRefCount() < 2) , "~ZMesh : Too much references on mVertexArray");
		*/
	}

    void SetIndexBuffer(IIndexArray *pBuf)
    {
        mIndexArray.Attach(pBuf);
    }
    void SetVertexBuffer(IVertexArray *pBuf)
    {
        mVertexArray.Attach(pBuf);
    }

    IIndexArray * GetIndexBuffer( ) const
    { 
        return mIndexArray.ptr(); 
    }
    IVertexArray * GetVertexBuffer( ) const
    { 
        return mVertexArray.ptr(); 
    }

    ZBoundingVolume& GetBVolume()
    {
        return mBVolume;
    }

	/*
	const tvector4& GetBSphere() const { return mBSphere; }
	void SetBSphere(const tvector4& bsphere) { mBSphere = bsphere; }
	*/
	void ComputeBSphere();

    void AddSubMesh(uint32_t aStart, uint32_t aCount)
    {
        mSubMeshes.push_back(tSubMesh(aStart, aCount));
    }
    void SetSubMesh(uint32_t aSMNb, uint32_t aStart, uint32_t aCount)
    {
        mSubMeshes[aSMNb].mStart = aStart;
        mSubMeshes[aSMNb].mCount = aCount;
    }
    void GetSubMesh(uint32_t aSMNb, uint32_t *aStart, uint32_t *aCount) const
    {
        *aStart = mSubMeshes[aSMNb].mStart;
        *aCount = mSubMeshes[aSMNb].mCount;
    }

    static void RemoveRedundancy(void *pVT, int vtSize, int vtCount, uint16_t *pind, int* nind, void *pNewVT, int *pNewVtNb);
	static void RemoveRedundancy2(void *pVTSrc, int nbVTSrc, int vtSize, uint16_t *pind, int nind, void *pVTDst, int *nbVTDst);

    void Draw(uint32_t aSubMesh)
    {

        if (mIndexArray.ptr())
        {
			if (mPreviousVA != mVertexArray)
				if (!mVertexArray->Bind()) return;

            if (mPreviousIA != mIndexArray)
                mIndexArray->Bind();

			GDD->DrawIndexedPrimitives(mPrimitive, 0, 0, mVertexArray->GetVertexCount(), mSubMeshes[aSubMesh].mStart, mSubMeshes[aSubMesh].mCount);

			mIndexArray->AddRef();
			mVertexArray->AddRef();

			if (mPreviousIA)
				mPreviousIA->SubRef();

			if (mPreviousVA)
				mPreviousVA->SubRef();

			mPreviousIA = mIndexArray;
			mPreviousVA = mVertexArray;
        }

        else
        {
			if (mPreviousIA)
				mPreviousIA->Unbind();

            if (mPreviousVA != mVertexArray)
            {
                mVertexArray->Bind();
            }

			GDD->DrawPrimitives(mPrimitive, mSubMeshes[aSubMesh].mStart, mSubMeshes[aSubMesh].mCount);
			mVertexArray->AddRef();
			if (mPreviousVA)
				mPreviousVA->SubRef();

			mPreviousVA = mVertexArray;
			
			if (mPreviousIA)
				mPreviousIA->SubRef();

			mPreviousIA = NULL;
        }

    }

	void DrawInstanced(uint32_t /*aSubMesh*/, uint32_t /*aQty*/)
	{
#if 0
		IDirect3DDevice9 *pd3dDevice = GDD->GetD3D9Device();
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | aQty ) ;
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul ) ;
		
		if (mIndexArray.ptr())
        {
			mIndexArray->Bind();
			GDD->DrawIndexedPrimitives(mPrimitive, 0, 0, mVertexArray->GetVertexCount(), mSubMeshes[aSubMesh].mStart, mSubMeshes[aSubMesh].mCount);
        }
        else
        {
			GDD->DrawPrimitives(mPrimitive, mSubMeshes[aSubMesh].mStart, mSubMeshes[aSubMesh].mCount);
		}
		pd3dDevice->SetStreamSourceFreq( 0, 1 );
		pd3dDevice->SetStreamSourceFreq( 1, 1 );
		mPreviousIA = NULL;
		mPreviousVA = NULL;
#endif
	}


    bool PickRay(const tvector3 & origin, const tvector3 &dir, tvector3& normal, float &t, unsigned int *pTriNb = NULL);

    void SetPrimitive(uint8_t aPrim) { mPrimitive = aPrim; }
    uint8_t GetPrimitive() const { return mPrimitive; }

    tulong GetMemoryUsed() const { return sizeof(ZMesh); }

    uint32_t GetNbSubMeshes() const { return (uint32_t)mSubMeshes.size(); }

	void Explode();
	tvector3 GetFaceNormal(unsigned int aFaceNb);

	void SetFaceUV(unsigned int aFaceNb, const tvector2 &uv1, const tvector2 &uv2, const tvector2 &uv3);
	void SetFaceUV(unsigned int aFaceNb, const tvector2 &translation, const tvector2& scale);
	void GetAdjacentFaces(unsigned int aFaceNb, unsigned int &face1, unsigned int &face2, unsigned int &face3);

	bool BuildMerged(std::vector<ZMesh*>& aMeshList, unsigned int aVAFormat);

	void SetPhysicTriMesh(ZPhysicTriMesh *pModel) 
	{ 
		mTriMesh.Attach(pModel);
	}

	ZPhysicTriMesh* GetPhysicTriMesh() const 
	{ 
		return mTriMesh; 
	}

protected:
	//tvector4 mBSphere;
    smartptr<IIndexArray> mIndexArray;
    smartptr<IVertexArray> mVertexArray;
    uint8_t mPrimitive;
    ZBoundingVolume mBVolume;

    typedef struct tSubMesh
    {
        tSubMesh(uint32_t aStart, uint32_t aCount)
        {
            mStart = aStart;
            mCount = aCount;
        }
        uint32_t mStart, mCount;
        

    } tSubMesh;
    std::vector<tSubMesh> mSubMeshes;

	smartptr<ZPhysicTriMesh> mTriMesh;

};

#endif

