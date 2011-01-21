// therush.cpp : définit le point d'entrée pour l'application console.
//

#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWdecoderNode.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../librender/ZRenderQueue.h"
#include "../libworld/ZMesh.h"
#include "../libgame/ZDedicatedGames.h"
#include "../libplatform/ZenFWGame.h"
#include "../libplatform/ZenFWSDLWindow.h"
#include "../libplatform/ZenFWRenderer.h"
#include "../libplatform/ZenFWStdNodes.h"
#include "../libplatform/ZenFWViewer.h"

void ClientServer()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);


	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;
	ZenFWMeshModifiers mMeshModifiers;


	ZenFWRenderQueueBuilder mRQueueBuilder;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);


	ZDedicatedGames mGames;

	ZenFWGame aGame;
	aGame.BuildClientServer(mGames.GetPort());
	aGame.SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track", GameSet_t::GAME_TYPE_CLASSIC));
	
	mManager.Run();
}

void DedicatedGame()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);

	ZenFWSleep aSleep(1);

	smartptr<IDisplayDevice> dev = (IDisplayDevice*)_New(ZDisplayDeviceNULL);



	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;

	dev->Create(NULL);

	smartptr<ZAtmosphere> atmos = (ZAtmosphere*)_New(ZAtmosphere);

	ZenFWRenderQueueBuilder mRQueueBuilder;
	


	ZDedicatedGames mGames;


	ZenFWGame aGame;
	aGame.BuildServer(mGames.GetPort());
	aGame.SetGameSet(mGames.GetNextGame());
	
	mManager.Run();
}

void ClientGame(const char *szURL)
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);

	ZenFWSleep aSleep(5);

	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;
	ZenFWMeshModifiers mMeshModifiers;

	ZenFWRenderQueueBuilder mRQueueBuilder;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);


	ZenFWGame aGame;
	aGame.BuildClient();
	aGame.Connect(szURL, 49888);
	
	mManager.Run();
}

void SoloGame()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);


	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;
	ZenFWMeshModifiers mMeshModifiers;

	ZenFWRenderQueueBuilder mRQueueBuilder;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWSleep aSleep(5);

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);

	ZenFWGame aGame;
	aGame.BuildSolo();
	aGame.SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track", GameSet_t::GAME_TYPE_CLASSIC));
	
	mManager.Run();
}

void MainMenu()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);


	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;
	ZenFWMeshModifiers mMeshModifiers;

	ZenFWRenderQueueBuilder mRQueueBuilder;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWSleep aSleep(5);

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);

	ZenFWGame aGame;
	GProtoGui->BuildGameSetForMenu();

	mManager.Run();
}

int main(int argc, char **argv)
{
	GLOG.SetLog((ILogger*)_New(ZLoggerPrintf));
	GLOG.SetWarn((ILogger*)_New(ZLoggerPrintf));
	GLOG.SetError((ILogger*)_New(ZLoggerPrintf));

		
	PROFILER_START(Main);


	if (argc == 0 || !argv[1])
	{
		// normal game
		MainMenu();
	}
	else
	{
		if ( (!strcmp(argv[1], "dedicated")) || (!strcmp(argv[1], "server")) )
		{
			// dedicated
			DedicatedGame();
		}
		else if (!strcmp(argv[1], "client"))
		{
			// client connect
			if (!argv[2])
				ClientGame("127.0.0.1");
			else
				ClientGame(argv[2]);
		}
		else if (!strcmp(argv[1], "clientserver"))
		{
			// server and connected client
			ClientServer();
		}
		else if (!strcmp(argv[1], "solo"))
		{
			// Solo
			SoloGame();
		}
	}

	PROFILER_END();

	LogProfiler();

	// list files

	LOG("*** Remaining ZClasses: ***\r\n");
	DumpClasses();
	DumpInstances();
	LOG("***************************\r\n");

	return 0;
}

