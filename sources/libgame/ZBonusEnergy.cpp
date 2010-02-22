#include "stdafx.h"
#include "ZBonusEnergy.h"
#include "ZBonusParameters.h"

void ZBonusEnergy::SetActive()
{
	mbActive = true;
}

void ZBonusEnergy::Use()
{
	if (mbCanModifyShip)
		mShip->IncLife(GBonusParameters->EnergyQuantity); // bonus parameter
	mbActive = false;
}

void ZBonusEnergy::Tick(float aTimeEllapsed)
{
}

bool ZBonusEnergy::IsActive() const
{
	return mbActive;
}