///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWLane.cpp
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

ZenFWLane::~ZenFWLane()
{
	if (mLineNumber)
		ZThread::Stop();
	LOG("** Lane %d is terminating\n",this->mLineNumber);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWLane::Run()
{
	while(ZThread::IsRunning())
	{
		Tick();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWLane::TickOneFrame()
{
	if (!mJobsToDo.empty())
	{
		std::list<ZenFWNode*>::iterator iter = mJobsToDo.begin();
		while (iter != mJobsToDo.end())
		{
			ZenFWNode *pNode = (*iter);

			// profile start of work
			float curt = gTimer.ForceGetTime();

			pNode->Run();

			pNode->SetTimingInfos(mLineNumber, curt, gTimer.ForceGetTime()-curt);

			pNode->IncLastFrame();

			iter = mJobsToDo.erase(iter);
		}
	}
	ZManager->HandleEndOfWork(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWLane::Tick()
{
	while (!ZManager->QuitAsked())
	{
		if (mLineNumber == DISKIOLANE)
			mGLoaderService->Run();
		else
		{
			TickOneFrame();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
