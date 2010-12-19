#ifndef ZBONUSENERGY_H__
#define ZBONUSENERGY_H__

#include "ZBonus.h"

class ZBonusEnergy : public ZBonus
{
public:
	ZBonusEnergy(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
		mbActive = false;
	}
	virtual ~ZBonusEnergy()
	{
	}
	virtual void SetActive();
	virtual void Use();
	virtual void Tick(float aTimeEllapsed);
	virtual bool IsActive() const;
protected:
	bool mbActive;
};

#endif
