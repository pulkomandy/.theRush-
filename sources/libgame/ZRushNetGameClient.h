#ifndef ZRUSHNETGAMECLIENT_H__
#define ZRUSHNETGAMECLIENT_H__



#include "ZRushNetGame.h"

class ZRushNetGameClient : public ZRushNetGame
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRushNetGameClient();
	virtual ~ZRushNetGameClient();
	
	virtual void Update(float aTimeEllapsed);
	virtual void SetSpeed(float fSpeed) {  }
	virtual void Connect(const tstring& url, unsigned short aPort);
	virtual void ActAsServer(unsigned short aPort) { }
	
	virtual bool IsServer() const { return false; }
	virtual void RestartRace() { ERR("Can't restart race on a NET Client."); }
	virtual void EnableAllShipsControls();
	virtual void HandleMapFinishedLoading() {}
	virtual void GiveBonusToShip(char aBonusNb, ZShip *pShip) {}
	virtual void SendShipUseBonus(ZShip *pShip);
	virtual void HandleShipUseBonus(ZShip *pShip) { pShip->UseBonus(); }

	// Game Objects
	virtual ZMissile* SpawnMissile(const tmatrix& dropMatrix) { return NULL; }
	virtual ZMine* SpawnMine(const tmatrix& dropMatrix) { return NULL; }
	virtual ZExplosion* SpawnExplosion(const tmatrix& dropMatrix) { return NULL; }
	// don't explicitely destroy entity. Replication management will handle it.
	virtual void DestroyEntity(ZGameEntity *pEnt) {  }
protected:
	// send keys
	float mSendMessageTime;
	void SendShipKeys();

	// time for ping
	virtual void HandleTimeTest(Packet *packet)
	{
		// Forward it
		rakInterface->Send((const char*)packet->data, sizeof(ZNMSG_TimedTest)+1, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);

	}
	
	virtual void HandleConnectionAttemptFailed();
	virtual void HandleConnectionLost(const SystemAddress &addr);
	virtual void HandleIncomingNewConnection(const SystemAddress& addr) {}
	virtual void HandleShipSync(Packet *packet, float timeEllapsed);
	virtual void HandleDisconnectionNotification(const SystemAddress &addr) { WARN("GOT A DisconnectionNotification on client\n"); }

};

#endif
