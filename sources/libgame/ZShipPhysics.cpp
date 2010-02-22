///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZShipPhysics.h
// Creation : 13/07/2007
// Author : Cedric Guillemet
// Description :
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZShipPhysics)
SERIALIZE_FLOAT(mLinearDamping)
SERIALIZE_FLOAT(mAngularDamping)
SERIALIZE_FLOAT(mMass)
SERIALIZE_FLOAT(mCompensateNoseForce)
SERIALIZE_FLOAT(mCompensateRightLeftForce)
SERIALIZE_FLOAT(mTorqueZ)
SERIALIZE_FLOAT(mTorqueY)
SERIALIZE_FLOAT(mRunForceFactor)
SERIALIZE_FLOAT(mGravityForce)
SERIALIZE_FLOAT(mGroundForce)
SERIALIZE_FLOAT(mBrakeForce)
SERIALIZE_FLOAT(mBrakeLeftRightForce)
SERIALIZE_FLOAT(mBrakeLeftRightForceBackward)
SERIALIZE_FLOAT(mDrunkEffectFactor)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZShipPhysics::ZShipPhysics()
{

}

ZShipPhysics::ZShipPhysics(ZRushGame *pGame)
{
	mbHasGroundRaycasted = false;
	mbPhysEnable = true;
	m_bOnFloor=false;
	m_Speed=0;


	GravityDirection = tvector3(0, -1, 0);
	mCurrentPlan = vector4(tvector3(0, 1, 0), 0);

	mWrongWay = false;
	maCurIdx = 9999999;

	mbHalfTrackDone = false;
	mbTrackDone = false;
	mTurning = 0;

	mBrakeLeftStrength = 0;
	mBrakeRightStrength = 0;
	mRunStrength = 0;
	mBrakeFullStrength = 0;

	mNbBricksRunned = GTrack->GetSpawnMatrices().size() - pGame->GetClientCount();// >mShips.size();


	mLastKnownGoodBrickIndexOnTrack = 9999999;

	static float ariseCounter = 0;
	arise = ariseCounter++;


	mWallZoneWithCollision = 1.f;


	//
	mMass = 100.f;
	mLinearDamping = 0.99f;
	mAngularDamping = 0.99f;
	
	mCompensateNoseForce = 3200.f;
	mCompensateRightLeftForce = 3200.f;
	mTorqueZ = 1000.f; // axe Z
	mTorqueY = 3200.f; // axe Y
	mRunForceFactor = 27500.f;
	mGravityForce = 1200.f;
	mGroundForce = 1000.f;
	mBrakeForce = 1000.f;
	mBrakeLeftRightForce = 200.f;
	mBrakeLeftRightForceBackward = 750.f;

	// interpolation
	mCurrentFrameInterpolation = 0.f;
	mbHasServerSync = false;

}

ZShipPhysics::~ZShipPhysics()
{

}

void ZShipPhysics::LoadShip(ZPhysicWorldBullet *aPhysicWorld, ZShip *pShip, ZPhysicTriMesh * aCollisionMesh, ZTransform *pTransform, const tmatrix &initialMatrix)
{
	mPhysicWorld = aPhysicWorld;
	mTransform = pTransform;
	mTime0RestartMatrix = initialMatrix;

	tmatrix transup;
	transup.Translation(initialMatrix.V4.position.x, initialMatrix.V4.position.y+10.f, initialMatrix.V4.position.z);

	mShipBody = mPhysicWorld->CreateRigidBodyShip(aCollisionMesh , mMass, pShip);
	mShipBody->SetGravity(tvector3(0, 0, 0));
	mShipBody->SetDamping(mLinearDamping, mAngularDamping);

	Reset(initialMatrix);
}

void ZShipPhysics::Reset(const tmatrix &aMat)
{
	Reset();
	tmatrix mt2, mt3;
	mt2.Translation(0, 6.f, 0);// drop height
	mt3 = aMat*mt2;

	m_Cnt = mt3.V4.position;
	mLastKnownGoodPointOnTrack = m_Cnt;

	//SetDirection(aMat.V4.dir, aMat.V4.up);

	GravityDirection = -aMat.V4.up;
	mCurrentPlan = vector4(aMat.V4.up, tvector3(aMat.V4.position).Dot(tvector3(aMat.V4.up)));//tvector3(0, 1, 0), 0);

	mShipBody->SetMatrix(mt3);
	mShipBody->ResetForceAndTorque();
}

void ZShipPhysics::HandleBulletCollisions(float aTimeEllapsed)
{
	tmatrix trackMat = mGroundMatrix;
	int nbCols;
	PhysCollision_t *pCols = mPhysicWorld->GetCollisionList(nbCols);
	tvector3 aright = trackMat.V4.right;

	if (nbCols)
	{
		/*
				static int frictionCount = 0;
				frictionCount++;
				LOG("Friction %d\n", frictionCount);
*/

		tvector4 colplan;
		colplan = vector4(trackMat.V4.position, trackMat.V4.right);

		float closerPlaneDist = 999999.f;
		int closerPlaneDistIndex = -1;

		
		float biggestWallPenetration=0;
		for (int co = 0;co<nbCols;co++)
		{
			// ship/ship Collision
			ZBaseClass *pObj1 = pCols[co].mObject1;
			ZBaseClass *pObj2 = pCols[co].mObject2;
			// ship/wall
			if  ( ( (pObj1 == this) && (!pObj2) ) ||
					( (pObj2 == this) && (!pObj1) ) )
			{

				
				if (pCols[co].mDistance < biggestWallPenetration)
					biggestWallPenetration = pCols[co].mDistance;

			}
			// ship/ship
			else if  ( ((pObj2 && (pObj1 == this) && ( pObj2->GetClassID() == ClassIDZShipPhysics ) )) ||
				((pObj1 && (pObj2 == this) && ( pObj1->GetClassID() == ClassIDZShipPhysics ) )) )
			{
				if (fabsf(pCols[co].mDistance)>0.f)
				{
					ZShipPhysics *pPhysicOther = (ZShipPhysics*)((pObj1==this)?pObj2:pObj1);
					tvector3 dif = pCols[co].mWorldPosition - GetPosition();
					dif.Normalize();
					dif -= dif.Dot(mGroundMatrix.V4.up);

					// position
					tvector3 decalage = dif * pCols[co].mDistance*0.5f;
					m_Cnt += decalage;
					pPhysicOther->m_Cnt -= decalage;

					// force
					/*
					m_Force *= 0.6f;
					pPhysicOther->m_Force *= 0.6f;
					tvector3 force1 = m_Force;
					tvector3 force2 = pPhysicOther->m_Force;
					m_Force += force2*0.6f;
					pPhysicOther->m_Force += force1*0.6f;
					*/

					pCols[co].mDistance = 0.f;
				}
			}
		}
		mWallZoneWithCollision = 1.f - biggestWallPenetration +0.8f;
	}
	else
	{
		mWallZoneWithCollision = 1;
	}
}

void ZShipPhysics::HandleGroundRayCast()
{
	tvector3 hitPosition, hitNormal;
	float hitFactor;
	tvector3 mGroundRayCast;
	mbHasGroundRaycasted = false;
	if (mPhysicWorld->SyncRayCast(m_Cnt, m_Cnt+GravityDirection*50, &hitPosition, &hitNormal, &hitFactor, 0xFFFFFFFE))
	{
		mbHasGroundRaycasted = true;
	}

	// leaving track
	tvector3 trackHeightDif = mLastKnownGoodPointOnTrack-m_Cnt;
	if (trackHeightDif.Dot(GravityDirection) < 0.f)
	{
		tmatrix restartMat;
		if (mLastKnownGoodBrickIndexOnTrack == 9999999)
		{
			restartMat = mTime0RestartMatrix;
		}
		else
		{
			restartMat = GTrack->GetBrickMatrix(mLastKnownGoodBrickIndexOnTrack);
		}

		Reset(restartMat);
	}
}

void ZShipPhysics::HandleTrackResponse(float aTimeEllapsed)
{
	tvector3 closestPoint;
	tquaternion closestQuat;
	tvector3 trackMiddle;
	tmatrix trackMat;
	float leftrightfactor;

	float distanceToBorder = 0;

	int prevmacuridx = maCurIdx;



	float mUpDownFactor; // unused for now

	if (!mbHasGroundRaycasted)
	{
		tvector3 fakePtCol = m_Cnt;
		fakePtCol += GravityDirection*50;

		mCurrentPlan = vector4(fakePtCol , -GravityDirection);
		trackMat.Translation(fakePtCol);
		//trackMat.V4.position = vector4(fakePtCol, 1);
		mGroundMatrix = trackMat;
	}
	else if (GTrack->GetClosestSampledPoint(m_Cnt, trackMat, trackMiddle, maCurIdx, leftrightfactor,-GravityDirection, mUpDownFactor, distanceToBorder))
	{
		if ( ( maCurIdx > (GTrack->GetNbSamples()*0.4f)) && ( maCurIdx < (GTrack->GetNbSamples()*0.6f)) )
		{
			mbHalfTrackDone = true;
		}
		if ( ( maCurIdx < GTrack->GetNumberOfStartBricksSamples()) && (mbHalfTrackDone))
		{
			mbHalfTrackDone = false;
			mbTrackDone = true;
		}
		mLastKnownGoodPointOnTrack = trackMat.V4.position;
		mLastKnownGoodBrickIndexOnTrack = maCurIdx;

		GravityDirection = -trackMat.V4.up;


		mCurrentPlan = vector4(trackMat.V4.position, trackMat.V4.up);
		mGroundMatrix = trackMat;


		tvector3 hitPosition, hitNormal;
		float hitFactor;

		float aDistanceToWall = -1.f;

		if (leftrightfactor>0.5f)
		{
			if (mPhysicWorld->SyncRayCast(m_Cnt, m_Cnt-mGroundMatrix.V4.right*50, &hitPosition, &hitNormal, &hitFactor, 0xFFFFFFFE))
			{
				aDistanceToWall = (hitPosition-m_Cnt).Length();
			}
		}
		else 
		{
			if (mPhysicWorld->SyncRayCast(m_Cnt, m_Cnt+mGroundMatrix.V4.right*50, &hitPosition, &hitNormal, &hitFactor, 0xFFFFFFFE))
			{
				aDistanceToWall = (hitPosition-m_Cnt).Length();
			}
		}

		if (aDistanceToWall > -1.f)
		{

			tvector3 aright = trackMat.V4.right;
			tvector3 aright2 = tvector3(trackMat.V4.position)-trackMiddle;
			aright2.Normalize();

			float tolerance =0.02f;
			float borderFriction = 0.5f;
			static const float rebond = 1.0f;

			float aWallZone = 1.8f;//mWallZoneWithCollision;//0.0f;
			if (aDistanceToWall<aWallZone)
			{
				if (leftrightfactor<0.5f)
				{
					tvector3 trans = aright*(aWallZone- aDistanceToWall);

					m_Cnt -= trans;
					//m_Force -= aright*rebond;
					//m_Force *= (1.f - (borderFriction*aTimeEllapsed));
				}
				else
				{
					tvector3 trans = aright*(aWallZone -aDistanceToWall);//(distanceToBorder);

					m_Cnt += trans;
					//m_Force += aright*rebond;
					//m_Force *= (1.f -(borderFriction*aTimeEllapsed));
				}
			}
			
		}


/*
		if (abs(int(maCurIdx-prevmacuridx))<10000)
			mNbBricksRunned += (maCurIdx-prevmacuridx);
			*/
		int nbIdx = GTrack->GetNbSamples();
		if ( (maCurIdx-prevmacuridx) < -(nbIdx>>1)) // new 5 prev 10000
			mNbBricksRunned += ( ( maCurIdx + nbIdx ) - prevmacuridx);
		else if ( (prevmacuridx-maCurIdx) < -(nbIdx>>1) ) // new 10000 prev 5
			mNbBricksRunned += ( maCurIdx - ( nbIdx + prevmacuridx ) );
		else
			mNbBricksRunned += ( maCurIdx - prevmacuridx );

	}
}

void ZShipPhysics::Update(float totalElapsed)
{
	if (totalElapsed>1.f) 
		return;

	PROFILER_START(ZShipPhysicsUpdate);

	// new transform matrix
	tmatrix mNewTransformMatrix;
	
	// get bullet collisions and handle them
	HandleBulletCollisions(totalElapsed);
	
	// do a raycast to know if we are still on track
	HandleGroundRayCast();

	// physic-fake ship floating matrix


	mTransform->Update();

	
	
		// server sync
		if (mbHasServerSync)
			HandleServerResync(totalElapsed);

		// ship transform

		mShipBody->GetMatrix(&mNewTransformMatrix);//ComputeShipTransform();
		



		// apply new matrix
		mTransform->SetLocalMatrix(mNewTransformMatrix);


	tmatrix currentShipMatrix = mTransform->GetLocalMatrix();
	m_Cnt = currentShipMatrix.V4.position;

	HandleTrackResponse(totalElapsed);


	

	// here starts everything

	tmatrix GNewCalcsGround = mGroundMatrix;//mTime0RestartMatrix;


	
	tvector3 shipDirAxe = currentShipMatrix.V4.dir;
	tvector3 shipRightAxe = currentShipMatrix.V4.right;
	tvector3 shipUpAxe = GNewCalcsGround.V4.up;

	// wrong way computation
	mWrongWay = (shipDirAxe.Dot(mGroundMatrix.V4.dir)<0);

	// pique du nez
	
	tmatrix curGround = GNewCalcsGround;
	curGround.Inverse();
	tvector3 invSRA;
	invSRA.TransformVector(shipDirAxe, curGround);


	tvector3 invSRightA;
	invSRightA.TransformVector(shipRightAxe, curGround);

	tvector3 newTorque;
	newTorque = tvector3::zero;

	// physic values


	// nose lower
	invSRightA.y += cosf(arise)*mDrunkEffectFactor;
	invSRA.y += cosf(arise)*sinf(arise*2.f)*mDrunkEffectFactor;


	newTorque += shipRightAxe*mCompensateNoseForce*invSRA.y;
	
	// left/right compensate
	
	float leftrighttang = invSRightA.y;
	
	newTorque -= shipDirAxe*mCompensateRightLeftForce*invSRightA.y;



	tmatrix mtCompensateTang;
	mtCompensateTang.Identity();

	float ngZ = asinf(invSRightA.y);

	//LOG("ngZ %5.4f\n",ngZ);

	if (fabsf(ngZ)>0.5f)
	{
		float ngZToApply = (ngZ<0.f)?ngZ+0.5f:ngZ-0.5f;
		mtCompensateTang.RotationZ(-ngZToApply);
	}


	tmatrix mtCompensateNose;
	mtCompensateNose.Identity();
	
	float ngX = asinf(invSRA.y);
	float mNGXMAxDelta = 0.2f;
	//LOG("ngX %5.4f\n",ngX);
	if (fabsf(ngX)>mNGXMAxDelta)
	{
		float ngXToApply = (ngX<0.f)?ngX+mNGXMAxDelta:ngX-mNGXMAxDelta;
		mtCompensateNose.RotationX(ngXToApply);
	}

	// turning

	static const float timeRot = 0.5f;
	if (mTurning == 1)
	{
		newTorque += (shipUpAxe * (-mTorqueY));

		if (leftrighttang<0.5f)
		{
			newTorque += (shipDirAxe * (mTorqueZ) * mTurnStrength);
		}

	}
	else if (mTurning == 2)
	{
		newTorque += (shipUpAxe * (mTorqueY));
		if (leftrighttang>-0.5f)
		{
			newTorque += (shipDirAxe * (-mTorqueZ) * mTurnStrength);
		}
	}
	
	mShipBody->ApplyTorque(newTorque /**totalElapsed*/);


	// forces
	tvector3 ForceToApply(tvector3::zero);

	// running
	tvector3 forceVector = currentShipMatrix.V4.dir;
	forceVector = tvector3(mGroundMatrix.V4.dir)*currentShipMatrix.V4.dir.Dot(tvector3(mGroundMatrix.V4.dir)) +
		tvector3(mGroundMatrix.V4.right)*currentShipMatrix.V4.dir.Dot(tvector3(mGroundMatrix.V4.right));
	forceVector.Normalize();


	ForceToApply += mRunStrength * forceVector * mRunForceFactor;//275.f;
	float aSpeed = Clamp(mShipBody->GetLinearVelocity().Length()-10, 0, 1000);
	//LOG("LV %5.4f \n", aSpeed);
	// braking
	if (mBrakeFullStrength>0.f)
	{
		ForceToApply += currentShipMatrix.V4.dir * aSpeed * (-mBrakeForce) * mBrakeFullStrength;
	}
	else if (mBrakeLeftStrength>0.f)
	{
		ForceToApply += currentShipMatrix.V4.right * aSpeed * (-mBrakeLeftRightForce) * mBrakeLeftStrength;
		ForceToApply += currentShipMatrix.V4.dir * aSpeed * (-mBrakeLeftRightForceBackward) * mBrakeLeftStrength;
	}
	else if (mBrakeRightStrength>0.f)
	{
		ForceToApply -= currentShipMatrix.V4.right * aSpeed * (-mBrakeLeftRightForce) * mBrakeRightStrength;
		ForceToApply += currentShipMatrix.V4.dir * aSpeed * (-mBrakeLeftRightForceBackward) * mBrakeRightStrength;

	}



	// gravity

	//static float fakegrav = 0.f;
	float deadZone = 0.f;

	float fakegrav = arise *1.33f;//totalElapsed*4.f;
	float height = 5.f + cos(fakegrav+arise)*0.30f;
	tvector4 cPlan = vector4(GNewCalcsGround.V4.position, GNewCalcsGround.V4.up);
	float signedDist = cPlan.SignedDistanceTo(currentShipMatrix.V4.position);
	float ftStrength = (fabsf(signedDist-height)+1.f);
	
	float minimalHeightEver = 1.f;
	if (signedDist<minimalHeightEver)
	{
		m_Cnt += cPlan*(minimalHeightEver-signedDist);
	}
	if ( signedDist>(height+deadZone))
	{
		ForceToApply += cPlan *(-mGravityForce)*ftStrength;
		
	}
	else
	if ( signedDist<(height-deadZone))
	{
		ForceToApply += cPlan *(mGroundForce)*ftStrength;
	}

	mShipBody->ApplyForce(ForceToApply*totalElapsed );

	mNewTransformMatrix.V4.position = vector4(m_Cnt.x, m_Cnt.y, m_Cnt.z, 1);
	tmatrix newShipBodyMatrix = mtCompensateTang*mtCompensateNose*mNewTransformMatrix;
	mShipBody->SetMatrix(newShipBodyMatrix);


	mPreviousQuaternion = mCurrentQuaternion;
	mPreviousPosition = mCurrentPosition;
	mCurrentPosition = newShipBodyMatrix.V4.position;
	mCurrentQuaternion = mShipBody->GetOrientationQuaternion();


	arise += totalElapsed*3.f;

	PROFILER_END();

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::TurnRight(float strength)
{
	mTurning = 2;
	mTurnStrength = Clamp(strength,0.f,1.f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::TurnLeft(float strength)
{
	mTurning = 1;
	mTurnStrength = Clamp(strength,0.f,1.f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::NoTurn()
{
	mTurning = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::Run()
{
	mRunStrength = 1.f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::NoRun()
{
	mRunStrength = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::Brake(float strength )
{
	mBrakeFullStrength = strength;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::BrakeRight(float strength )
{
	mBrakeRightStrength = strength;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::BrakeLeft(float strength )
{
	mBrakeLeftStrength = strength;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::NoBrake()
{
	mBrakeRightStrength = 0;
	mBrakeLeftStrength = 0;
	mBrakeFullStrength = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::Reset()
{
	m_bOnFloor=false;
	m_Speed=0;

	GravityDirection = tvector3(0, -1, 0);
	mCurrentPlan = vector4(tvector3(0, 1, 0), 0);

	mWrongWay = false;
	maCurIdx = 9999999;

	mbHalfTrackDone = false;
	mTurning = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::SetSync(const ShipSync_t& ss)
{
	mShipBody->SetMatrix(ss.mMatrix);
	mShipBody->SetLinearVelocity(ss.mVelocity);
	mShipBody->SetAngularVelocity(ss.mAngularVelocity);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::GetSync(ShipSync_t* ss) const
{
	mShipBody->GetMatrix(&ss->mMatrix);
	ss->mVelocity = mShipBody->GetLinearVelocity();
	ss->mAngularVelocity = mShipBody->GetAngularVelocity();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::Interpolate(float aLerp)
{
	mCurrentFrameInterpolation = aLerp;
	tquaternion newQuat;
	newQuat.Slerp(mPreviousQuaternion, mCurrentQuaternion, aLerp);
	tvector3 newPosition;
	newPosition.Lerp(mPreviousPosition, mCurrentPosition, aLerp);
	tmatrix newMat;
	newMat.RotationQuaternion(newQuat);
	newMat.V4.position = vector4(newPosition.x, newPosition.y, newPosition.z, 1);

	//mShipBody->SetMatrix(newMat);
	mTransform->SetLocalMatrix(newMat);
	mTransform->Update();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::SetServerResync(const ServerShipSync_t& serverSync)
{
	mServerSync = serverSync;
	mbHasServerSync = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShipPhysics::HandleServerResync(float totalElapsed)
{
	float tightness = Clamp(totalElapsed*5.f, 0.f, 1.f);
	tquaternion newActiveOrientation = mShipBody->GetOrientationQuaternion();

	float distDif = (mCurrentPosition-mServerSync.mPos).Length();

	
	if (distDif> 5.f)
	{
		// snap
		tmatrix newMat;
		mPreviousQuaternion = mCurrentQuaternion = mServerSync.mOrientation;
		mPreviousPosition = mCurrentPosition = mServerSync.mPos;
		newMat.RotationQuaternion(mServerSync.mOrientation);
		newMat.V4.position = vector4(mServerSync.mPos.x, mServerSync.mPos.y, mServerSync.mPos.z, 1);
		mShipBody->SetMatrix(newMat);
	}
	else 
	if (distDif >0.5f)
	{
		// position/force
		tvector3 diffForce = (mServerSync.mPos-mCurrentPosition);
		diffForce *= mShipBody->GetMass() * totalElapsed * 20.f;
		mShipBody->ApplyForce(diffForce);
		// rotation
		newActiveOrientation.Slerp(mShipBody->GetOrientationQuaternion(),
			mServerSync.mOrientation, tightness);

	}
	else
		mbHasServerSync = false;

	mShipBody->SetOrientationQuaternion(newActiveOrientation);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
