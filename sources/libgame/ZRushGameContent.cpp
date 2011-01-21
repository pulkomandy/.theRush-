#include "ZRushGameContent.h"
#include "libbase/ZFile.h"
#include "ZRushTrack.h"
#include "ZProtoGUI.h"
#include "libplatform/ZenFWLoaderNode.h"

BEGIN_SERIALIZATION(ZRushGameContent)
END_SERIALIZATION()

ZRushGameContent *gRushGameContent = NULL;

ZRushGameContent::ZRushGameContent()
{
	gRushGameContent = this;
	mCSLoadMap = NewCriticalSection();
	mCurrentWorld = "";
	mCurrentMap = "";

	mCommonRes.Attach((ZGameResources*)_New(ZGameResources));
	mShips.Attach((ZGameResources*)_New(ZGameResources));
	mDecors.Attach((ZGameResources*)_New(ZGameResources));
	mMapBricks.Attach((ZGameResources*)_New(ZGameResources));

	ZFile::GetFilesList(mTeams, "Prototype/Teams/", "", false, true, false);
	assert(mTeams.size() > 0);
	ZFile::GetFilesList(mWorlds, "Prototype/Worlds/", "", false, true, false);
	ZFile::GetFilesList(mMusicsFileNames, "Prototype/Common/Musics/", "", false, false, true);	

	// teams
	for (unsigned int i=0;i<mTeams.size();i++)
	{
		const tstring &teamName = mTeams[i];
		ZFile::GetFilesList(mTeamsShipsNames[teamName], tstring("Prototype/Teams/")+teamName+"/", "", false, true, false);
	}

	// worlds
	for (unsigned int i=0;i<mWorlds.size();i++)
		ZFile::GetFilesList(mWorldsTracksNames[mWorlds[i]], tstring("Prototype/Worlds/")+mWorlds[i]+"/Tracks/", ".track", false, false, false);

	mbBaseDatasLoaded = false;

}

void ZRushGameContent::LoadBaseDatas()
{
	if (!mbBaseDatasLoaded)
	{
		mbBaseDatasLoaded = true;


		// Standard shits
		mCommonRes->AddResource("./Prototype/Common/HUD/crosshair.dds", "CrossHair");
		// standard sounds
		mCommonRes->AddResource("./Prototype/Common/Sounds/MenuError.wav", "MenuError");
		mCommonRes->AddResource("./Prototype/Common/Sounds/MenuNoCancel.wav", "MenuNoCancel");
		mCommonRes->AddResource("./Prototype/Common/Sounds/MenuQuestion.wav", "MenuQuestion");
		mCommonRes->AddResource("./Prototype/Common/Sounds/MenuYesOK.wav", "MenuYesOK");
		// objects
		mCommonRes->AddResource("./Prototype/Common/GameObjects/missile.DAE", "Missile");
		mCommonRes->AddResource("./Prototype/Common/GameObjects/mine.DAE", "Mine");

		// Droid voice
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/missile_available.wav", "missile_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/homing_missile_available.wav", "homing_missile_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/smoke_available.wav", "smoke_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/protection_shield_available.wav", "protection_shield_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/destruction_shield_available.wav", "destruction_shield_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/mines_available.wav", "mines_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/more_energy_available.wav", "more_energy_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/gravity_grip_available.wav", "gravity_grip_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/booster_available.wav", "booster_available");
		mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/autopilot_available.wav", "autopilot_available");
		//mCommonRes->AddResource("./Prototype/Common/Sounds/Voice/machinegun_available.wav", "machinegun_available");

		// ships

		const std::vector<tstring>& aTeams = GetTeams();
		foreach_const(iter, aTeams, tstring)
		{
			const std::vector<tstring>& aShips = GetShipsNames( (*iter) );
			foreach_const(iters, aShips, tstring)
			{
				tstring toLoad;
				toLoad.Printf("Prototype/Teams/%s/%s/%s.DAE", 
					(*iter).c_str(),
					(*iters).c_str(),
					(*iters).c_str() );
				tstring toPath;
				toPath.Printf("Prototype/Teams/%s/%s/", 
					(*iter).c_str(),
					(*iters).c_str() );

				mNamedShipsDiskPath[(*iters)] = toPath;


				mNamedShips[(*iters)]=( ((ZMeshInstance*)mGLoaderService->CreateInstance(toLoad.c_str(), mShips))->GetTransform()->GetRootTransform() );
			}
		}
	}
}

ZRushGameContent::~ZRushGameContent()
{
	gRushGameContent = NULL;
	DestroyCriticalSection(mCSLoadMap);
}

/*
	
*/
const std::vector<tstring>& ZRushGameContent::GetTeams()
{
	return mTeams;
}
const std::vector<tstring>& ZRushGameContent::GetWorlds()
{
	return mWorlds;
}
const std::vector<tstring>& ZRushGameContent::GetShipsNames(const tstring& sTeam)
{
	return mTeamsShipsNames[sTeam];
}
const std::vector<tstring>& ZRushGameContent::GetTracksNames(const tstring& sWorld)
{
	return mWorldsTracksNames[sWorld];
}

/* 
	Get Standard resource from menu, bonus,...
*/
ZBaseClass * ZRushGameContent::GetRes(const tstring& sResName)
{
	return mCommonRes->GetResource(sResName.c_str());
}

/*
	Get Ship
*/
ZTransform * ZRushGameContent::GetShip(const tstring& sShipName)
{
	return mNamedShips[sShipName];
}

ZTransform * ZRushGameContent::GetRandomShip()
{
	const std::vector<tstring>& aShipsNames = GetShipsNames(GetTeams()[rand()%GetTeams().size()]);
	return mNamedShips[aShipsNames[rand()%aShipsNames.size()]];
}

const tstring& ZRushGameContent::GetRandomShipName()
{
	assert(GetTeams().size() > 0);
	const std::vector<tstring>& aShipsNames = GetShipsNames(GetTeams()[rand()%GetTeams().size()]);
	return aShipsNames[rand()%aShipsNames.size()];
}

const tstring& ZRushGameContent::GetShip(int teamIdx, int shipIdx)
{
	const std::vector<tstring>& aShipsNames = GetShipsNames( GetTeams()[teamIdx] );
	return aShipsNames[ shipIdx ];
}

/*
	Get Ship disk path
*/
const char* ZRushGameContent::GetShipDiskPath(const tstring& sShipName)
{
	return mNamedShipsDiskPath[sShipName].c_str();
}
/*
	Load a map:
	Decors , brick and .track
*/
void ZRushGameContent::LoadWorld(const tstring& sWorldName)
{
	// world
	if (mCurrentWorld != sWorldName)
	{
		mCurrentWorld = sWorldName;

		tstring worldPath = "./Prototype/Worlds/";
		worldPath += sWorldName;
		tstring bricksName = worldPath;
		bricksName += "/Bricks.DAE";
		tstring bonusName = worldPath;
		bonusName += "/Bonus.DAE";

		mMapBricks->Clear();
		mGLoaderService->CreateInstance(bricksName.c_str(), mMapBricks.ptr());
		mGLoaderService->CreateInstance(bonusName.c_str(), mMapBricks.ptr());
	}
}

void ZRushGameContent::LoadMap(const tstring& sWorldName, const tstring& sMapName)
{
	LockCriticalSection(mCSLoadMap);

	LoadBaseDatas();

	LoadWorld(sWorldName);

	// map
	if ( (!GTrack) || (mCurrentMap != sMapName) )
	{
		mCurrentMap = sMapName;

		if (GProtoGui)
			GProtoGui->BuildLoadingScreen(mCurrentMap.c_str());

		mGLoaderService->CreateInstance(mCurrentMap.c_str());
	}

	UnLockCriticalSection(mCSLoadMap);
}
