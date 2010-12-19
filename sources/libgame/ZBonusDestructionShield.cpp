#include "ZBonusDestructionShield.h"
#include "ZBonusParameters.h"

#include "ZShip.h"

void ZBonusDestructionShield::SetActive()
{
	mbActive = true;
	mActiveTime = -1;
}

void ZBonusDestructionShield::Use()
{
	mActiveTime = GBonusParameters->DestructionShieldTimeDuration; // parameters
	if (mbCanModifyShip) // server
	{
		mShip->EnableDestructionShield(true);
	}
	if (mbCanModifyScene && mShip->GetShieldMesh())
	{
		mShip->GetShieldMesh()->SetVisible(true);
	}
}

void ZBonusDestructionShield::Tick(float aTimeEllapsed)
{
	if (mbActive && (mActiveTime>0.f) )
	{
		mActiveTime -= aTimeEllapsed;
		if (mActiveTime <= 0.f)
		{
			mbActive = false;
			if (mbCanModifyShip)
			{
				mShip->EnableDestructionShield(false);
			}
			if (mbCanModifyScene && mShip->GetShieldMesh())
			{
				mShip->GetShieldMesh()->SetVisible(false);
			}
		}
	}
}

bool ZBonusDestructionShield::IsActive() const
{
	return mbActive;
}
