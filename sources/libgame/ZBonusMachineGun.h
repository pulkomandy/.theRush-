#ifndef ZBONUSMACHINEGUN_H__
#define ZBONUSMACHINEGUN_H__

#include "ZBonus.h"

class ZBonusMachineGun : public ZBonus
{
public:
	ZBonusMachineGun(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
	}
	virtual ~ZBonusMachineGun()
	{
	}
	virtual void SetActive();
	virtual void Use();
	virtual void Tick(float aTimeEllapsed);
	virtual bool IsActive() const;
protected:

};

#endif
