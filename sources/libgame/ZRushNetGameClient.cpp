#include "stdafx.h"
#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"
using namespace RakNet;

#include "ZNetFactory.h"

#include "ZRushNetGameClient.h"
#include "ZNetShip.h"

#include "ZNetMissile.h"
#include "ZNetMine.h"
#include "ZNetExplosion.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZRushNetGameClient)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZRushNetGameClient::ZRushNetGameClient() : ZRushNetGame()
{
	mSendMessageTime = 0;
	mbSupportDamages = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZRushNetGameClient::~ZRushNetGameClient()
{
#ifdef _DEBUG
	LOG("NET CLIENT destroyed\n");
#endif


	BitStream bitStream;
	unsigned char msgid = ID_CONNECTION_LOST;
	bitStream.Write(msgid);
	rakInterface->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::Update(float aTimeEllapsed)
{
	PROFILER_START(RushNetGameClientUpdate);

	ZRushNetGame::Update(aTimeEllapsed);

	SendShipKeys();
	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::Connect(const tstring& url, unsigned short aPort)
{
	networkIdManager.SetIsNetworkIDAuthority(false);
	rakInterface->AttachPlugin(&replicaManager2);
	rakInterface->SetNetworkIDManager(&networkIdManager);
	// Register our custom connection factory
	mGameObjectsFactory.mBelongingGame = this;
	replicaManager2.SetConnectionFactory(&mGameObjectsFactory);
	replicaManager2.SetAutoAddNewConnections(true);

	// --
	SocketDescriptor socketDescriptor(0,0);
	rakInterface->Startup(1, 30, &socketDescriptor, 1);
	rakInterface->Connect(url.c_str(), aPort, 0, 0);
	rakInterface->SetOccasionalPing(true);
	mServerAdress = url;
	mServerPort = aPort;
	LOG("NET : Connecting %s on port %d\n", url.c_str(), aPort);
	GProtoGui->ShowConnectingToServer(url.c_str());

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::SendShipKeys()
{
	if (mClientShip)
	{
		mSendMessageTime += gTimer.GetEllapsed();
		static const float aSendInputFrequency = 1.f/20.f;
		if (mSendMessageTime >= aSendInputFrequency)
		{
			//ZASSERT(mNetGameBelonging, "This ship doesn't belong to any net game.");
			ZNMSG_UserInput aUserInput;
			aUserInput.mInput = mClientShip->GetCurrentControls();
			
			SendNetMessageFast(aUserInput);
			mSendMessageTime -= aSendInputFrequency;
			
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::HandleShipSync(Packet *packet, float timeEllapsed)
{
	RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_ShipSync), false);
	unsigned short nbShips;
	zrakpack.Read(nbShips);
	for (int i=0;i<nbShips;i++)
	{
		SystemAddress addr;
		zrakpack.Read(addr);
		ZNetShip *pShip = GetShipBySystemAddress(addr);
		if (pShip)
		{
			tvector3 serverPos;
			zrakpack.Read(serverPos);
			tquaternion serverQuat;
			zrakpack.Read(serverQuat);
			keysStruct ks;
			zrakpack.Read(ks);
			if (pShip != mClientShip)
			{
				pShip->ControlsToPhysic(ks, timeEllapsed);
			}
			pShip->GetShipPhysic()->SetServerResync(ServerShipSync_t(serverPos, serverQuat));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
	
void ZRushNetGameClient::HandleConnectionAttemptFailed()
{
	GProtoGui->BuildUnableToConnect(GLoc->GetString("DISCON").c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::HandleConnectionLost(const SystemAddress &addr) 
{
	GProtoGui->HideEndResults();
	GProtoGui->BuildDisconnection(GLoc->GetString("DISCON").c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::EnableAllShipsControls()
{
	if (mClientShip)
		mClientShip->SetInteractionType(INTERACTION_USER_INPUT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushNetGameClient::SendShipUseBonus(ZShip *pShip)
{
	SendNetMessage(ZNMSG_UseBonus());
	pShip->UseBonus();
}
