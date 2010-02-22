#include "stdafx.h"
#include "ZBonusProtectionShield.h"
#include "ZBonusParameters.h"

void ZBonusProtectionShield::SetActive()
{
	mbActive = true;
	mActiveTime = -1;
}

void ZBonusProtectionShield::Use()
{
	mActiveTime = GBonusParameters->ProtectionShieldTimeDuration; // parameters
	if (mbCanModifyShip) // server
	{
		mShip->EnableProtectionShield(true);
	}
	if (mbCanModifyScene && mShip->GetShieldMesh())
	{
		mShip->GetShieldMesh()->SetVisible(true);
	}
}

void ZBonusProtectionShield::Tick(float aTimeEllapsed)
{
	if (mbActive && (mActiveTime>0.f) )
	{
		mActiveTime -= aTimeEllapsed;
		if (mActiveTime <= 0.f)
		{
			mbActive = false;
			if (mbCanModifyShip)
			{
				mShip->EnableProtectionShield(false);
			}
			if (mbCanModifyScene && mShip->GetShieldMesh())
			{
				mShip->GetShieldMesh()->SetVisible(true);
			}
		}
	}
}

bool ZBonusProtectionShield::IsActive() const
{
	return mbActive;
}