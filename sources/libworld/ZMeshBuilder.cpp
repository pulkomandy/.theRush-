///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZMeshBuilder.h
// Creation : 03/12/2008
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


#include "ZMeshBuilder.h"
#include "ZMesh.h"
#include "ZMeshInstance.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct helpersMeshVT
{
    tvector3 pos;
    //tvector3 normal;
    uint32_t col;
	float u,v;
} helpersMeshVT;

typedef struct helpersMeshVTX
{
    tvector3 pos;
    tvector3 normal;
    uint32_t col;
	float u,v;
} helpersMeshVTX;

typedef struct helpersMeshVTXF
{
    tvector3 pos;
    tvector3 normal;
	tvector2 uv;
} helpersMeshVTXF;

typedef struct helpersMeshVT2
{
    float x,y,z;
	float nx,ny,nz;
	float u,v;
} helpersMeshVT2;

typedef struct helpersMeshVT3
{
    tvector3 vt;
	tvector2 uv;
} helpersMeshVT3;


typedef struct helpersMeshVT4
{
    tvector4 vt;
	tvector2 uv;
} helpersMeshVT4;

typedef struct guiMeshVT
{
    tvector3 pos;
    tvector2 uv;
} guiMeshVT;

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateBox(const tvector3 &size, const tcolor &col)
{

    IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0|VAF_NORMAL, 8 );
    indices->Init(36);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedBox");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f,-0.5f,-0.5f), tvector3( 0.5f, 0.5f, 0.5f));
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, 36 );

    helpersMeshVTX * vts = (helpersMeshVTX*)normvt->Lock(VAL_WRITE);
	unsigned long cc = col.operator tulong();
    
    vts[0].pos = tvector3(-0.5f,-0.5f,-0.5f)*size;
	vts[0].normal = tvector3(-0.5f,-0.5f,-0.5f);
    vts[0].col = cc;
	vts[0].u = 0; vts[0].v = 0;
    vts[1].pos = tvector3(-0.5f, 0.5f,-0.5f)*size;
	vts[1].normal = tvector3(-0.5f, 0.5f,-0.5f);
    vts[1].col = cc;
	vts[1].u = 0; vts[1].v = 1;
    vts[2].pos = tvector3( 0.5f, 0.5f,-0.5f)*size;
	vts[2].normal = tvector3(0.5f, 0.5f,-0.5f);
    vts[2].col = cc;
	vts[2].u = 1; vts[2].v = 1;
    vts[3].pos = tvector3( 0.5f,-0.5f,-0.5f)*size;
	vts[3].normal = tvector3(0.5f,-0.5f,-0.5f);
    vts[3].col = cc;
	vts[3].u = 1; vts[3].v = 0;
    vts[4].pos = tvector3(-0.5f,-0.5f, 0.5f)*size;
	vts[4].normal = tvector3(-0.5f,-0.5f, 0.5f);
    vts[4].col = cc;
	vts[4].u = 0; vts[4].v = 0;
    vts[5].pos = tvector3(-0.5f, 0.5f, 0.5f)*size;
	vts[5].normal = tvector3(-0.5f, 0.5f, 0.5f);
    vts[5].col = cc;
	vts[5].u = 0; vts[5].v = 1;
    vts[6].pos = tvector3( 0.5f, 0.5f, 0.5f)*size;
	vts[6].normal = tvector3(0.5f, 0.5f, 0.5f);
    vts[6].col = cc;
	vts[6].u = 1; vts[6].v = 1;
    vts[7].pos = tvector3( 0.5f,-0.5f, 0.5f)*size;
	vts[7].normal = tvector3(0.5f,-0.5f, 0.5f);
    vts[7].col = cc;
	vts[7].u = 1; vts[7].v = 0;


    normvt->Unlock();

    uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);

    ids[0] = 0;
    ids[1] = 1;
    ids[2] = 2;
    ids[3] = 0;
    ids[4] = 2;
    ids[5] = 3;

    ids[6] = 6;
    ids[7] = 5;
    ids[8] = 4;
    ids[9] = 7;
    ids[10] = 6;
    ids[11] = 4;

    ids[12] = 7;
    ids[13] = 4;
    ids[14] = 0;
    ids[15] = 3;
    ids[16] = 7;
    ids[17] = 0;

    ids[18] = 1;
    ids[19] = 5;
    ids[20] = 6;
    ids[21] = 1;
    ids[22] = 6;
    ids[23] = 2;

    ids[24] = 6;
    ids[25] = 7;
    ids[26] = 3;
    ids[27] = 2;
    ids[28] = 6;
    ids[29] = 3;

    ids[30] = 0;
    ids[31] = 4;
    ids[32] = 5;
    ids[33] = 0;
    ids[34] = 5;
    ids[35] = 1;

    indices->Unlock();

	nnormGen->ComputeBSphere();
    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateFacetedBox(const tvector3 &size)
{

    IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ|VAF_TEX0|VAF_NORMAL, 24 );
    indices->Init(36);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedFacetedBox");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f,-0.5f,-0.5f), tvector3( 0.5f, 0.5f, 0.5f));
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, 36 );

    helpersMeshVTXF * vts = (helpersMeshVTXF*)normvt->Lock(VAL_WRITE);
    
	// haut
    vts[0].pos = tvector3(-0.5f, 0.5f,-0.5f)*size;
	vts[0].normal = tvector3(0, 1, 0);
	vts[0].uv = tvector2(0,0);
    vts[1].pos = tvector3( 0.5f, 0.5f,-0.5f)*size;
	vts[1].normal = tvector3(0, 1, 0);
	vts[1].uv = tvector2(1,0);
    vts[2].pos = tvector3( 0.5f, 0.5f, 0.5f)*size;
	vts[2].normal = tvector3(0, 1, 0);
	vts[2].uv = tvector2(1,1);
    vts[3].pos = tvector3(-0.5f, 0.5f, 0.5f)*size;
	vts[3].normal = tvector3(0, 1, 0);
	vts[3].uv = tvector2(0,1);
	// bas
    vts[4+0].pos = tvector3(-0.5f, -0.5f,-0.5f)*size;
	vts[4+0].normal = tvector3(0, -1, 0);
	vts[4+0].uv = tvector2(0,0);
    vts[4+1].pos = tvector3( 0.5f, -0.5f,-0.5f)*size;
	vts[4+1].normal = tvector3(0, -1, 0);
	vts[4+1].uv = tvector2(1,0);
    vts[4+2].pos = tvector3( 0.5f, -0.5f, 0.5f)*size;
	vts[4+2].normal = tvector3(0, -1, 0);
	vts[4+2].uv = tvector2(1,1);
    vts[4+3].pos = tvector3(-0.5f, -0.5f, 0.5f)*size;
	vts[4+3].normal = tvector3(0, -1, 0);
	vts[4+3].uv = tvector2(0,1);
	// gauche
    vts[8+0].pos = tvector3(-0.5f, -0.5f,-0.5f)*size;
	vts[8+0].normal = tvector3(-0.5f,-0.5f,-0.5f);
	vts[8+0].uv = tvector2(0,0);
    vts[8+1].pos = tvector3(-0.5f, 0.5f,-0.5f)*size;
	vts[8+1].normal = tvector3(-0.5f, 0.5f,-0.5f);
	vts[8+1].uv = tvector2(1,0);
    vts[8+2].pos = tvector3(-0.5f, 0.5f, 0.5f)*size;
	vts[8+2].normal = tvector3(0.5f, 0.5f,-0.5f);
	vts[8+2].uv = tvector2(1,1);
    vts[8+3].pos = tvector3(-0.5f, -0.5f, 0.5f)*size;
	vts[8+3].normal = tvector3(-1, 0, 0);
	vts[8+3].uv = tvector2(0,1);
	// droite
    vts[12+0].pos = tvector3(0.5f, -0.5f,-0.5f)*size;
	vts[12+0].normal = tvector3(-0.5f,-0.5f,-0.5f);
	vts[12+0].uv = tvector2(0,0);
    vts[12+1].pos = tvector3(0.5f, 0.5f,-0.5f)*size;
	vts[12+1].normal = tvector3(-0.5f, 0.5f,-0.5f);
	vts[12+1].uv = tvector2(1,0);
    vts[12+2].pos = tvector3(0.5f, 0.5f, 0.5f)*size;
	vts[12+2].normal = tvector3(0.5f, 0.5f,-0.5f);
	vts[12+2].uv = tvector2(1,1);
    vts[12+3].pos = tvector3(0.5f, -0.5f, 0.5f)*size;
	vts[12+3].normal = tvector3(1, 0, 0);
	vts[12+3].uv = tvector2(0,1);
	// arriere
    vts[16+0].pos = tvector3(-0.5f, -0.5f,-0.5f)*size;
	vts[16+0].normal = tvector3(-0.5f,-0.5f,-0.5f);
	vts[16+0].uv = tvector2(0,0);
    vts[16+1].pos = tvector3(0.5f, -0.5f,-0.5f)*size;
	vts[16+1].normal = tvector3(-0.5f, 0.5f,-0.5f);
	vts[16+1].uv = tvector2(1,0);
    vts[16+2].pos = tvector3(0.5f, 0.5f, -0.5f)*size;
	vts[16+2].normal = tvector3(0.5f, 0.5f,-0.5f);
	vts[16+2].uv = tvector2(1,1);
    vts[16+3].pos = tvector3(-0.5f, 0.5f, -0.5f)*size;
	vts[16+3].normal = tvector3(0, 0, -1);
	vts[16+3].uv = tvector2(0,1);
	// avant
    vts[20+0].pos = tvector3(-0.5f, -0.5f,0.5f)*size;
	vts[20+0].normal = tvector3(-0.5f,-0.5f,-0.5f);
	vts[20+0].uv = tvector2(0,0);
    vts[20+1].pos = tvector3(0.5f, -0.5f,0.5f)*size;
	vts[20+1].normal = tvector3(-0.5f, 0.5f,-0.5f);
	vts[20+1].uv = tvector2(1,0);
    vts[20+2].pos = tvector3(0.5f, 0.5f, 0.5f)*size;
	vts[20+2].normal = tvector3(0.5f, 0.5f,-0.5f);
	vts[20+2].uv = tvector2(1,1);
    vts[20+3].pos = tvector3(-0.5f, 0.5f, 0.5f)*size;
	vts[20+3].normal = tvector3(0, 0, 1);
	vts[20+3].uv = tvector2(0,1);



    normvt->Unlock();

	uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);

	int ind = 0;
	for (int i=0;i<36;i+=6)
	{
		if (ind & 4)
		{
			ids[i+5] = ind+0;
			ids[i+4] = ind+1;
			ids[i+3] = ind+2;
			ids[i+2] = ind+0;
			ids[i+1] = ind+2;
			ids[i+0] = ind+3;
		}
		else
		{
			ids[i+0] = ind+0;
			ids[i+1] = ind+1;
			ids[i+2] = ind+2;
			ids[i+3] = ind+0;
			ids[i+4] = ind+2;
			ids[i+5] = ind+3;
		}
		ind +=4;
	}
	/*
    ids[6] = 6;
    ids[7] = 5;
    ids[8] = 4;
    ids[9] = 7;
    ids[10] = 6;
    ids[11] = 4;

    ids[12] = 7;
    ids[13] = 4;
    ids[14] = 0;
    ids[15] = 3;
    ids[16] = 7;
    ids[17] = 0;

    ids[18] = 1;
    ids[19] = 5;
    ids[20] = 6;
    ids[21] = 1;
    ids[22] = 6;
    ids[23] = 2;

    ids[24] = 6;
    ids[25] = 7;
    ids[26] = 3;
    ids[27] = 2;
    ids[28] = 6;
    ids[29] = 3;

    ids[30] = 0;
    ids[31] = 4;
    ids[32] = 5;
    ids[33] = 0;
    ids[34] = 5;
    ids[35] = 1;
*/
    indices->Unlock();
	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateWireBox(const tvector3 &size, const tcolor &col)
{

    IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0, 8 );
    indices->Init(24);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedWireBox");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f,-0.5f,-0.5f)*size, tvector3( 0.5f, 0.5f, 0.5f)*size);
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_LINELIST);
    nnormGen->AddSubMesh(0, 24 );

    helpersMeshVT * vts = (helpersMeshVT*)normvt->Lock(VAL_WRITE);
    
    vts[0].pos = tvector3(-0.5f,-0.5f,-0.5f)*size;
    vts[0].col = (tulong)col;
    vts[1].pos = tvector3(-0.5f, 0.5f,-0.5f)*size;
    vts[1].col = (tulong)col;
    vts[2].pos = tvector3( 0.5f, 0.5f,-0.5f)*size;
    vts[2].col = (tulong)col;
    vts[3].pos = tvector3( 0.5f,-0.5f,-0.5f)*size;
    vts[3].col = (tulong)col;
    vts[4].pos = tvector3(-0.5f,-0.5f, 0.5f)*size;
    vts[4].col = (tulong)col;
    vts[5].pos = tvector3(-0.5f, 0.5f, 0.5f)*size;
    vts[5].col = (tulong)col;
    vts[6].pos = tvector3( 0.5f, 0.5f, 0.5f)*size;
    vts[6].col = (tulong)col;
    vts[7].pos = tvector3( 0.5f,-0.5f, 0.5f)*size;
    vts[7].col = (tulong)col;


    normvt->Unlock();

    uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);

    ids[0] = 0;
    ids[1] = 1;
    ids[2] = 1;
    ids[3] = 2;
    ids[4] = 2;
    ids[5] = 3;
    ids[6] = 3;
    ids[7] = 0;

    ids[8] = 4;
    ids[9] = 5;
    ids[10] = 5;
    ids[11] = 6;
    ids[12] = 6;
    ids[13] = 7;
    ids[14] = 7;
    ids[15] = 4;

    ids[16] = 0;
    ids[17] = 4;
    ids[18] = 1;
    ids[19] = 5;
    ids[20] = 2;
    ids[21] = 6;
    ids[22] = 3;
    ids[23] = 7;


    indices->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * Create2DDisk(float size, float segments)
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    normvt->Init(VAF_XYZ, int(segments) +2 ); //+1 for center, +1 for 1st vertex after center
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("Generated2DDisk");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-1.f,-1.f,0.f), tvector3( 1.f, 1.f, 0.f));
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLEFAN);
    nnormGen->AddSubMesh(0, int(segments) +2 );

    tvector3 * vts = (tvector3 *)normvt->Lock(VAL_WRITE);
    
	vts[0] = tvector3::zero;
	for (int i=1;i<segments +2;i++)
	{
		float ng = -((ZPI*2.0f)/(float)segments)*(i-1);
		vts[i] = tvector3(cosf(ng), sinf(ng), 0);
	}
    
    normvt->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


ZMesh * CreateWireGrid()
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    normvt->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0, 40 );
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedWireGrid");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3::zero, tvector3(10,0,10));
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_LINELIST);
    nnormGen->AddSubMesh(0, 40 );

    helpersMeshVT * vts = (helpersMeshVT*)normvt->Lock(VAL_WRITE);
    
	int i;
	for (i=0;i<10;i++)
	{
		vts[i*2].pos = tvector3(float(i),0,0);
		vts[i*2].col = (i==0)?(tulong)0x40404040:(tulong)0x80808080;
		vts[i*2+1].pos = tvector3(float(i), 0,10.f);
		vts[i*2+1].col = (i==0)?(tulong)0x40404040:(tulong)0x80808080;
	}
	for (i=0;i<10;i++)
	{
		vts[i*2+20].pos = tvector3(0, 0, float(i));
		vts[i*2+20].col = (i==0)?(tulong)0x40404040:(tulong)0x80808080;
		vts[i*2+21].pos = tvector3(10.f, 0, float(i));
		vts[i*2+21].col = (i==0)?(tulong)0x40404040:(tulong)0x80808080;
	}


    normvt->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh* CreateSphere(int nbSeg, float rayon)
{
    tulong    ind = 0;
    tulong    i,j;
    float    theta1,theta2,theta3;

	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
	int nbVts = nbSeg*(nbSeg+1);
    normvt->Init(VAF_XYZ|VAF_NORMAL|VAF_TEX0, nbVts );
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedSphere");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3::zero, tvector3(10,0,10));
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLESTRIP);
    nnormGen->AddSubMesh(0, nbVts );
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-rayon, -rayon, -rayon), tvector3(rayon, rayon, rayon));
	nnormGen->GetBVolume().SetBSphere(vector4(0, 0, 0, rayon));

    helpersMeshVT2 * vts = (helpersMeshVT2*)normvt->Lock(VAL_WRITE);

	tvector3 centre = tvector3::zero;
    //// Init Vertices

    if (rayon < 0)
        rayon = -rayon;

    if (nbSeg < 4 || rayon <= 0)
    {
        return NULL;
    }

    for (j=0;j<(tulong)(nbSeg>>1);j++)
    {
        theta1 = j * PI_MUL_2 / nbSeg - PI_DIV_2;
        theta2 = (j + 1) * PI_MUL_2 / nbSeg - PI_DIV_2;

        for (i=0;i<=(tulong)(nbSeg);i++)
        {
            theta3 = i * PI_MUL_2 / nbSeg;

			tvector3 nnorm;
			nnorm.x    =    MathCos(theta2) * MathCos(theta3);
            nnorm.y    =    MathSin(theta2);
            nnorm.z    =    MathCos(theta2) * MathSin(theta3);
			nnorm.Normalize();


			vts[ind].nx    =    nnorm.x;
            vts[ind].ny    =    nnorm.y;
            vts[ind].nz    =    nnorm.z;
            vts[ind].u     =    i/(float)nbSeg;
            vts[ind].v     =    2*(j+1)/(float)nbSeg;
            vts[ind].x     =    centre.x + rayon * vts[ind].nx;
            vts[ind].y     =    centre.y + rayon * vts[ind].ny;
            vts[ind].z     =    centre.z + rayon * vts[ind].nz;
            //vts[ind].diffuse = color;
            ind++;

			nnorm.x    =    MathCos(theta1) * MathCos(theta3);
            nnorm.y    =    MathSin(theta1);
            nnorm.z    =    MathCos(theta1) * MathSin(theta3);
			nnorm.Normalize();
            vts[ind].nx     =    nnorm.x;
            vts[ind].ny     =    nnorm.y;
            vts[ind].nz     =    nnorm.z;
            vts[ind].u      =    (float) (i/(float)nbSeg);
            vts[ind].v      =    (float) (2*(j)/(float)nbSeg);
            vts[ind].x      =    centre.x + rayon * vts[ind].nx;
            vts[ind].y      =    centre.y + rayon * vts[ind].ny;
            vts[ind].z      =    centre.z + rayon * vts[ind].nz;
            //vts[ind].diffuse = color;
            ind++;

        }
    }
	normvt->Unlock();

	return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh* CreateQuad(const tvector3& vt1, const tvector3& vt2, const tvector3& vt3, const tvector3& vt4, 
				  const tvector2& uv1, const tvector2& uv2, const tvector2& uv3, const tvector2& uv4)
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    normvt->Init(VAF_XYZ|VAF_TEX0, 6 );
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedQuad");
#endif
	tvector3 bmin, bmax;
	bmin = vt1;
	bmin.Minimize(vt2);
	bmin.Minimize(vt3);
	bmin.Minimize(vt4);
	bmax = vt1;
	bmax.Maximize(vt2);
	bmax.Maximize(vt3);
	bmax.Maximize(vt4);


	nnormGen->GetBVolume().SetBoxMinMax(bmin, bmax);
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, 6 );

    helpersMeshVT3 * vts = (helpersMeshVT3*)normvt->Lock(VAL_WRITE);
    
	vts[0].vt = vt1;
	vts[0].uv = uv1;
	vts[1].vt = vt2;
	vts[1].uv = uv2;
	vts[2].vt = vt3;
	vts[2].uv = uv3;

	vts[3].vt = vt1;
	vts[3].uv = uv1;
	vts[4].vt = vt3;
	vts[4].uv = uv3;
	vts[5].vt = vt4;
	vts[5].uv = uv4;

    normvt->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh* CreatePyramid(float width, float depth, float height, const tcolor & col)
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0, 5 );
    indices->Init(18);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedPyramid");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f * width,-0.5f * depth,-0.5f*height),
		tvector3(0.5f * width,0.5f * depth,0.5f*height));
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, 18 );

    helpersMeshVT * vts = (helpersMeshVT*)normvt->Lock(VAL_WRITE);
    
    vts[0].pos = tvector3(-0.5f * width,-0.5f * depth,-0.5f*height);
	vts[0].u = 241.f/1024.f;
	vts[0].v = 285.f/1024.f;
    vts[0].col = (tulong)col;
    vts[1].pos = tvector3(-0.5f * width, 0.5f * depth,-0.5f*height);
	vts[1].u = 241.f/1024.f;
	vts[1].v = 285.f/1024.f;
    vts[1].col = (tulong)col;
    vts[2].pos = tvector3( 0.5f * width, 0.5f * depth,-0.5f*height);
	vts[2].u = 241.f/1024.f;
	vts[2].v = 285.f/1024.f;
    vts[2].col = (tulong)col;
    vts[3].pos = tvector3( 0.5f * width,-0.5f * depth,-0.5f*height);
	vts[3].u = 241.f/1024.f;
	vts[3].v = 285.f/1024.f;
    vts[3].col = (tulong)col;
    vts[4].pos = tvector3(0,0, 0.5f*height);
	vts[4].u = 241.f/1024.f;
	vts[4].v = 285.f/1024.f;
    vts[4].col = (tulong)col;


    normvt->Unlock();

    uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);

    ids[0] = 0;
    ids[1] = 1;
    ids[2] = 2;
    ids[3] = 0;
    ids[4] = 2;
    ids[5] = 3;

    ids[6] = 4;
    ids[7] = 0;
    ids[8] = 1;

    ids[9] = 4;
    ids[10] = 1;
    ids[11] = 2;

    ids[12] = 4;
    ids[13] = 2;
    ids[14] = 3;

    ids[15] = 4;
    ids[16] = 3;
    ids[17] = 0;
    


    indices->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateDummySphere()
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
	int nbVts = 32*2 * 3 + 6;// 32 segments * 2 vertices par cercle * 3 cercles + 6 pour les axes
    normvt->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0, nbVts ); 
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedDummySphere");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f,-0.5f,-0.5f), tvector3( 0.5f, 0.5f, 0.5f));
	nnormGen->GetBVolume().SetBSphere(vector4(0,0,0,0.5f));
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_LINELIST);
    nnormGen->AddSubMesh(0, nbVts );

    helpersMeshVT * vts = (helpersMeshVT*)normvt->Lock(VAL_WRITE);
	unsigned long col = 0xFFFF00FF;

	// Z = 0
	for (int i=0;i<32;i++)
	{
		float ng = ZPI*2/32.f*float(i);
		vts[i*2].pos = tvector3(cos(ng), sin(ng), 0);
		vts[i*2].col = col;
		ng = ZPI*2/32.f*float(i+1);
		vts[i*2+1].pos = tvector3(cos(ng), sin(ng), 0);
		vts[i*2+1].col = col;
	}

    
	// Y = 0
	for (int i=0;i<32;i++)
	{
		float ng = ZPI*2/32.f*float(i);
		vts[i*2 + 64].pos = tvector3(cos(ng), 0, sin(ng));
		vts[i*2 + 64].col = col;
		ng = ZPI*2/32.f*float(i+1);
		vts[i*2+1 + 64].pos = tvector3(cos(ng), 0, sin(ng));
		vts[i*2+1 + 64].col = col;
	}

	// X = 0
	for (int i=0;i<32;i++)
	{
		float ng = ZPI*2/32.f*float(i);
		vts[i*2 + 128].pos = tvector3(0, cos(ng), sin(ng));
		vts[i*2 + 128].col = col;
		ng = ZPI*2/32.f*float(i+1);
		vts[i*2+1 + 128].pos = tvector3(0, cos(ng), sin(ng));
		vts[i*2+1 + 128].col = col;
	}
    normvt->Unlock();


	vts[192+0].pos = tvector3(0, -1, 0);
	vts[192+0].col = col;
	vts[192+1].pos = tvector3(0, 1, 0);
	vts[192+1].col = col;

	vts[192+2].pos = tvector3(0, 0, -1);
	vts[192+2].col = col;
	vts[192+3].pos = tvector3(0, 0, 1);
	vts[192+3].col = col;


	vts[192+4].pos = tvector3(-1, 0, 0);
	vts[192+4].col = col;
	vts[192+5].pos = tvector3(1, 0, 0);
	vts[192+5].col = col;

	nnormGen->ComputeBSphere();

    return nnormGen;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateDummyCone()
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
	int nbVts = 32+ 8;//32*2 * 3 + 6;// 32 segments * 2 vertices par cercle * 3 cercles + 6 pour les axes
    normvt->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0, nbVts ); 
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedDummyCone");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f,-0.5f,0), tvector3( 0.5f, 0.5f, 1.f));
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_LINELIST);
    nnormGen->AddSubMesh(0, nbVts );

    helpersMeshVT * vts = (helpersMeshVT*)normvt->Lock(VAL_WRITE);
	unsigned long col = 0xFFFF00FF;

	// Z = 1
	for (int i=0;i<16;i++)
	{
		float ng = ZPI*2/16.f*float(i);
		vts[i*2].pos = tvector3(cos(ng)*0.5f, sin(ng)*0.5f, 1);
		vts[i*2].col = col;

		ng = ZPI*2/16.f*float(i+1);
		vts[i*2+1].pos = tvector3(cos(ng)*0.5f, sin(ng)*0.5f, 1);
		vts[i*2+1].col = col;
	}

	// lines
	int stidx = 32;
	vts[stidx].pos = tvector3(0, 0, 0);
	vts[stidx].col = col;
	vts[stidx+1].pos = tvector3(0.5, 0, 1);
	vts[stidx+1].col = col;

	vts[stidx+2].pos = tvector3(0, 0, 0);
	vts[stidx+2].col = col;
	vts[stidx+3].pos = tvector3(-0.5, 0, 1);
	vts[stidx+3].col = col;

	vts[stidx+4].pos = tvector3(0, 0, 0);
	vts[stidx+4].col = col;
	vts[stidx+5].pos = tvector3(0, 0.5, 1);
	vts[stidx+5].col = col;


	vts[stidx+6].pos = tvector3(0, 0, 0);
	vts[stidx+6].col = col;
	vts[stidx+7].pos = tvector3(0, -0.5, 1);
	vts[stidx+7].col = col;

    normvt->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateCone(float radius, float length, int nbSegments)
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ, nbSegments+1 );
	int nbidx = (nbSegments-2 + nbSegments) *3;
    indices->Init(nbidx);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedCone");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-1.f*radius,-1.f*radius,0), tvector3( radius, radius, length));
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, nbidx );

    tvector3 * vts = (tvector3*)normvt->Lock(VAL_WRITE);
    
	for (int i=0;i<nbSegments;i++)
	{
		float ng = ZPI*2/float(nbSegments)*float(i);
		vts[i] = tvector3(cos(ng)*0.5f, sin(ng)*0.5f, 1);
	}
	vts[nbSegments] = tvector3::zero;
    normvt->Unlock();

    uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);

	// cap
	for (int i=0;i<(nbSegments-2);i++)
	{
		ids[i*3 + 2] = 0;
		ids[i*3 + 1] = (i+1)%nbSegments;
		ids[i*3 + 0] = (i+2)%nbSegments;
	}

	// sides
	int stx = (nbSegments-2)*3;
	for (int i=0;i<nbSegments;i++)
	{
		ids[stx + i*3 + 0] = nbSegments;
		ids[stx + i*3 + 1] = i;
		ids[stx + i*3 + 2] = (i+1)%nbSegments;
	}
    indices->Unlock();


	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * CreateSkyBox()
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ, 8 );
	int nbidx = 36;
    indices->Init(nbidx);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedSkybox");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3::zero, tvector3::zero);
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, nbidx );

    tvector3 * vts = (tvector3*)normvt->Lock(VAL_WRITE);
    
    vts[0] = tvector3(-0.558012f,-0.558012f,-0.558012f);
    vts[1] = tvector3(-0.558012f, 0.558012f,-0.558012f);
    vts[2] = tvector3( 0.558012f, 0.558012f,-0.558012f);
    vts[3] = tvector3( 0.558012f,-0.558012f,-0.558012f);
    vts[4] = tvector3(-0.558012f,-0.558012f, 0.558012f);
    vts[5] = tvector3(-0.558012f, 0.558012f, 0.558012f);
    vts[6] = tvector3( 0.558012f, 0.558012f, 0.558012f);
    vts[7] = tvector3( 0.558012f,-0.558012f, 0.558012f);

    normvt->Unlock();

    uint16_t* indexbuffer = (uint16_t*)indices->Lock(VAL_WRITE);
/*
	indexbuffer[0] = 0;
	indexbuffer[1] = 1;
	indexbuffer[2] = 2;
	indexbuffer[3] = 0;
	indexbuffer[4] = 2;
	indexbuffer[5] = 3;

	indexbuffer[6] = 6;
	indexbuffer[7] = 5;
	indexbuffer[8] = 4;
	indexbuffer[9] = 7;
	indexbuffer[10] = 6;
	indexbuffer[11] = 4;

	indexbuffer[12] = 7;
	indexbuffer[13] = 4;
	indexbuffer[14] = 0;
	indexbuffer[15] = 3;
	indexbuffer[16] = 7;
	indexbuffer[17] = 0;

	indexbuffer[18] = 1;
	indexbuffer[19] = 5;
	indexbuffer[20] = 6;
	indexbuffer[21] = 1;
	indexbuffer[22] = 6;
	indexbuffer[23] = 2;

	indexbuffer[24] = 6;
	indexbuffer[25] = 7;
	indexbuffer[26] = 3;
	indexbuffer[27] = 2;
	indexbuffer[28] = 6;
	indexbuffer[29] = 3;

	indexbuffer[30] = 0;
	indexbuffer[31] = 4;
	indexbuffer[32] = 5;
	indexbuffer[33] = 0;
	indexbuffer[34] = 5;
	indexbuffer[35] = 1;
*/

	indexbuffer[0] = 2;
	indexbuffer[1] = 1;
	indexbuffer[2] = 0;
	indexbuffer[3] = 3;
	indexbuffer[4] = 2;
	indexbuffer[5] = 0;

	indexbuffer[6] = 4;
	indexbuffer[7] = 5;
	indexbuffer[8] = 6;
	indexbuffer[9] = 4;
	indexbuffer[10] = 6;
	indexbuffer[11] = 7;

	indexbuffer[12] = 0;
	indexbuffer[13] = 4;
	indexbuffer[14] = 7;
	indexbuffer[15] = 0;
	indexbuffer[16] = 7;
	indexbuffer[17] = 3;

	indexbuffer[18] = 6;
	indexbuffer[19] = 5;
	indexbuffer[20] = 1;
	indexbuffer[21] = 2;
	indexbuffer[22] = 6;
	indexbuffer[23] = 1;

	indexbuffer[24] = 3;
	indexbuffer[25] = 7;
	indexbuffer[26] = 6;
	indexbuffer[27] = 3;
	indexbuffer[28] = 6;
	indexbuffer[29] = 2;

	indexbuffer[30] = 5;
	indexbuffer[31] = 4;
	indexbuffer[32] = 0;
	indexbuffer[33] = 1;
	indexbuffer[34] = 5;
	indexbuffer[35] = 0;

	indices->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh * BuildScreenQuad(float SizeX, float SizeY)
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    normvt->Init(VAF_XYZRHW|VAF_TEX0, 4 );
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedScreenQuad");
#endif
	nnormGen->GetBVolume().SetBoxMinMax(tvector3::zero, tvector3::zero);
    nnormGen->SetVertexBuffer(normvt);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLESTRIP);
    nnormGen->AddSubMesh(0, 4 );


	float fWidth5 = (float)SizeX - 0.5f;
	float fHeight5 = (float)SizeY - 0.5f;

	float fTexWidth1 = (float)1;
	float fTexHeight1 = (float)1;



    helpersMeshVT4 * vts = (helpersMeshVT4*)normvt->Lock(VAL_WRITE);
    
	vts[0].vt = vector4(fWidth5, -0.5f, 0.0f, 1.0f);
	vts[0].uv = tvector2(fTexWidth1, 0.0f);

	vts[1].vt = vector4(fWidth5, fHeight5, 0.0f, 1.0f);
	vts[1].uv = tvector2(fTexWidth1, fTexHeight1);

	vts[2].vt = vector4(-0.5f, -0.5f, 0.0f, 1.0f);
	vts[2].uv = tvector2(0.0f, 0.0f);

	vts[3].vt = vector4(-0.5f, fHeight5, 0.0f, 1.0f);
	vts[3].uv = tvector2(0.0f, fTexHeight1);

    normvt->Unlock();

	nnormGen->ComputeBSphere();

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMesh* CreateGeodesicSphere(int anbIter, float radius)
{
	uint16_t *Ind = new uint16_t [24];
	int curnbindices = 24;
	tvector3 *Verts = new tvector3 [6];
	int curnbverts = 6;

	// build iteration 0

	Verts[0] = tvector3(0, 1, 0) * radius;

	Verts[1] = tvector3(1, 0, 0) * radius;
	Verts[2] = tvector3(0, 0, 1) * radius;
	Verts[3] = tvector3(-1, 0, 0) * radius;
	Verts[4] = tvector3(0, 0, -1) * radius;

	Verts[5] = tvector3(0, -1, 0) * radius;

	Ind[0] = 0;
	Ind[1] = 1;
	Ind[2] = 2;

	Ind[3] = 0;
	Ind[4] = 2;
	Ind[5] = 3;

	Ind[6] = 0;
	Ind[7] = 3;
	Ind[8] = 4;

	Ind[9] = 0;
	Ind[10] = 4;
	Ind[11] = 1;

	Ind[12] = 2;
	Ind[13] = 1;
	Ind[14] = 5;

	Ind[15] = 3;
	Ind[16] = 2;
	Ind[17] = 5;

	Ind[18] = 4;
	Ind[19] = 3;
	Ind[20] = 5;

	Ind[21] = 1;
	Ind[22] = 4;
	Ind[23] = 5;
/*

	Ind[2] = 0;
	Ind[1] = 1;
	Ind[0] = 2;

	Ind[5] = 0;
	Ind[4] = 2;
	Ind[3] = 3;

	Ind[8] = 0;
	Ind[7] = 3;
	Ind[6] = 4;

	Ind[11] = 0;
	Ind[10] = 4;
	Ind[9] = 1;

	Ind[14] = 2;
	Ind[13] = 1;
	Ind[12] = 5;

	Ind[17] = 3;
	Ind[16] = 2;
	Ind[15] = 5;

	Ind[20] = 4;
	Ind[19] = 3;
	Ind[18] = 5;

	Ind[23] = 1;
	Ind[22] = 4;
	Ind[21] = 5;
*/
	for (int iter = 0;iter<anbIter; iter++)
	{
		int newnbindices = curnbindices*4;
		int newnbverts = (curnbindices/3) * 6;
		uint16_t *nextInd = new uint16_t [newnbindices];
		tvector3 *nextVerts = new tvector3 [newnbverts];

		tvector3 *pnv = nextVerts;
		uint16_t *pni = nextInd;
		int vtDecal = 0;

		for (int i=0;i<curnbindices;i+=3)
		{

			*pnv++ = Verts[Ind[i]];
			*pnv++ = Verts[Ind[i+1]];
			*pnv++ = Verts[Ind[i+2]];


			tvector3 midv1 = (Verts[Ind[i]] + Verts[Ind[i+1]]) * 0.5f;
			midv1.Normalize();
			midv1 *= radius;

			*pnv++ = midv1;

			midv1 = (Verts[Ind[i+1]] + Verts[Ind[i+2]]) * 0.5f;
			midv1.Normalize();
			midv1 *= radius;

			*pnv++ = midv1;

			midv1 = (Verts[Ind[i+2]] + Verts[Ind[i]]) * 0.5f;
			midv1.Normalize();
			midv1 *= radius;

			*pnv++ = midv1;



			// 12 indices

			*pni++ = 0 + vtDecal;
			*pni++ = 3 + vtDecal;
			*pni++ = 5 + vtDecal;

			*pni++ = 3 + vtDecal;
			*pni++ = 4 + vtDecal;
			*pni++ = 5 + vtDecal;

			*pni++ = 3 + vtDecal;
			*pni++ = 1 + vtDecal;
			*pni++ = 4 + vtDecal;

			*pni++ = 5 + vtDecal;
			*pni++ = 4 + vtDecal;
			*pni++ = 2 + vtDecal;

			vtDecal += 6;
		}
		delete [] Ind;
		delete [] Verts;

		Ind = nextInd;
		Verts = nextVerts;
		curnbindices = newnbindices;
		curnbverts = newnbverts;
	}

	int optiNbVt;
	int optiNbInd = curnbindices;
	tvector3 *optiVt = new tvector3 [curnbverts];
	//uint16 *optiInd = new uint16 [curnbindices];

	ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
#ifdef _DEBUG
	nnormGen->SetName("GeneratedGeodesicSphere");
#endif
#if 1
    nnormGen->RemoveRedundancy2(Verts, curnbverts, sizeof(tvector3), Ind, optiNbInd,
        optiVt,
        &optiNbVt);
#else
	memcpy(optiVt, Verts, curnbverts*sizeof(tvector3));


	optiNbInd = curnbindices;
	optiNbVt = curnbverts;
#endif
	delete [] Verts;

	// fill struct
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ|VAF_NORMAL, optiNbVt );
    indices->Init(optiNbInd);

	nnormGen->GetBVolume().SetBoxMinMax(tvector3(-0.5f,-0.5f,-0.5f)*radius, tvector3( 0.5f, 0.5f, 0.5f)*radius);
	nnormGen->GetBVolume().SetBSphere(vector4(0,0,0,radius*0.5f));
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);
    
    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, optiNbInd );

    tvector3* vts = (tvector3*)normvt->Lock(VAL_WRITE);
	for (int i = 0;i< optiNbVt*2;i+=2)
	{
		vts[i+1] = vts[i] = optiVt[i>>1];
		vts[i+1].Normalize();
	}
    normvt->Unlock();

    uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);
	memcpy(ids, Ind, sizeof(uint16_t)*optiNbInd);
    indices->Unlock();


	delete [] Ind;
	delete [] optiVt;

	

    return nnormGen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMeshInstance * AddRect(float x, float y, float sx, float sy, float u1, float v1, float u2, float v2)
{
    ZMeshInstance * mi = (ZMeshInstance*)_New(ZMeshInstance);
#ifdef _DEBUG
	mi->SetName("ZGuiRectangle");
#endif

    IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
    IIndexArray * indices = GDD->NewIndexArray(); ////
	DPIA(indices);
    normvt->Init(VAF_XYZ|VAF_TEX0, 4 );
    indices->Init(6);
    ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
    nnormGen->SetVertexBuffer(normvt);
    nnormGen->SetIndexBuffer(indices);

    nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
    nnormGen->AddSubMesh(0, 6 );

    mi->SetMesh(nnormGen);

    guiMeshVT * vts = (guiMeshVT*)normvt->Lock(VAL_WRITE);

    vts[0].pos = tvector3(x, y, 0);
    vts[0].uv =  tvector2(u1, v1);
    vts[1].pos = tvector3(x+sx, y, 0);
    vts[1].uv =  tvector2(u2, v1);
    vts[2].pos = tvector3(x+sx, y+sy, 0);
    vts[2].uv =  tvector2(u2, v2);
    vts[3].pos = tvector3(x,y+sy, 0);
    vts[3].uv =  tvector2(u1, v2);

    normvt->Unlock();

    uint16_t* ids = (uint16_t*)indices->Lock(VAL_WRITE);
	
    ids[0] = 2;
    ids[1] = 1;
    ids[2] = 0;
    ids[3] = 3;
    ids[4] = 2;
    ids[5] = 0;

    indices->Unlock();

    mi->GetTransform()->SetLocalMatrix(tmatrix::identity);
    mi->GetTransform()->Update();

    mi->SetGui2DMesh(true);
    return mi;
}
