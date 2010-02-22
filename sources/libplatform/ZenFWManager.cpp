///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWManager.cpp
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

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWManager *ZManager;
IInput *GInputDevice = NULL;
ZenFWRenderQueueBuilder *GRQBNode;


ZenFWNode::ZenFWNode(const char *szNodeName)
	{
		ZManager->AddNodeToDictionnary(this);
		mbIsActive = true;
		mLastFrame = 0;
		mszNodeName = szNodeName;
		mbIsBeingUsed = false;
		mbFrameDone = false;
		mTimingLine = 0;
		mTimingStartTime = mTimingDuration = 0;
	}

IInput *GetInputDevice()
{
	return GInputDevice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DumpTimings()
{
	ZManager->DumpTimings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWManager::ZenFWManager()
{
	ZManager = this;
	mbStopEverything = false;
	mTimingsDump = false;
	mbIsRunning = false;
	mNnbCores = 0;
	mbInited = false;
	mFrameDurationStart = 9999999999999.f;
	GAskForQuit = false;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::AskToQuit()
{
	GAskForQuit = true;
	// unlock all threads
	ResumeAllSuspendedLanes();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::ManualStartIOLane() 
{ 
	mIOLane->Start() ; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWManager::~ZenFWManager()
{
	Uninit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::Init(int aForceNbCores)
{
	if (mbInited)
		return;

	GAskForQuit = false;

	mCriticalSection = NewCriticalSection();
	
	// default line/This line
	mLanes.push_back(new ZenFWLane(0));

	// multicore lines
	mNnbCores = aForceNbCores?aForceNbCores:GetCoreCount();
	mNnbCores = (mNnbCores>31)?31:mNnbCores;
	mActiveThreads = (1<<mNnbCores)-1;

	for (uint i=1;i<mNnbCores;i++)
	{
		mLanes.push_back(new ZenFWLane(i));
	}
	// IO/Disk line
	
	mIOLane = new ZenFWLane(DISKIOLANE);
	mLanes.push_back(mIOLane);
	

	// New world to make something with
	mScene = (ZScene*)_New(ZScene);

	// config
	mConfig = (ZProtoConfig*)_New(ZProtoConfig);

	// localization
	mLocalisation = (ZLocalisation*)_New(ZLocalisation);

	// Content manager
	/*
	extern GameContentManager *gContentManager;
	gContentManager = new GameContentManager;
	*/


	mbInited = true;

	mPhysWorld.Attach((ZPhysicWorldBullet*)_New(ZPhysicWorldBullet));
	mPhysWorld->Init();

	
	LOG("Framework Manager inited.\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::ResumeAllSuspendedLanes()
{
	for (uint i=1;i<mNnbCores;i++)
	{
		mLanes[i]->Resume();
	}
	mLane0Suspended = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::Uninit()
{
	if (!mbInited)
		return;

	Stop();
	for (uint i = 0;i<mLanes.size();i++)
	{
		delete mLanes[i];
	}
	mLanes.clear();

	//delete gContentManager;

	mScene.Detach();

	mbInited = false;

	DestroyCriticalSection(mCriticalSection);

	LOG("Framework Manager uninited.\n");

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::Tick()
{
	mLanes[0]->Tick();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::TickOneFrame()
{
	//for (uint i=0;i<mNodesDictionnary.size();i++)
	{
		mLanes[0]->TickOneFrame();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::Run()
{
	mbIsRunning = true;
	
	LOG("Framework Manager running.\n");

	for (uint i=0;i<mLanes.size();i++)
	{
		mLanes[i]->StartLane();
	}
	while(!mbStopEverything)
	{
		mLanes[0]->Tick();
		if (GAskForQuit)
			Stop();
	}
	for (uint i = 0;i<mLanes.size();i++)
	{
		delete mLanes[i];
	}
	mLanes.clear();
	mbIsRunning = false;

	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::AddNodeToDictionnary(ZenFWNode *pNode)
{
	mNodesDictionnary.push_back(pNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int NodeSortPredicate(const void* a, const void* b)
{
	ZenFWNode *lhs = *(ZenFWNode **)a;
	ZenFWNode *rhs = *(ZenFWNode **)b;

	/*
	if (lhs->GetLastFrame() > rhs->GetLastFrame())
		return 1;
	if (lhs->GetLastFrame() < rhs->GetLastFrame())
		return -1;
		*/
	if ( lhs->GetPriority() > rhs->GetPriority())
		return -1;
	if ( lhs->GetPriority() < rhs->GetPriority())
		return 1;

	return 0;
}

int NodeSortPredicateDescendingDuration(const void* a, const void* b)
{
	ZenFWNode *lhs = *(ZenFWNode **)a;
	ZenFWNode *rhs = *(ZenFWNode **)b;

	if ( lhs->GetTimingDuration() < rhs->GetTimingDuration())
		return -1;
	if ( lhs->GetTimingDuration() > rhs->GetTimingDuration())
		return 1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::DumpList()
{
	//printf("\nNodes dump:\n");
	std::vector<ZenFWNode*>::iterator iter = mNodesDictionnary.begin();
	for (;iter != mNodesDictionnary.end(); ++iter)
	{
		char tmps[512];
		sprintf_s(tmps, 512, "  --- %s Frm %d Priority %d\n", (*iter)->GetName(), (*iter)->GetLastFrame(), (*iter)->GetPriority());
		OutputDebugStringA(tmps);
	}
	OutputDebugStringA("\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::Stop()
{
	mbStopEverything = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool LoadingPipeIsEmpty()
{

	bool bRet = true;
	
	if (VRAMService)
		bRet = (mGLoaderService->IsDataListEmpty() &&
			mGDecoderService->GetDataList().empty()&&
			(!VRAMService->ServiceHasWorkToDo()));
	else
		bRet = (mGLoaderService->IsDataListEmpty() &&
			mGDecoderService->GetDataList().empty());

	bRet &= (!mGLoaderService->GetProcessingCount());
	bRet &= (!mGDecoderService->GetProcessingCount());
	if (VRAMService)
		bRet &= (!VRAMService->GetProcessingCount());

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void WaitForEmptyLoadingPipe()
{
	while(!LoadingPipeIsEmpty())
	{
		if (ZManager->IsRunning())
		{
			ZSleep(1);
		}
		else
		{
			ZManager->TickOneFrame();//Tick();
			//ZManager->Tick();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWManager::HandleEndOfWork(ZenFWLane *pAskerLine)
{
	LockCriticalSection(mCriticalSection);
	mActiveThreads &= (~( 1<<pAskerLine->GetLineNumber() ) );
	if ( (mActiveThreads == 0) && (pAskerLine->GetLineNumber() == 0 ))
	{
		// sort and assign jobs
		//ZSleep(1);

		// get time needed for lane 0
		std::vector< std::vector<ZenFWNode*> > mAllJobs;
		std::vector< ZenFWNode* > mNotOnLane0Nodes;
		mAllJobs.resize(mNnbCores);
		float aLane0Time = 0;
		std::vector<ZenFWNode*>::iterator iter = mNodesDictionnary.begin();
		for (; iter != mNodesDictionnary.end() ; ++iter)
		{
			if ( (*iter)->IsActive())
			{
				if ( (*iter)->GetPreferredLine() == 0)
				{
					aLane0Time += (*iter)->GetTimingDuration();
					mAllJobs[0].push_back( (*iter) );
				}
				else
				{
					if ( (*iter) != mGLoaderService )
						mNotOnLane0Nodes.push_back( (*iter) );
				}
			}
		}

		// sort nodes not on lane 0 with descending time duration

		qsort(&mNotOnLane0Nodes[0], mNotOnLane0Nodes.size(), sizeof(ZenFWNode*), NodeSortPredicateDescendingDuration);

		// order nodes whose are not on lane 0

		// add nodes on other than 0 lanes. As much to compensate time already taken on other lanes
		std::vector<float> laneTime;
		laneTime.resize(mNnbCores);
		laneTime[0] = aLane0Time;
		for (unsigned int i=0;i<(mNnbCores-1);i++)
			laneTime[i] = 0;

		for (unsigned int i=0;i<mNotOnLane0Nodes.size();i++)
		{
			// get lowest predicted duration lane
			float lowestTimeDurationPrediction = 99999999.f;
			unsigned int bestLane = 0;
			for (unsigned int j=0;j<mNnbCores;j++)
			{
				if (laneTime[j]<= lowestTimeDurationPrediction)
				{
					bestLane = j;
					lowestTimeDurationPrediction = laneTime[j];
				}
			}
			mAllJobs[bestLane].push_back(mNotOnLane0Nodes[i]);
			laneTime[bestLane] += mNotOnLane0Nodes[i]->GetTimingDuration();
		}


//#define SHOWTASKSASSIGNS
		// sort and assign jobs
		for (unsigned int i=0;i<mNnbCores;i++)
		{
#ifdef SHOWTASKSASSIGNS
			tstring nfo;
			nfo.Printf("Lane %d : ", i);
#endif
			qsort(&mAllJobs[i][0], mAllJobs[i].size(), sizeof(ZenFWNode*), NodeSortPredicate);
			std::vector<ZenFWNode*>::iterator itern = mAllJobs[i].begin();
			for (; itern != mAllJobs[i].end() ; ++itern)
			{
				mLanes[i]->AddNewJob( (*itern) );
#ifdef SHOWTASKSASSIGNS				
				nfo += (*itern)->GetName();
				nfo += " , ";
#endif
			}
#ifdef SHOWTASKSASSIGNS
			nfo += "\n";
			LOG(nfo.c_str());
#endif

		}



		mActiveThreads = (1<<mNnbCores)-1;
		// per frame computation
		extern int frameNb;
		frameNb++;
		// swap queues
		GDD->SwapQueues();
		UnLockCriticalSection(mCriticalSection);
		// start a new frame
		gTimer.Tick();
		//ResumeAllSuspendedLanes();
	}
	else
	{
		UnLockCriticalSection(mCriticalSection);
/*
		if (pAskerLine->GetLineNumber())
		{
			pAskerLine->Suspend();
		}
		*/
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

