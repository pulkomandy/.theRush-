#ifndef ZENFWGame_H__
#define ZENFWGame_H__

#include "..\libgame\ZGameSet.h"

class ZMesh;
class ZenFWGame;
extern ZenFWGame *GGame;
class ZShipPhysics;
class ZRushGameContent;

#include "ZenFWNode.h"

class ZGame : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZGame() : mSpeed(1.f)
	{
	}
	virtual ~ZGame()
	{
	}
	virtual void Update(float /*aTimeEllapsed*/)
	{
	}
	virtual void SetSpeed(float fSpeed) { mSpeed = fSpeed; }
	ZPhysicWorldBullet *GetPhysicWorld() const { return mPhysicWorld; }
protected:
	float mSpeed;
	smartptr<ZPhysicWorldBullet> mPhysicWorld;
};

class ZenFWGame : public ZenFWNode
{
public:
	ZenFWGame();
	virtual ~ZenFWGame();

	
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 4; }
	virtual void Run();

	virtual void SetActive(bool bActive);
	void SetGameSpeed(float aSpeed);

	// Game Creation
	void BuildSolo();
	void BuildClientServer(unsigned short aPort);
	void BuildServer(unsigned short aPort);
	void BuildClient();
	void CleanRushGames();
	void Connect(const tstring& url, unsigned short aPort);
	void BuildDDE();

	void SetGameSet(const GameSet_t& gameSet);
	ZGame * GetClientGame() const { return mClientGame.ptr(); }
	ZGame * GetServerGame() const { return mServerGame.ptr(); }
	bool HasClientServer() const
	{
		if (!mClientGame.ptr())
			return false;
		if (!mServerGame.ptr())
			return false;
		return true;
	}
protected:
	
	smartptr<ZGame> mClientGame;
	smartptr<ZGame> mServerGame;

	ZRushGameContent* rushContent;
};

#endif
