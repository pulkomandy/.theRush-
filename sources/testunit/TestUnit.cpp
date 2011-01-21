#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
#include "../libplatform/ZenFWSDLWindow.h"
#include "../libplatform/ZenFWRenderer.h"
#include "../libplatform/ZenFWViewer.h"
#include "../libplatform/ZenFWDecoderNode.h"
#include "../libplatform/ZenFWSTDNodes.h"
#include "TestExecutionNodes.h"
#include <stdio.h>
#include <stdlib.h>

#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
//#include "TransformationHistory.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"
using namespace RakNet;
#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
#include "../libgame/FSMBonus.h"
#include "../libgame/ZDedicatedGames.h"

#include "../libbase/ZThread.h"
#include "../libbase/ZTimer.h"
#include "../libbase/ZFile.h"

class CheckThread : public ZThread
{
public:
	CheckThread()
	{
	}
	virtual ~CheckThread()
	{
	}

	virtual void Run()
	{
		PROFILER_START(ThreadedComputation);

		tmatrix mt;
		mt.Identity();
		for (int i=0;i<1000000;i++)
		{
			tmatrix mt2;
			//PROFILER_START(ThreadMul);
			mt2.RotationQuaternion(tquaternion(0.1f, 0.2f, 0.3f, 0.4f));
			mt *= mt2;
			//PROFILER_END();
		}
		LOG("-> res is %5.2f , %5.2f , %5.2f\n", mt.V4.dir.y, mt.V4.dir.y, mt.V4.dir.z);
		PROFILER_END();
	}

};

// Raknet test for headers & linking
void TestRaknet()
{
	RakPeerInterface *rakInterface;
	NetworkIDManager networkIdManager;

	rakInterface = RakNetworkFactory::GetRakPeerInterface();

	networkIdManager.SetIsNetworkIDAuthority(true);
	//rakInterface->AttachPlugin(&replicaManager2);
	rakInterface->SetNetworkIDManager(&networkIdManager);
	/*
	// Register our custom connection factory
	mShipFactory.mBelongingGame = this;
	replicaManager2.SetConnectionFactory(&mShipFactory);
	replicaManager2.SetAutoAddNewConnections(false);
	*/


	unsigned short port = 49888;

	SocketDescriptor socketDescriptor(port,0);
	rakInterface->Startup(32,30,&socketDescriptor, 1);
	rakInterface->SetMaximumIncomingConnections(32);

}

void TestUnitMile1()
{
	tmatrix mt;
	mt.RotationQuaternion(tquaternion(0.1f, 0.2f, 0.3f, 0.4f));


	LOG("System has %d processor(s). Image path is %s.\n", GetCoreCount(), GetPictureDirectory());
	tmatrix res;
	res.Inverse(mt);
	LOG("-> %5.2f vs %5.2f\n", mt.V4.dir.y, res.V4.dir.y);


	char tmps[512];
	GetCurrentDirectory(512, tmps);
	LOG("Current Directory is %s\n", tmps);

	ZTimer mTime;
	LOG("Time is %5.3f\n ", mTime.GetTime());
	mTime.Tick();
	ZSleep(1234);
	mTime.Tick();
	LOG("Ellapsed after 1234ms sleep is %5.3f\n", mTime.GetEllapsed());


	float timeStart = mTime.ForceGetTime();
	{
		CheckThread mThread;
		mThread.Start();
		mThread.Stop();
	}
	LOG("One thread took %5.3fs\n",mTime.ForceGetTime()-timeStart);

	timeStart = mTime.ForceGetTime();
	{
		CheckThread mThread,mThread2;
		mThread.Start();
		mThread2.Start();
		mThread.Stop();
		mThread2.Stop();
	}
	LOG("two threads took %5.3fs\n",mTime.ForceGetTime()-timeStart);
}


void TestUnitMile2()
{
	bool bDumpTiming = true;
	// 3 Threads
	for (int i=0;i<3;i++)
	{
		ZenFWManager mManager;
		mManager.Init(3-i);
		mManager.DumpTimings(bDumpTiming);

		FakeRender fakeRender;
		FakePresentation fakePresentation;

		FakeScenegraph asg;
		FakeGameplay agp;

		FakeWindowInput awi;

		FakeSound as;

		FakeAnimation aan;

		FakeParticlesAndFX apfx;

		FakeDecoder ad;

		mManager.Run();
	}
	
}


void TestUnitMile3()
{
	ZenFWManager mManager;
	mManager.Init();
	mManager.DumpTimings(false);
	FakePresentationNoQuit afpnq;
FakeScenegraph asg;
 FakeGameplay agp;

 FakeWindowInput awi;

 FakeSound as;

 FakeAnimation aan;

 FakeParticlesAndFX apfx;

 FakeDecoder ad;
	ZenFWSDLWindow mWin;
	mWin.Init();

	mManager.Run();
}

void TestUnitMile4()
{
	ZenFWManager mManager;
	mManager.Init();
	mManager.DumpTimings(false);
	FakePresentationNoQuit afpnq;
FakeScenegraph asg;
 FakeGameplay agp;

 FakeWindowInput awi;

 FakeSound as;

 FakeAnimation aan;

 FakeParticlesAndFX apfx;

 FakeDecoder ad;
	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();


	mManager.Run();
}

void TestUnitMile5()
{
	ZenFWManager mManager;
	mManager.Init();
	mManager.DumpTimings(false);
	FakePresentationNoQuit afpnq;

FakeScenegraph asg;
 FakeGameplay agp;
 FakeWindowInput awi;
 FakeSound as;
 FakeAnimation aan;
 FakeParticlesAndFX apfx;



	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	//aViewer.SetFile("./Prototype/Teams/Wipeout/Feisar/Feisar.DAE");
	aViewer.SetFile("./Prototype/Worlds/Stratos/Bricks.DAE");
	


	mManager.Run();
}
// chargement, calcul et affichage de track
void TestUnitMile6()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);


	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;


	ZenFWRenderQueueBuilder mRQueueBuilder;
	//ZenFWPhysic mPhysic;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetFile("./Prototype/Worlds/Stratos/Tracks/Stratos1.track");//"./Prototype/Worlds/Stratos/Bricks.DAE");
	


	mManager.Run();
}

void TestUnitMile7()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);


	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;


	ZenFWRenderQueueBuilder mRQueueBuilder;
	//ZenFWPhysic mPhysic;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);
	/*
	aViewer.SetFile("./Prototype/Worlds/Stratos/Tracks/Stratos1.track");//"./Prototype/Worlds/Stratos/Bricks.DAE");
*/	


	ZenFWGame aGame;
	//aGame.SetFile("./Prototype/Worlds/Stratos/Tracks/Stratos1.track");
	aGame.BuildSolo();
	aGame.SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track"));
	
mManager.Run();


}

void TestUnitMile8()
{
	ZenFWManager mManager;
	mManager.Init(1);
	mManager.DumpTimings(false);


	ZenFWLoaderNode mLoader;
	ZenFWDecoderNode mDecoder;
	ZenFWVRAMService mVRAMService;


	ZenFWRenderQueueBuilder mRQueueBuilder;
	//ZenFWPhysic mPhysic;
	ZenFWPresent mPresenter;

	ZenFWSDLWindow mWin;
	mWin.Init();

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);
	/*
	aViewer.SetFile("./Prototype/Worlds/Stratos/Tracks/Stratos1.track");//"./Prototype/Worlds/Stratos/Bricks.DAE");
*/	


	ZenFWGame aGame;
	//aGame.SetFile("./Prototype/Worlds/Stratos/Tracks/Stratos1.track");
	aGame.BuildClientServer(49888);
	aGame.SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track"));
	
mManager.Run();


}

void PrintList(const char *listtype, const std::vector<tstring>& alist)
{
	LOG("%s\n", listtype);
	for (unsigned int i=0;i<alist.size(); i++)
	{
		LOG("%s\n", alist[i].c_str());
	}
}

void TestFindFiles()
{
	{
	std::vector<tstring> mylist;
	ZFile::GetFilesList(mylist, "Prototype/Teams/", "", false, true, false);
	PrintList("Team List", mylist);
	}

	{
	std::vector<tstring> mylist;
	ZFile::GetFilesList(mylist, "Prototype/", ".DAE", true, false, false);
	PrintList("DAE List", mylist);
	}

}



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


void DDEGame()
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


	ZenFWGame aGame;
	aGame.BuildDDE();
	
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

	//ZenFWSleep aSleep(5);

	ZenFWRenderer aRenderer;
	aRenderer.Init();

	ZenFWViewer aViewer;
	aViewer.Init();
	aViewer.SetActive(false);

	ZenFWGame aGame;
	/*
	aGame.BuildSolo();
	aGame.SetGameSet(GameSet_t("Stratos", "./Prototype/Worlds/Stratos/Tracks/Stratos1.track", GameSet_t::GAME_TYPE_MENU));
*/
	GProtoGui->BuildGameSetForMenu();

	mManager.Run();
}

int main(int argc, char **argv)
{
	GLOG.SetLog((ILogger*)_New(ZLoggerPrintf));
	GLOG.SetWarn((ILogger*)_New(ZLoggerPrintf));
	GLOG.SetError((ILogger*)_New(ZLoggerPrintf));
	//GLOG.SetLog((ILogger*)_New(ZLoggerFile));


/*
	{
		smartptr<ZHUDMeshModifier> mTrail;
		mTrail.Attach((ZHUDMeshModifier*)_New(ZHUDMeshModifier));

		ZMeshModifierContainer::Tick(0);
	}
	ZMeshModifierContainer::Tick(0);
	tstring filename = GetHomeDirectoy();
	filename += "toto.txt";
	FILE *fp = fopen(filename.c_str(), "wb");
	if (fp)
	{
		char tmp[]="ahahhahah";
		fwrite(tmp, strlen(tmp)+1, 1, fp);
		fclose(fp);
	}
	return 0;
*/		
		
	PROFILER_START(Main);
	/*
	ShipBonus aBonus;
	aBonus.Tick(0);
	NewBonus nb;
	nb.mBonusIndex = 3;
	aBonus.Handle(nb);
	aBonus.Tick(0);
	aBonus.Handle(TriggerBonus());
	aBonus.Tick(0);
*/
	if (!argv[1])
	{
		// normal game
		SoloGame();
	}
	else
	{
		if (!stricmp(argv[1], "dde")) 
		{
			// dedicated
			DDEGame();
		}
		else if ( (!stricmp(argv[1], "dedicated")) || (!stricmp(argv[1], "server")) )
		{
			// dedicated
			DedicatedGame();
		}
		else if (!stricmp(argv[1], "client"))
		{
			// client connect
			if (!argv[2])
				ClientGame("127.0.0.1");
			else
				ClientGame(argv[2]);
		}
		else if (!stricmp(argv[1], "clientserver"))
		{
			// server and connected client
			ClientServer();
		}
		else if (!stricmp(argv[1], "menu"))
		{
			// server and connected client
			MainMenu();
		}
	}
	


	//TestRaknet();

	//TestFindFiles();

	//TestUnitMile1();

	//TestUnitMile2();
	//TestUnitMile3();
	//TestUnitMile4();
	//TestUnitMile5();
	//TestUnitMile6();
	//TestUnitMile8();
/*
	std::vector<smartptr<ZTexture> > mtexs;
	mtexs.push_back((ZTexture*)_New(ZTexture));
	mtexs.push_back((ZTexture*)_New(ZTexture));
	mtexs.push_back((ZTexture*)_New(ZTexture));

	mtexs[1] = (ZTexture*)_New(ZTexture);

	std::vector<smartptr<ZTexture> > mtexcopy;
	mtexcopy = mtexs;
	LOG("tex 0 RC = %d\n", mtexs[0]->GetRefCount());
	*/
	// --

	PROFILER_END();

	LogProfiler();

	

	// list files


	LOG("*** Remaining ZClasses: ***\r\n");
	DumpClasses();
	DumpInstances();
	LOG("***************************\r\n");


	return 0;
}
