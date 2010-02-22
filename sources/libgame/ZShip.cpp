#include "stdafx.h"
#include "../libplatform/libplatform.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZShip)
SERIALIZE_FLOAT(mBrakeLerpSpeed)
SERIALIZE_FLOAT(mAeroAngleLeftRight)
SERIALIZE_FLOAT(mAeroAngleMid)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////

char *ZShip::mBotNames[]={
"August Oannes","Lasseya Arlos","Corana Mrrgwharr","Seit Sunfell","Roy Zatoq",
"Shivas Beelen","Damien Aban","Vaj Bahol","Rangu Keel",
"Athala Pavish","Lisa Reed","Lesher Reed","Logra Talonspyre",
"Rob Hardin","Jens Droma","Jope Bela-Trepada","Minno Dol",
"Thoja Crow","Tulan Silero","Khorde Chi","Irizon Bain",
"Abav Parlayy","Noval Zarbo","J.P. Donos","Jorg Vandorack",
"Mena Styles","Lana Charr","Henri Oligard","Indiglo Hooge",
"Julia Tonor","Keth Irimore","Doja Dyer","Darin Greenback",
"Joshua Jerikko","Loretta  Tachi","Herub Leuf","Joti Damar",
"Kalira Lars","Mikal Renning","Mal Varm","Nomar Trite",
"Ait Vapasi","John Filtayn","Vyrim Gregory","Zeno Thrall",
"Palaras Amadis","Sei Xen","Montross Sa-Vin","Hak Hocha",
"Huff Brie","Benny Arcturus","Neo Paul","Ben Tucker",
"Miwa Toarinar","Gramor Formoe","Rath Terrik","Rizann Walker",
"Raihane Richardson","Megara Davout","Bic  Orailus","Wain Cass",
"Mando Panne","Gadon Vox","Kraton Zamba","Jalek Flynn",
"Kennex H'darr","Markus Greeta","Vifi Kiyanu","Mikal Vel Aath",
"Caralho Riv","Geopin Rhane","Telon Mishkoll","Aklee Cahob",
"Loman Bintaghr","Ral Mackali","Stella Kobylarz","Thes Logan",
"Groms Magnus","Xoquon Fodao","Calaveylon  Kasra","Hersh Voddher",
"Nicole Joyriak","Wor Verbenti","Rellius Tierney","Arash  Gunstar",
"Illusian Odai","Relain Arcaina","Lando Thashin","Crevil  Jien",
"Zaphone Saleem","Amseth Starfall","Erik Asto","Auros Tallon",
"Oret Tanoor","Tarrex Shodon","Thaneo Paesante","Kista Turnell",
"Yun Crane","Gaen Claermoore","Pimmelolymp Alucard"};

///////////////////////////////////////////////////////////////////////////////////////////////////


void ZShip::InitDefaultValues()
{
	
	mSpeed = 0;

	mbHasFinishedLap = false;

	mCurrentLapTime = 0;
	mbLapTimeComputationEnabled = false;

	mCurrentLap = 1;

	mRanking = 1;

	mLife = 1.f;
	mCurrentControls.Zero();

	mBrakeLeftLerp = mBrakeRightLerp = mBrakeMidLerp = 0;
	mInteractionType = INTERACTION_NONE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////


ZShip::ZShip() : mInteractionType(INTERACTION_NONE)
{
	mGame = NULL;
	mBrakeLerpSpeed = 10.0f;
	mAeroAngleLeftRight = -1.0f;
	mAeroAngleMid = -1.0f;	

	InitDefaultValues();

	mBotName = mBotNames[ (rand()&0xFFFF) % (sizeof(mBotNames)/sizeof(char*)) ];
	mShipNameString = "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZShip::~ZShip()
{
	mShipTransform->RemoveFromScene();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::Init(ZRushGame *pGame, ZPhysicWorldBullet *aPhysicWorld, const char *szShipName, int aStart, bool bServerSide)
{
	if (mShipNameString == szShipName)
		return;

	PROFILER_START(ZShipInit);

	mShipNameString = szShipName;
	tmatrix spawnMat;
	if (aStart<0)
		spawnMat = mShipPhysic->GetTransform()->GetWorldMatrix();
	else
		spawnMat = GTrack->GetSpawnMatrices()[aStart];


	mGame = pGame;
	ZTransform *ship = gRushGameContent->GetShip(szShipName)->Clone();

	if (mShipTransform.ptr()&&mShipTransform->GetScene())
		mShipTransform->RemoveFromScene();

	mShipTransform.Attach((ZTransform*)_New(ZTransform));
	ship->SetParent(mShipTransform);


	// trigger
	ZTrigger *pTrig = (ZTrigger*)_New(ZTrigger);
	pTrig->GetOriginVolume().mBSphere = vector4(0, 0, 0, 4.f);
	pTrig->mTriggerType = ZTrigger::TRIGGER_SPHERE;
	pTrig->mFilterGroup = TGSHIPGROUP;
	pTrig->mFilterMask = TGSHIPFILTER;
	pTrig->SetUserPointer(this);

	mShipTransform->AddChild(pTrig->GetTransform());


	mShipTransform->AddToScene(GScene);

	if (bServerSide)
	{
		// hide ship on server
		mShipTransform->SetVisible(false);
	}

	mShipPhysic = new ZShipPhysics(pGame);
	_AddInstance(mShipPhysic);

	mAI = (ZShipAI*)_New(ZShipAI);
	mAI->Init(this);

	// xml properties for ship, physics and IA

	tstring propertiesFileName = gRushGameContent->GetShipDiskPath(szShipName);
	propertiesFileName += szShipName;
	propertiesFileName += ".xml";
	
	std::vector<ZBaseClass*> mToSer;
	mToSer.push_back(this);
	mToSer.push_back(mShipPhysic);
	mToSer.push_back(mAI);

	XMLizeIn(mToSer, propertiesFileName.c_str());

	// --
	ZTransform *potenphys = mShipTransform->GetFirstWithSpatialEntity();
	if (potenphys)
	{
		ZMeshInstance *pInst = (ZMeshInstance *)potenphys->GetSpatialEntity()->QueryInstance();
		if (pInst->GetMesh()->GetPhysicTriMesh())
		{
			mShipPhysic->LoadShip(aPhysicWorld, this, pInst->GetMesh()->GetPhysicTriMesh(), mShipTransform, 
				spawnMat);
		}
		
	}


	// Aero freins
	mAeroD.clear();
	mAeroG.clear();
	mAeroM.clear();

	mShipTransform->FindByName("AeroD", mAeroD);
	mShipTransform->FindByName("AeroG", mAeroG);
	mShipTransform->FindByName("AeroM", mAeroM);

	std::vector<ZTransform*> ShieldTransforms;
	mShipTransform->FindByName("Shield", ShieldTransforms);
	if (!ShieldTransforms.empty())
	{
		mShieldMesh = (ZMeshInstance*)ShieldTransforms[0]->GetSpatialEntity()->QueryInstance();
		if (!mShieldMesh)
		{
			WARN("Shield transform found for ship %s but no spatial entity linked.", szShipName);
		}
	}
	// Effects
	// could be done with this->BuildEffectManager();
	if (pGame->GetClassID() != ClassIDZRushNetGameServer)
		mEffects.Attach((IShipEffects*)_New(ZShipEffects));
	else
		mEffects.Attach((IShipEffects*)_New(ZShipEffectsNULL));

	// Bonus
	if (mGame->GetClassID() == ClassIDZRushGame)
		ZShipBonus::Init( this, true, true );
	else if (mGame->GetClassID() == ClassIDZRushNetGameClient)
		ZShipBonus::Init( this, false, true);
	else if (mGame->GetClassID() == ClassIDZRushNetGameServer)
		ZShipBonus::Init( this, true, false);

	// pour tests
	//mGame->GiveBonusToShip( 0, this );
	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float falseTime = 0.f;
void ZShip::Update(float timeEllapsed)
{
	if (mShipPhysic)
	{
		switch(mInteractionType)
		{
		case INTERACTION_NONE:
			ControlsToPhysic(mCurrentControls, timeEllapsed);
			break;
		case INTERACTION_USER_INPUT:
			TickInputDevice(timeEllapsed);
			falseTime += timeEllapsed;
			break;
		case INTERACTION_IA:
			mAI->ComputeAI(timeEllapsed, &mCurrentControls);
			ControlsToPhysic(mCurrentControls, timeEllapsed);
			break;
		default:
			break;
		}

		mPreviousPos = mShipPhysic->GetPosition();
		mShipPhysic->Update(timeEllapsed);

		// speed computation
		tvector3 lenDif =(mShipPhysic->GetPosition() - mPreviousPos);
		const tmatrix& groundMat = mShipPhysic->GetGroundMatrix();
		tvector3 roadDif = groundMat.V4.right * tvector3(groundMat.V4.right).Dot(lenDif);
		roadDif += groundMat.V4.dir * tvector3(groundMat.V4.dir).Dot(lenDif);
		mSpeed = roadDif.Length() / timeEllapsed;

		// aerofreins
		UpdateBrakesMatrices();

		// lap done check
#define RETAIL


		if ( (mShipPhysic->IsLapDone()) 
#ifndef RETAIL
			|| (falseTime > 2.f) 
#endif
			)
		{
			falseTime -= 2.f;
			mbHasFinishedLap = true;

			mShipPhysic->ClearLapDoneFlag();
		}


		if (HasFinishedLap())
		{
			ClearFinishedLapFlag();
			mGame->SendLapTime(this, GetCurrentLapTime());
			ResetCurrentLapTime();
			mCurrentLap++;
		}


		if (mbLapTimeComputationEnabled)
			mCurrentLapTime += timeEllapsed;

		// Bonus
		ZShipBonus::Tick(timeEllapsed);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float GetInputValueReleased(const char *szCtrl)
{
	int dev, axis, but;
	int dev2, axis2, but2;
	if (GConfig->GetControl(szCtrl, 0, dev, but, axis))
	{
		if (GConfig->GetControl(szCtrl, 1, dev2, but2, axis2))
		{
			float v1 = GetInputDevice()->GetValueReleased(dev, but, axis);
			float v2 = GetInputDevice()->GetValueReleased(dev2, but2, axis2);
			return ZMAX(v1, v2);
		}
		else
		{
			return GetInputDevice()->GetValueReleased(dev, but, axis);
		}
	}
	else
	{
		if (GConfig->GetControl(szCtrl, 1, dev, but, axis))
		{
			return GetInputDevice()->GetValueReleased(dev, but, axis);
		}

	}
	return 0.f;
}

float GetInputValuePushed(const char *szCtrl)
{
	int dev, axis, but;
	int dev2, axis2, but2;
	if (GConfig->GetControl(szCtrl, 0, dev, but, axis))
	{
		if (GConfig->GetControl(szCtrl, 1, dev2, but2, axis2))
		{
			float v1 = GetInputDevice()->GetValuePushed(dev, but, axis);
			float v2 = GetInputDevice()->GetValuePushed(dev2, but2, axis2);
			return ZMAX(v1, v2);
		}
		else
		{
			return GetInputDevice()->GetValuePushed(dev, but, axis);
		}
	}
	else
	{
		if (GConfig->GetControl(szCtrl, 1, dev, but, axis))
		{
			return GetInputDevice()->GetValuePushed(dev, but, axis);
		}

	}
	return 0.f;
}

float GetInputValue(const char *szCtrl)
{
	int dev, axis, but;
	int dev2, axis2, but2;
	if (GConfig->GetControl(szCtrl, 0, dev, but, axis))
	{
		if (GConfig->GetControl(szCtrl, 1, dev2, but2, axis2))
		{
			float v1 = GetInputDevice()->GetValue(dev, but, axis);
			float v2 = GetInputDevice()->GetValue(dev2, but2, axis2);
			return ZMAX(v1, v2);
		}
		else
		{
			return GetInputDevice()->GetValue(dev, but, axis);
		}
	}
	else
	{
		if (GConfig->GetControl(szCtrl, 1, dev, but, axis))
		{
			return GetInputDevice()->GetValue(dev, but, axis);
		}

	}
	return 0.f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::GetHumanControls(keysStruct& ks)
{

	ks.runVal = (int)GetInputValue("Accelerate")*63;
	/*
	if (ks.runVal >0)
	{
		int a=1;
	}
	 */
	ks.rightVal = (int)(GetInputValue("RightTurn")*63);
	ks.leftVal = (int)(GetInputValue("LeftTurn")*63);
	ks.ShootVal = GetInputValueReleased("AttackBonus")>0.2f?1:0;

	ks.leftBrake = (int)(GetInputValue("LeftBrake")*63);
	ks.rightBrake = (int)(GetInputValue("RightBrake")*63);

	

	//mPreviousViewChange = viewchange;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::ControlsToPhysic(const keysStruct& ks, float timeEllapsed)
{
	mCurrentControls = ks;
	//if (!IsControlsAllowed()&&(!mbIAPiloting)) return;
	float RightBrake = 0, LeftBrake = 0;
	if (ks.runVal)
		mShipPhysic->Run();
	else
		mShipPhysic->NoRun();

	bool bturn = false;
	if (ks.leftVal)
	{
		mShipPhysic->TurnLeft();
		bturn = true;
	}

	if (ks.rightVal)
	{
		mShipPhysic->TurnRight();
		bturn = true;
	}

	if (!bturn)
	{
		mShipPhysic->NoTurn();
	}

	mShipPhysic->Brake(0);
	if (ks.rightBrake&&ks.leftBrake)
	{
		mShipPhysic->Brake( float(ZMAX(ks.rightBrake,ks.leftBrake)) * (1.f/63.f) );
		RightBrake = 1;
		LeftBrake = 1;
	}
	else
	{
		if (ks.rightBrake)
		{
			RightBrake = float(ks.rightBrake) * (1.f/63.f);
			mShipPhysic->BrakeRight( RightBrake );
		}
		else if (ks.leftBrake)
		{
			LeftBrake = float(ks.leftBrake) * (1.f/63.f);
			mShipPhysic->BrakeLeft( LeftBrake );
		}
		else
			mShipPhysic->NoBrake();
	}

	if ( ks.ShootVal )
	{
		mGame->SendShipUseBonus(this) ;//>HandleBonus(this);
	}
/*
	if (mRacingGameShip)
		mRacingGameShip->AeroBrake(GetTimer()->GetEllapsed(), LeftBrake, RightBrake);
		*/
	AeroBrake(timeEllapsed, float(ks.leftBrake), float(ks.rightBrake));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::GetControls(keysStruct *pKS) const
{
	*pKS = mCurrentControls;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::TickInputDevice(float timeEllapsed)
{
	if (timeEllapsed == 0.f)
		return;

	keysStruct ks;
	GetHumanControls(ks);
	ControlsToPhysic(ks, timeEllapsed);
		


}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::AeroBrake(float timeEllapsed, float brakeLeft, float brakeRight)
{
	float brakelerp = timeEllapsed * mBrakeLerpSpeed;	

	if (brakeRight) 
		mBrakeLeftLerp = LERP(mBrakeLeftLerp, mAeroAngleLeftRight, brakelerp);
	else
		mBrakeLeftLerp = LERP(mBrakeLeftLerp, 0, brakelerp);

	if (brakeLeft) 
		mBrakeRightLerp = LERP(mBrakeRightLerp, mAeroAngleLeftRight, brakelerp);
	else
		mBrakeRightLerp = LERP(mBrakeRightLerp, 0, brakelerp);

	if (brakeLeft&&brakeRight)
		mBrakeMidLerp = LERP(mBrakeMidLerp, mAeroAngleMid, brakelerp);
	else
		mBrakeMidLerp = LERP(mBrakeMidLerp, 0, brakelerp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::UpdateBrakesMatrices()
{
	tmatrix mMatBrakeLeft, mMatBrakeRight, mMatBrakeMid;

	mMatBrakeLeft.RotationAxis(tvector3::XAxis, mBrakeLeftLerp);
	mMatBrakeRight.RotationAxis(tvector3::XAxis, mBrakeRightLerp);
	mMatBrakeMid.RotationAxis(tvector3::XAxis, mBrakeMidLerp);

	foreach(iter, mAeroM, ZTransform*)
	{
		(*iter)->SetLocalMatrix(mMatBrakeMid);
	}
	foreach(iter, mAeroG, ZTransform*)
	{
		(*iter)->SetLocalMatrix(mMatBrakeLeft);
	}
	foreach(iter, mAeroD, ZTransform*)
	{
		(*iter)->SetLocalMatrix(mMatBrakeRight);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZShip::Reset(int aPos)
{
	InitDefaultValues();
	mShipPhysic->Reset(GTrack->GetSpawnMatrices()[aPos]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
