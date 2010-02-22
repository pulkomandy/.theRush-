///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWNode.h
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

#ifndef ZENFWNODE_H__
#define ZENFWNODE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZenFWManager.h"

#define DISKIOLANE 9999

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenFWNode
{
public:
	ZenFWNode(const char *szNodeName);
	virtual ~ZenFWNode()
	{
	}

	bool IsActive() const { return mbIsActive; }
	virtual void SetActive(bool bActive) { mbIsActive = bActive; }
	virtual int GetPreferredLine() const { return -1; }

	unsigned int GetLastFrame() const { return mLastFrame; }
	void IncLastFrame() { mLastFrame ++; }
	// return true if should run
	virtual void Run()
	{

	}
	virtual unsigned int GetPriority() const { return 5; }
	const char *GetName(){ return mszNodeName; }

	bool IsBeingUsed() const { return mbIsBeingUsed; }
	void SetUsed(bool bUsed)
	{
		/*
		if (!bUsed)
		{
			extern bool GANewJobIsAvailable;
			GANewJobIsAvailable = true;
		}
		*/
		mbIsBeingUsed = bUsed;
	}
	virtual	bool HasJobTodo() { return true; }

	void SetFrameDone(bool bDone) { mbFrameDone = bDone; }
	bool IsFrameDone() { return mbFrameDone; }

	// timing
	void SetTimingInfos(int aLine, float aStartTime, float aDuration)
	{
		mTimingLine = aLine;
		mTimingStartTime = aStartTime;
		mTimingDuration = aDuration;
	}
	int GetTimeLine() { return mTimingLine; }
	float GetTimingDuration() { return mTimingDuration; }
	float GetTimingStart() { return mTimingStartTime; }

protected:
	volatile bool mbIsBeingUsed;
	bool mbIsActive;
	unsigned int mLastFrame;
	const char *mszNodeName;
	bool mbFrameDone;

	int mTimingLine;
	float mTimingStartTime, mTimingDuration;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
