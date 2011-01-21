///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ShipCamera.h
// Creation : 05/08/2008
// Author : Cedric Guillemet
// Description :
// Reliability :
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

#ifndef SHIPCAMERA_H__
#define SHIPCAMERA_H__

#include "ZShip.h"

class CameraMotion
{
public:

	CameraMotion()
	{
		mpShip = NULL;
		mpZShip = NULL;
		mCameraInterpolationFactor = 10;
		mCameraRelativePosition = tvector3(0,3.5f,-5.5f);
		mCameraRelativeTarget = tvector3(0,0,9.0f);
		mCamera = NULL;
		mHasToggled = false;

		mRideVal = 0;
		mObserveTransTime = 3;
		mObserveTime = 0;
		mObserveFunc = 0;

		randNg = ((rand()&255)/255.f)*2*ZPI;

		// track observer
		mTimeTrav = 6.f;
		mDeltaVectorMapObserv = 4.5f;


	}
	virtual ~CameraMotion()
	{

	}

	void SetActiveCamera(ZCamera *pCam)
	{
		mCamera = pCam;
	}

	void SetCameraPiloting(ZTransform *pShip)
	{
		mpShip = pShip;
		mCameraType = CAMERA_PILOTING;
	}

	void SetCameraUnused()
	{
		mCameraType = CAMERA_UNDEFINED;
		mpShip = NULL;
	}

	void ToggleCameraPiloting()
	{
		mHasToggled = true;
	}

	void SetCameraOrbiting(ZTransform *pShip)
	{
		if (mCameraType != CAMERA_ORBITING)
		{
			mpShip = pShip;
			mObserveTime = 0;
			mCameraType = CAMERA_ORBITING;
		}
	}

	void SetCameraOrbitingClose(ZTransform *pShip)
	{
		mpShip = pShip;
		if (mCameraType != CAMERA_ORBITINGCLOSE)
			mObserveTime = 0;
		mCameraType = CAMERA_ORBITINGCLOSE;
	}

	void SetCameraTransitionedObserver(ZShip *pShip)
	{
		if ( (mCameraType != CAMERA_OBSERVER) || (mpZShip == pShip))
		{
			mCameraType = CAMERA_OBSERVER;
			mpZShip = pShip;
			InitNewObservation();
		}
	}

	ZTransform* GetObservedShip() const { return mpShip; }

	void SetCameraMapObserver()
	{
		mCameraType = CAMERA_MAPOBSERVER;
		mpShip = NULL;
		InitNewMapObservation();
	}

	void SetCameraMapObserverMenu();

	void SetCameraStartupMode(unsigned int aCount)
	{
		mCameraType = CAMERA_STARTUPMODE;
		mStartupIterationsCount = aCount+1;
		mStartupModeTime = 1;

	}

	void Tick()
	{
		switch (mCameraType)
		{
		case CAMERA_PILOTING:
			TickCameraPiloting();
			break;
		case CAMERA_MAPOBSERVER:
			TickCameraMapObserver();
			break;

		case CAMERA_ORBITING:
			TickCameraOrbiting();
			break;
		case CAMERA_OBSERVER:
			TickCameraObserver();
			break;

		case CAMERA_MAPOBSERVER_MENU:
			TickCameraMapObserverMenu();
			break;
		case CAMERA_STARTUPMODE:
			TickCameraStartupMode();
			break;
		case CAMERA_ORBITINGCLOSE:
			TickCameraOrbitingClose();
			break;

		default:
			break;
		}

	}


	bool IsInCockpit() const { return ((!mTPSCamera) && (mCameraType == CAMERA_PILOTING)); }
	void SetInCockpit(bool bInCockpit) { mTPSCamera = (!bInCockpit); }

protected:
	void TickCameraPiloting();
	void TickCameraObserver();
	void TickCameraOrbiting();
	void TickCameraMapObserver();
	void TickCameraMapObserverMenu();
	void TickCameraStartupMode();
	void TickCameraOrbitingClose();


	void InitNewObservation();
	void InitNewMapObservation();

	// observation
	tmatrix mObserveLockpt;
	float mObserveTransTime;
	tvector3 mFixedCamPos,mFixedCamInterp;
	float mRideVal;
	float mObserveTime;
	int mObserveFunc;

	// camera piloting
	bool mTPSCamera;
	ZTransform *mpShip;
	ZShip *mpZShip;
	ZCamera *mCamera;
	bool mHasToggled;

	// map observ
	tvector3 mVectMapObserv, mPointMapObserv, mDirMapObserv, mUpMapObserv;
	float mTimeMapObserv;
	//

	tvector3 mCurrentPosition, mCurrentTarget;
	float mCameraInterpolationFactor;
	tvector3 mCameraRelativePosition, mCameraRelativeTarget;


	enum CAMERA_TYPES
	{
		CAMERA_UNDEFINED,
		CAMERA_PILOTING,
		CAMERA_ORBITING,
		CAMERA_OBSERVER,
		CAMERA_MAPOBSERVER,
		CAMERA_MAPOBSERVER_MENU,
		CAMERA_STARTUPMODE,
		CAMERA_ORBITINGCLOSE,

	};
	CAMERA_TYPES mCameraType;

	// startup/warmup Mode
	float mStartupModeTime;
	tvector3 mSMTDesiredPosition, mSMTCurrentPosition;
	tvector3 mSMTUp, mSMTRight;
	float mSMTRollNg;
	int mStartupIterationsCount;

	float randNg;

	// track observer
	float mTimeTrav;
	float mDeltaVectorMapObserv;

	// map observer menu
	float mTCMOMbrckidx;
	float nbAv;
	tvector3 mTCcamPos;
	float mRangeSize;

	float mTimeBeforeSpeedUp;
	float mTimeBeforeSpeedDown;
	float teFactor;
};


extern CameraMotion gCameraMotion;

#endif
