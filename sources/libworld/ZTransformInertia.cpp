///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTransformInertia.h
// Creation : 28/11/2009
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

#include "ZTransformInertia.h"

///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_SERIALIZATION(ZTransformInertia)
END_SERIALIZATION()

void ZTransformInertia::ComputeWorldMatrix()
{
	// build base Matrix
	tmatrix basesc, basetr, basert;
	basert.RotationQuaternion(mBaseRotation);
	basetr.Translation(mBaseTranslation);
	basesc.Scaling(mBaseScale);
	tmatrix mBaseMatrix;

	mBaseMatrix = (basesc * basert * basetr );

	tmatrix res;
	if (mParent)
	{
		const tmatrix& mtpar = mParent->GetWorldMatrix();
		//mtpar.OrthoNormalize();
		res = mPivot * mLocalMatrix * mBaseMatrix * mtpar;
	}
	else
	{
		res = mPivot * mLocalMatrix * mBaseMatrix ;
	}
	tvector4 currentPos = mWorldMatrix.V4.position;
	const tvector4& targetPos = res.V4.position;

	mWorldMatrix = res;
/*
	static tvector4 prevff = tvector4::zero;
	tvector4 velocity = currentPos;
	velocity -= targetPos;
	velocity *= gTimer.GetEllapsed();
	tvector4 ff = velocity * 100.f * gTimer.GetEllapsed();
	*/
	mWorldMatrix.V4.position = targetPos;
	/*
	tvector4 nff;
	nff.Lerp(prevff, ff, 0.5f);
	mWorldMatrix.V4.position += nff;

	prevff = ff;
*/

	float DistSq = Distance(currentPos, targetPos);
	if ( DistSq > (mMaximumDistance * mMaximumDistance) )
	{
		tvector3 Vect = tvector3(targetPos)-tvector3(currentPos);
		Vect.Normalize();

		mWorldMatrix.V4.position = currentPos;
		tvector3 recal = Vect*mMaximumDistance;
		mWorldMatrix.V4.position = targetPos;
		mWorldMatrix.V4.position.x -= recal.x;
		mWorldMatrix.V4.position.y -= recal.y;
		mWorldMatrix.V4.position.z -= recal.z;
			

	}

	
	mWorldMatrix.V4.position.w = 1.f;
	mWorldMatrix.OrthoNormalize();
}
