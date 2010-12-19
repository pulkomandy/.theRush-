#ifndef ZSHIPBONUS_H__
#define ZSHIPBONUS_H__

#include "ZBonus.h"

#include "..\libworld\ZMeshInstance.h"

class ZShipBonus
{
public:
	ZShipBonus() 
		: 
		mCurrentBonusIndex(-1), 
			mCurrentBonus(NULL), 
		mbInited(false), mShieldMesh(NULL)
	{
	}
	virtual ~ZShipBonus();
	void Init( ZShip *pShip, bool bCanModifyShip, bool bCanModifyScene );
	void Tick(float aTimeEllapsed);
	virtual void GiveBonus(char aBonusNb);
	void UseBonus();
	char GetCurrentBonusIndex() const { return mCurrentBonusIndex; }
	int GetAvailableBonusCount() const { return mAvailableBonus.size(); }

	void EnableDestructionShield(bool bEnable) 
	{ 
		mBonusState.Values.Dirty = (bEnable!=mBonusState.Values.DestructionShield); 
		mBonusState.Values.DestructionShield = bEnable; 
	}
	void EnableProtectionShield(bool bEnable) 
	{ 
		mBonusState.Values.Dirty = (bEnable!=mBonusState.Values.ProtectionShield); 
		mBonusState.Values.ProtectionShield = bEnable; 
	}
	void EnableSmoke(bool bEnable) 
	{ 
		mBonusState.Values.Dirty = (bEnable!=mBonusState.Values.Smoke); 
		mBonusState.Values.Smoke = bEnable; 
	}
	void EnableGravityGrip(bool bEnable) 
	{ 
		mBonusState.Values.Dirty = (bEnable!=mBonusState.Values.GravityGrip); 
		mBonusState.Values.GravityGrip = bEnable; 
	}
	void EnableBooster(bool bEnable) 
	{ 
		mBonusState.Values.Dirty = (bEnable!=mBonusState.Values.Booster); 
		mBonusState.Values.Booster = bEnable; 
	}

	ZMeshInstance *GetShieldMesh() { return mShieldMesh; }
protected:
	ZMeshInstance *mShieldMesh;

	bool mbInited;
	char mCurrentBonusIndex;
	ZBonus *mCurrentBonus;
	std::vector<ZBonus*> mAvailableBonus;

	// Bonus States
	typedef struct BonusState_t
	{
		union
		{
			unsigned char BonusStateByte;
			struct 
			{
				unsigned Dirty : 1;
				unsigned DestructionShield : 1;
				unsigned ProtectionShield : 1;
				unsigned Smoke : 1;
				unsigned GravityGrip : 1;
				unsigned Booster : 1;
			} Values;
		};
	} BonusState_t;
	BonusState_t mBonusState;
};

#endif
