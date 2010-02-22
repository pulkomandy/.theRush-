#ifndef ZBONUSMISSILE_H__
#define ZBONUSMISSILE_H__

class ZBonusMissile : public ZBonus
{
public:
	ZBonusMissile(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene ), mbMissileFired(false)
	{
	}
	virtual ~ZBonusMissile()
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