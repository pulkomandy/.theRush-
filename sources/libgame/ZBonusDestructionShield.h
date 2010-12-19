#ifndef ZBONUSDESTRUCTIONSHIELD_H__
#define ZBONUSDESTRUCTIONSHIELD_H__

#include "ZBonus.h"

class ZBonusDestructionShield : public ZBonus
{
public:
	ZBonusDestructionShield(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
	}
	virtual ~ZBonusDestructionShield()
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
