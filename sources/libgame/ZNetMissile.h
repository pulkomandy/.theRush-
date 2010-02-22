#ifndef ZNETMISSILE_H__
#define ZNETMISSILE_H__

#include "ZMissile.h"
#include "ReplicaManager2.h"
using namespace RakNet;

class ZNetMissile : public ZMissile, public Replica2
{
public:
    DECLARE_SERIALIZABLE
public:
	ZNetMissile();
	virtual ~ZNetMissile();

	virtual void BroadcastDestruction() { Replica2::BroadcastDestruction(); }

	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	SystemAddress mNetworkOwner;
	ZRushNetGame *mNetGameBelonging;
};

#endif