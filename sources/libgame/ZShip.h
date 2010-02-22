#ifndef ZSHIP_H__
#define ZSHIP_H__

#include "ZShipEffects.h"
#include "ZShipAI.h"
#include "ZShipBonus.h"

DECLAREZCLASS(ZShip);
DECLAREZCLASS(ZNetShip);

enum INTERACTION_TYPE
{
	INTERACTION_NONE,
	INTERACTION_USER_INPUT,
	INTERACTION_IA
};

typedef struct keysStruct
{
	keysStruct()
	{
		leftBrake = rightBrake = ShootVal = runVal = leftVal = rightVal = 0;
	}
	unsigned runVal : 6;
	unsigned leftVal : 6;
	unsigned rightVal : 6;

	unsigned leftBrake : 6;
	unsigned rightBrake : 6;

	unsigned ShootVal : 1;
	unsigned paddedVal : 1; // unassigned for now

	bool operator != (const keysStruct & ks)
	{
		bool dif = false;
		dif |= (runVal != ks.runVal);
		dif |= (leftVal != ks.leftVal);
		dif |= (rightVal != ks.rightVal);
		dif |= (ShootVal != ks.ShootVal);
		dif |= (leftBrake != ks.leftBrake);
		dif |= (rightBrake != ks.rightBrake);
		return dif;
	}

	void Zero()
	{
		memset(this, 0, sizeof(keysStruct));
	}
} keysStruct;


typedef struct ShipSync_t
{
	ShipSync_t()
	{
	}
	ShipSync_t(const tmatrix &mt, const tvector3& aVelocity, const tvector3& aAngularVelocity)
	{
		mMatrix = mt;
		mVelocity = aVelocity;
		mAngularVelocity = aAngularVelocity;
	}

	tmatrix mMatrix;
	tvector3 mVelocity;
	tvector3 mAngularVelocity;
} ShipSync_t;



class ZShip : public ZBaseClass, public ZShipBonus
{
public:
    DECLARE_SERIALIZABLE
public:
	ZShip();
	virtual ~ZShip();

	void InitDefaultValues();

	ZTransform *GetTransform() const { return mShipTransform; }

	virtual void Update(float timeEllapsed);
	virtual void Interpolate(float aLerp)
	{
		mShipPhysic->Interpolate(aLerp);
	}

	virtual void Init(ZRushGame *pGame, ZPhysicWorldBullet *aPhysicWorld, const char *szShipName, int aStart, bool bServerSide);

	void SetInteractionType(INTERACTION_TYPE aInteraction) { mInteractionType = aInteraction; }
	INTERACTION_TYPE GetInteractionType() const { return mInteractionType; }

	void ControlsToPhysic(const keysStruct& ks, float timeEllapsed);
	void SetSync(const ShipSync_t& ss)
	{
		mShipPhysic->SetSync(ss);
	}
	void GetSync(ShipSync_t* ss) const
	{
		mShipPhysic->GetSync(ss);
	}
	void GetControls(keysStruct *pKS) const;
	ZShipPhysics* GetShipPhysic() { return mShipPhysic.ptr(); }
	const keysStruct& GetCurrentControls() const { return mCurrentControls; }

	bool HasFinishedLap() const { return mbHasFinishedLap; }
	void ClearFinishedLapFlag() { mbHasFinishedLap = false; }

	float GetCurrentLapTime() const { return mCurrentLapTime; }
	void ResetCurrentLapTime() { mCurrentLapTime = 0.f; }
	void EnableLapTimeComputation(bool bEnable) { mbLapTimeComputationEnabled = bEnable; }
	int GetCurrentLap() const { return mCurrentLap; }
	const char* GetBotName() const { return mBotName; }
	int GetRanking() const { return mRanking; }
	void SetRanking(int aRanking) { mRanking = aRanking; }

	virtual void DecLife(float aVal) 
	{ 
		if (mBonusState.Values.ProtectionShield)
			return;
		mLife -= aVal; 
		mLife = Clamp(mLife, 0.f, 1.f);
	}
	virtual void IncLife(float aVal) 
	{ 
		mLife += aVal; 
		mLife = Clamp(mLife, 0.f, 1.f);
	}
	float GetLife() const { return mLife; }

	void Reset(int aPos);
	float GetSpeed() const { return mSpeed; }
	ZRushGame *GetGameItBelongsTo() const { return mGame; }
	// Bonus
	//virtual void GiveBonus(char aBonusNb) { mBonus.GiveBonus(aBonusNb); }


	
protected:
	
	
	float mSpeed;
	ZRushGame *mGame;
	tvector3 mPreviousPos;
	bool mbInputControlsEnabled;
	int mRanking;
	float mCurrentLapTime;
	int mCurrentLap;
	bool mbLapTimeComputationEnabled;
	bool mbHasFinishedLap;

	smartptr<ZTransform> mShipTransform;
	smartptr<ZShipPhysics> mShipPhysic;

	void TickInputDevice(float timeEllapsed);


	INTERACTION_TYPE mInteractionType;

	// Aerofreins
	std::vector<ZTransform*> mAeroD;
	std::vector<ZTransform*> mAeroG;
	std::vector<ZTransform*> mAeroM;
	
	float mBrakeLeftLerp, mBrakeRightLerp, mBrakeMidLerp;
	

	void UpdateBrakesMatrices();
	void AeroBrake(float timeEllapsed, float brakeLeft, float brakeRight);

	keysStruct mCurrentControls;
	virtual void GetHumanControls(keysStruct& ks);
	
	static char *mBotNames[];
	char *mBotName;

	// Life
	float mLife;

	// Effects
	smartptr<IShipEffects> mEffects;

	// AI
	smartptr<ZShipAI> mAI;

	tstring mShipNameString;
	
public:
	float mBrakeLerpSpeed;
	float mAeroAngleLeftRight, mAeroAngleMid;
	IShipEffects *GetEffects() const { return mEffects.ptr(); }
	

};


#endif
