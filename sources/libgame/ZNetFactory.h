#ifndef ZNETFACTORY_H__
#define ZNETFACTORY_H__

class ZRushNetGame;
class GameConnection : public Connection_RM2
{
public:
	GameConnection(ZRushNetGame *pGame) 
	{ 
		mBelongingGame = pGame; 
	}
	virtual ~GameConnection() {}

	// Callback used to create objects
	// See Connection_RM2::Construct in ReplicaManager2.h for a full explanation of each parameter
	Replica2* Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, 
		RakNetTime timestamp, NetworkID networkId, bool networkIDCollision);
	ZRushNetGame *mBelongingGame;
};

///////////////////////////////////////////////////////////////////////////////////////////////////


// This is a required class factory, that creates and destroys instances of ReplicaManager2DemoConnection
class GameFactory : public Connection_RM2Factory 
{
	virtual Connection_RM2* AllocConnection(void) const 
	{
		return new GameConnection(mBelongingGame);
	}
	virtual void DeallocConnection(Connection_RM2* s) const 
	{
		delete s;
	}

	// Server side only functions
	
public:
	void SetCurrentOwnerAddress(const SystemAddress& addr) { mCurrentOwnerAddress = addr; }

	ZRushNetGame *mBelongingGame;
	ZShip* NewNetworkReplicatedShip();
	ZMissile* NewNetworkReplicatedMissile();
	ZMine* NewNetworkReplicatedMine();
	ZExplosion* NewNetworkReplicatedExplosion();

	SystemAddress mCurrentOwnerAddress;
};


#endif