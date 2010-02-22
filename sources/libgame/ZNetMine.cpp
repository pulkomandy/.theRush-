#include "stdafx.h"
#include "ZNetMine.h"
#include "StringTable.h"

BEGIN_SERIALIZATION(ZNetMine)
END_SERIALIZATION()

ZNetMine::ZNetMine()
{
}

ZNetMine::~ZNetMine()
{
}

bool ZNetMine::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	StringTable::Instance()->EncodeString("NetMine", 128, bitStream);
	bitStream->Write((char*)&mNetworkOwner, sizeof(SystemAddress));

	return true;
}

bool ZNetMine::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	return false;
}

void ZNetMine::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
}
