#include "stdafx.h"

BEGIN_SERIALIZATION(ZShipAI)
SERIALIZE_FLOAT(mPID.Ki)
SERIALIZE_FLOAT(mPID.Kp)
SERIALIZE_FLOAT(mPID.Kd)
END_SERIALIZATION()

void ZShipAI::Init(ZShip *pShip)
{
	mShip = pShip;
}

void ZShipAI::ComputeAI(float aTimeEllapsed, keysStruct* res)
{
	ZShipPhysics *pPhys = mShip->GetShipPhysic();
	int maCurIdx = pPhys->GetCurrentRoadBlockIndex();
	if (maCurIdx == 9999999) return;

	memset(res, 0, sizeof(keysStruct));
	// fill input struct


	//mRaceShip.Run();
	res->runVal = 63;

	//ZRushTrack *roadgen = GetRoad();

	int tPoint = ( maCurIdx + 8 + GTrack->GetNbSamples() ) % GTrack->GetNbSamples();

	tvector3 mIAPoint = GTrack->GetAIPoints()[tPoint].mAIPos;

	tvector3 mMobile = mShip->GetShipPhysic()->GetPosition();

	tvector3 desireddir = (mIAPoint-mMobile);
	desireddir.Normalize();



	tvector3 wright = pPhys->GetTransform()->GetWorldMatrix().V4.right; // previously IAmat
	float iaDir = wright.Dot(desireddir);



	float output = mPID.Compute(0, iaDir, aTimeEllapsed);
	iaDir = 1-output;


	float astrength = fabs(iaDir);
	float deadzone = 0.1f;

	if (iaDir<-deadzone) 
	{
		res->leftVal = int (astrength*63);

	}
	else if (iaDir>deadzone)
	{
		res->rightVal = int (astrength*63);
	}


}