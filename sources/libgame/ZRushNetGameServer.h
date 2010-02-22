#ifndef ZRUSHNETGAMESERVER_H__
#define ZRUSHNETGAMESERVER_H__

#include "ZRushNetGame.h"
#include "FSMServerBase.h"

class ZRushNetGameServer : public ZRushNetGame 
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRushNetGameServer();
	virtual ~ZRushNetGameServer();
	virtual void Update(float aTimeEllapsed);
	virtual void SetSpeed(float fSpeed) {  }
	virtual void Connect(const tstring& url, unsigned short aPort) { ZASSERT(0, "Can't connect from a server game"); }
	virtual void ActAsServer(unsigned short aPort);

	virtual void SetGameSet(const GameSet_t& gameSet);
	//
	virtual void SetClientShip(ZShip *pShip) {}

	
	virtual bool IsServer() const { return true; }
	virtual void EnableAllShipsControls() {}

	virtual void HandleShipUseBonus(ZShip *pShip);

	// Game Objects
	virtual ZMissile* SpawnMissile(const tmatrix& dropMatrix);
	virtual ZMine* SpawnMine(const tmatrix& dropMatrix);
	virtual ZExplosion* SpawnExplosion(const tmatrix& dropMatrix);

	virtual void DestroyEntity(ZGameEntity *pEnt) { pEnt->BroadcastDestruction(); delete pEnt; }
protected:
	float mPingTime;
	
	float mfSendSync;
	void SendShipSyncs();
	
	virtual void HandleTimeTest(Packet *packet);
	virtual void HandleMapFinishedLoading();
	virtual void HandleIncomingNewConnection(const SystemAddress& addr);
	virtual void HandleDisconnectionNotification(const SystemAddress& addr);
	virtual void HandleConnectionLost(const SystemAddress &addr);

	FSMServerBase mFSMServerBase;
};

#endif
