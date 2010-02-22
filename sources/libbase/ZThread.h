///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZThread.h
// Creation : 02/02/2004
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

#ifndef ZTHREAD_H__
#define ZTHREAD_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

// Constants //////////////////////////////////////////////////////////////////////////////////////

// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZThread
{

    public:
        volatile bool        m_bRunning;
        bool        m_bExitWanted;
        ThreadHandle_t    thHand;

    public:
        ZThread();
        virtual ~ZThread();

        void Start();
        void Stop();

        bool IsRunning();

        virtual void Run()=0;

		/*
		/!\ BEWARE /!\
		on Win32, any thread can suspend another one immediately.
		on posix, only a thread can suspend itself
		*/
        void Suspend();
        void Resume();

    protected:
        bool CanRun();

#if defined( LINUX ) || defined(MAC_OS)
	 pthread_mutex_t m_SuspendMutex;
	 pthread_cond_t m_ResumeCond;
#endif


};


// Inlines ////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////


#endif
