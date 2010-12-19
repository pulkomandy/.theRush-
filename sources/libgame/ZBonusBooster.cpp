#include "ZBonusBooster.h"
#include "ZBonusParameters.h"

#include "ZShip.h"

void ZBonusBooster::SetActive()
{
	mbActive = true;
	mActiveTime = -1;
}

void ZBonusBooster::Use()
{
	mActiveTime = GBonusParameters->BoosterTimeDuration; // parameters
	if (mbCanModifyShip) // server
	{
		mShip->EnableBooster(true);
	}
	if (mbCanModifyScene)
	{
		// Eye candy stuff
	}
}

void ZBonusBooster::Tick(float aTimeEllapsed)
{
	if (mbActive && (mActiveTime>0.f) )
	{
		mActiveTime -= aTimeEllapsed;
		if (mActiveTime <= 0.f)
		{
			mbActive = false;
			if (mbCanModifyShip)
			{
				mShip->EnableBooster(false);
			}
		}
	}
}

bool ZBonusBooster::IsActive() const
{
	return mbActive;
}
