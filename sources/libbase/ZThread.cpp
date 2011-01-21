///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZThread.cpp
// Creation : 02/02/2004
// Author : Cedric Guillemet
// Description :
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
// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZThread.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD WINAPI ThreadProc(  LPVOID lpParameter   )
{
    ((ZThread*)lpParameter)->Run();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Start()
{
    DWORD thId;

    m_bRunning=true;
    m_bExitWanted=false;

    thHand=::CreateThread(NULL,NULL,ThreadProc,this,NULL,&thId);
    //SetThreadPriority(thHand,THREAD_PRIORITY_HIGHEST);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Stop()
{
    m_bExitWanted=true;
    m_bRunning=false;
	if (thHand)
	{
		WaitForSingleObject(thHand,INFINITE);
		CloseHandle(thHand);
		thHand = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Suspend()
{
    SuspendThread(thHand);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Resume()
{
    ResumeThread(thHand);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
#if (defined(LINUX) || defined(MAC_OS))
#include <pthread.h>

void*         Zthrdproc  (void *data)
{
    ((ZThread*)data)->Run();
    return NULL;
}

void ZThread::Start()
{

    m_bRunning=true;
    m_bExitWanted=false;

	pthread_t   *threadId;

						  //It receives a single void parameter

	pthread_attr_t  attr;
	int             rc = 0;


	if (rc = pthread_attr_init(&attr))
		 return ;  // EINVAL, ENOMEM

	if (rc = pthread_attr_setstacksize(&attr, 1024*100))
		 return ;   // EINVAL, ENOSYS

	if (rc = pthread_create(&thHand, &attr, Zthrdproc, this))
		 return ;      // EINVAL, EAGAIN

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Stop()
{
    m_bExitWanted=true;
    m_bRunning=false;
	if (thHand)
	{
        unsigned long rc=0;
        void *status;

        rc = pthread_join(thHand, &status);
		thHand = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Suspend()
{
	pthread_cond_wait(&m_ResumeCond,&m_SuspendMutex);
	/*
    //SuspendThread(thHand);
     pthread_mutex_lock(&mutex);
     flag--;
     pthread_mutex_unlock(&mutex);
	 */

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZThread::Resume()
{
	/*
	pthread_mutex_lock(&mutex);
	flag++;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	*/
	pthread_cond_signal(&m_ResumeCond);

}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

ZThread::ZThread()
{
    m_bRunning=false;
	thHand = 0;
#ifdef LINUX
	pthread_mutex_init(&m_SuspendMutex,NULL); // Return Check
	pthread_cond_init(&m_ResumeCond,NULL); // Return Check
#endif
	 
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZThread::~ZThread()
{
    if (m_bRunning)
    {
        Stop();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZThread::IsRunning()
{
    return m_bRunning;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZThread::CanRun()
{
    return (!m_bExitWanted);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
