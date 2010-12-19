#ifndef ZRUSHNETGAME_H__
#define ZRUSHNETGAME_H__

#include <string>

#include "ZNetMessages.h"
#include "ZNetFactory.h"

#include <NetworkIDManager.h>

class ZShip;
class ZNetShip;

class ZRushNetGame : public ZRushGame 
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRushNetGame();
	virtual ~ZRushNetGame();
	virtual ZShip* AddShip(const char *szShipName, bool bServerSide);
	virtual void Update(float aTimeEllapsed);
	virtual void SetSpeed(float fSpeed) {  }
	virtual void Connect(const tstring& url, unsigned short aPort) { }
	virtual void ActAsServer(unsigned short aPort) { }


	// messages
	ZNETMESSAGE(ZNMSG_AskSlot, ZNMSG_ASKSLOT);
	ZNETMESSAGE(ZNMSG_ServerMapLoadingDone, ZNMSG_SERVERMAPLOADINGDONE);
	ZNETMESSAGE(ZNMSG_UserInput, ZNMSG_USERINPUT);
	ZNETMESSAGE(ZNMSG_GameSet, ZNMSG_GAMESET);
	ZNETMESSAGE(ZNMSG_TimedTest, ZNMSG_TIMEDTEST);
	ZNETMESSAGE(ZNMSG_ShipSync, ZNMSG_SHIPSYNC);
	ZNETMESSAGE(ZNMSG_ClientMapLoadingDone, ZNMSG_CLIENTMAPLOADINGDONE);

	ZNETMESSAGE(ZNMSG_RaceStartsInNSeconds, ZNMSG_RACESTARTSINNSECONDS);
	ZNETMESSAGE(ZNMSG_CountDownStarts, ZNMSG_COUNTDOWNSTARTS);
	ZNETMESSAGE(ZNMSG_SlotsAvailable, ZNMSG_SLOTSAVAILABLE);
	ZNETMESSAGE(ZNMSG_RaceEndsInNSeconds, ZNMSG_RACEENDSINNSECONDS);
	ZNETMESSAGE(ZNMSG_NextMapInNSeconds, ZNMSG_NEXTMAPINNSECONDS);
	ZNETMESSAGE(ZNMSG_UseBonus, ZNMSG_USEBONUS);

	ZNETMESSAGE(ZNMSG_ShipLapTime, ZNMSG_SHIPLAPTIME);
	virtual void SendLapTime(ZShip *pShip, float aTime);



	virtual bool IsServer() const { ZASSERT(0, "Build client or server for Net."); return false; }
	virtual bool IsSoloGame() { return false; }
	ZNetShip* GetShipBySystemAddress(const SystemAddress& addr);
	// Bonus
	virtual void SendShipUseBonus(ZShip *pShip) {}
	virtual void HandleShipUseBonus(ZShip *pShip) {}
protected:

	

	// server infos

	std::string mServerAdress;
	unsigned short mServerPort;

	RakPeerInterface *rakInterface;
	ReplicaManager2 replicaManager2;
	NetworkIDManager networkIdManager;
	GameFactory mGameObjectsFactory;

	void TickPackets(float timeEllapsed);


	friend class GameFactory;
	friend class GameConnection;
	// pings
	std::map<SystemAddress, int > mPings;
	int GetHalfPing(const SystemAddress& addr);



	virtual void HandleTimeTest(Packet *packet) { ZASSERT(0, "Can't have time test here. Should be a server or a client."); }

	// special messages handling
	virtual void HandleDisconnectionNotification(const SystemAddress &addr) { LOG("NET : HandleDisconnectionNotification not handled\n"); }
	virtual void HandleConnectionLost(const SystemAddress &addr) { LOG("NET : HandleConnectionLost not handled\n"); }
	virtual void HandleConnectionAttemptFailed() { LOG("NET : HandleConnectionAttemptFailed not handled\n"); }
	virtual void HandleIncomingNewConnection(const SystemAddress& addr) { LOG("NET : HandleIncomingNewConnection not handled\n"); }
	virtual void HandleShipSync(Packet *packet, float timeEllapsed) {}



};

#endif
