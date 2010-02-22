#ifndef ZNETSHIP_H__
#define ZNETSHIP_H__

#include "ZNetMessages.h"
class ZRushNetGame;
class ZNetShip : public ZShip, public Replica2
{
public:
    DECLARE_SERIALIZABLE
public:
	ZNetShip();
	virtual ~ZNetShip();

	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);

	SystemAddress mNetworkOwner;
	//keysStruct mCurrentUserInput;
	ZRushNetGame *mNetGameBelonging;

	virtual void GetHumanControls(keysStruct& ks);
#if 0
	virtual void Update(float timeEllapsed);
#endif
	bool mbNetworkNeedsSync;

	virtual void DecLife(float aVal);
	virtual void IncLife(float aVal);
	virtual void GiveBonus(char aBonusNb);
};



#endif