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
#include "ZRushTrack.h"
#include "ZNetShip.h"
#include "FSMClientBase.h"

BEGIN_SERIALIZATION(ZRushNetGame)
END_SERIALIZATION()

ZRushNetGame::ZRushNetGame() : ZRushGame()
{
	rakInterface = RakNetworkFactory::GetRakPeerInterface();
	static bool mbRKInit = false;
	if (!mbRKInit)
	{
		mbRKInit = true;
		
		StringTable::Instance()->AddString("NetShip",false);
		StringTable::Instance()->AddString("NetMissile",false);
		StringTable::Instance()->AddString("NetMine",false);
		StringTable::Instance()->AddString("NetExplosion",false);	
	}
}

ZRushNetGame::~ZRushNetGame()
{
	if (rakInterface)
	{
		//rakInterface->CloseConnection(UNASSIGNED_SYSTEM_ADDRESS, true);
		rakInterface->Shutdown(0);
		RakNetworkFactory::DestroyRakPeerInterface(rakInterface);
	}
}

void ZRushNetGame::Update(float aTimeEllapsed)
{
	PROFILER_START(RushNetGameUpdate);

	TickPackets(aTimeEllapsed);

	ZRushGame::Update(aTimeEllapsed);

	PROFILER_END();
}

ZShip* ZRushNetGame::AddShip(const char *szShipName, bool bServerSide)
{
	ZNetShip *nShip = (ZNetShip*)_New(ZNetShip);
	
	nShip->Init(this, mPhysicWorld, szShipName, mShips.size(), bServerSide);
	
	mShips.push_back(nShip);

	return nShip;
}

int ZRushNetGame::GetHalfPing(const SystemAddress& addr)
{
	std::map<SystemAddress, int >::iterator iter = mPings.find(addr);
	if ( iter != mPings.end())
	{
		return ((*iter).second>>1);
	}
	return 0;
}

void ZRushNetGame::SendLapTime(ZShip *pShip, float aTime)
{
	if (pShip == mClientShip)
	{
		// send time to server only for the client ship
		// this means that the client-side predicted ship will not send time

		ZRushGame::SendLapTime(pShip, aTime);
		this->SendNetMessage(ZNMSG_ShipLapTime(aTime));
	}
}

void ZRushNetGame::TickPackets(float timeEllapsed)
{
	PROFILER_START(TickPackets);

	Packet *packet;
	packet = rakInterface->Receive();


	while (packet)
	{
		mGameObjectsFactory.SetCurrentOwnerAddress(packet->systemAddress);

		switch (packet->data[0])
		{
		case ID_DISCONNECTION_NOTIFICATION:
			LOG("NET : Disconnection\n");
			this->HandleDisconnectionNotification(packet->systemAddress);
			break;
		case ID_CONNECTION_LOST:
			LOG("NET : Connection Lost\n");
			this->HandleConnectionLost(packet->systemAddress);
			//this->HandleDisconnectionNotification(packet->systemAddress);
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			LOG("NET : Connection request accepted\n");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			LOG("NET : Connection attempt failed\n");
			this->HandleConnectionAttemptFailed();
			break;
		case ID_NEW_INCOMING_CONNECTION:
			LOG("NET : Incoming Connection\n");
			this->HandleIncomingNewConnection(packet->systemAddress);
			break;
		case ZNMSG_ASKSLOT:
			if ( mShips.size() < GTrack->GetSpawnMatrices().size() )
			{
				mGameObjectsFactory.NewNetworkReplicatedShip();
				this->SendNetMessage(ZNMSG_SlotsAvailable(GTrack->GetSpawnMatrices().size()-mShips.size()));
			}
			break;
		case ZNMSG_SERVERMAPLOADINGDONE:
			mFSMClientBase->Handle(ServerMapLoaded());
			break;
		case ZNMSG_CLIENTMAPLOADINGDONE:
			replicaManager2.AddNewConnection(packet->systemAddress);
			break;
		case ZNMSG_SHIPSYNC:
			{
				this->HandleShipSync(packet, timeEllapsed);
				
#if 0
				ZNMSG_ShipSync zmsgstruct;
				zrakpack.Read(zmsgstruct);

				ZNetShip *pShip = GetShipBySystemAddress(packet->systemAddress);
				if (pShip)
				{
					//pShip->mCurrentUserInput = zmsgstruct.mInput;
					int halfPing = mPings[packet->systemAddress]>>1;
					/*
					mInterpolator.PushShipSync(pShip, 
						RakNet::GetTime(),//-GetHalfPing(packet->systemAddress), 
						zmsgstruct.mSync, zmsgstruct.mInput);
						*/

				}
#endif
			}
			break;
		case ZNMSG_USERINPUT:
			{
				RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_UserInput), false);
				ZNMSG_UserInput zmsgstruct;
				zrakpack.Read(zmsgstruct);


				ZNetShip *pShip = GetShipBySystemAddress(packet->systemAddress);
				if (pShip)
				{
					
					pShip->ControlsToPhysic( zmsgstruct.mInput, 0 );

				}
			}
			break;
		case ZNMSG_GAMESET:
			{
				int szZNMSG_GameSet = sizeof(ZNMSG_GameSet);
				RakNet::BitStream zrakpack(packet->data+1, szZNMSG_GameSet, false);
				ZNMSG_GameSet zmsgstruct;
				zrakpack.Read(zmsgstruct);

				// received a new gameset
				this->SetGameSet(zmsgstruct.mGameSet);
			}
			break;
		case ZNMSG_TIMEDTEST:
			this->HandleTimeTest(packet);
			break;
		case ZNMSG_RACESTARTSINNSECONDS:
			{
				RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_RaceStartsInNSeconds), false);
				ZNMSG_RaceStartsInNSeconds zmsgstruct;
				zrakpack.Read(zmsgstruct);

				GProtoGui->ShowTimeBeforeCountDown(zmsgstruct.mSecondsBeforeRaceStarts);
			}
			break;
		case ZNMSG_COUNTDOWNSTARTS:
			{
				mFSMClientBase->mFSMClientLCS.Handle(CountDownStarts());
			}
			break;
		case ZNMSG_SLOTSAVAILABLE:
			{
				RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_SlotsAvailable), false);
				ZNMSG_SlotsAvailable zmsgstruct;
				zrakpack.Read(zmsgstruct);

				GProtoGui->SetNbSlotsAvailable(zmsgstruct.mNbSlotsAvailable);
			}
			break;
		case ZNMSG_RACEENDSINNSECONDS:
			{
				RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_RaceEndsInNSeconds), false);
				ZNMSG_RaceEndsInNSeconds zmsgstruct;
				zrakpack.Read(zmsgstruct);

				GProtoGui->ShowTimeBeforeRaceEnds(zmsgstruct.mSecondsBeforeRaceEnds);
			}
			break;
		case ZNMSG_NEXTMAPINNSECONDS:
			{
				RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_NextMapInNSeconds), false);
				ZNMSG_NextMapInNSeconds zmsgstruct;
				zrakpack.Read(zmsgstruct);

				GProtoGui->ShowTimeBeforeNextMap(zmsgstruct.mSecondsBeforeNextMap);
			}
			break;
		case ZNMSG_SHIPLAPTIME:
			{
				RakNet::BitStream zrakpack(packet->data+1, sizeof(ZNMSG_ShipLapTime), false);
				ZNMSG_ShipLapTime zmsgstruct;
				zrakpack.Read(zmsgstruct);

				ZShip *pShip = GetShipBySystemAddress(packet->systemAddress);
				if (pShip)
				{
					mLapsTime.PushTime(pShip, zmsgstruct.mShipLapTime);
				}
				else
				{
					LOG("Can't find ship %s for lap push time", packet->systemAddress.ToString());
				}
				// broadcast results
				mLapsTime.BroadCast(rakInterface);
			}
			break;
		case ZNMSG_GAMELAPSTIMES:
			{
				RakNet::BitStream zrakpack(packet->data+1, packet->bitSize, false);
				mLapsTime.Read(this, zrakpack);
			}
			break;
		case ZNMSG_USEBONUS:
			{
				ZShip *pShip = GetShipBySystemAddress(packet->systemAddress);
				if (pShip)
					this->HandleShipUseBonus(pShip);
			}
			break;
		default:
			LOG("NET : Unsupported packet ID %d\n", packet->data[0]);
			break;
		}

		rakInterface->DeallocatePacket(packet);
		packet = rakInterface->Receive();
	}	

	
	PROFILER_END();
}

ZNetShip* ZRushNetGame::GetShipBySystemAddress(const SystemAddress& addr)
{
	std::list<smartptr<ZShip> >::iterator iter = mShips.begin();
	for (;iter != mShips.end();++iter)
	{
		ZNetShip *ps = (ZNetShip*)(*iter).ptr();
		if (ps->mNetworkOwner == addr)
			return ps;
	}

	return NULL;
}
