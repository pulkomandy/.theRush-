#include "stdafx.h"

#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"

#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
//#include "TransformationHistory.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"
using namespace RakNet;

#include "ZNetFactory.h"
#include "ZRushNetGame.h"
#include "ZNetShip.h"

#include "ZNetMissile.h"
#include "ZNetMine.h"
#include "ZNetExplosion.h"


static const int DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES=50;


Replica2* GameConnection::Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, 
									ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)
{
	char objectName[128];
	StringTable::Instance()->DecodeString(objectName,128,replicaData);
	SystemAddress aOwner;
	replicaData->Read((char*)&aOwner, sizeof(SystemAddress));

	LOG("NET : New Replicated Object (%s) from %s\n", objectName, aOwner.ToString());

	if (strcmp( objectName, "NetShip" )==0)
	{
		ZNetShip *pShip = (ZNetShip*)mBelongingGame->AddShip("Feisar", false);
		pShip->mNetworkOwner = aOwner;
		pShip->mNetGameBelonging = mBelongingGame;
		pShip->SetReplicaManager(&mBelongingGame->replicaManager2);

		if (mBelongingGame->rakInterface->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS) == aOwner)
		{
			mBelongingGame->SetClientShip(pShip);
		}
		if (GProtoGui)
		{
			GProtoGui->UpdateConnectedPlayers();
		}
		return pShip;//ret;
	}
	
	else if (strcmp(objectName,"NetMissile")==0)
	{
		tmatrix wmat;
		replicaData->Read(wmat);
		ZNetMissile* ret =(ZNetMissile*)_New(ZNetMissile);
		mBelongingGame->AddEntity(ret);

		ret->mNetworkOwner = aOwner;
		ret->mNetGameBelonging = mBelongingGame;
		ret->SetReplicaManager(&mBelongingGame->replicaManager2);

		return ret;
	}
	else if (strcmp(objectName,"NetMine")==0)
	{
		tmatrix wmat;
		replicaData->Read(wmat);
		ZNetMine* ret = (ZNetMine*)_New(ZNetMine);//)mBelongingGame->SpawnMine(wmat);//NetMine::CreateMine(mBelongingGame, aOwner, wmat);
		mBelongingGame->AddEntity(ret);

		ret->mNetworkOwner = aOwner;
		ret->mNetGameBelonging = mBelongingGame;
		ret->SetReplicaManager(&mBelongingGame->replicaManager2);

		return ret;
	}
	else if (strcmp(objectName,"NetExplosion")==0)
	{
		tmatrix wmat;
		replicaData->Read(wmat);
		ZNetExplosion* ret = (ZNetExplosion*)_New(ZNetExplosion);//mBelongingGame->SpawnExplosion(wmat);
		mBelongingGame->AddEntity(ret);

		ret->mNetworkOwner = aOwner;
		ret->mNetGameBelonging = mBelongingGame;
		ret->SetReplicaManager(&mBelongingGame->replicaManager2);

		return ret;
	}
	

	return 0;
}


ZShip* GameFactory::NewNetworkReplicatedShip()
{
	LOG("NET : New Ship Object To Replicate\n");
	ZNetShip *p = (ZNetShip *)mBelongingGame->AddShip("Feisar", true);//_New(ZNetShip);

	p->mNetworkOwner = mCurrentOwnerAddress;
	p->mNetGameBelonging = mBelongingGame;
	p->SetReplicaManager(&mBelongingGame->replicaManager2);

	// Automatically call Serialize every DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES milliseconds and send out changes if they occur
	p->AddAutoSerializeTimer(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);
	// All connected systems should create this object
	p->BroadcastConstruction();
	// Force the first serialize to go out. We could have also just wrote the data in SerializeConstruction()
	// Without this call, no serialize would occur until something changed from the intial value
	p->BroadcastSerialize();

	return p;
}

ZMissile* GameFactory::NewNetworkReplicatedMissile()
{
	LOG("NET : New Missile To Replicate\n");
	ZNetMissile *p = (ZNetMissile*)_New(ZNetMissile);

	p->mNetworkOwner = mCurrentOwnerAddress;
	p->mNetGameBelonging = mBelongingGame;
	p->SetReplicaManager(&mBelongingGame->replicaManager2);

	p->AddAutoSerializeTimer(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);
	p->BroadcastConstruction();
	p->BroadcastSerialize();

	return p;
}

ZMine* GameFactory::NewNetworkReplicatedMine()
{
	LOG("NET : New Mine To Replicate\n");
	ZNetMine *p = (ZNetMine*)_New(ZNetMine);

	p->mNetworkOwner = mCurrentOwnerAddress;
	p->mNetGameBelonging = mBelongingGame;
	p->SetReplicaManager(&mBelongingGame->replicaManager2);

	p->AddAutoSerializeTimer(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);
	p->BroadcastConstruction();
	p->BroadcastSerialize();

	return p;
}


ZExplosion* GameFactory::NewNetworkReplicatedExplosion()
{
	LOG("NET : New Mine To Replicate\n");
	ZNetExplosion *p = (ZNetExplosion*)_New(ZNetExplosion);

	p->mNetworkOwner = mCurrentOwnerAddress;
	p->mNetGameBelonging = mBelongingGame;
	p->SetReplicaManager(&mBelongingGame->replicaManager2);

	p->AddAutoSerializeTimer(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);
	p->BroadcastConstruction();
	p->BroadcastSerialize();

	return p;
}