#include "ZMissile.h"


#include "../libworld/ZMeshInstance.h"
#include "../libgame/ZRushGameContent.h"
#include "../libgame/ZBonusParameters.h"
#include "../libgame/ZGameEntitiesManager.h"
#include "../libgame/ZRushTrack.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZMissile)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMissile::ZMissile()
{
	mMissileLife = 0.f;
	maCurIdx = 9999999;
	mbComputeMissilePhysics = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZMissile::~ZMissile()
{
#ifdef _DEBUG	
	LOG("Missile 0x%d destroyed.\n", this);
#endif
	mMissileTransform->RemoveFromScene();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMissile::Init(const tmatrix& mat, bool bComputeMissilePhysics)
{
	mMissileTransform = ((ZMeshInstance*)gRushGameContent->GetRes("Missile"))->GetTransform()->Clone();
	// add to scene
	mMissileTransform->AddToScene(GScene);
	mMissileTransform->SetWorldMatrix(mat);
	mMissileTransform->Update();

	mGravityDirection = -mat.V4.up;
	mPos = mat.V4.position;
	mVel = mat.V4.dir;

	mbComputeMissilePhysics = bComputeMissilePhysics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMissile::Tick(float timeEllapsed)
{
	if (mbComputeMissilePhysics)
	{
		mMissileLife += timeEllapsed;
		if (mMissileLife >= GBonusParameters->MissileTTL)
		{
			mManager->SpawnExplosion(tmatrix::identity);
			mManager->DestroyEntity(this);
		}
		else
		{
			tvector3 closestPoint;
			tquaternion closestQuat;
			tvector3 trackMiddle;
			tmatrix trackMat;
			float leftrightfactor;

			float distanceToBorder = 0;
			//bool mHasReportedCollision = false;

			//int prevmacuridx = maCurIdx;
			float mUpDownFactor = 0;

			bool bFrameFrame = (maCurIdx==9999999);

			tvector3 oldpos = mPos;
			mPos += mVel * timeEllapsed * 20.f;// * GetVelocityFactor();// * (this->IsBullet()?2.f:1.f); //10ms-1
			//mLife += timeElapsed*0.1f;



			if (GTrack->GetClosestSampledPoint(mPos, 
					trackMat, 
					trackMiddle, 
					maCurIdx, 
					leftrightfactor,
					-mGravityDirection, 
					mUpDownFactor, 
					distanceToBorder))
			{
				mGravityDirection = -trackMat.V4.up;

				tvector4 mCurrentPlan = vector4(trackMat.V4.position, trackMat.V4.up);
				float distoplan = mCurrentPlan.SignedDistanceTo( mPos );

				float MissileGroundDistance = 2;
				if (/*(!IsBullet()) && */(distoplan <MissileGroundDistance))
				{
					mPos += trackMat.V4.up * (MissileGroundDistance-distoplan);

					if (!bFrameFrame)
					{
						mVel = (mPos - oldpos);// /(timeElapsed* GetVelocityFactor()*0.1f);
						mVel.Normalize();
					}
				}
				/*
				char tmps[512];
				sprintf_s(tmps, 512, "%5.2f\n", leftrightfactor);
				OutputDebugString(tmps);
				*/
				/*
				float missileTolerance = 0.01f;
				if ( (leftrightfactor<missileTolerance)||(leftrightfactor>(1.f-missileTolerance)))
				mbHasWallCollision = true;
				*/
			}

			// --
			/*
			if (mGameBelonging->IsAuthoritative())
			{
			if (mLockedShip)
			{
			mVel.Normalize();
			mVel = mGameBelonging->GetHomingMissileVector(mPos, mVel, timeElapsed);
			}
			}
			*/
			if (mMissileTransform.ptr())
			{
				tmatrix mt;
				mt.Translation(mPos);

				tvector3 av = mVel;
				av.Normalize();
				mt.V4.dir = av;
				if (fabs(av.Dot(tvector3::YAxis))> 0.8f)
				{
					tvector3 right, up;
					right.Cross(av, tvector3::XAxis);
					up.Cross(right, av);
					right.Normalize();
					up.Normalize();
					mt.V4.right = right;
					mt.V4.up = up;
				}
				else
				{
					tvector3 right, up;
					right.Cross(av, tvector3::YAxis);
					up.Cross(right, av);
					right.Normalize();
					up.Normalize();
					mt.V4.right = right;
					mt.V4.up = up;
				}

				mMissileTransform->SetLocalMatrix(mt);
				mMissileTransform->Update();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
