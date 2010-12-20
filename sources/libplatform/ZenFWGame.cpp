#include "ZenFWGame.h"
#include "IInput.h"
#include "../libgame/ZRushGameContent.h"
#include "../libgame/ZRushTrack.h"
#include "../libgame/ZRushGame.h"
#include "../libgame/CameraMotion.h"
#include "../libbase/ZTimer.h"

ZenFWGame *GGame = NULL;

DECLAREZCLASS(ZDDEGame);

BEGIN_SERIALIZATION(ZGame)
END_SERIALIZATION()

ZenFWGame::ZenFWGame() : ZenFWNode("ZenFWGame")
{
	GGame = this;

	if (GetInputDevice())
		GetInputDevice()->SetExclusive(true);

	rushContent = (ZRushGameContent*)_New(ZRushGameContent);
}


ZenFWGame::~ZenFWGame()
{
	if (GTrack)
		delete GTrack;
	if (GGame == this)
		GGame = NULL;
	if (rushContent)
		delete rushContent;
}


void ZenFWGame::SetActive(bool bActive)
{
	ZenFWNode::SetActive(bActive);
	if (!bActive)
	{
		if (mClientGame.ptr())
			mClientGame->SetSpeed(0);
	}
	else
	{
		if (mClientGame.ptr())
			mClientGame->SetSpeed(1);
	}
}

void ZenFWGame::SetGameSpeed(float aSpeed)
{
	if (mbIsActive)
	{
		if (mClientGame.ptr())
			mClientGame->SetSpeed(aSpeed);
	}
}

void ZenFWGame::BuildSolo()
{
	mClientGame.Attach((ZRushGame*)_New(ZRushGame));
	mServerGame = NULL;
}

void ZenFWGame::BuildDDE()
{
	mClientGame.Attach((ZGame*)_New(ZDDEGame));
	mServerGame = NULL;
}

void ZenFWGame::BuildClientServer(unsigned short aPort)
{
	ZRushGame* rushNetClient = (ZRushGame*)_New(ZRushNetGameClient);
	mClientGame.Attach(rushNetClient);
	rushNetClient->DisableSceneMaster();

	ZRushGame* rushNetServer = (ZRushGame*)_New(ZRushNetGameServer);
	mServerGame.Attach(rushNetServer);
	rushNetServer->ActAsServer(aPort);
	rushNetClient->Connect("127.0.0.1", aPort);
}

void ZenFWGame::BuildServer(unsigned short aPort)
{
	mClientGame = NULL;
	ZRushGame* rushNetServer = (ZRushGame*)_New(ZRushNetGameServer);
	mServerGame.Attach(rushNetServer);
	rushNetServer->ActAsServer(aPort);
}

void ZenFWGame::BuildClient()
{
	mClientGame.Attach((ZRushGame*)_New(ZRushNetGameClient));
	mServerGame = NULL;
}

void ZenFWGame::SetGameSet(const GameSet_t& gameSet)
{
	if (mServerGame.ptr())
	{
		((ZRushGame*)mServerGame.ptr())->SetGameSet(gameSet);
	}
	else if (mClientGame.ptr())
	{
		((ZRushGame*)mClientGame.ptr())->SetGameSet(gameSet);
	}
}

void ZenFWGame::Connect(const tstring& url, unsigned short aPort)
{
	if (mClientGame.ptr() ) //&& (mClientGame->GetClassID() == ClassIDZRushGame))
	{
		((ZRushGame*)mClientGame.ptr())->Connect(url, aPort);
	}
}
	
void ZenFWGame::CleanRushGames()
{
	mServerGame = NULL;
	mClientGame = NULL;
}

void ZenFWGame::Run()
{
	PROFILER_START(ZenFWGame::Run);

	if (GetInputDevice())
		GetInputDevice()->Tick();

	if (mServerGame.ptr())
	{
		mServerGame->Update(gTimer.GetEllapsed());
	}

	if (mClientGame.ptr())
	{
		mClientGame->Update(gTimer.GetEllapsed());
	}

	gCameraMotion.Tick();


	PROFILER_END();
}
