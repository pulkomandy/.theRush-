#include "ZGameTriggers.h"
#include "ZShip.h"
#include "ZTrackBonus.h"

void ShipTrackBonusTrigger::CallBack(ZTrigger* trig1, ZTrigger* trig2)
{
	ZShip *pShip;
	ZTrackBonus *pBonus;

	if ( trig1->GetUserPointer() && 
		( ((ZBaseClass*)trig1->GetUserPointer())->GetClassID() == ClassIDZShip) )
	{
		pShip = (ZShip*)trig1->GetUserPointer();
		pBonus = (ZTrackBonus*)trig2->GetUserPointer();
	}
	else
	{
		pShip = (ZShip*)trig2->GetUserPointer();
		pBonus = (ZTrackBonus*)trig1->GetUserPointer();
	}

	pBonus->Trigger(pShip);
}
