#ifndef ZBONUSPARAMETERS_H__
#define ZBONUSPARAMETERS_H__

#include "../libbase/ZBaseClass.h"
#include "../libbase/ZSerializator.h"

DECLAREZCLASS(ZBonusParameters);

class ZBonusParameters : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZBonusParameters();
	virtual ~ZBonusParameters()
	{
	}

	float SmokeTimeDuration;
	float DistanceBetweenTwoMines;
	int NumberOfMinesToDrop;
	float GravityGripTimeDuration;
	float EnergyQuantity;
	float DestructionShieldTimeDuration;
	float ProtectionShieldTimeDuration;
	float BoosterTimeDuration;
	float AutopilotTimeDuration;
	float MissileTTL;
	float MineTTL;
};

extern ZBonusParameters *GBonusParameters;
#endif
