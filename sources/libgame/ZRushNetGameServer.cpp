#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"
#include "ZNetFactory.h"
#include "ZRushNetGameServer.h"
#include "ZNetShip.h"
#include "ZMissile.h"
#include "ZMine.h"
#include "ZExplosion.h"
#include "libbase/ZTimer.h"

using namespace RakNet;

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZRushNetGameServer)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZRushNetGameServer::ZRushNetGameServer() : ZRushNetGame()
{
	mfSendSync = 0.f;
	mFSMServerBase.mGame = this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZRushNetGameServer::~ZRushNetGameServer()
{
#ifdef _DEBUG
	LOG("NET SERVER destroyed\n");
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::Update(float aTimeEllapsed)
{
	PROFILER_START(RushNetGameServerUpdate);

	mFSMServerBase.Tick(aTimeEllapsed);
	ZRushNetGame::Update(aTimeEllapsed);

	// times for ping
	mPingTime = 0;
	mPingTime += aTimeEllapsed;
	if (mPingTime>1.0f)
	{
		mPingTime -= 1.0f;
	
		ZNMSG_TimedTest timet;
		timet.mTime = RakNet::GetTime();
		SendNetMessage(timet);
	}

	// ships resync
	SendShipSyncs();

	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::ActAsServer(unsigned short aPort)
{
	networkIdManager.SetIsNetworkIDAuthority(true);
	rakInterface->AttachPlugin(&replicaManager2);
	rakInterface->SetNetworkIDManager(&networkIdManager);

	// Register our custom connection factory
	mGameObjectsFactory.mBelongingGame = this;
	replicaManager2.SetConnectionFactory(&mGameObjectsFactory);
	replicaManager2.SetAutoAddNewConnections(false);
	
	// --
	SocketDescriptor socketDescriptor(aPort,0);
	rakInterface->Startup(32,30,&socketDescriptor, 1);
	rakInterface->SetMaximumIncomingConnections(32);
	rakInterface->SetOccasionalPing(true);

	mPingTime = 0;
	LOG("NET : Starting server on port %d\n", aPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::HandleMapFinishedLoading()
{
	SendNetMessage(ZNMSG_ServerMapLoadingDone());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::SendShipSyncs()
{
	// send ship Syncs
	mfSendSync += gTimer.GetEllapsed();
	static const float sendSyncFrequency = 1.f/20.f;
	if (mfSendSync >= sendSyncFrequency)
	{
		typedef struct syncClients_t
		{
			SystemAddress mAddr;
			tvector3 mPosition;
			tquaternion mRotation;
		} syncClients_t;
		
		BitStream bitStream;
		unsigned char msgid = ZNMSG_SHIPSYNC;
		bitStream.Write(msgid);
		unsigned short nbShips = mShips.size();
		bitStream.Write(nbShips);

		std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
		for (;iter != mShips.end();++iter)
		{
			ZNetShip *ps = (ZNetShip*)(*iter).ptr();
			bitStream.Write(ps->mNetworkOwner);
			bitStream.Write(ps->GetShipPhysic()->mCurrentPosition);
			bitStream.Write(ps->GetShipPhysic()->mCurrentQuaternion);
			bitStream.Write(ps->GetCurrentControls());
		}

		rakInterface->Send(&bitStream, LOW_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		mfSendSync -= sendSyncFrequency;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::HandleTimeTest(Packet *packet)
{
	RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_TimedTest), false);
	ZNMSG_TimedTest zmsgstruct;
	zrakpack.Read(zmsgstruct.mTime);

	RakNetTime clientping = RakNet::GetTime() - zmsgstruct.mTime;
	LOG("Ping with %s is %d\n", packet->systemAddress.ToString(), clientping);
	mPings[packet->systemAddress] = (int)clientping;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::HandleIncomingNewConnection(const SystemAddress& addr)
{
	SendNetMessage(addr, ZNMSG_GameSet(mCurrentGameSet));
	
	if (LoadingPipeIsEmpty())
	{
		SendNetMessage(addr, ZNMSG_ServerMapLoadingDone());
		SendNetMessage(ZNMSG_SlotsAvailable(GTrack->GetSpawnMatrices().size()-mShips.size()));
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::SetGameSet(const GameSet_t& gameSet)
{
	//if (mCurrentGameSet != gameSet)
	{
		mCurrentGameSet = gameSet;
		mCurrentGameSetDirty = true;
		
		mLapsTime.Clear();
	
		// stop replication for connected ships
		std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
		for (; iter != mShips.end(); ++iter)
		{
			ZNetShip *pship = (ZNetShip*)(*iter).ptr();
			replicaManager2.RemoveConnection(pship->mNetworkOwner);
		}

		mFSMServerBase.Handle(NewGameSet());
		SendNetMessage(ZNMSG_GameSet(mCurrentGameSet));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::HandleDisconnectionNotification(const SystemAddress &addr)
{


}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::HandleConnectionLost(const SystemAddress &addr)
{
	ZShip *pShip = GetShipBySystemAddress(addr);
	if (pShip)
	{
		//NetShip::DeleteShipByAddress(this, packet->systemAddress);
		mShips.remove(pShip);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameServer::HandleShipUseBonus(ZShip *pShip)
{
	pShip->UseBonus();
	// we don't broadcast bonus use. or each ship on each client will try to instanciate missile
	
	//SendNetMessage(ZNMSG_UseBonus());
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZMissile* ZRushNetGameServer::SpawnMissile(const tmatrix& dropMatrix)
{
	ZMissile * pMissile = mGameObjectsFactory.NewNetworkReplicatedMissile();
	AddEntity(pMissile);
	return pMissile;
}

ZMine* ZRushNetGameServer::SpawnMine(const tmatrix& dropMatrix)
{
	ZMine * pMine = mGameObjectsFactory.NewNetworkReplicatedMine();
	AddEntity(pMine);
	return pMine;
}

ZExplosion* ZRushNetGameServer::SpawnExplosion(const tmatrix& dropMatrix)
{
	ZExplosion * pExplosion = mGameObjectsFactory.NewNetworkReplicatedExplosion();
	AddEntity(pExplosion);
	return pExplosion;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
