#ifndef ZNETEXPLOSION_H__
#define ZNETEXPLOSION_H__

#include "ZExplosion.h"
#include "ReplicaManager2.h"
using namespace RakNet;

class ZNetExplosion : public ZExplosion, public Replica2
{
public:
    DECLARE_SERIALIZABLE
public:
	ZNetExplosion();
	virtual ~ZNetExplosion();

	virtual void BroadcastDestruction() { Replica2::BroadcastDestruction(); }

	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	SystemAddress mNetworkOwner;
	ZRushNetGame *mNetGameBelonging;
};

#endif