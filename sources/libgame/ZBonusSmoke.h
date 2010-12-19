#ifndef ZBONUSSMOKE_H__
#define ZBONUSSMOKE_H__

#include "ZBonus.h"

class ZBonusSmoke : public ZBonus
{
public:
	ZBonusSmoke(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
	}
	virtual ~ZBonusSmoke()
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
