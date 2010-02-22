#ifndef ZBONUSHOMINGMISSILE_H__
#define ZBONUSHOMINGMISSILE_H__

class ZBonusHomingMissile : public ZBonus
{
public:
	ZBonusHomingMissile(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene ), mbMissileFired(false)
	{
	}
	virtual ~ZBonusHomingMissile()
	{
	}
	virtual void SetActive();
	virtual void Use();
	virtual void Tick(float aTimeEllapsed);
	virtual bool IsActive() const;
protected:
	bool mbMissileFired;
};

#endif