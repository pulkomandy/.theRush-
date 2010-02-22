#ifndef ZBONUSAUTOPILOT_H__
#define ZBONUSAUTOPILOT_H__

class ZBonusAutopilot : public ZBonus
{
public:
	ZBonusAutopilot(ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene) :
	  ZBonus( pShip, bCanModifyShip, bCanModifyScene )
	{
	}
	virtual ~ZBonusAutopilot()
	{
	}
	virtual void SetActive();
	virtual void Use();
	virtual void Tick(float aTimeEllapsed);
	virtual bool IsActive() const;
protected:
	bool mbActive;
	float mTimeBeforeDeactivated;
};

#endif