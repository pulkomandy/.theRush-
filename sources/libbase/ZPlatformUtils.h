///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZPlatformUtils.h
// Creation : 28/05/2009
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

#ifndef ZPLATFORMUTILS_H__
#define ZPLATFORMUTILS_H__

#ifdef _WIN32
#undef __STRICT_ANSI__
#include <windows.h>
#include <windef.h>
#include <process.h>
#include <direct.h>
#endif

#ifdef LINUX
#include <pthread.h>
#endif

#include "ZBaseDefs.h"

const char * GetPictureDirectory();
__inline const char * GetHomeDirectory();


///////////////////////////////////////////////////////////////////////////////////////////////////

// Get the physical core count
__inline unsigned int GetCoreCount()
{
#ifdef LINUX
	unsigned int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
	return numCPU;
#else 
	#ifdef MAC_OS
	return MPProcessorsScheduled();
#else
	DWORD procCoreCount = 0;    // Return 0 on any failure.  That'll show them.
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	procCoreCount = si.dwNumberOfProcessors;
	return procCoreCount;
#endif
#endif
}
// Critical Section

__inline ZCriticalSection_t* NewCriticalSection()
{
#ifdef LINUX
	ZCriticalSection_t *cs = new pthread_mutex_t;
	//(*cs) = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init (cs, NULL);
	return cs;
#else 
#ifdef MAC_OS
	MPCriticalRegionID* criticalRegion = new MPCriticalRegionID;
	OSStatus err = MPCreateCriticalRegion (criticalRegion);
	if (err != 0)
	{
		delete criticalRegion;
		criticalRegion = NULL;
	}
	
	return criticalRegion;
#else	// WIN32
	CRITICAL_SECTION *cs = new CRITICAL_SECTION;
	InitializeCriticalSection(cs);
	return cs;
#endif
#endif
}

__inline void DestroyCriticalSection(ZCriticalSection_t *cs)
{
#ifdef LINUX
	delete cs;
#else 
#ifdef MAC_OS
	MPDeleteCriticalRegion(*cs);
#else	// WIN32
	DeleteCriticalSection(cs);
	delete cs;
#endif
#endif
}

__inline void LockCriticalSection(ZCriticalSection_t *cs)
{
#ifdef LINUX
	pthread_mutex_lock( cs );
#else 
#ifdef MAC_OS
	MPEnterCriticalRegion(*cs, kDurationForever);
#else	// WIN32
	EnterCriticalSection(cs);
#endif
#endif
}

__inline void UnLockCriticalSection(ZCriticalSection_t *cs)
{
#ifdef LINUX
	pthread_mutex_unlock( cs );
#else 
#ifdef MAC_OS
	MPExitCriticalRegion(*cs);
#else	// WIN32
	LeaveCriticalSection(cs);
#endif
#endif
}

__inline void ZSleep(unsigned int ams)
{
#ifdef LINUX
	usleep( ams*1000 );
#else 
#ifdef MAC_OS
	sleep( ams );
#else	// WIN32
	Sleep( ams );
#endif
#endif
}

void LogFramesPerSecond();

#endif
