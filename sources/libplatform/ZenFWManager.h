///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWManager.h
// Creation : 02/05/2009
// Author : Cedric Guillemet
// Description :
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZENFWMANAGER_H__
#define ZENFWMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
//#include "ZenFWLane.h"
#include "../libbase/LibBase.h"
#include "../libgame/ZLocalisation.h"
#include "../libgame/ZProtoConfig.h"
class ZenFWLane;
class ZenFWManager;
class ZenFWNode;
extern ZenFWManager *ZManager;
class ZGameResources;


typedef struct DataToLoad_t
{
	DataToLoad_t(const char *szFileName, ZGameResources *aResource)
	{
		mFileName = szFileName;
		mResource = aResource;
	}
	DataToLoad_t()
	{
	}

	tstring mFileName;
	ZGameResources *mResource;
	unsigned char *mBuffer;
	unsigned long mBufferSize;
} DataToLoad_t;

///////////////////////////////////////////////////////////////////////////////////////////////////

bool LoadingPipeIsEmpty();
void WaitForEmptyLoadingPipe();

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenFWManager
{
public:
	ZenFWManager();
	virtual ~ZenFWManager();

	void Init(int aForceNbCores = 0);
	void Run();
	void Tick();
	void TickOneFrame();
	void Uninit();

	void ManualStartIOLane();



	void Stop();
	void DumpTimings(bool bDump = true ) { mTimingsDump = bDump; }
	bool IsRunning() { return mbIsRunning; }

	// async call this method when you want to exit the manager and potentially the application
	void AskToQuit();
	bool QuitAsked() const { return GAskForQuit; }
	void HandleEndOfWork(ZenFWLane *pAskerLine);
protected:


	friend class ZenFWNode;
	friend class ZenFWLane;

	void AddNodeToDictionnary(ZenFWNode *pNode);

	// ( 1 line per core + 1 line for disk IO -1 because of main thread)
	std::vector<ZenFWLane*> mLanes;
	std::vector<ZenFWNode*> mNodesDictionnary;

	ZCriticalSection_t* mCriticalSection;

	volatile bool mbStopEverything;

	smartptr<ZScene> mScene;
	void DumpList();

	// scheduling
	volatile unsigned int mActiveThreads;

	bool mbIsRunning;
	unsigned int mNnbCores;
	ZenFWLane * mIOLane;
	bool mbInited;
	// timings
	bool mTimingsDump;

	
	float mFrameDurationStart;

	volatile bool GAskForQuit;
	volatile bool mLane0Suspended;
	void ResumeAllSuspendedLanes();

	//Physic
	smartptr<ZPhysicWorldBullet> mPhysWorld;

	// config
	smartptr<ZProtoConfig> mConfig;

	// localisation
	smartptr<ZLocalisation> mLocalisation;
};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif
