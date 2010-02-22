#include "stdafx.h"
#include "ZBonusSmoke.h"
#include "ZBonusParameters.h"

void ZBonusSmoke::SetActive()
{
	mbActive = true;
	mActiveTime = -1;
}

void ZBonusSmoke::Use()
{
	mActiveTime = GBonusParameters->SmokeTimeDuration; // parameters
	if (mbCanModifyShip) // server
	{
		mShip->EnableSmoke(true);
	}
	if (mbCanModifyScene)
	{
		// Eye candy stuff
	}
}

void ZBonusSmoke::Tick(float aTimeEllapsed)
{
	if (mbActive && (mActiveTime>0.f) )
	{
		mActiveTime -= aTimeEllapsed;
		if (mActiveTime <= 0.f)
		{
			mbActive = false;
			if (mbCanModifyShip)
			{
				mShip->EnableSmoke(false);
			}
		}
	}
}

bool ZBonusSmoke::IsActive() const
{
	return mbActive;
}