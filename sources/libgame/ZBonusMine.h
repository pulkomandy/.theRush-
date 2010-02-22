#ifndef ZBONUSMINE_H__
#define ZBONUSMINE_H__

class ZBonusMine : public ZBonus
{
public:
	ZBonusMine(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
		mbActive = false;
	}
	virtual ~ZBonusMine()
	{
	}
	virtual void SetActive();
	virtual void Use();
	virtual void Tick(float aTimeEllapsed);
	virtual bool IsActive() const;
protected:
	bool	mbActive;
	tvector3 mLastPosition;
	int mNbMinesToDrop;
};

#endif