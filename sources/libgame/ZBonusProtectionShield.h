#ifndef ZBONUSPROTECTIONSHIELD_H__
#define ZBONUSPROTECTIONSHIELD_H__

#include "ZBonus.h"

class ZBonusProtectionShield : public ZBonus
{
public:
	ZBonusProtectionShield(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
	}
	virtual ~ZBonusProtectionShield()
	{
	}
	virtual void SetActive();
	virtual void Use();
	virtual void Tick(float aTimeEllapsed);
	virtual bool IsActive() const;
protected:
	float mActiveTime;
	bool mbActive;
};

#endif
