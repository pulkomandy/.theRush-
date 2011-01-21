///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : CameraMotion.cpp
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

#include "ZShip.h"

#include "CameraMotion.h"
#include "libgame/ZRushTrack.h"
#include "libbase/ZTimer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

CameraMotion gCameraMotion;

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::TickCameraPiloting()
{
	if (!mpShip) return;
	//IShipPhysics *ps = mpShip->GetPhysics();




	bool reinittps = false;
	if (mHasToggled) //GetInputDevice()->KeyReleased(TKEY_TAB))
	{
		mTPSCamera = !mTPSCamera;
		reinittps = true;
		mHasToggled = false;
	}





	// update camera
	tmatrix hoho = mpShip->GetWorldMatrix();

	if (mTPSCamera)
	{
		//tmatrix hoho2 = hoho;
		//hoho2.Inverse();
		//hoho2.OrthoNormalize();
		/*
		tmatrix alittleup;
		alittleup.Translation(0,25,-70);
		hoho2 *= alittleup;
		*/
		tvector3 mDesiredPosition, mDesiredTarget;
		mDesiredPosition.TransformPoint(mCameraRelativePosition , hoho);
		mDesiredTarget.TransformPoint(mCameraRelativeTarget , hoho);


		if (reinittps)
		{
			//mCurrentPosition = mCameraRelativePosition;
			mCurrentPosition.TransformPoint(tvector3(0,0,0) , hoho);
			//mCurrentTarget = mCameraRelativeTarget;
			mCurrentTarget.TransformPoint(mCameraRelativeTarget, hoho);
			
			reinittps = false;
		}
		else
		{
			
			float camFact = gTimer.GetEllapsed() * /*GClientGame->GetGameSpeed() **/ mCameraInterpolationFactor ;
			mCurrentPosition.Lerp(mCurrentPosition, mDesiredPosition, Clamp(camFact,0,1));

			float LengthBehind = (mCurrentPosition-hoho.V4.position).Dot(hoho.V4.dir);
			float difBehind = LengthBehind - mCameraRelativePosition.z;
			mCurrentPosition -= difBehind*hoho.V4.dir;

			//mCurrentPosition

			mCurrentTarget.Lerp(mCurrentTarget, mDesiredTarget, 1);//Clamp(camFact,0,1));
		}
/*
		tvector3 tup;
		tup.Lerp(hoho.up, 0.5f);
		*/
		hoho.LookAt(mCurrentPosition, mCurrentTarget, hoho.V4.up);
	}
	else
	{
		
		hoho = mpShip->GetFirstChild()->GetWorldMatrix();
		tmatrix matCockPit, matCockPitDecal;
		// inside cockpit
		//matCockPitDecal.Translation(0, 0.64f, 0.45f);
		// left side
		//matCockPitDecal.Translation(0.3f, 0.84f, -1.45f);
		// behind middle
		//matCockPitDecal.Translation(0.0f, 1.25f, -4.0f);
		//front
		//matCockPitDecal.Translation(0.0f, 0.6f, 0.3f);
		matCockPitDecal.Translation(0.0f, 0.15f, 0.6f);
		// below
		//matCockPitDecal.Translation(0.0f, -0.5f, -2.3f);

		matCockPit = matCockPitDecal * hoho ;
		mCurrentPosition = matCockPit.V4.position;
		hoho.LookAt(mCurrentPosition, mCurrentPosition+hoho.V4.dir, hoho.V4.up);
	}


	mCamera = GetActiveCamera();
	mCamera->GetTransform()->SetLocalMatrix(hoho);
	mCamera->Update();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_OBSERVER 3

void CameraMotion::InitNewObservation()
{

	if (!mpZShip) 
		return;
/*
	if ((mObserveFunc == mForcedObservedType) && (mForcedObservedType != -1))
		return;
*/	
	mObserveTime = 0;
	mObserveFunc = rand()%MAX_OBSERVER;
/*
	if (mForcedObservedType != -1)
		mObserveFunc = mForcedObservedType;
*/
	if (mpZShip == (ZShip*)2)
	{
		//mRideVal = 0;
		mObserveLockpt = GTrack->GetBrickMatrix(0);
	}
	else if (mpZShip == (ZShip*)1)
		mObserveLockpt = GTrack->GetBrickMatrix(0);
	else
		mObserveLockpt = GTrack->GetBrickMatrix((mpZShip->GetShipPhysic()->GetCurrentRoadBlockIndex()>>3)+5);
	
	float ng = float(rand()&255) * float(1.f/255.f);
	mFixedCamInterp = tvector3(cos(ng)*4 , sin(ng)*4 , 0);
	mFixedCamPos = tvector3(0,15,-0.01f) - (mFixedCamInterp*mObserveTransTime);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::TickCameraObserver()
{
	if (!mpZShip) return;

	ZCamera *pCam = (ZCamera*)FirstInstanceOf(ZCamera);
	pCam->SetActive();

	static tmatrix mt;
	tvector3 pos;
	

	if (mpZShip == (ZShip*)2)
	{
		// ride
		tmatrix mt1 = GTrack->GetBrickMatrix((int)floorf(mRideVal));
		tmatrix mt2 = GTrack->GetBrickMatrix((int)floorf(mRideVal)+1);
		mt.Lerp(mt1, mt2, fmodf(mRideVal,1));
		mt.OrthoNormalize();
		mObserveFunc = 11;
	}
	else if (mpZShip == (ZShip*)1)
	{
		mt = GTrack->GetBrickMatrix(0);
		pos  = mt.V4.position;

	}
	else
	{
/*
		tmatrix mt1 = GTrack->GetBrickMatrix(mpShip->GetPhysics()->GetCurrentRoadBlockIndex());
		tmatrix mt2 = GTrack->GetBrickMatrix(mpShip->GetPhysics()->GetCurrentRoadBlockIndex()+1);
		mt.Lerp(mt1, mt2, mpShip->GetPhysics()->GetUpDownFactor());
		mt.OrthoNormalize();
*/

		mt.Lerp(mt, mpZShip->GetShipPhysic()->GetGroundMatrix(), gTimer.GetEllapsed());
		mt.OrthoNormalize();
		pos  = mpZShip->GetShipPhysic()->GetTransform()->GetWorldMatrix().V4.position;
	}


	switch (mObserveFunc)
	{
	case 0: // tourne autour
		{	
			tvector3 loc(cos(mObserveTime) * 4, 1.5f, sin(mObserveTime) * 4);
			loc.TransformVector(mt);
			tmatrix tmpmat;
			tmpmat.LookAt(
				pos + loc,
				pos, 
				mt.V4.up);

			pCam->GetTransform()->SetLocalMatrix(tmpmat);
		}
		break;
	case 1: // caméra de coté
		{
			tvector3 loc(3,0,7);
			loc.TransformVector(mt);

			tvector3 loc2(3,0,0);
			loc2.TransformVector(mt);

			tmatrix tmpmat;
			tmpmat.LookAt(
				pos + loc,
				pos + loc2, 
				mt.V4.up);

			pCam->GetTransform()->SetLocalMatrix(tmpmat);
		}
		break;
		/*
	case 2: // vue de dessus
		{
			tvector3 loc(0,15,-0.01f);
			loc.TransformVector(mt);

			tmatrix tmpmat;
			tmpmat.LookAt(
				pos + loc,
				pos , 
				mt.up);

			pCam->GetTransform()->SetLocalMatrix(tmpmat);
		}
		break;
		*/
	case 2: // camera fixe
		{

			tvector3 loc = mFixedCamPos;
			loc += mFixedCamInterp * mObserveTime;
			loc.TransformVector(mt);

			tmatrix mt = mObserveLockpt;
			tmatrix tmpmat;
			tmpmat.LookAt(
				tvector3(mt.V4.position) + loc,
				pos , 
				mt.V4.up);

			pCam->GetTransform()->SetLocalMatrix(tmpmat);
		}
		break;
	case 10: // tourne autour
		{	
			GDD->SetPPfocalDist(10);
			GDD->SetPPfocalFactor(0.008f);


			tvector3 loc(cos(mObserveTime) * 7, 2, sin(mObserveTime) * 7);
			loc.TransformVector(mt);
			tmatrix tmpmat;
			tmpmat.LookAt(
				pos + loc,
				pos, 
				mt.V4.up);

			pCam->GetTransform()->SetLocalMatrix(tmpmat);
		}
		break;
	case 11: // suit le ride
		{
			GDD->SetPPfocalDist(70);
			GDD->SetPPfocalFactor(0.004f);

			tvector3 loc(0,5,0);
			tvector3 target(0,5,1);
			loc.TransformPoint(mt);
			target.TransformPoint(mt);
			tmatrix tmpmat;
			tmpmat.LookAt(
				loc,
				target, 
				mt.V4.up);

			pCam->GetTransform()->SetLocalMatrix(tmpmat);
		}
		break;
	}

	pCam->Update();
	
	mObserveTime += gTimer.GetEllapsed() * 0.5f;// * 0.1f;
	mRideVal+=gTimer.GetEllapsed() ;
	if (mObserveTime > mObserveTransTime)
	{
		InitNewObservation();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::TickCameraOrbiting()
{
	ZCamera *pCam = GetActiveCamera();
	const tmatrix& mt = mpShip->GetWorldMatrix();
	const tvector3& pos  = mpShip->GetWorldMatrix().V4.position;


	 tvector3 dcup = mt.V4.up*1.8f;
	tvector3 loc(cosf(mObserveTime) * 8.f, 2.3f, sinf(mObserveTime) * 8.f);
	loc.TransformVector(mt);
	tmatrix tmpmat;
	tvector3 eye = pos + loc + dcup;
	tvector3 target = pos + dcup;
	tmpmat.LookAt(
		eye,
		target, 
		mt.V4.up);

	GDD->SetPPfocalDist(Distance(eye, target));
	pCam->GetTransform()->SetLocalMatrix(tmpmat);
	pCam->Update();
	mObserveTime += gTimer.GetEllapsed() * 0.5f;// * 0.1f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::TickCameraMapObserver()
{
	ZCamera *pCam = GetActiveCamera();
	if (!pCam)
	{
		pCam = (ZCamera*)FirstInstanceOf(ZCamera);
		pCam->SetActive();
	}
	//loc.TransformVector(mt);
	tmatrix tmpmat;
	tmpmat.LookAt(
		mPointMapObserv ,
		mPointMapObserv + mDirMapObserv, 
		mUpMapObserv);

	pCam->GetTransform()->SetLocalMatrix(tmpmat);
	pCam->Update();

	float te = gTimer.GetEllapsed();
	mTimeMapObserv += te;


	mPointMapObserv += te*mVectMapObserv * ((mDeltaVectorMapObserv*2.f)/mTimeTrav);// * 0.5f;// * 0.1f;

	if (mTimeMapObserv>=mTimeTrav)
	{
		InitNewMapObservation();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::InitNewMapObservation()
{
	const tmatrix& mt1 = GTrack->GetBrickMatrix(rand()&0xFFFF);
	mPointMapObserv = mt1.V4.position;
	mPointMapObserv += (mt1.V4.up*10.f);
	mVectMapObserv = tvector3(float(rand()&0xFF), float(rand()&0xFF), float(rand()&0x3F));
	mVectMapObserv -= tvector3(128.f, 128.f, 32.f);
	mVectMapObserv.TransformVector(mt1);
	mVectMapObserv.Normalize();

	mPointMapObserv -= mVectMapObserv * mDeltaVectorMapObserv;
	mDirMapObserv.Cross(mVectMapObserv, mt1.V4.up);
	//mDirMapObserv.Normalize();
	mUpMapObserv = mt1.V4.up;
	mTimeMapObserv = 0;


}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::SetCameraMapObserverMenu()
{
	mCameraType = CAMERA_MAPOBSERVER_MENU;
	mpShip = NULL;
	//InitNewMapObservation();

	mTCMOMbrckidx = 0;
	nbAv = 0;
	mTCcamPos = tvector3::zero;
	mRangeSize = 30;

	mTimeBeforeSpeedUp = 10;
	mTimeBeforeSpeedDown = -1;
	teFactor = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::TickCameraMapObserverMenu()
{
	/*
	if (!GClientGame)
		return;
	if (!GClientGame->mClientShip)
		return;
*/
	ZCamera *pCam = GetActiveCamera();
/*
	if (!pCam)
	{
		pCam = (ZCamera*)FirstInstanceOf(ZCamera);
		pCam->SetActive();
	}
	*/
	float totalT = gTimer.GetEllapsed();
	float df = 0.01f;
	float DofDist;
	while (totalT > 0.f)
	{
		const tmatrix & rdm = GTrack->GetBrickMatrix((int)mTCMOMbrckidx);
		//loc.TransformVector(mt);
		float aGDist = 8.f;
		mRangeSize = aGDist + (cos(nbAv*0.2f)+1)*(sin(nbAv*0.33f)+1) * (aGDist*0.75f);
		tvector3 trpt(cos(nbAv*0.2f)*mRangeSize, fabsf(sin(nbAv*0.2f)*mRangeSize),0);
		trpt.TransformPoint(rdm);

		const tvector3& shpPos = ((ZRushGame*)GGame->GetClientGame())->GetFirstShip()->GetShipPhysic()->GetPosition();

		tmatrix tmpmat;
		tmpmat.LookAt(
			mTCcamPos ,
			shpPos, 
			tvector3::YAxis);

		DofDist = Distance(mTCcamPos, shpPos);

		pCam->GetTransform()->SetLocalMatrix(tmpmat);
		pCam->Update();

		if (mTimeBeforeSpeedUp<=0)
		{
			teFactor = 12;
			mTimeBeforeSpeedDown = 12;
			mTimeBeforeSpeedUp = 999999;
		}
		if (mTimeBeforeSpeedDown<=0)
		{
			teFactor = 1;	
			mTimeBeforeSpeedUp = (float)(4+(rand()&3));
			mTimeBeforeSpeedDown = 99999;
		}
		float te = df*teFactor;
		
		mTimeBeforeSpeedUp-=te;
		mTimeBeforeSpeedDown-=te;

		mTimeMapObserv += te;
#ifdef _DEBUG
		mTCMOMbrckidx += te*2.5f;
#else
		mTCMOMbrckidx += te*8.5f;
		
#endif

		nbAv += te;
		mTCcamPos = LERP(mTCcamPos, trpt, te*0.5f);



		totalT -= df;
	}


	//DOF
	GDD->SetPPfocalDist(DofDist);
	GDD->SetPPfocalThreshold(0.04f);
	GDD->SetPPfocalFactor(0.65f);
	GDD->SetPPLuminance(1.f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void CameraMotion::TickCameraStartupMode()
{
	if (!GGame) 
		return;
	if (!GGame->GetClientGame())
		return;
	if (!((ZRushGame*)GGame->GetClientGame())->GetClientShip())
		return;

	float te = gTimer.GetEllapsed();
	mStartupModeTime += te;
	if (mStartupModeTime>=1.f)
	{
		mStartupIterationsCount--;
		te = 0;
		if (mStartupIterationsCount <= 0)
		{
			//SetCameraPiloting(GClientGame->mClientShip);
			return;
		}
		
		
		tvector3 direction(cos(randNg), 1.f, sinf(randNg));
		direction.Normalize();

		const tvector3& shipPos = ((ZRushGame*)GGame->GetClientGame())->GetClientShip()->GetShipPhysic()->GetPosition();
		mSMTCurrentPosition = shipPos +direction*60;
		mSMTDesiredPosition = shipPos ;

		mSMTRight.Cross(direction, tvector3::YAxis );
		mSMTUp.Cross(mSMTRight, direction );
		mSMTRight.Normalize();
		mSMTUp.Normalize();


		mStartupModeTime = 0;
		mSMTRollNg = (float(rand()&255)/255.f *2)-1.f;

		randNg += 2.f;

	}

	mSMTCurrentPosition = LERP(mSMTCurrentPosition, mSMTDesiredPosition, te*2);
	mSMTRollNg = LERP(mSMTRollNg, 0, te);

	tmatrix mt;
	tvector3 locUp;
	locUp = mSMTRight*sin(mSMTRollNg) + mSMTUp*cos( mSMTRollNg);
	mt.LookAt(mSMTCurrentPosition, mSMTDesiredPosition, locUp);


	GetActiveCamera()->GetTransform()->SetLocalMatrix(mt);
	GetActiveCamera()->GetTransform()->Update();
	GetActiveCamera()->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CameraMotion::TickCameraOrbitingClose()
{

	ZCamera *pCam = GetActiveCamera();
	const tmatrix& mt = mpShip->GetWorldMatrix();
	const tvector3& pos  = mpShip->GetWorldMatrix().V4.position;


	 tvector3 dcup = mt.V4.up*1.8f;
	tvector3 loc(cosf(mObserveTime) * 5.f, 1.5f, sinf(mObserveTime) * 5.f);
	loc.TransformVector(mt);
	tmatrix tmpmat;
	tvector3 eye = pos + loc + dcup;
	tvector3 target = pos + dcup;
	tmpmat.LookAt(
		eye,
		target, 
		mt.V4.up);

	GDD->SetPPfocalDist(Distance(eye, target));
	pCam->GetTransform()->SetLocalMatrix(tmpmat);
	pCam->Update();
	mObserveTime += gTimer.GetEllapsed() * 0.5f;// * 0.1f;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
// camera around ship (previously menu camera)
void ZProtoGUI::TickCameraMotion()
{
	static float tthuh = .4f;
	static tvector3 ppos, pdir, pright, pup;
	static tvector2 pSens;

	GDD->SetPPfocalDist(1.f);
	GDD->SetPPfocalFactor(0.045f);

	ZCamera* pCam = GetActiveCamera();

	tmatrix tmpmat;
	tvector3 travpos = ppos + (pSens.x-.5f)*(tthuh*pright) + pSens.y*(tthuh*pup);
	tmpmat.LookAt(travpos, travpos+pdir, tvector3::YAxis);
	pCam->GetTransform()->SetLocalMatrix(tmpmat);
	pCam->Update();
	tthuh += gTimer.GetEllapsed()*0.05f;
	if (tthuh>=.4f)
	{
		tthuh = -.4f;
		// compute new vector
		float rnd1 = float (rand()&0xFF) / 255.0f * ZPI * 2.0f;
		float rnd2 = float (rand()&0xFF) / 255.0f * ZPI;
		ppos = tvector3(cos(rnd1), sin(rnd2), sin(rnd1));
		ppos.Normalize();
		pdir = -ppos;
		ppos*=4.f;
		pright.Cross(pdir, tvector3::YAxis);
		pright.Normalize();
		pup.Cross(pdir, pright);

		switch (rand()&7)
		{
		case 0: pSens=tvector2(1,0); break;
		case 1: pSens=tvector2(-1,0);break;
		case 2: pSens=tvector2(0,1);break;
		case 3: pSens=tvector2(0,-1);break;

		case 4: pSens=tvector2(1,1); break;
		case 5: pSens=tvector2(-1,1);break;
		case 6: pSens=tvector2(-1,-1);break;
		case 7: pSens=tvector2(1,-1);break;
		}

	}
}
#endif
