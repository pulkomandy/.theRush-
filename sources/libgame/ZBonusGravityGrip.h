#ifndef ZBONUSGRAVITYGRIP_H__
#define ZBONUSGRAVITYGRIP_H__

#include "ZBonus.h"

class ZBonusGravityGrip : public ZBonus
{
public:
	ZBonusGravityGrip(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
	}
	virtual ~ZBonusGravityGrip()
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
