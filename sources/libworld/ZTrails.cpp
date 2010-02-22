///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTrails.cpp
// Creation : 02/08/2007
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
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZTrails)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZTrails::Init(uint32 aNbPairs, ZFx *afx, ZTexture *ptex)
{
	if (mTrailVT)
		delete [] mTrailVT;
	mTrailVT = new trailVT[aNbPairs*2];

	if (mMesh)
	{
		mMesh->GetTransform()->RemoveFromScene();
		mMesh->GetTransform()->SubRef();
	}
    mMesh.Attach((ZMeshInstance*)_New(ZMeshInstance));

    ZMesh *msh = (ZMesh*)_New(ZMesh);
    msh->SetPrimitive(PRIM_TRIANGLESTRIP);
    msh->SetVertexBuffer(GDD->NewVertexArray());
    //msh->GetVertexBuffer()->Init(VAF_XYZ|VAF_TEX0|VAF_NORMAL|VAF_TEX1, aNbPairs*2);
	VRAMService->AddVertexArrayJob(msh, msh->GetVertexBuffer(), true,
		NULL, aNbPairs*2, false, VAF_XYZ|VAF_TEX0|VAF_NORMAL|VAF_TEX1, false);

    msh->AddSubMesh(0, aNbPairs*2);
    mMesh->SetName("Trail");

    mMesh->SetMesh(msh);
	mMesh->SetTransparent(true);
	mMesh->GetTransform()->AddToScene(GScene);


    mMaxIndex = aNbPairs*2;

    ZMaterial *pMat = mMesh->GetMaterial(0);
    pMat->addTexture(ptex);
    pMat->setEffect(afx);
	pMat->connectEffect(true, false);
	mTrailColor = pMat->getNamedParam("Color");
	mTrailColor->setVector(vector4(1,1,1,1));
	mTrailColor->Apply();

}

void ZTrails::SetLastPair(const tvector3 &vt, float aAlpha, float aWidth, float timeEllapsed)
{
    if (!mMesh) return;


    trailVT *vts = (trailVT *)mTrailVT;//mMesh->GetMesh()->GetVertexBuffer()->Lock(VAL_WRITE);

	vts += (mMaxIndex-2);

    vts->pos = vt;
	vts++;
	vts->pos = vt;

	UpdateAlphaAndBB(timeEllapsed);

	IVertexArray *pva = mMesh->GetMesh()->GetVertexBuffer();
	VRAMService->AddVertexArrayJob(mMesh->GetMesh(), pva,
		false, mTrailVT, mIndex, false, pva->GetFormat(), false);
}

void ZTrails::AddPair(const tvector3 &vt, float aAlpha, float aWidth, float timeEllapsed)
{
    if (!mMesh) return;


	// copy
	memcpy(&mTrailVT[0], &mTrailVT[2], sizeof(trailVT) * (mMaxIndex-2) );

	// --

    trailVT *vts = (trailVT *)mTrailVT;//mMesh->GetMesh()->GetVertexBuffer()->Lock(VAL_WRITE);
	vts += (mMaxIndex-2);

    vts->pos = vt;
	vts->dir = mLastDir;

	vts->uv = tvector2(0.f, 0.f);

    vts->width = aWidth;
    vts->alpha = aAlpha;

    // VT 2
    vts ++;

	vts->pos = vt;
	vts->dir = mLastDir;

    vts->uv = tvector2(1.f, 1.f);

    vts->width = aWidth;
    vts->alpha = aAlpha;




    mLastDir = vt-mLastPair;
    mLastPair = vt;


	UpdateAlphaAndBB(timeEllapsed);

	IVertexArray *pva = mMesh->GetMesh()->GetVertexBuffer();
	VRAMService->AddVertexArrayJob(mMesh->GetMesh(), pva,
		false, mTrailVT, mIndex, false, pva->GetFormat(), false);

}