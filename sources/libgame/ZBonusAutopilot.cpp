#include "ZBonusAutopilot.h"
#include "ZBonusParameters.h"

#include "ZShip.h"

void ZBonusAutopilot::SetActive()
{
	mbActive = true;
	mTimeBeforeDeactivated = -1;
}

void ZBonusAutopilot::Use()
{
	if (mbCanModifyScene)
		mShip->SetInteractionType(INTERACTION_IA);
	mTimeBeforeDeactivated = GBonusParameters->AutopilotTimeDuration; // bonus parameters
}

void ZBonusAutopilot::Tick(float aTimeEllapsed)
{
	if ( (mbActive) && (mTimeBeforeDeactivated>0.f) ) 
	{
		mTimeBeforeDeactivated -= aTimeEllapsed;
		if (mTimeBeforeDeactivated <= 0.f)
		{
			mbActive = false;
		}
		if (!mbActive)
		{
			if ( (mbCanModifyScene) && ( mShip->GetGameItBelongsTo()->GetClientShip() == mShip) )
				mShip->SetInteractionType(INTERACTION_IA);
		}
	}
}

bool ZBonusAutopilot::IsActive() const
{
	return mbActive;
}
