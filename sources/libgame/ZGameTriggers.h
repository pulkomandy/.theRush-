#ifndef ZGAMETRIGGERS_H__
#define ZGAMETRIGGERS_H__

#include "..\libbase\ZSerializator.h"
#include "..\libworld\ZScene.h"

DECLAREZCLASS(ZShip);


#define TGSHIPGROUP 0x1
#define TGSHIPFILTER 0xFFFE

#define TGTRACKBONUSGROUP 0x2
#define TGTRACKBONUSFILTER 0x1

#define TGMINEGROUP 0x4
#define TGMINEFILTER 0x1

class ShipTrackBonusTrigger : public ZTriggerCallback
{
public:
	void CallBack(ZTrigger* trig1, ZTrigger* trig2);
};

class TriggerResponders
{
public:
	TriggerResponders()
	{
		GScene->RegisterTriggerCallback(TGSHIPGROUP, TGTRACKBONUSGROUP, &mShipBonusTrigger);
	}
	virtual ~TriggerResponders()
	{
	}
protected:
	ShipTrackBonusTrigger mShipBonusTrigger;
};

#endif
