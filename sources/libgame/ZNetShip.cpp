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

#include "ZNetShip.h"
#include "ZRushNetGame.h"

BEGIN_SERIALIZATION(ZNetShip)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZNetShip::ZNetShip() : ZShip()
{
	mNetGameBelonging = NULL;
	mbNetworkNeedsSync = false;
}

ZNetShip::~ZNetShip()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZNetShip::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	StringTable::Instance()->EncodeString("NetShip", 128, bitStream);
	bitStream->Write((char*)&mNetworkOwner, sizeof(SystemAddress));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZNetShip::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	if ( (mbNetworkNeedsSync) || (mBonusState.Values.Dirty) )
	{
		bitStream->Write(mLife);
		char aBonus = this->GetCurrentBonusIndex();
		bitStream->Write(aBonus);
		bitStream->Write(mBonusState.BonusStateByte);
		mbNetworkNeedsSync = false;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZNetShip::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	bitStream->Read(mLife);
	char aBonus;
	bitStream->Read(aBonus);
	this->GiveBonus(aBonus);
	bitStream->Read(mBonusState.BonusStateByte);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZNetShip::GetHumanControls(keysStruct& ks)
{
	ZShip::GetHumanControls(ks);
	mCurrentControls = ks;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZNetShip::DecLife(float aVal)
{
	if (mBonusState.Values.ProtectionShield)
		return;
	ZShip::DecLife(aVal);
	mbNetworkNeedsSync = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZNetShip::IncLife(float aVal)
{
	ZShip::DecLife(aVal);
	mbNetworkNeedsSync = true;
}
#if 0
void ZNetShip::Update(float timeEllapsed)
{
	if (mShipPhysic)
	{
		switch(mInteractionType)
		{
		case INTERACTION_USER_INPUT:
			TickInputDevice(timeEllapsed);
			break;
		default:
			ControlsToPhysic(mCurrentControls, timeEllapsed);
			break;
		}
		mShipPhysic->Update(timeEllapsed);
		UpdateBrakesMatrices();
/*
		if (!mNetGameBelonging->IsServer())
		{
			static const float timeBetween2syncs = 4.f;
			mTimeBeforeResync += timeEllapsed;
			if (mTimeBeforeResync >= timeBetween2syncs)
			{
				mTimeBeforeResync -= timeBetween2syncs;

				ShipSync_t curSync;
				GetSync(&curSync);
				mNetGameBelonging->SendNetMessage(ZNMSG_ShipSync(curSync, mCurrentControls));
			}
		}
		*/
	}
<<<<<<< .mine

}=======
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZNetShip::GiveBonus(char aBonusNb)
{
	if (this->GetCurrentBonusIndex() != aBonusNb)
	{
		ZShip::GiveBonus(aBonusNb);
		mbNetworkNeedsSync = true;
	}
}
