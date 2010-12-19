#ifndef ZBONUSBOOSTER_H__
#define ZBONUSBOOSTER_H__

#include "ZBonus.h"

class ZBonusBooster : public ZBonus
{
public:
	ZBonusBooster(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
		mbActive = false;
	}
	virtual ~ZBonusBooster()
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
