///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWLane.h
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

#ifndef ZENFWLane_H__
#define ZENFWLane_H__

#include "../libbase/LibBase.h"
class ZenFWNode;
class ZenFWLane : public ZThread
{
public:
	ZenFWLane(unsigned int aLineNb)
	{
		mLineNumber = aLineNb;
	}
	virtual ~ZenFWLane();
	void Run();
	void Tick();
	void TickOneFrame();

	int GetLineNumber() const { return mLineNumber; }
	void StartLane()
	{
		if (mLineNumber)
			ZThread::Start();
		else
			ZThread::m_bRunning = true;
	}
	void AddNewJob(ZenFWNode * pNode) { mJobsToDo.push_back(pNode); }
protected:
	int mLineNumber;
	std::list<ZenFWNode*> mJobsToDo;
};

#endif
