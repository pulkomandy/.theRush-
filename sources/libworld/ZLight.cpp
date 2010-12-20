///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZLight.cpp
// Creation : 26/11/2008
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

#include "ZLight.h"
#include "../libbase/ZFile.h"

BEGIN_SERIALIZATION(ZLight)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////

ZSpatialEntity* ZLight::Clone()
{
	ZLight *nLight = (ZLight*)_New(ZLight);
	nLight->mRadius = mRadius;
	nLight->mColor = mColor;
	nLight->mbActive = mbActive;
	nLight->mLength = mLength;
	nLight->mLightType = mLightType;
	nLight->mAttenNear = mAttenNear;
	nLight->mAngle1 = mAngle1;
	nLight->mAngle2 = mAngle2;

	return nLight;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZLight::LoadLight(ZFile *pFile)
{
	ZFile &mFile = (*pFile);
	mFile >> mbActive;
	mFile >> mRadius;
	mFile >> mColor;
	mFile >> mLength;
	mFile >> mLightType;
	mFile >> mAttenNear;
	mFile >> mAngle1;
	mFile >> mAngle2;
	tmatrix mt;
	mFile >> mt;
	mTransform->SetLocalMatrix(mt);
	mTransform->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZLight::SaveLight(ZFile *pFile)
{
	ZFile &mFile = (*pFile);
	mFile << mbActive;
	mFile << mRadius;
	mFile << mColor;
	mFile << mLength;
	mFile << mLightType;
	mFile << mAttenNear;
	mFile << mAngle1;
	mFile << mAngle2;

	tmatrix mt = mTransform->GetLocalMatrix();
	mFile << mt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
