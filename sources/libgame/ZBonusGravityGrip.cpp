#include "ZBonusGravityGrip.h"
#include "ZBonusParameters.h"
#include "ZShip.h"

void ZBonusGravityGrip::SetActive()
{
	mbActive = true;
	mActiveTime = -1;
}

void ZBonusGravityGrip::Use()
{
	mActiveTime = GBonusParameters->GravityGripTimeDuration; // parameters
	if (mbCanModifyShip) // server
	{
		mShip->EnableGravityGrip(true);
	}
	if (mbCanModifyScene)
	{
		// Eye candy stuff
	}
}

void ZBonusGravityGrip::Tick(float aTimeEllapsed)
{
	if (mbActive && (mActiveTime>0.f) )
	{
		mActiveTime -= aTimeEllapsed;
		if (mActiveTime <= 0.f)
		{
			mbActive = false;
			if (mbCanModifyShip)
			{
				mShip->EnableGravityGrip(false);
			}
		}
	}
}

bool ZBonusGravityGrip::IsActive() const
{
	return mbActive;
}
