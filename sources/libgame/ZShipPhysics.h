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

#ifndef ZSHIPPHYSICS_H__
#define ZSHIPPHYSICS_H__

#include "../libbase/ZBaseMaths.h"
#include "..\libbase\ZSmartPtr.h"
#include "..\libworld\ZRigidBodyBullet.h"
#include "..\libworld\ZTransform.h"

class ZPhysicWorldBullet;
class ZPhysicTriMesh;
class ZTransform;
class ZShip;
class ZRushGame;

struct ShipSync_t;
struct ServerShipSync_t;

typedef struct ServerShipSync_t
{
	ServerShipSync_t()
	{
	}
	ServerShipSync_t(const tvector3 &pos, const tquaternion& orient)
	{
		mPos = pos;
		mOrientation = orient;
	}

	tvector3 mPos;
	tquaternion mOrientation;
} ServerShipSync_t;


class ZShipPhysics : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE

public:
	ZShipPhysics();
	ZShipPhysics(ZRushGame *pGame);
	virtual ~ZShipPhysics();

    void Update(float aTimeEllapsed);
	void Interpolate(float aLerp);
	
    void LoadShip(ZPhysicWorldBullet *aPhysicWorld, ZShip *pShip, ZPhysicTriMesh * aCollisionMesh, ZTransform *pTransform, const tmatrix &initialMatrix);

    tvector3 m_Cnt;
	// piloting

    void TurnLeft(float strength = 1.0f);
    void TurnRight(float strength = 1.0f);
    void NoTurn();
    void Run();
    void NoRun();
	void Brake(float strength = 1.0f);
	void BrakeRight(float strength = 1.0f);
	void BrakeLeft(float strength = 1.0f);
	void NoBrake();

	// end piloting

    bool m_bOnFloor;



    bool mWrongWay;
    int maCurIdx;
    ZTransform *GetTransform() { return mTransform; }

    bool mbHalfTrackDone;
	bool mbTrackDone;

    int mTurning;

	const tvector3& GetPosition() { return m_Cnt; }
	void SetPosition(const tvector3 &pos)
	{
		m_Cnt = pos;
	}

	virtual float GetSpeed() { return m_Speed; }


	virtual tmatrix& GetIAMat() { return mIAMat; }


	virtual const tmatrix& GetGroundMatrix() { return mGroundMatrix; }

	int GetCurrentRoadBlockIndex() { return maCurIdx; }

	void Reset();
	void Reset(const tmatrix &aMat);
	void EnablePhysics(bool bEnable) { mbPhysEnable = bEnable; }

	virtual tvector3 GetRoadDirectedVector()
	{
		const tvector3& shipdir = GetTransform()->GetWorldMatrix().V4.dir;
		const tmatrix& mat1 = GetGroundMatrix();
		tvector3 avel = mat1.V4.right*mat1.V4.right.Dot(shipdir);
		avel += mat1.V4.dir*GetGroundMatrix().V4.dir.Dot(shipdir);
		avel.Normalize();
		return avel;
	}

	int GetNbBricksRunned() { return mNbBricksRunned; }
	void SetSync(const ShipSync_t& ss);
	void GetSync(ShipSync_t* ss) const;
	float GetLocalTime() const { return arise; }
	void SetLocalTime(float aVal) { arise = aVal; }
	void SetServerResync(const ServerShipSync_t& serverSync);

	bool IsLapDone() const { return mbTrackDone; }
	void ClearLapDoneFlag() { mbTrackDone = false; }
public:
	int mNbBricksRunned;
	bool mbPhysEnable;

    ZTransform *mTransform;

    tvector3 GravityDirection;
    tvector4 mCurrentPlan;
	tmatrix mIAMat;

	float mTurnStrength;
	float m_Speed;


	tmatrix mGroundMatrix;

public:
	// controls
	float mBrakeLeftStrength;
	float mBrakeRightStrength;
	float mRunStrength;
	float mBrakeFullStrength;

	tmatrix mTime0RestartMatrix;
	float mWallZoneWithCollision;

	// fake moving
	float arise;

	// left track
	tvector3 mLastKnownGoodPointOnTrack;
	unsigned int mLastKnownGoodBrickIndexOnTrack;
	void HandleBulletCollisions(float aTimeEllapsed);

	// ground raycast
	bool mbHasGroundRaycasted;
	void HandleGroundRayCast();

	// track response
	void HandleTrackResponse(float aTimeEllapsed);

	smartptr<ZRigidBody> mShipBody;
	ZPhysicWorldBullet *mPhysicWorld;

	// new version
	float mLinearDamping, mAngularDamping;
	float mMass;
	float mCompensateNoseForce;
	float mCompensateRightLeftForce;
	float mTorqueZ;
	float mTorqueY;
	float mRunForceFactor;
	float mGravityForce;
	float mGroundForce;
	float mBrakeForce;
	float mBrakeLeftRightForce;
	float mBrakeLeftRightForceBackward;
	float mDrunkEffectFactor;

	// Interpolation
	tquaternion mPreviousQuaternion, mCurrentQuaternion;
	tvector3 mPreviousPosition, mCurrentPosition;
	float mCurrentFrameInterpolation;
	ServerShipSync_t mServerSync;
	bool mbHasServerSync;
	void HandleServerResync(float totalElapsed);
};


#endif
