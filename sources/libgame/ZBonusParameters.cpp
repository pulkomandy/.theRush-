#include "ZBonusParameters.h"

BEGIN_SERIALIZATION(ZBonusParameters)
SERIALIZE_FLOAT(SmokeTimeDuration )
SERIALIZE_FLOAT( DistanceBetweenTwoMines )
SERIALIZE_I32( NumberOfMinesToDrop )
SERIALIZE_FLOAT( GravityGripTimeDuration )
SERIALIZE_FLOAT( EnergyQuantity )
SERIALIZE_FLOAT( DestructionShieldTimeDuration )
SERIALIZE_FLOAT( ProtectionShieldTimeDuration )
SERIALIZE_FLOAT( BoosterTimeDuration )
SERIALIZE_FLOAT( AutopilotTimeDuration )
END_SERIALIZATION()

ZBonusParameters *GBonusParameters = NULL;

ZBonusParameters::ZBonusParameters()
{
	GBonusParameters = this;

	SmokeTimeDuration = 3.f;
	DistanceBetweenTwoMines = 5.f;
	NumberOfMinesToDrop = 5;
	GravityGripTimeDuration = 3.f;
	EnergyQuantity = 0.2f;
	DestructionShieldTimeDuration = 4.f;
	ProtectionShieldTimeDuration = 5.f;
	BoosterTimeDuration = 4.f;
	AutopilotTimeDuration = 4.f;
	MissileTTL = 6.f;
	MineTTL = 60.f;
}
