#ifndef ZRUSHGAME_H__
#define ZRUSHGAME_H__

#include "libbase/ZSerializator.h"

class FSMClientBase;


DECLAREZCLASS(ZRushGame);
DECLAREZCLASS(ZRushNetGame);
DECLAREZCLASS(ZRushNetGameClient);
DECLAREZCLASS(ZRushNetGameServer);

class ZShip;
class ZPhysicWorldBullet;
struct ZNMSG_AskSlot;
struct ZNMSG_ClientMapLoadingDone;
struct ZNMSG_RaceStartsInNSeconds;
struct ZNMSG_CountDownStarts;
struct ZNMSG_RaceEndsInNSeconds;
struct ZNMSG_SlotsAvailable;
struct ZNMSG_ShipLapTime;
struct ZNMSG_UseBonus;

#include "ZLapsTimes.h"
#include "ZGameEntitiesManager.h"
#include "ZBonusParameters.h"
#include "libplatform/ZenFWNode.h"
#include "libplatform/ZenFWGame.h"
#include "libgame/ZGameTriggers.h"

class ZMine;
class ZMissile;
class ZExplosion;
class ZHUDMeshModifier;

class ZRushGame : public ZGame, public ZGameEntitiesManager
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRushGame();
	virtual ~ZRushGame();
	virtual ZShip* AddShip(const char *szShipName, bool bServerSide);
	virtual void Update(float aTimeEllapsed);
	
	virtual void Connect(const tstring& /*url*/, unsigned short /*aPort*/) {}
	virtual void ActAsServer(unsigned short /*aPort*/) {}
	virtual void SetGameSet(const GameSet_t& gameSet);
	void BuildEditorGameTest();

	

	void DisableSceneMaster();
	virtual bool IsServer() const { return false; }

	void HandleNewGameSet();
	void HandleLoadingDone();

	void SetClientShip(ZShip *pShip);

	// messages
	virtual void SendNetMessage(const ZNMSG_AskSlot & msg, bool bBroadcast = true);
	virtual void SendNetMessage(const ZNMSG_ClientMapLoadingDone & /*msg*/, bool bBroadcast = true) {}
	virtual void SendNetMessage(const ZNMSG_RaceStartsInNSeconds& /*msg*/, bool bBroadcast = true) {}
	virtual void SendNetMessage(const ZNMSG_CountDownStarts& /*msg*/, bool bBroadcast = true) {}
	virtual void SendNetMessage(const ZNMSG_RaceEndsInNSeconds& /*msg*/, bool bBroadcast = true) {}
	virtual void SendNetMessage(const ZNMSG_SlotsAvailable& /*msg*/, bool bBroadcast = true) {}
	virtual void SendLapTime(ZShip *pShip, float aTime);
	virtual void SendShipUseBonus(ZShip *pShip);

	ZHUDMeshModifier *GetHUD() { return mHUD.ptr(); }
	ZShip *GetClientShip() const { return mClientShip;}
	ZShip *GetFirstShip() const { return (*mShips.begin()); }
	const std::list<smartptr<ZShip> >& GetShips() const { return mShips; }

	// game set
	const GameSet_t& GetGameSet() const { return mCurrentGameSet; }

	void GUIAsksForSlot();
	void GUIAsksForSpectator();
	void GUIShipChosen();

	int GetClientCount() const 
	{ 
		return mShips.size(); 
	}
	void ShowHUD(bool bShow);
	virtual bool IsSoloGame() { return true; }
	virtual const char *GetNickName(ZShip *pShip);
	const ZLapsTimes& GetLapsTimes() const { return mLapsTime; }
	virtual void RestartRace();
	virtual void HandleShipCollision( ZShip *pShip, ZBaseClass *pOther, float fDistance);
	void AllowInGameMenu(bool bAllow);
	// method asked when race starts
	virtual void EnableAllShipsControls();
	virtual void EnableLapTimeComputationForEveryShip();

	// Bonus
	virtual void GiveBonusToShip(char aBonusNb, ZShip *pShip);

protected:
	// client ship
	ZShip *mClientShip;
	//
	std::list<smartptr<ZShip> > mShips;
	

	// Gameset
	GameSet_t mCurrentGameSet;
	bool mCurrentGameSetDirty;

	bool mbIsSceneMaster;
	bool mbInGameMenuAllowed;

	friend class ZNetShip;
	friend class ZShip;
	friend class FSMClientBase;
	// 

	virtual void HandleMapFinishedLoading();

	// Physic
	
	float mPhysicUpdateTime;

	// FSM
	FSMClientBase *mFSMClientBase;

	// trigger callbacks
	TriggerResponders mTriggerCallbackResponse;
	
	// HUD
	smartptr<ZHUDMeshModifier> mHUD;
	bool mbHUDVisible;

	// Laps time
	ZLapsTimes mLapsTime;

	// Rankings
	static int RankingSort(const void *p1, const void *p2);
	typedef struct ShipRanking_t
	{
		ShipRanking_t()
		{
		}
		ShipRanking_t(ZShip *aShip, int bricksRunned)
		{
			pShip = aShip;
			nbBricksRunned = bricksRunned;
		}

		ZShip *pShip;
		int nbBricksRunned;
		bool operator < (const ShipRanking_t& other)
		{
			return (nbBricksRunned< other.nbBricksRunned);
		}
	} ShipRanking_t;
	void ComputeRankings();

	// Damages
	bool mbSupportDamages;

	// game parameters
	smartptr<ZBonusParameters> mBonusParameters;

};

#endif
