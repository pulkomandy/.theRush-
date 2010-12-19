#include "ZShipBonus.h"

#include "ZBonusMissile.h"
#include "ZBonusHomingMissile.h"
#include "ZBonusSmoke.h"
#include "ZBonusProtectionShield.h"
#include "ZBonusDestructionShield.h"
#include "ZBonusMine.h"
#include "ZBonusEnergy.h"
#include "ZBonusGravityGrip.h"
#include "ZBonusMachineGun.h"
#include "ZBonusBooster.h"
#include "ZBonusAutopilot.h"
#include "../libbase/ZProfiler.h"

ZShipBonus::~ZShipBonus()
{
	std::vector<ZBonus*>::iterator iter = mAvailableBonus.begin();
	for (; iter != mAvailableBonus.end() ; ++iter)
	{
		delete (*iter);
	}
}

void ZShipBonus::Init( ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene )
{
	if (mbInited) 
		return;

	mAvailableBonus.push_back(new ZBonusMissile( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusHomingMissile( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusSmoke( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusProtectionShield( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusDestructionShield( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusMine( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusEnergy( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusGravityGrip( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusBooster( pShip, bCanModifyShip, bCanModifyScene));
	mAvailableBonus.push_back(new ZBonusAutopilot( pShip, bCanModifyShip, bCanModifyScene));
	//mAvailableBonus.push_back(new ZBonusMachineGun( pShip, bCanModifyShip, bCanModifyScene));

	mbInited = true;
}


void ZShipBonus::Tick(float aTimeEllapsed)
{
	PROFILER_START(ZShipBonus::Tick);
	if (mCurrentBonus)
	{
		mCurrentBonus->Tick(aTimeEllapsed);
		if (!mCurrentBonus->IsActive())
		{
			mCurrentBonus = NULL;
			mCurrentBonusIndex = -1;
		}
	}
	PROFILER_END();
}

void ZShipBonus::GiveBonus(char aBonusNb)
{
	if (mCurrentBonus)
		return;
	mCurrentBonusIndex = aBonusNb;
	mCurrentBonus = mAvailableBonus[mCurrentBonusIndex];
	mCurrentBonus->SetActive();
}

void ZShipBonus::UseBonus()
{
	if (mCurrentBonus)
		mCurrentBonus->Use();
}

