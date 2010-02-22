#ifndef ZNETMINE_H__
#define ZNETMINE_H__

#include "ZMine.h"
#include "ReplicaManager2.h"
using namespace RakNet;

class ZNetMine : public ZMine, public Replica2
{
public:
    DECLARE_SERIALIZABLE
public:
	ZNetMine();
	virtual ~ZNetMine();

	virtual void BroadcastDestruction() { Replica2::BroadcastDestruction(); }

	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	SystemAddress mNetworkOwner;
	ZRushNetGame *mNetGameBelonging;
};

#endif