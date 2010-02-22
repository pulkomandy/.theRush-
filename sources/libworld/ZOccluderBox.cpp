///////////////////////////////////////////////////////////////////////////////////////////////////
// Azote Studio Engine
// File Name : ZOccluderBox.cpp
// Creation : 18/08/2001
// Author : Cedric Guillemet
// Description : 
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

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"


///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZOccluderBox)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

#define Max(a,b) ((a>b)?a:b)
#define OCCLUDER

#ifdef OCCLUDER
#define MAX_OCCLUDER_PLANES	((int)(256/9)*9)
#else
#define MAX_OCCLUDER_PLANES	(1)
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////

struct FOccluderBox
{
	tvector4	mCenter;
	tvector4	mVertex[8];
	tvector4	mPlanes[6];
};

struct FActiveOccluder
{
	float	mSolidAngle;
	int	    mNbPlanes;
	float	mPad1;
	float	mPad2;
	tvector4	mPlanes[9];
};

static	tvector3 BoxCorners[8] =
{
	tvector3(1.f, -1.f, -1.f),
	tvector3(-1.f, -1.f, -1.f),
	tvector3(-1.f, 1.f, -1.f),
	tvector3(1.f, 1.f, -1.f),
	tvector3(-1.f, -1.f, 1.f),
	tvector3(1.f, -1.f, 1.f),
	tvector3(1.f, 1.f, 1.f),
	tvector3(-1.f, 1.f, 1.f),
};


struct Edge
{
	int vertex[2];
	int face;
};

struct Quad
{
	Edge edge[4];
};


static	int FaceVertexIndex[6][4] =
{
	{0,1,2,3},
	{0,3,6,5},
	{2,7,6,3},
	{4,5,6,7},
	{4,7,2,1},
	{5,4,1,0},
};

static Quad BoxQuads[6] =
{
	// face 0

	{{
		{{0,1}, 5},
		{{1,2}, 4},
		{{2,3}, 2},
		{{3,0}, 1}
	}},

	// face 1
	{{
		{{0,3}, 0},
		{{3,6}, 2},
		{{6,5}, 3},
		{{5,0}, 5}
	}},

	// face 2
	{{
	
		{{3,2}, 0},
		{{2,7}, 4},
		{{7,6}, 3},
		{{6,3}, 1}
	}},

	// face 3
	{{
	
		{{4,5}, 5},
		{{5,6}, 1},
		{{6,7}, 2},
		{{7,4}, 4}
	}},


	// face 4
	{{
		{{1,4}, 5},
		{{4,7}, 3},
		{{7,2}, 2},
		{{2,1}, 0}
	}},

	// face 5
	{{
		{{1,0}, 0},
		{{0,5}, 1},
		{{5,4}, 3},
		{{4,1}, 4}
	}},
};


///////////////////////////////////////////////////////////////////////////////////////////////////

float gSilhouetteTreshold = 0.0f;

struct BoxSilhouette
{
	tvector4*	vertices;
	float		dots[6];
	unsigned int		visitedFaces;

	int findSilhouette(tvector4* out);
	tvector4* recurseFindSilhouette(int firstFace, tvector4* out);
};

///////////////////////////////////////////////////////////////////////////////////////////////////

int BoxSilhouette::findSilhouette(tvector4* out)
{
	visitedFaces = 0;
    int nbVTs = 0;
	for (int i=0; i<6; i++)
    {
		if (dots[i] < 0.0f)
        {
            for (int j=0;j<4;j++)
            {
                if (dots[BoxQuads[i].edge[j].face] >= 0)
                {
                    out[0] = vertices[BoxQuads[i].edge[j].vertex[0]];
                    out[1] = vertices[BoxQuads[i].edge[j].vertex[1]];
                    out+=2;
                    nbVTs +=2;
                }
            }
        }
    }
	return nbVTs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool					gDebugOccluderPlanes = false;
bool					gDebugSilhouette = false;
int						gAddNearPlane = 1;
bool					gRebuildOccluders = false;
bool					gEnableOccluders=true;
tarray<FOccluderBox>	    gOccluderBoxes;
FActiveOccluder	        gActiveOccluders[100];// = (FActiveOccluder*) SPAD_START;

int						gMaxCandidateOccluders = 100;
int						gMaxActiveOccluders = MAX_OCCLUDER_PLANES/9;
int						gNbActiveOccluders = 0;
float    				gMinSolidAngle = 0.0f;


///////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int IsSphereOccluded(const tvector4& aSphere)//, float aRadius) 
{
    FActiveOccluder*		pOccluder = &gActiveOccluders[0];
    for (int j= 0;j<gNbActiveOccluders; j++)
	{
		int num = pOccluder->mNbPlanes;
		int i;
		tvector4* pPlanes = &pOccluder->mPlanes[0];

		// sphere is occluded if it lies on all precomputed occluders halfplanes
		for (i=0; i<num; i++)
		{
			float d= DotProduct(pPlanes[i], aSphere) + pPlanes[i].w;
			if (d > -aSphere.w)
				break;
		}
		if (i == num)
			return 2;
		pOccluder++;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void addDynamicOccluder(ZTransform* aTransform) 
{
	int i;

	FOccluderBox	obox;
    tvector3		extend(1,1,1);
	const tmatrix& boxmat = aTransform->GetWorldMatrix();

	for (i=0; i<8; i++)
	{
        tvector3 bvt(BoxCorners[i].x, BoxCorners[i].y, BoxCorners[i].z),bvt2;
		bvt2.TransformPoint((bvt*extend), boxmat);
        obox.mVertex[i] = vector4(bvt2.x, bvt2.y, bvt2.z, 0);
	}

	for (i=0; i<6; i++)
	{
		tvector3 cross = obox.mVertex[FaceVertexIndex[i][1]];
		cross -= obox.mVertex[FaceVertexIndex[i][0]];
		tvector3 cr2 = obox.mVertex[FaceVertexIndex[i][2]];
		cr2 -= obox.mVertex[FaceVertexIndex[i][0]];

        cross.Cross(cr2);
		obox.mPlanes[i] = vector4(cross.x, cross.y, cross.z, 0);
        obox.mPlanes[i].Normalize();
		obox.mPlanes[i].w = -DotProduct(obox.mPlanes[i], obox.mVertex[FaceVertexIndex[i][0]]);
		obox.mVertex[i].w = fabs(obox.mPlanes[i].Dot( cross));		// save area in vertex w component
	}
    obox.mCenter = vector4(boxmat.m16[12], boxmat.m16[13], boxmat.m16[14], 0);
	obox.mCenter.w = Distance(obox.mCenter, obox.mVertex[0]);
	gOccluderBoxes.push_back(obox);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

static int compareOccluder(const void* elem1, const void* elem2)
{
	if (((FActiveOccluder*)elem1)->mSolidAngle > ((FActiveOccluder*)elem2)->mSolidAngle)
		return -1;
	else if (((FActiveOccluder*)elem1)->mSolidAngle < ((FActiveOccluder*)elem2)->mSolidAngle)
		return 1;
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int PreprocessOccluders(const tmatrix &invCameraMat)
{
	if (!GetInstancesCount(ZOccluderBox))
		return 0;

	PROFILER_START(PreprocessOccluders);

	tvector3 campos = invCameraMat.V4.position;
	tvector3 camdir = invCameraMat.V4.dir;

	FActiveOccluder* pActiveOccluder = &gActiveOccluders[0]; 
    tvector4 viewPoint = vector4(campos.x, campos.y, campos.z, 0);
    tvector4 viewDir = vector4(camdir.x, camdir.y, camdir.z, 0);
	float sqrFar = 1000.0f * 1000.0f;
	float sqrDist;
	int i;

	gNbActiveOccluders = 0;
	gOccluderBoxes.clear();

	ZOccluderBox *pocc = (ZOccluderBox*)FirstInstanceOf(ZOccluderBox);
	while (pocc)
	{
		addDynamicOccluder(pocc->GetTransform());

		pocc = (ZOccluderBox*)NI(pocc);
	}

    
    for (unsigned int ju = 0;ju<gOccluderBoxes.size(); ju++)
	{
		// todo : frustum culling of the  occluder (except far plane)
		// todo : compute solid angle to reorder occluder accordingly
		FOccluderBox *obox = &gOccluderBoxes[ju];

         //= oboxiter.Get();
		// check for far plane
		const tvector3 &oboxcenter = obox->mCenter;//pocc->GetTransform()->GetWorldMatrix().position;
		sqrDist= SquaredDistance(viewPoint, oboxcenter);
		if (sqrDist > sqrFar)
		{
			continue;
		}

		// check for near plane
		if (DotProduct(tvector3(viewDir), tvector3(oboxcenter - viewPoint)) < 0.0f)
		{
			continue;
		}

		// select planes of the occluder box that lies on the viewing direction
		// todo : reduce to 3 planes instead of 6 (due to box symetry)
		float invSqrDist = 1.0f/sqrDist;//Rcp(sqrDist);

		pActiveOccluder->mSolidAngle = 0.0f;

		BoxSilhouette	silhouette;

		silhouette.vertices = &obox->mVertex[0];

		for (i=0; i<6; i++)
		{
			tvector4 dir= obox->mVertex[FaceVertexIndex[i][0]];
			dir -= viewPoint;
			float vdotp = silhouette.dots[i] = DotProduct(obox->mPlanes[i], dir);

			// compute the maximum solidAngle of the box : -area * v.p/d.d
			pActiveOccluder->mSolidAngle = Max(-obox->mVertex[i].w * vdotp * invSqrDist, pActiveOccluder->mSolidAngle);

		}

		// exit if the occluder is not relevant enough
		if (pActiveOccluder->mSolidAngle < gMinSolidAngle)
			continue;


		int	  nPlanes = 0;
		tvector4*	pPlanes = &pActiveOccluder->mPlanes[0];

		// find silhouette
		tvector4		vertices[12];
        int			nVertices = silhouette.findSilhouette(vertices);

		// create a plane with a edge of the occluder and the viewpoint
        
		for (i=0; i<nVertices; i+=2)
		{
            //tplane plan(campos, vertices[i], vertices[i+1]);
            
			tvector3	v1 = vertices[i];
			v1 -= viewPoint;
			tvector3	v2 = vertices[i+1];
			v2 -= viewPoint;

            v1.Normalize();
            v2.Normalize();
			*pPlanes = CrossProduct(v1, v2);
			pPlanes->Normalize();
			
			pPlanes->w = - DotProduct(*pPlanes, vertices[i]);

			pPlanes++;
			nPlanes ++;

		}
    
		if (gAddNearPlane)
		{
			for (int i=0; i<6; i++)
			{
				if (silhouette.dots[i] < 0.0f)
				{
					pActiveOccluder->mPlanes[nPlanes] = obox->mPlanes[i];
					nPlanes++;
				}
			}
		}
        
		pActiveOccluder->mNbPlanes = nPlanes;

		pActiveOccluder++;
		gNbActiveOccluders++;

		if (gNbActiveOccluders >= gMaxCandidateOccluders)
			break;
		
	}
	

	if (gNbActiveOccluders)
	{
		qsort(gActiveOccluders, gNbActiveOccluders, sizeof(FActiveOccluder), compareOccluder);
		if (gNbActiveOccluders > gMaxActiveOccluders)
			gNbActiveOccluders = gMaxActiveOccluders;
	}

	PROFILER_END();
	return gNbActiveOccluders;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
