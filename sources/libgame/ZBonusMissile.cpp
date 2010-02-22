#include "stdafx.h"
#include "ZBonusMissile.h"
#include "ZBonusParameters.h"

void ZBonusMissile::SetActive()
{
	mbMissileFired = false;
}

void ZBonusMissile::Use()
{
	if (!IsActive())
		return;
	if (mbCanModifyShip)
	{
		mShip->GetGameItBelongsTo()->SpawnMissile( mShip->GetShipPhysic()->GetTransform()->GetWorldMatrix() );
	}

	mbMissileFired = true;
}

void ZBonusMissile::Tick(float aTimeEllapsed)
{
}

bool ZBonusMissile::IsActive() const
{
	return (!mbMissileFired);
}