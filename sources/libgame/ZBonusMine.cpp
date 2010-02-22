#include "stdafx.h"
#include "ZBonusMine.h"
#include "ZBonusParameters.h"

void ZBonusMine::SetActive()
{
	mbActive = true;
}

void ZBonusMine::Use()
{
	mNbMinesToDrop = GBonusParameters->NumberOfMinesToDrop;
	mLastPosition = mShip->GetTransform()->GetWorldMatrix().V4.position;
}

void ZBonusMine::Tick(float aTimeEllapsed)
{
	if (mbCanModifyShip)
	{
		float len = (mLastPosition - mShip->GetTransform()->GetWorldMatrix().V4.position).LengthSq();
		if ( len >= (GBonusParameters->DistanceBetweenTwoMines * GBonusParameters->DistanceBetweenTwoMines) )
		{
			tmatrix mat = mShip->GetTransform()->GetWorldMatrix();
			tmatrix mat2;
			mat2.Translation( 0, 0, -3.f );
			mat *= mat2;
			mShip->GetGameItBelongsTo()->SpawnMine(mat);
			
			mNbMinesToDrop --;

			if ( mNbMinesToDrop <= 0)
			{
				mbActive = false;
			}
		}
	}
}

bool ZBonusMine::IsActive() const
{
	return mbActive;
}