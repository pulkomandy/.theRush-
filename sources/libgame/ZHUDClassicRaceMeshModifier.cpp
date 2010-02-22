#include "stdafx.h"
#include "../libplatform/libplatform.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZHUDClassicRaceMeshModifier)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZHUDClassicRaceMeshModifier::ZHUDClassicRaceMeshModifier()
{
	mCurrentBonus = -1;
	mHUDBaseTexture = DirectLoadTex("./Prototype/Common/HUD/hud.dds");
	mHUDMesh->GetMaterial(0)->addTexture(mHUDBaseTexture);
	ZFx *guifx = DirectLoad("./ZenithDatas/HUD.fx");
	mHUDMesh->GetMaterial(0)->setEffect(guifx);
	mCurrentLifeEnergy = 1.f;



	SetRectSource(0, vector4( 22, 20,  63, 76)); // 0
	SetRectSource(1, vector4( 65, 20,  88, 76)); // 1
	SetRectSource(2, vector4( 88, 20, 128, 76)); // 2
	SetRectSource(3, vector4(128, 20, 167, 76)); // 3
	SetRectSource(4, vector4(167, 20, 207, 76)); // 4
	SetRectSource(5, vector4(207, 20, 248, 76)); // 5
	SetRectSource(6, vector4(248, 20, 291, 76)); // 6
	SetRectSource(7, vector4(291, 20, 326, 76)); // 7
	SetRectSource(8, vector4(326, 20, 365, 76)); // 8
	SetRectSource(9, vector4(365, 20, 405, 76)); // 9


	SetRectSource(10, vector4(  464,29, 498, 64)); // LAP 10
	SetRectSource(11, vector4(685,30, 715, 60)); // / 11
	SetRectSource(12, vector4(645,101, 655, 111)); // " 12
	SetRectSource(13, vector4(637,101, 644, 111)); // ' 13
	SetRectSource(14, vector4(565,30, 630, 61)); // POS 14

	SetRectSource(15, vector4(  64,257, 314, 342)); // top energy 15
	SetRectSource(16, vector4(  64,377, 314, 462));  // down speed 16
	SetRectSource(17, vector4(  394,342, 644, 257)); // grid 17
	SetRectSource(18, vector4(  394,462, 644, 377)); // grid 18


	SetRectSource(19, vector4(  0,505, 313, 646)); // Damage red 
	SetRectSource(20, vector4(  0,659, 313, 646+154)); // Damage green

	SetRectSource(21, vector4(  504,835, 840, 880)); // LOCKED

	SetRectSource(22, vector4(  365,750, 365+360, 750+62)); // GRAVITY GRIP


	SetRectSource(23, vector4(  415,918, 415+386, 918+54)); // MACHINE GUN
	SetRectSource(24, vector4(  0,918, 266, 918+66)); // autopilot

	SetRectSource(25, vector4(  0,840, 418, 903)); // target locked

	//bonus

	// protect shield
	SetRectSource(53, vector4(  734, 259, 814, 339)); 
	// destruction shield
	SetRectSource(54, vector4(  734+83, 259, 814+83, 339)); 
	// attraction shield
	//SetRectSource(952, vector4(  734+83*2, 259, 814+83*2, 339)) 



	// Energy
	SetRectSource(58, vector4(  734, 259+80, 814, 339+80)); 
	// Mines
	SetRectSource(52, vector4(  734+83, 259+80, 814+83, 339+80)); 
	// Smoke
	SetRectSource(56, vector4(  734+83*2, 259+80, 814+83*2, 339+80)); 



	// AutoPilot
	SetRectSource(59, vector4(  734, 259+80*2, 814, 339+80*2)); 
	// repair
	//SetRectSource(957, vector4(  734+83, 259+80*2, 814+83, 339+80*2)) 
	// Missile
	SetRectSource(51, vector4(  734+83*2, 259+80*2, 814+83*2, 339+80*2)); 


	// Homing
	SetRectSource(55, vector4(  734, 259+80*3, 814, 339+80*3)); 
	// Machine gun
	SetRectSource(57, vector4(  734+83, 259+80*3, 814+83, 339+80*3)); 

	mEsp = 4;
	mScX = 4;
	mScY = 4;
	mRotY = 0.08f;//0.25f;
	mRotZ = 0.08f;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZHUDClassicRaceMeshModifier::~ZHUDClassicRaceMeshModifier()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tmatrix ZHUDClassicRaceMeshModifier::computePanelMatrix(float atrx, float atry, float ngy, float ngz)
{
	tmatrix tm, tm2,tm3, tm4;
	tm3.Translation(atrx, atry,0);
	
	tm.Scaling(mScX, mScY,0);

	tm2.RotationY(ngy);
	tm4.RotationZ(ngz);
	//tm*=tm4;
	//tm*=tm2;
	tm*=tm3;
	

	return tm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZHUDClassicRaceMeshModifier::Tick(float aTimeEllapsed)
{
	if (!mGame)
		return;

	ZShip *pShip = mGame->GetClientShip();
	if (!pShip)
		return;

	typedef struct newBonusEffect_t
	{
		const char *szSoundName;
		unsigned int iconIndex;
	} newBonusEffect_t;
	static newBonusEffect_t newBonusEffect[]={
		{"missile_available", 51 },
		{"homing_missile_available", 55},
		{"smoke_available", 56 },
		{"protection_shield_available", 53},
		{"destruction_shield_available", 54 },
		{"mines_available", 52 },
		{"more_energy_available", 58},
		{"gravity_grip_available", 57},
		{"booster_available", 57},
		{"autopilot_available", 59} };

	PROFILER_START(HUDClassicMeshModifierTick);
	// bonus management

	if (mCurrentBonus != pShip->GetCurrentBonusIndex())
	{
		mCurrentBonus = pShip->GetCurrentBonusIndex();

		if (mCurrentBonus > -1)
		{
			// bonus acquired
			ZSoundSample *pDroidVoiceBonus = (ZSoundSample*)gRushGameContent->GetRes(newBonusEffect[mCurrentBonus].szSoundName);
			ZSoundInstance *pDroidVoiceInstance = pDroidVoiceBonus->Instance();
			pDroidVoiceInstance->setCurrentVolume(float(GConfig->GetQuality("SFXVOLUME")) * 0.01f);
			pDroidVoiceInstance->play();
		}
		else
		{
			// bonus used/lost
		}
		
	}


	// standard stuff

	mCurrentLifeEnergy = LERP(mCurrentLifeEnergy, pShip->GetLife(), gTimer.GetEllapsed()*0.3f);
	StartQuads();

	float ratio;
	ratio = float(GDD->GetWidth()) / float(GDD->GetHeight());

	


	mScX = 4;
	mScY = -4;

	mEsp = 0.3f;

	tmatrix mtTopLeft = computePanelMatrix(-mEsp*ratio*12.f, mEsp*12.f, 0,0);// -mRotY, -mRotZ);
	tmatrix mtTopRight = computePanelMatrix(mEsp*ratio, mEsp*12.f, 0,0);//mRotY, -mRotZ);
	tmatrix mtBottomLeft = computePanelMatrix(-mEsp*ratio, mEsp, 0,0);//-mRotY, mRotZ);
	tmatrix mtBottomRight = computePanelMatrix(mEsp*ratio, mEsp, 0,0);//mRotY, mRotZ);
	tmatrix mtTopMiddle = computePanelMatrix(0, mEsp*12.f, 0,0);//mRotY, -mRotZ);
// --
	float centaine, dizaine, unit;

	float shipSpeed = pShip->GetSpeed() * 5.f;
	DecompNumber(shipSpeed, centaine, dizaine, unit);

	PushQuad((int)centaine,tvector2(0  + 0.5f, 0.40f*0.5f + 0.5f), tvector2(0.1f,0.1f), mtBottomRight);
	PushQuad((int)dizaine,tvector2(0.1f+ 0.5f, 0.40f*0.5f + 0.5f), tvector2(0.1f,0.1f), mtBottomRight);
	PushQuad((int)unit,tvector2(0.2f+ 0.5f, 0.40f*0.5f + 0.5f), tvector2(0.1f,0.1f), mtBottomRight);


	// energy/speed factor
	PushQuad(15,tvector2(0.5f,0.5f), tvector2(0.5f,0.25f), mtBottomRight, tcolor::white, mCurrentLifeEnergy/*GetEnergyFactor()*/);
	PushQuad(16,tvector2(0.5f,0.75f), tvector2(0.5f,0.25f), mtBottomRight, tcolor::white, shipSpeed/600.0f);

	PushQuad(17,tvector2(0.5f,0.75f), tvector2(0.5f,0.25f), mtBottomRight); // grid speed
	PushQuad(18,tvector2(0.5f,0.5f), tvector2(0.5f,0.25f), mtBottomRight);  // grid energy

	// pos/lap
	PushQuad(14,tvector2(0.5f  ,0.05f), tvector2(0.2f,0.2f), mtTopRight);
	PushQuad (pShip->GetRanking()/*GetCurrentPos()*/, tvector2(0.7f  ,0.05f), tvector2(0.1f,0.18f), mtTopRight);
	PushQuad(11,tvector2(0.8f  ,0.05f), tvector2(0.08f,0.18f), mtTopRight);
	PushQuad( mGame->GetClientCount() ,tvector2(0.88f  ,0.05f), tvector2(0.1f,0.18f), mtTopRight);

	PushQuad(10,tvector2(0.53f  ,0.25f), tvector2(0.18f,0.16f), mtTopRight);
	PushQuad( pShip->GetCurrentLap(), tvector2(0.7f  ,0.25f), tvector2(0.1f,0.14f), mtTopRight);
	PushQuad(11,tvector2(0.8f  ,0.25f), tvector2(0.08f,0.14f), mtTopRight);
	PushQuad( mGame->GetGameSet().mNbLaps, tvector2(0.88f  ,0.25), tvector2(0.1f,0.14f), mtTopRight);

	// current time

	float minutes, secondes, dixiemes;
	DecompTime( pShip->GetCurrentLapTime()*100.f, minutes, secondes, dixiemes);
	float cmin, dmin, umin;
	DecompNumber(minutes, cmin, dmin, umin);
	PushQuad( (int)dmin,tvector2(0.00  ,0.02f), tvector2(0.08f,0.1f), mtTopLeft);
	PushQuad( (int)umin,tvector2(0.08f  ,0.02f), tvector2(0.08f,0.1f), mtTopLeft);

	PushQuad(12,tvector2(0.16f  ,0.02f), tvector2(0.06f,0.03f), mtTopLeft);

	float csec, dsec, usec;
	DecompNumber(secondes, csec, dsec, usec);
	PushQuad( (int)dsec,tvector2(0.22f  ,0.02f), tvector2(0.08f,0.1f), mtTopLeft);
	PushQuad( (int)usec,tvector2(0.30f  ,0.02f), tvector2(0.08f,0.1f), mtTopLeft);

	PushQuad(13,tvector2(0.38f  ,0.02f), tvector2(0.03f,0.03f), mtTopLeft);

	float cdix, ddix, udix;
	DecompNumber(dixiemes, cdix, ddix, udix);
	PushQuad( (int)ddix,tvector2(0.41f  ,0.02f), tvector2(0.04f,0.05f), mtTopLeft);
	PushQuad( (int)udix,tvector2(0.45f  ,0.02f), tvector2(0.04f,0.05f), mtTopLeft);

	const std::vector<float>* listTimes = mGame->GetLapsTimes().GetTimesForShip(pShip);


	if (listTimes)
	{
		int i = 0;
		for (int itt = (listTimes->size()-1); itt>=0 ; itt--)
		{
			float minutes = 0, secondes = 0, dixiemes = 0;
			DecompTime((*listTimes)[itt]*100.f, minutes, secondes, dixiemes);
			float cmin, dmin, umin;
			DecompNumber(minutes, cmin, dmin, umin);
			float csec, dsec, usec;
			DecompNumber(secondes, csec, dsec, usec);
			float cdix, ddix, udix;
			DecompNumber(dixiemes, cdix, ddix, udix);
			/*
			LOG("%d %d - %d %d - %d %d \n", (int)dmin, (int)umin, (int)dsec,
				(int)usec, (int)ddix, (int)udix);
				*/

			PushQuad( (int)dmin,tvector2(0.00 +0.2f ,0.12f + i*0.05f), tvector2(0.04f,0.05f), mtTopLeft);
			PushQuad( (int)umin,tvector2(0.04f +0.2f ,0.12f + i*0.05f), tvector2(0.04f,0.05f), mtTopLeft);
			PushQuad(12,tvector2(0.08f +0.2f ,0.12f + i*0.05f), tvector2(0.03f,0.015f), mtTopLeft);
			PushQuad( (int)dsec,tvector2(0.11f +0.2f ,0.12f + i*0.05f), tvector2(0.04f,0.05f), mtTopLeft);
			PushQuad( (int)usec,tvector2(0.15f +0.2f ,0.12f + i*0.05f), tvector2(0.04f,0.05f), mtTopLeft);
			PushQuad(13,tvector2(0.19f +0.2f ,0.12f + i*0.05f), tvector2(0.015f,0.015f), mtTopLeft);
			PushQuad( (int)ddix,tvector2(0.2005f +0.2f ,0.12f + i*0.05f), tvector2(0.03f,0.04f), mtTopLeft);
			PushQuad( (int)udix,tvector2(0.225f +0.2f ,0.12f + i*0.05f), tvector2(0.03f,0.04f), mtTopLeft);

			
			i++;
		}
	}

	if (mCurrentBonus>-1)
	{
		/*
		mRotBonusAngle = LERP(mRotBonusAngle, 0, aTimeEllapsed);
		tmatrix mtRotBonus;
		mtRotBonus.RotationX(mRotBonusAngle);
		*/

		PushQuad( newBonusEffect[mCurrentBonus].iconIndex ,tvector2(-0.2f ,0.05f), tvector2(0.4f,0.4f), mtTopMiddle);
	}
	//;
	

	EndQuads();

	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
