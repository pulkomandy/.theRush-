#include "stdafx.h"
#include "ZNetMissile.h"
#include "StringTable.h"

BEGIN_SERIALIZATION(ZNetMissile)
END_SERIALIZATION()

ZNetMissile::ZNetMissile()
{
}

ZNetMissile::~ZNetMissile()
{
}

bool ZNetMissile::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	StringTable::Instance()->EncodeString("NetMissile", 128, bitStream);
	bitStream->Write((char*)&mNetworkOwner, sizeof(SystemAddress));

	return true;
}

bool ZNetMissile::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	const tmatrix& mt = this->mMissileTransform->GetWorldMatrix();
	tquaternion qt(mt);

	return false;
}

void ZNetMissile::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
}
