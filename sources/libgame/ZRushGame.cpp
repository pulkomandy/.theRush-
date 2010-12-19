//////////////////////////////////////////////////////////////////////////////////////////////

#include "../libplatform/libplatform.h"
#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"
using namespace RakNet;
#include "ZNetMessages.h"
#include "FSMClientBase.h"



//////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZRushGame)
END_SERIALIZATION()

//////////////////////////////////////////////////////////////////////////////////////////////

ZRushGame::ZRushGame()
{
	mClientShip = NULL;

	mbIsSceneMaster = true;

	mPhysicWorld.Attach( (ZPhysicWorldBullet*)_New(ZPhysicWorldBullet) );
	mPhysicWorld->Init();
	mPhysicUpdateTime = 0.f;
	mCurrentGameSetDirty = false;
	mFSMClientBase = new FSMClientBase;
	mFSMClientBase->mGame = this;
	mbSupportDamages = true;
	mbInGameMenuAllowed = false;
	mbHUDVisible = false;

	mBonusParameters = (ZBonusParameters*)_New(ZBonusParameters);
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZRushGame::~ZRushGame()
{
	delete mFSMClientBase;
	// When track is deleted (in ZenFWGame), it destroys track collision mesh
	// Actually, it adds new jobs in physic world node. And this job add ref on the world itself
	// So, we have to tick the physic world here, wich will dec ref on physic world
	// and it will be destroyed when this game will also dec ref on it.
	mPhysicWorld->Tick(0.f);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::DisableSceneMaster() 
{ 
	mbIsSceneMaster = false; 
}

void ZRushGame::BuildEditorGameTest()
{
	GTrack->AddPhysicMeshesToPhysicWorld(mPhysicWorld);

	mCurrentGameSet.mGameType = GameSet_t::GAME_TYPE_EDITOR;
	mFSMClientBase->mGameMode = GameSet_t::GAME_TYPE_EDITOR;
	mFSMClientBase->mFSMClientFFA.mGame = this;
    mFSMClientBase->mFSMClientFFA.SetState(FSMClientFFA::FFAAskSlot);

	EnableAllShipsControls();
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::SetGameSet(const GameSet_t& gameSet)
{
	//if (mCurrentGameSet != gameSet)
	{
		mCurrentGameSet = gameSet;
		mCurrentGameSetDirty = true;
		mFSMClientBase->mGameMode = gameSet.mGameType;
		mFSMClientBase->Handle(NewGameSet());
		GProtoGui->SetNbSlotsAvailable(0);

		mLapsTime.Clear();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

float GetInputValueReleased(const char *szCtrl);
void ZRushGame::Update(float aTimeEllapsed)
{
	PROFILER_START(RushGameUpdate);


	mFSMClientBase->Tick(aTimeEllapsed);

	if (GTrack)
		GTrack->Tick(aTimeEllapsed);


	if (LoadingPipeIsEmpty())
	{
		mFSMClientBase->Handle(ClientMapLoaded());
		if (this->GetClassID() == ClassIDZRushGame)
			mFSMClientBase->Handle(ServerMapLoaded());

	}


	static int GNbPhysIter = 0;
	static float GNbPhysIterTime = 0;
	GNbPhysIterTime += aTimeEllapsed*mSpeed;


	mPhysicUpdateTime += aTimeEllapsed*mSpeed;
	static const float aUpdateTime = 1.f/60.f;
	if (mPhysicUpdateTime >= aUpdateTime)
	{
		if (mPhysicUpdateTime>1.f)
			mPhysicUpdateTime = aUpdateTime;
#ifndef _DEBUG
		// big big issue when physic update takes more than aUpdateTime
		while (mPhysicUpdateTime >= aUpdateTime)
#endif
		{
			std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
			for (; iter != mShips.end(); ++iter)
			{
				(*iter)->Update(aUpdateTime);
			}

			mPhysicWorld->Tick(aUpdateTime);

			GNbPhysIter ++;
			mPhysicUpdateTime -= aUpdateTime;
		}
	}
	else
	{
		// Interpolate
		std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
		for (; iter != mShips.end(); ++iter)
		{
			(*iter)->Interpolate(mPhysicUpdateTime/aUpdateTime);
		}

	}

	if (GNbPhysIterTime> 5.f)
	{
#ifdef _DEBUG
		LOG("%d Physic iterations in %5.2f seconds\n", GNbPhysIter, GNbPhysIterTime);
#endif
		GNbPhysIterTime = 0;
		GNbPhysIter = 0;
	}
	
	if (!this->IsServer())
	{
		float viewchange = GetInputValueReleased("ViewChange");
		//if ((viewchange>0.2f)&&(mPreviousViewChange <0.1f))
		if (viewchange)
			gCameraMotion.ToggleCameraPiloting();
		//

		if (GProtoGui && mbInGameMenuAllowed && (GetInputDevice()->KeyReleased(TKEY_ESC)||GetInputValueReleased("PauseInGame")))
		{
			GProtoGui->BuildIngameMenu();
		}
	}

	// Ranking
	ComputeRankings();

	// game entities
	ZGameEntitiesManager::Tick(aTimeEllapsed);

	PROFILER_END();
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZShip* ZRushGame::AddShip(const char *szShipName, bool bServerSide)
{
	ZShip *nShip = (ZShip*)_New(ZShip);
	
	nShip->Init(this, mPhysicWorld, szShipName, mShips.size(), bServerSide);
	
	mShips.push_back(nShip);

	return nShip;
}


//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::EnableAllShipsControls()
{
	std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
	for (; iter != mShips.end() ; ++iter)
	{
		if ( (*iter) == mClientShip)
			(*iter)->SetInteractionType(INTERACTION_USER_INPUT);
		else
			(*iter)->SetInteractionType(INTERACTION_IA);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::HandleMapFinishedLoading()
{
	if (mCurrentGameSet.mGameType == GameSet_t::GAME_TYPE_EDITOR)
		return;

	// Solo game -> add bots
	unsigned int nbBotsToSpawn = 0;
	if (mCurrentGameSet.mGameType == GameSet_t::GAME_TYPE_MENU)
		nbBotsToSpawn = GTrack->GetSpawnMatrices().size();
	else
		nbBotsToSpawn = GTrack->GetSpawnMatrices().size()-1;

	for (unsigned int i = 0; i < nbBotsToSpawn; i ++)
	{
		ZShip *nShip2 = AddShip( gRushGameContent->GetRandomShipName().c_str(), false );
	}

	if (mCurrentGameSet.mGameType == GameSet_t::GAME_TYPE_MENU)
		EnableAllShipsControls();
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::HandleNewGameSet()
{
	// clear dirty flag
	mCurrentGameSetDirty = false;
	// new loading
	mShips.clear();
	mClientShip = NULL;
	gCameraMotion.SetCameraUnused();

	if (mbIsSceneMaster)
	{
		GScene->Clear();
	}

	gRushGameContent->LoadMap(mCurrentGameSet.mCurrentWorldName, mCurrentGameSet.mCurrentMapName);
	gAtmosphere->LoadSkybox("./Prototype/Worlds/Stratos/pond.dds");
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::SendLapTime(ZShip *pShip, float aTime)
{
	ZASSERT(mClientShip, "Can't push time for client ship. Game::Ship is NULL");
	mLapsTime.PushTime(pShip, aTime);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::EnableLapTimeComputationForEveryShip()
{
	std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
	for (; iter != mShips.end() ; ++iter)
	{
		(*iter)->EnableLapTimeComputation(true);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::HandleLoadingDone()
{
	if (mbIsSceneMaster)
	{
		GTrack->Compute();
		GScene->AddCluterizeJob();
	}

	GTrack->AddPhysicMeshesToPhysicWorld(mPhysicWorld);


	this->HandleMapFinishedLoading();


	if (GProtoGui)
		GProtoGui->SetProgress(1.f);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::SendNetMessage(const ZNMSG_AskSlot&msg, bool bBroadcast)
{
	// when arrived here, a solo client game asked for a slot
	ZShip *nShip = AddShip( GProtoGui->GetChosenShipString().c_str(), false );
	this->SetClientShip(nShip);
}	

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::SetClientShip(ZShip *pShip)
{
	// Handle Got slot
	mClientShip = pShip;
	

	// branching got slot message
	switch (mFSMClientBase->mGameMode)
	{
	case 0:
		mFSMClientBase->mFSMClientFFA.Handle(GotSlot());
		break;
	case 1: //classic race
		if (this->IsSoloGame())
		{
			mFSMClientBase->mFSMClientFFA.Handle(GotSlot());
		}
		else
		{
			mFSMClientBase->mFSMClientLCS.Handle(LCSGotSlot());
		}
		break;
	case 3: // Editor
		mFSMClientBase->mFSMClientFFA.Handle(GotSlot());
		break;

	}

	// build its HUD
	switch (mCurrentGameSet.mGameType)
	{
	case 0: // Raw
	case 1: // classic
		mHUD.Attach((ZHUDMeshModifier*)_New(ZHUDClassicRaceMeshModifier));
		break;
	default:
		break;
	}
	if (mHUD)
		mHUD->SetGame(this);
	ShowHUD(mbHUDVisible); 
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::GUIAsksForSlot()
{
	switch (mFSMClientBase->mGameMode)
	{
	case 0:
		break;
	case 1: //classic race
		mFSMClientBase->mFSMClientLCS.Handle(PlayerAskedSlot());
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::GUIAsksForSpectator()
{
	mFSMClientBase->mFSMClientLCS.Handle(PlayerAskedSpectator());
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::GUIShipChosen()
{
	switch (mFSMClientBase->mGameMode)
	{
	case 0:
		break;
	case 1: //classic race
		mFSMClientBase->mFSMClientLCS.Handle(ShipSelected());
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::ShowHUD(bool bShow)
{
	mbHUDVisible = bShow;
	if (mHUD.ptr())
		mHUD->Show(bShow);
}

//////////////////////////////////////////////////////////////////////////////////////////////

const char *ZRushGame::GetNickName(ZShip *pShip)
{
	if (pShip == mClientShip)
	{
		return GConfig->GetPlayerName();
	}
	else 
		return pShip->GetBotName();
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::RestartRace()
{
	int av = 0;
	std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
	for (; iter != mShips.end() ; ++iter)
	{
		(*iter)->Reset( av++ );
	}
	// reset FSM
	mFSMClientBase->mFSMClientFFA.SetState(FSMClientFFA::FFAPlaying);
	// reset laps times
	mLapsTime.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////

int ZRushGame::RankingSort(const void *p1, const void *p2)
{
	ShipRanking_t *r1 = (ShipRanking_t*)p1;
	ShipRanking_t *r2 = (ShipRanking_t*)p2;

	if (r1->nbBricksRunned < r2->nbBricksRunned)
		return 1;
	if (r1->nbBricksRunned > r2->nbBricksRunned)
		return -1;
	return 0;
}

void ZRushGame::ComputeRankings()
{
	ShipRanking_t ranks[128];
	int av = 0;
	std::list<smartptr<ZShip> >::const_iterator iter = mShips.begin();
	for (; iter != mShips.end() ; ++iter)
	{
		ranks[av].pShip = (*iter).ptr();
		ranks[av].nbBricksRunned = (*iter)->GetShipPhysic()->GetNbBricksRunned();
		av++;
	}
	qsort(ranks, av, sizeof(ShipRanking_t), RankingSort);

	for (int i = 0; i< av ; i++)
	{
		ranks[i].pShip->SetRanking(i+1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::HandleShipCollision( ZShip *pShip, ZBaseClass *pOther, float fDistance)
{
	if (mbSupportDamages)
	{
		pShip->DecLife( 0.03f );//* fDistance
	}
	pShip->GetEffects()->Collision();
		//3 cas:
	// server -> dec life
	// client : FX
	// solo : dec life + FX
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::AllowInGameMenu(bool bAllow)
{
	mbInGameMenuAllowed = bAllow;
	if ( (!bAllow) && GProtoGui)
	{
		GProtoGui->HideInGameMenu();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::GiveBonusToShip(char aBonusNb, ZShip *pShip)
{ 
	pShip->GiveBonus(aBonusNb); 
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZRushGame::SendShipUseBonus(ZShip *pShip)
{
	pShip->UseBonus();
}

//////////////////////////////////////////////////////////////////////////////////////////////
