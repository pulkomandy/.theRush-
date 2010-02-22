#include "stdafx.h"
#include "ZNetExplosion.h"
#include "StringTable.h"

BEGIN_SERIALIZATION(ZNetExplosion)
END_SERIALIZATION()

ZNetExplosion::ZNetExplosion()
{
}

ZNetExplosion::~ZNetExplosion()
{
}

bool ZNetExplosion::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	StringTable::Instance()->EncodeString("NetExplosion", 128, bitStream);
	bitStream->Write((char*)&mNetworkOwner, sizeof(SystemAddress));
	return true;
}

bool ZNetExplosion::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	return false;
}

void ZNetExplosion::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
}
