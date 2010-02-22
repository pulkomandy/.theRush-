#ifndef ZBONUS_H__
#define ZBONUS_H__

class ZShip;

class ZBonus
{
public:
	ZBonus(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  mbCanModifyShip(bCanModifyShip), mbCanModifyScene(bCanModifyScene), mShip(pShip)
	{
	}
	virtual ~ZBonus()
	{
	}
	virtual void SetActive() = 0;
	virtual void Use() = 0;
	virtual void Tick(float aTimeEllapsed) = 0;
	virtual bool IsActive() const = 0;
protected:
	bool mbCanModifyShip;
	bool mbCanModifyScene;
	ZShip *mShip;
};

#endif