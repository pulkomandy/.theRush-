///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZTimer.h
// Creation : 24/06/2007
// Author : Cedric Guillemet
// Description :
// Reliability : 
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

#ifndef ZTIMER_H__
#define ZTIMER_H__

#ifdef MAC_OS
#import <CoreFoundation/CoreFoundation.h>
#endif

#ifdef LINUX
#include <sys/time.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZTimer 
{
public:
    ZTimer()
	{
		mCurTime = 0;
		mSpeed = 1.f;
#ifdef WIN32
        //LARGE_INTEGER  hrCurrentTime;
        LARGE_INTEGER  hrFreq;
        //period = (1 / freq) * 1000;
        //QueryPerformanceCounter( &hrCurrentTime );
        mPrevTime = GetTime(); //currentTime = (float)hrCurrentTime.QuadPart;
        QueryPerformanceFrequency( &hrFreq );
        mFreq = (float)hrFreq.QuadPart;
		mPrevTime = GetTime();
		Tick();
#endif
#ifdef MAC_OS
		mPrevTime = mFirstTime = CFAbsoluteTimeGetCurrent();
		Tick();
#endif
#ifdef LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux
		mBaseTime = ts.tv_sec;

		Tick();
#endif

	}

    virtual ~ZTimer()
    {
    }


    float GetTime()
    {
#ifdef WIN32
		return mCurTime;
#endif
#ifdef MAC_OS
		return float(mCurTime - mFirstTime);
#endif
#ifdef LINUX
		return mCurTime;
#endif
    }


	void Tick() 
	{ 
#ifdef WIN32
		LARGE_INTEGER  hrCurrentTime;
        QueryPerformanceCounter( &hrCurrentTime );
        mCurTime = hrCurrentTime.QuadPart /** 1000000*/ / mFreq;


		float curt = GetTime();
		mEllapsed = curt - mPrevTime;
		mPrevTime = curt;
#endif
#ifdef MAC_OS
		mCurTime = CFAbsoluteTimeGetCurrent();
		mEllapsed = mCurTime - mPrevTime;
		mPrevTime = mCurTime;
#endif
#ifdef LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux

		mPrevTime = mCurTime;

		float ms = float(int(ts.tv_sec-mBaseTime));
		ms += float(int(ts.tv_nsec/1000000LU))*0.001f;


		mCurTime = ms;
		mEllapsed = mCurTime - mPrevTime;
		
#endif
	}

	float ForceGetTime()
	{
#ifdef WIN32
		LARGE_INTEGER  hrCurrentTime;
        QueryPerformanceCounter( &hrCurrentTime );
        float aCurTime = hrCurrentTime.QuadPart /** 1000000*/ / mFreq;

		return aCurTime;
#endif
#ifdef MAC_OS
		return CFAbsoluteTimeGetCurrent() - mFirstTime;
#endif
#ifdef LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux
		float ms = float(int(ts.tv_sec-mBaseTime));
		ms += float(int(ts.tv_nsec/1000000LU))*0.001f;
		return ms;
#endif
	}


	float GetEllapsed() const
	{ 
		return mEllapsed;
	}

protected:
    float mSpeed;
    float mPrevTime;
#ifdef MAC_OS
	float mFirstTime;
#endif
    float mFreq;
	float mEllapsed;
	float mCurTime;
#ifdef LINUX
	long int mBaseTime;
#endif
};


///////////////////////////////////////////////////////////////////////////////////////////////////
extern ZTimer gTimer;

#endif
