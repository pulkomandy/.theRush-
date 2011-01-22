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

#include "ZMesh.h"

#include "MiniBall/Sphere.h"
#include "libbase/ZLogger.h"

IIndexArray *mPreviousIA = NULL;
IVertexArray *mPreviousVA = NULL;


BEGIN_SERIALIZATION(ZMesh)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMesh::ComputeBSphere()
{
	if (!mVertexArray.ptr()) 
		return;
	MiniBall::Sphere mbs = MiniBall::Sphere::miniBall((unsigned char*)mVertexArray->Lock(VAL_READONLY), mVertexArray->GetVertexSize(), mVertexArray->GetVertexCount());
	mBVolume.mBSphere.x = mbs.center.getX();
	mBVolume.mBSphere.y = mbs.center.getY();
	mBVolume.mBSphere.z = mbs.center.getZ();
	mBVolume.mBSphere.w = mbs.radius;
	mVertexArray->Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZMesh::PickRay(const tvector3 & origin, const tvector3 &dir, tvector3& normal, float &t, unsigned int *pTriNb)
{
    if (mIndexArray.ptr())
    {
        // with index
        switch (mPrimitive)
        {
        case     PRIM_LINELIST: return false; break;
        case     PRIM_LINESTRIP: return false; break;
        case     PRIM_TRIANGLELIST:
            {
                float u,v;
                float aNT = 2.0f;
				bool bCol = false;

                uint8_t * aVts = (uint8_t *)mVertexArray->Lock(VAL_READONLY);
                uint16_t *inds = (uint16_t*)mIndexArray->Lock(VAL_READONLY);
				for (unsigned int asbm = 0;asbm<mSubMeshes.size();asbm++)
				{
                for (unsigned int i=0;i<mSubMeshes[asbm].mCount/3;i++)
                {
                    uint8_t *aVts1 = aVts + mVertexArray->GetVertexSize()*inds[i*3 + mSubMeshes[asbm].mStart];
                    uint8_t *aVts2 = aVts + mVertexArray->GetVertexSize()*inds[i*3 + 1 + mSubMeshes[asbm].mStart];
                    uint8_t *aVts3 = aVts + mVertexArray->GetVertexSize()*inds[i*3 + 2 + mSubMeshes[asbm].mStart];
                    /*
                    aVts2 
                    tvector3* vert2 = (tvector3*)aVts;
                    aVts += mVertexArray->GetVertexSize()*inds[i*3+1];
                    tvector3* vert3 = (tvector3*)aVts;
                    aVts += mVertexArray->GetVertexSize()*inds[i*3+2];
*/
                    if (intersect_triangle((float*)&origin.x, (float*)&dir.x,
                        (float*)aVts1, (float*)aVts2, (float*)aVts3,
                        &t, &u, &v) )
                    {
                        if ((t<aNT)&& (t>0) && (t<1))
                        {
                            normal.Normal((tvector3&)*aVts1, (tvector3&)*aVts2, (tvector3&)*aVts3);
                            normal.Normalize();
                            /*
                            vert1++;
                            vert2++;
                            vert3++;
                            normal =  ((*vert1) + (*vert2) + (*vert3) ) * 0.333333f;
                            */

                            //position = rayOrig + rayDir*t;
                            aNT = t;
							bCol = true;
							if (pTriNb)
								*pTriNb = i + mSubMeshes[asbm].mStart/3;
                            //bCollid = true;
                        }
                    }
                }
				}

                mVertexArray->Unlock();
                mIndexArray->Unlock();
				//bool bRet = ( (t>=0.f) && (t<=1.0f));
				t = aNT;

                return bCol;
            }
            break;
        case     PRIM_TRIANGLESTRIP: return false; break;

        case     PRIM_TRIANGLEFAN: return false; break;
        default : return false; break;
        }
    }
    else
    {
        // no index
        switch (mPrimitive)
        {
        case     PRIM_LINELIST: return false; break;
        case     PRIM_LINESTRIP: return false; break;
        case     PRIM_TRIANGLELIST:
            {
                float u,v;
                float aNT = 2.0f;
				bool bCol = false;

                uint8_t * aVts = (uint8_t *)mVertexArray->Lock(VAL_READONLY);
                for (unsigned int i=0;i<mVertexArray->GetVertexCount()/3;i++)
                {
                    tvector3* vert1 = (tvector3*)aVts;
                    aVts += mVertexArray->GetVertexSize();
                    tvector3* vert2 = (tvector3*)aVts;
                    aVts += mVertexArray->GetVertexSize();
                    tvector3* vert3 = (tvector3*)aVts;
                    aVts += mVertexArray->GetVertexSize();

                    if (intersect_triangle((float*)&origin.x, (float*)&dir.x,
                        (float*)vert1, (float*)vert2, (float*)vert3,
                        &t, &u, &v) )
                    {
                        if ((t<aNT)&& (t>0) && (t<1))
                        {
                            //normal.Normal(*vert1, *vert2, *vert3);
                            //normal.Normalize();
                            vert1++;
                            vert2++;
                            vert3++;
                            normal =  ((*vert1) + (*vert2) + (*vert3) ) * 0.333333f;

                            //position = rayOrig + rayDir*t;
                            aNT = t;
							bCol = true;

							if (pTriNb)
								*pTriNb = i;// + mSubMeshes[asbm].mStart/3;
                            //bCollid = true;
                        }
                    }
                }

                mVertexArray->Unlock();
				t = aNT;

                return bCol;
            }
            break;
        case     PRIM_TRIANGLESTRIP: return false; break;

        case     PRIM_TRIANGLEFAN: return false; break;
        default : return false; break;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
This function assumes that indices are not relevant as input.
Each triangle is composed of sequential 3 vertices.
So, implicitely, there will be vtcount/3 triangles.
*/
void ZMesh::RemoveRedundancy(void *pVT, int vtSize, int vtCount, uint16_t *pind, int* nind, void *pNewVT, int *pNewVtNb)
{
    unsigned int i;
    uint8_t *vts = (uint8_t *)pVT;
    int totalRedundancy = 0;
    uint16_t *nn = (uint16_t*)malloc(sizeof(uint16_t) * vtCount);
    uint16_t *nnRec = (uint16_t*)malloc(sizeof(uint16_t) * vtCount);

    bool *bKeepVt = (bool*)malloc(sizeof(bool) * vtCount);
    memset(bKeepVt, 1, sizeof(bool) * vtCount);
    
    for (i=0;i<(unsigned int)vtCount;i++)
    {
        nn[i] = i;
    }
    int recIdx = 0;

    for (i=0;i<(unsigned int)vtCount;i++)
    {
        int cnt = 0;

        if (bKeepVt[i])
        {
            nnRec[i] = recIdx++;
            for (int j=(i+1);j<vtCount;j++)
            {
                if ( (bKeepVt[j]) && (nn[j]>i) )
                {
                    if (!memcmp( (vts+(i*vtSize)), (vts+ (j*vtSize)), vtSize))
                    {
                        cnt++;
                        nnRec[j] = nnRec[i];
                        bKeepVt[j] = false;
                    }
                }
            }
        }
        totalRedundancy += cnt;
    }
    


    LOG("Total Vertex Redundancy = %d on %d vertices -> %d vertices.\r\n", totalRedundancy, vtCount, vtCount-totalRedundancy);
    // --
    *pNewVtNb = vtCount-totalRedundancy;
    uint8_t *tcpy = (uint8_t*)pNewVT;
	memcpy( pind, nnRec, sizeof(uint16_t)*vtCount);
	
    for (i=0;i<(unsigned int)vtCount;i++)
    {
        if (bKeepVt[i])
        {
            memcpy(tcpy, &vts[i*vtSize], vtSize);
            tcpy+=vtSize;
        }
		
    }
	
    *nind = vtCount;

    // free things

    free(nn);
    free(nnRec);
    free(bKeepVt);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ZMesh::RemoveRedundancy2(void *pVTSrc, int nbVTSrc, int vtSize, uint16_t *pind, int nind, void *pVTDst, int *nbVTDst)
{
	unsigned int i;
    uint8_t *vts = (uint8_t *)pVTSrc;
    int totalRedundancy = 0;

    uint16_t *nnRec = (uint16_t*)malloc(sizeof(uint16_t) * nbVTSrc);

    bool *bKeepVt = (bool*)malloc(sizeof(bool) * nbVTSrc);
    memset(bKeepVt, 1, sizeof(bool) * nbVTSrc);

    int recIdx = 0;
    for (i=0;i<(unsigned int)nbVTSrc;i++)
    {
		nnRec[i] = i;
	}

    for (i=0;i<(unsigned int)nbVTSrc;i++)
    {
        int cnt = 0;

        if (bKeepVt[i])
        {
            for (int j=(i+1);j<nbVTSrc;j++)
            {
                if ( (bKeepVt[j]) )
                {
                    if (!memcmp( (vts+(i*vtSize)), (vts+ (j*vtSize)), vtSize))
                    {
                        cnt++;
                        nnRec[j] = i;//nnRec[i];
                        bKeepVt[j] = false;
                    }
                }
            }
        }
        totalRedundancy += cnt;
    }
    


    LOG("Total Vertex Redundancy = %d on %d vertices -> %d vertices.\r\n", totalRedundancy, nbVTSrc, nbVTSrc-totalRedundancy);
    // --
    *nbVTDst = nbVTSrc-totalRedundancy;
    uint8_t *tcpy = (uint8_t*)pVTDst;
	//memcpy( pind, nnRec, sizeof(uint16)*vtCount);

	
	// compile vertex
	recIdx = 0;
    for (i=0;i<(unsigned int)nbVTSrc;i++)
    {
        if (bKeepVt[i]) // if we keep it, we stack it and set proper nnRec
        {
            memcpy(tcpy, &vts[i*vtSize], vtSize);
            tcpy+=vtSize;
			nnRec[i] = recIdx++;
        }
		else // if not, the previous corresponding index has been set, so we use it
		{
			nnRec[i] = nnRec[nnRec[i]];
		}
		
    }
	// remap indices
	for (int j = 0;j<nind;j++)
	{
		pind[j] = nnRec[pind[j]];
	}

	

    // free things

    free(nnRec);
    free(bKeepVt);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMesh::Explode()
{
	IVertexArray * normvt = GDD->NewVertexArray();
	DPVA(normvt);
	normvt->Init(mVertexArray->GetFormat(), mIndexArray->GetIndexCount(), true);

	unsigned char *ptrd = (unsigned char *)normvt->Lock(VAL_WRITE);
	unsigned char *ptrs = (unsigned char *)mVertexArray->Lock(VAL_READONLY);
	unsigned short *pind = (unsigned short *)mIndexArray->Lock(VAL_READONLY);
	for (unsigned int i=0;i<mIndexArray->GetIndexCount();i++)
	{
		memcpy(ptrd+(i*mVertexArray->GetVertexSize()), 
			ptrs+(pind[i]*mVertexArray->GetVertexSize()), 
			mVertexArray->GetVertexSize());

	}
	normvt->Unlock();
	mVertexArray->Unlock();
	mIndexArray->Unlock();

	mIndexArray.Attach(NULL);
	mVertexArray.Attach(normvt);


	// /!\ SHOULD HANDLE CORRECTLY submeshes
}

///////////////////////////////////////////////////////////////////////////////////////////////////


tvector3 ZMesh::GetFaceNormal(unsigned int aFaceNb)
{
	if (!mIndexArray.ptr())
	{
		if (aFaceNb*3<mVertexArray->GetVertexCount())
		{

			unsigned char *ptrs = (unsigned char *)mVertexArray->Lock(VAL_READONLY);
			ptrs += aFaceNb *3*mVertexArray->GetVertexSize();
			ptrs += 12;

			tvector3 ret(tvector3::zero);
			ret += *(tvector3*)ptrs;
			ptrs+=mVertexArray->GetVertexSize();
			ret += *(tvector3*)ptrs;
			ptrs+=mVertexArray->GetVertexSize();
			ret += *(tvector3*)ptrs;
			ret.Normalize();
		

			mVertexArray->Unlock();
			return ret;
		}
		return tvector3::zero;
	}
	else
	{
		// should handle it
	}
	return tvector3::zero;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMesh::SetFaceUV(unsigned int aFaceNb, const tvector2 &uv1, const tvector2 &uv2, const tvector2 &uv3)
{
	if (mIndexArray.ptr()) return;

	unsigned char *ptrs = (unsigned char *)mVertexArray->Lock(VAL_WRITE);
	ptrs += aFaceNb *3*mVertexArray->GetVertexSize();

	ptrs += 12; // position
	if (mVertexArray->GetFormat()&VAF_NORMAL) ptrs += 12;
	if (mVertexArray->GetFormat()&VAF_COLOR) ptrs += 4;
	if (mVertexArray->GetFormat()&VAF_BINORMAL) ptrs += 8;
	if (mVertexArray->GetFormat()&VAF_BITANGENT) ptrs += 8;

	//float *ptrf = (float*)ptrs;

	*((float*)ptrs) = uv1.x;
	*(((float*)ptrs)+1) = uv1.y;
	ptrs += mVertexArray->GetVertexSize();

	*((float*)ptrs) = uv2.x;
	*(((float*)ptrs)+1) = uv2.y;
	ptrs += mVertexArray->GetVertexSize();

	*((float*)ptrs) = uv3.x;
	*(((float*)ptrs)+1) = uv3.y;


	mVertexArray->Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMesh::SetFaceUV(unsigned int aFaceNb, const tvector2 &translation, const tvector2& scale)
{
	if (mIndexArray.ptr()) return;

	unsigned char *ptrs = (unsigned char *)mVertexArray->Lock(VAL_WRITE);
	ptrs += aFaceNb *3*mVertexArray->GetVertexSize();

	ptrs += 12; // position
	if (mVertexArray->GetFormat()&VAF_NORMAL) ptrs += 12;
	if (mVertexArray->GetFormat()&VAF_COLOR) ptrs += 4;
	if (mVertexArray->GetFormat()&VAF_BINORMAL) ptrs += 8;
	if (mVertexArray->GetFormat()&VAF_BITANGENT) ptrs += 8;

	//float *ptrf = (float*)ptrs;

	*((float*)ptrs) = (*((float*)ptrs)*scale.x)+translation.x;
	*(((float*)ptrs)+1) = (*(((float*)ptrs)+1)*scale.y)+translation.y;
	ptrs += mVertexArray->GetVertexSize();

	*((float*)ptrs) = (*((float*)ptrs)*scale.x)+translation.x;
	*(((float*)ptrs)+1) = (*(((float*)ptrs)+1)*scale.y)+translation.y;
	ptrs += mVertexArray->GetVertexSize();

	*((float*)ptrs) = (*((float*)ptrs)*scale.x)+translation.x;
	*(((float*)ptrs)+1) = (*(((float*)ptrs)+1)*scale.y)+translation.y;


	mVertexArray->Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMesh::GetAdjacentFaces(unsigned int aFaceNb, unsigned int &face1, unsigned int &face2, unsigned int &face3)
{
	if (mIndexArray.ptr()) return;

	unsigned char *ptrs = (unsigned char *)mVertexArray->Lock(VAL_READONLY);
	unsigned char *ptrs2 = ptrs; 
	ptrs += aFaceNb *3*mVertexArray->GetVertexSize();

	tvector3 pos1, pos2, pos3;
	pos1 = *((tvector3*)ptrs);
	ptrs+=mVertexArray->GetVertexSize();
	pos2 = *((tvector3*)ptrs);
	ptrs+=mVertexArray->GetVertexSize();
	pos3 = *((tvector3*)ptrs);
	
	ptrs = ptrs2;
	
	int ptav=0;
	for (unsigned int i=0;i<mVertexArray->GetVertexCount();i+=3)
	{
		if (i != (aFaceNb*3))
		{
			tvector3 pos1b, pos2b, pos3b;
			pos1b = *((tvector3*)ptrs);
			ptrs+=mVertexArray->GetVertexSize();
			pos2b = *((tvector3*)ptrs);
			ptrs+=mVertexArray->GetVertexSize();
			pos3b = *((tvector3*)ptrs);
			ptrs+=mVertexArray->GetVertexSize();

			if (((pos1b == pos1)&&(pos2b == pos2)) ||
				((pos1b == pos1)&&(pos2b == pos3)) ||
				((pos1b == pos2)&&(pos2b == pos1)) ||
				((pos1b == pos2)&&(pos2b == pos3)) ||
				((pos1b == pos3)&&(pos2b == pos1)) ||
				((pos1b == pos3)&&(pos2b == pos2)) ||

				((pos1b == pos1)&&(pos3b == pos2)) ||
				((pos1b == pos1)&&(pos3b == pos3)) ||
				((pos1b == pos2)&&(pos3b == pos1)) ||
				((pos1b == pos2)&&(pos3b == pos3)) ||
				((pos1b == pos3)&&(pos3b == pos1)) ||
				((pos1b == pos3)&&(pos3b == pos2)) ||

				((pos2b == pos1)&&(pos3b == pos2)) ||
				((pos2b == pos1)&&(pos3b == pos3)) ||
				((pos2b == pos2)&&(pos3b == pos1)) ||
				((pos2b == pos2)&&(pos3b == pos3)) ||
				((pos2b == pos3)&&(pos3b == pos1)) ||
				((pos2b == pos3)&&(pos3b == pos2)) 
)
			{
				switch(ptav)
				{
				case 0: 
					face1 = i/3; 
					break;
				case 1: 
					face2 = i/3; 
					break;
				case 2: 
					face3 = i/3; 
					return; 
					break;

				}
				ptav++;
			}
		}
		else
		{
			ptrs += 3*mVertexArray->GetVertexSize();
		}
	}

	mVertexArray->Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

inline void StackVertexData(float **pSrc, float **pDst, unsigned int currentChunk, unsigned int chunkFloatSize, unsigned int srcFormat, unsigned int dstFormat)
{
	if (dstFormat&currentChunk)
	{
		if ( srcFormat&currentChunk)
		{
			memcpy( *pDst, *pSrc, chunkFloatSize * sizeof(float));
			(*pDst) += chunkFloatSize;
			(*pSrc) += chunkFloatSize;
		}
		else
		{
			memset( *pDst, 0, chunkFloatSize * sizeof(float));
			(*pDst) += chunkFloatSize;
		}
	}
	else
	{
		if ( srcFormat&currentChunk)
			(*pSrc) += chunkFloatSize;
	}
}

bool ZMesh::BuildMerged(std::vector<ZMesh*>& aMeshList, unsigned int aVAFormat)
{
	if (aMeshList.empty())
		return false;

	// check types
	bool bHasIndex = (aMeshList[0]->GetIndexBuffer() != NULL);
	uint8_t prim = aMeshList[0]->GetPrimitive();
	
	unsigned int nbVertices = aMeshList[0]->GetVertexBuffer()->GetVertexCount();
	unsigned int nbIndices = (aMeshList[0]->GetIndexBuffer() != NULL)?aMeshList[0]->GetIndexBuffer()->GetIndexCount():0;

	for (unsigned int i = 1;i<aMeshList.size();i++)
	{
		if (aMeshList[i]->GetPrimitive() != prim)
			return false;
		if (bHasIndex != (aMeshList[i]->GetIndexBuffer() != NULL))
			return false;

		if (bHasIndex)
			nbIndices += aMeshList[i]->GetIndexBuffer()->GetIndexCount();

		nbVertices += aMeshList[i]->GetVertexBuffer()->GetVertexCount();

	}
	// not too much vertices?

	if (nbVertices>65535)
		return false;

	// good, we can mix up



	// allocate index/vertex arrays
	IVertexArray *nvt = GDD->NewVertexArray();
	DPVA(nvt);
	nvt->Init(aVAFormat, nbVertices, true);
	this->SetVertexBuffer(nvt);

	IIndexArray *nin = NULL;

	if (bHasIndex)
	{
		nin = GDD->NewIndexArray();
		DPIA(nin);
		nin->Init(nbIndices);
		this->SetIndexBuffer(nin);
	}

	// append vertex array
	std::vector<unsigned int> mVTDecals;
	mVTDecals.resize(aMeshList.size());
	unsigned int aDecal = 0;

	tvector3 vMin = aMeshList[0]->GetBVolume().GetBoxMin();
	tvector3 vMax = aMeshList[0]->GetBVolume().GetBoxMax();

	float *pDst = (float*)nvt->Lock(VAL_WRITE);
	//float *pDstSvg = pDst;
	for (unsigned int i = 0;i<aMeshList.size();i++)
	{
		mVTDecals[i] = aDecal;
		
		IVertexArray *pVertAS = aMeshList[i]->GetVertexBuffer();
		float *pSrc = (float*)pVertAS->Lock(VAL_READONLY);

		unsigned int srcFormat = pVertAS->GetFormat();

		vMin.Minimize(aMeshList[i]->GetBVolume().GetBoxMin());
		vMax.Maximize(aMeshList[i]->GetBVolume().GetBoxMax());

		for (unsigned int j = 0;j<aMeshList[i]->GetVertexBuffer()->GetVertexCount();j++)
		{
			// VAF_XYZ = 1,
			StackVertexData(&pSrc, &pDst, VAF_XYZ, 3, srcFormat, aVAFormat);
			// VAF_XYZRHW = 1<<1,
			StackVertexData(&pSrc, &pDst, VAF_XYZRHW, 4, srcFormat, aVAFormat);
			// VAF_NORMAL = 1<<2,
			StackVertexData(&pSrc, &pDst, VAF_NORMAL, 3, srcFormat, aVAFormat);
			// VAF_COLOR = 1<<3,
			StackVertexData(&pSrc, &pDst, VAF_COLOR, 1, srcFormat, aVAFormat);
			// VAF_BINORMAL = 1<<4,
			StackVertexData(&pSrc, &pDst, VAF_BINORMAL, 3, srcFormat, aVAFormat);
			// VAF_BITANGENT = 1<<5,
			StackVertexData(&pSrc, &pDst, VAF_BITANGENT, 3, srcFormat, aVAFormat);
			// VAF_TEX0 = 1<<6,
			StackVertexData(&pSrc, &pDst, VAF_TEX0, 2, srcFormat, aVAFormat);
			// VAF_TEX1 = 1<<7,
			StackVertexData(&pSrc, &pDst, VAF_TEX1, 2, srcFormat, aVAFormat);
			// VAF_TEX2 = 1<<8,
			StackVertexData(&pSrc, &pDst, VAF_TEX2, 2, srcFormat, aVAFormat);
			// VAF_TEX3 = 1<<9,
			StackVertexData(&pSrc, &pDst, VAF_TEX3, 2, srcFormat, aVAFormat);
		}

		pVertAS->Unlock();

		aDecal += pVertAS->GetVertexCount();
	}
/*
	VRAMService->AddVertexArrayJob(this,
		nvt, true,
		pDstSvg, nbVertices, true,
		aVAFormat, true);
*/
	nvt->Unlock();
	// bbox
	GetBVolume().SetBoxMinMax(vMin, vMax);

	// got indices?
	if (bHasIndex)
	{
		unsigned short *pDstI = (unsigned short*)nin->Lock(VAL_WRITE);
		//unsigned short *pDstISvg = pDstI;
		for (unsigned int i = 0;i<aMeshList.size();i++)
		{
			unsigned short *pSrcI = (unsigned short*)aMeshList[i]->GetIndexBuffer()->Lock(VAL_READONLY);

			for (unsigned int j = 0;j<aMeshList[i]->GetIndexBuffer()->GetIndexCount();j++)
			{
				*pDstI++ = *pSrcI++ + mVTDecals[i];
			}

			aMeshList[i]->GetIndexBuffer()->Unlock();
		}
		nin->Unlock();
		/*
		VRAMService->AddIndexArrayJob(this, nin, true, 
									   pDstISvg, nbIndices * sizeof(unsigned short), true);
									   */
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
