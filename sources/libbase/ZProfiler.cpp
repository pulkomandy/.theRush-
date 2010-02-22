///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZProfiler.cpp
// Creation : 30/01/2008
// Author : Cedric Guillemet
// Description :
// Reliability :
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
#include "stdafx.h"
#include <string>

#ifdef WIN32
#include <windows.h>

#include <Mmsystem.h>
#endif
#include <algorithm>
#include <map>
#include "ZLogger.h"
#include "ZProfiler.h"

// DEBUG
//char	gText[ 1024 ];

#define	_NAME_SEPARATOR_			"|"
#define	_THREADID_NAME_SEPARATOR_	"@"

#if defined(WIN32)
	#pragma comment(lib, "Winmm.lib")
#endif

using namespace std;


//#if defined(WIN32)
	double	startHighResolutionTimer( void );
	/*
#elif defined(_MAC_)
	void	startHighResolutionTimer(unsigned long time[2]);
#else
	void	startHighResolutionTimer(unsigned long time[2]);
#endif
*/
//unsigned long	endHighResolutionTimer(unsigned long time[2]);


#if defined(WIN32)
	// Create A Structure For The Timer Information
	struct
	{
		__int64			frequency;									// Timer Frequency
		float			resolution;									// Timer Resolution
		unsigned long	mm_timer_start;								// Multimedia Timer Start Value
		unsigned long	mm_timer_elapsed;							// Multimedia Timer Elapsed Time
		bool			performance_timer;							// Using The Performance Timer?
		__int64			performance_timer_start;					// Performance Timer Start Value
		__int64			performance_timer_elapsed;					// Performance Timer Elapsed Time
	} timer;														// Structure Is Named timer
#endif

//	Hold the state of the profiler
bool gProfilerEnable	= 0;

typedef struct stGenProfilerData
{
	double			totalTime;
	double			averageTime;
	double			minTime;
	double			maxTime;
	double			lastTime;				// Time of the previous passage
	double			elapsedTime;			// Elapsed Time
	unsigned long	nbCalls;				// Numbers of calls
	char			szBunchCodeName[1024];	// temporary.
} tdstGenProfilerData;

//  Hold the call stack
typedef vector<tdstGenProfilerData> tdCallStackType;

//	Hold the sequence of profiler_starts
map<string, tdstGenProfilerData> mapProfilerGraph;

// Hold profiler data vector in function of the thread
map<unsigned long, tdCallStackType> mapCallsByThread;

// Critical section
ZCriticalSection_t	*gProfilerCriticalSection;

// Check if the profiler option has been activated in the Commworld.ini file
bool ProfilerActivate();

// Critical section functions
/*
void NewCriticalSection( void );
void DestroyCriticalSection( void );
void LockCriticalSection( void );
void UnLockCriticalSection( void );
*/

//
// Activate the profiler
//
bool Zprofiler_enable()
{
	gProfilerEnable	= ProfilerActivate();
	if( !gProfilerEnable )
	{
		return false;
	}

	// Initialize the timer
	TimerInit();

	// Create the mutex
	gProfilerCriticalSection = NewCriticalSection();

	// Clear maps
	/*
	mapCallsByThread.clear();
	mapProfilerGraph.clear();
	mapCallsByThread.clear();
	*/



	return true;
}

//
// Deactivate the profiler
//
void Zprofiler_disable()
{
	if( !gProfilerEnable )
	{
		return ;
	}

	// Dump to file
	//Zprofiler_dumpToFile( DUMP_FILENAME );

	// Clear maps
	mapCallsByThread.clear();
	mapProfilerGraph.clear();
	mapCallsByThread.clear();

	// Delete the mutex
	DestroyCriticalSection(gProfilerCriticalSection);
}

//
// Start the profiling of a bunch of code
//
 void Zprofiler_start( const char *profile_name )
{
	if( !gProfilerEnable )
	{
		return ;
	}

    LockCriticalSection(gProfilerCriticalSection);

	unsigned long ulThreadId = GetCurrentThreadId();

	// Add the profile name in the callstack vector
	tdstGenProfilerData	GenProfilerData;
	ZeroMemory(&GenProfilerData, sizeof(GenProfilerData));
	GenProfilerData.lastTime	= startHighResolutionTimer();
	GenProfilerData.minTime		= 0xFFFFFFFF;

	// Find or add callstack
	tdCallStackType TmpCallStack;
	map<unsigned long, tdCallStackType>::iterator IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);
	if( IterCallsByThreadMap == mapCallsByThread.end() )
	{
		// Not found. So insert the new pair
		mapCallsByThread.insert(std::make_pair(ulThreadId, TmpCallStack));
		IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);
	}

	// It's the first element of the vector
	if ((*IterCallsByThreadMap).second.empty())
	{
		GenProfilerData.nbCalls	= 1;
		sprintf(GenProfilerData.szBunchCodeName, "%s%d%s%s", _NAME_SEPARATOR_, (int)ulThreadId, _THREADID_NAME_SEPARATOR_, profile_name);
		(*IterCallsByThreadMap).second.push_back( GenProfilerData );
	}
	// It's not the first element of the vector
	else
	{
		// Update the number of calls
		GenProfilerData.nbCalls++;

		// We need to construct the string with the previous value of the
		// profile_start
		char *previousString = (*IterCallsByThreadMap).second[(*IterCallsByThreadMap).second.size()-1].szBunchCodeName;

		// Add the current profile start string
		strcpy(GenProfilerData.szBunchCodeName, previousString);
		strcat(GenProfilerData.szBunchCodeName, _NAME_SEPARATOR_);
		strcat(GenProfilerData.szBunchCodeName, profile_name);

		// Push it
		(*IterCallsByThreadMap).second.push_back( GenProfilerData );
	}

UnLockCriticalSection(gProfilerCriticalSection);
}

//
// Stop the profiling of a bunch of code
//
void Zprofiler_end( )
{
	if( !gProfilerEnable )
	{
		return ;
	}

	unsigned long ulThreadId = GetCurrentThreadId();

	// Retrieve the right entry in function of the threadId
	map<unsigned long, tdCallStackType>::iterator IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);

	// Check if vector is empty
	if( (*IterCallsByThreadMap).second.empty() )
	{
		LOG( "Il y a une erreur dans le vecteur CallStack !!!\n\n");
		return;
	}

LockCriticalSection(gProfilerCriticalSection);

	// Retrieve the last element from the callstack vector
	tdstGenProfilerData	GenProfilerData;
	GenProfilerData	= (*IterCallsByThreadMap).second[(*IterCallsByThreadMap).second.size()-1];

	// Compute elapsed time
	GenProfilerData.elapsedTime += startHighResolutionTimer()-GenProfilerData.lastTime;
	GenProfilerData.totalTime	+= GenProfilerData.elapsedTime;

	// Find if this entry exists in the map
	map<string, tdstGenProfilerData>::iterator	IterMap;
	IterMap	= mapProfilerGraph.find( GenProfilerData.szBunchCodeName );
	if( IterMap!=mapProfilerGraph.end() )
	{
		(*IterMap).second.nbCalls++;

		// Retrieve time information to compute min and max time
		double minTime		= (*IterMap).second.minTime;
		double maxTime		= (*IterMap).second.maxTime;
		//double totalTime	= (*IterMap).second.totalTime;

		if( GenProfilerData.elapsedTime<minTime )
		{
			(*IterMap).second.minTime	= GenProfilerData.elapsedTime;
		}

		if( GenProfilerData.elapsedTime>maxTime )
		{
			(*IterMap).second.maxTime	= GenProfilerData.elapsedTime;
		}

		// Compute Total Time
		(*IterMap).second.totalTime	   += GenProfilerData.elapsedTime;
		// Compute Average Time
		(*IterMap).second.averageTime	= (*IterMap).second.totalTime/(*IterMap).second.nbCalls;

		// DEBUG
		//sprintf(gText, "* %s * ElapsedT %f  TotalT %f dt %f MinT %f MaxT %f Appels : %d\n\n",
		//	    profile_name,
		//		GenProfilerData.elapsedTime,
		//		(*IterMap).second.totalTime,
		//		(*IterMap).second.averageTime,
		//		(*IterMap).second.minTime,
		//		(*IterMap).second.maxTime,
		//		(*IterMap).second.nbCalls);
		//OutputDebugString(gText);
	}
	else
	{
		GenProfilerData.minTime	= GenProfilerData.elapsedTime;
		GenProfilerData.maxTime	= GenProfilerData.elapsedTime;

		// Compute Average Time
		GenProfilerData.averageTime	= GenProfilerData.totalTime/GenProfilerData.nbCalls;

		// DEBUG
		//sprintf(gText, "* %s * ElapsedT %f  TotalT %f dt %f MinT %f MaxT %f Appels : %d\n\n",
		//		profile_name,
		//		GenProfilerData.elapsedTime,
		//		GenProfilerData.totalTime,
		//		GenProfilerData.averageTime,
		//		GenProfilerData.minTime,
		//		GenProfilerData.maxTime,
		//		GenProfilerData.nbCalls);
		//OutputDebugString(gText);

		// Insert this part of the call stack into the Progiler Graph
		mapProfilerGraph.insert( std::make_pair(GenProfilerData.szBunchCodeName, GenProfilerData) );
	}

	// Now, pop back the GenProfilerData from the vector callstack
	(*IterCallsByThreadMap).second.pop_back();

UnLockCriticalSection(gProfilerCriticalSection);
}

//
// Dump all data in a file
//

bool MyDataSortPredicate(const tdstGenProfilerData un, const tdstGenProfilerData deux)
{
	return un.szBunchCodeName > deux.szBunchCodeName;
}

void LogProfiler()
{

		// Thread Id String
		char szThreadId[16];
		char textLine[1024];
		char *tmpString;
		long i;
		//long nbTreadIds	= 0;
		long size		= 0;

		// Map for calls
		map<string, tdstGenProfilerData> mapCalls;
		map<string, tdstGenProfilerData>::iterator IterMapCalls;
		mapCalls.clear();

		// Map for Thread Ids
		map<string, int> ThreadIdsCount;
		map<string, int>::iterator IterThreadIdsCount;
		ThreadIdsCount.clear();

		// Vector for callstack
		vector<tdstGenProfilerData> tmpCallStack;
		vector<tdstGenProfilerData>::iterator IterTmpCallStack;
		tmpCallStack.clear();

		// Copy map data into a vector in the aim to sort it
		map<string, tdstGenProfilerData>::iterator	IterMap;
		for(IterMap=mapProfilerGraph.begin(); IterMap!=mapProfilerGraph.end(); ++IterMap)
		{
			strcpy((*IterMap).second.szBunchCodeName, (*IterMap).first.c_str());
			tmpCallStack.push_back( (*IterMap).second );
		}

		// Sort the vector
		std::sort(tmpCallStack.begin(), tmpCallStack.end(), MyDataSortPredicate);

		// Create a map with thread Ids
		for(IterTmpCallStack=tmpCallStack.begin(); IterTmpCallStack!=tmpCallStack.end(); IterTmpCallStack++)
		{
			//// DEBUG
			//fprintf(DumpFile, "%s\n", (*IterTmpCallStack).szBunchCodeName );
			//// DEBUG

			tmpString	= strstr((*IterTmpCallStack).szBunchCodeName, _THREADID_NAME_SEPARATOR_);
			size		= (long)(tmpString-(*IterTmpCallStack).szBunchCodeName);
			strncpy(szThreadId, (*IterTmpCallStack).szBunchCodeName+1, size-1); szThreadId[size-1] = 0;
			ThreadIdsCount[szThreadId]++;
		}

		//// DEBUG
		//fprintf(DumpFile, "====\n");
		//fprintf(DumpFile, "====\n");
		//fprintf(DumpFile, "Number of threadids : %d\n", ThreadIdsCount.size());
		//for(IterThreadIdsCount=ThreadIdsCount.begin(); IterThreadIdsCount!=ThreadIdsCount.end(); ++IterThreadIdsCount)
		//{
		//	char text[256];
		//	sprintf(text, "%d %s\n", IterThreadIdsCount->second, IterThreadIdsCount->first.c_str());
		//	fprintf(DumpFile, text);
		//}
		//fprintf(DumpFile, "====\n");
		//fprintf(DumpFile, "====\n");
		//// DEBUG

		// Retrieve the number of thread ids
		//unsigned long nbThreadIds	= mapProfilerGraph.size();

		IterThreadIdsCount = ThreadIdsCount.begin();
		for(unsigned long nbThread=0;nbThread<ThreadIdsCount.size();nbThread++)
		{
			sprintf(szThreadId, "%s", IterThreadIdsCount->first.c_str() );

			sprintf(textLine, "CALLSTACK of Thread %s\n", szThreadId);
			LOG(textLine);
			sprintf(textLine, "_______________________________________________________________________________________\n");
			LOG(textLine);
			sprintf(textLine, "| Temps total  | Temps Moyen  |   Temps Min  |  Temps Max   | Appels | Nom de l'appel\n");
			LOG(textLine);
			sprintf(textLine, "_______________________________________________________________________________________\n");
			LOG(textLine);

			long nbSeparator = 0;
			for(IterTmpCallStack=tmpCallStack.begin(); IterTmpCallStack!=tmpCallStack.end(); IterTmpCallStack++)
			{
				tmpString	= (*IterTmpCallStack).szBunchCodeName+1;

				if( strstr(tmpString, szThreadId) )
				{
					// Count the number of separator in the string
					nbSeparator	= 0;
					while( *tmpString )
					{
						if( *tmpString++== '|' )
						{
							nbSeparator++;
						}
					}

					// Get times and fill in the dislpay string
					sprintf(textLine, "| %12.4f | %12.4f | %12.4f | %12.4f |%6d  | ",
							(*IterTmpCallStack).totalTime,
							(*IterTmpCallStack).averageTime,
							(*IterTmpCallStack).minTime,
							(*IterTmpCallStack).maxTime,
							(int)(*IterTmpCallStack).nbCalls);

					// Get the last start_profile_name in the string
					tmpString = strrchr((*IterTmpCallStack).szBunchCodeName, '|')+1;

					IterMapCalls	= mapCalls.find( tmpString );
					if( IterMapCalls!=mapCalls.end() )
					{
						double minTime			= (*IterMapCalls).second.minTime;
						double maxTime			= (*IterMapCalls).second.maxTime;
						//double totalTime		= (*IterMapCalls).second.totalTime;
						//double averageTime		= (*IterMapCalls).second.averageTime;
						//unsigned long nbCalls	= (*IterMapCalls).second.nbCalls;

						if( (*IterTmpCallStack).minTime<minTime )
						{
							(*IterMapCalls).second.minTime	= (*IterTmpCallStack).minTime;
						}
						if( (*IterTmpCallStack).maxTime>maxTime )
						{
							(*IterMapCalls).second.maxTime	= (*IterTmpCallStack).maxTime;
						}
						(*IterMapCalls).second.totalTime		+= (*IterTmpCallStack).totalTime;
						(*IterMapCalls).second.nbCalls			+= (*IterTmpCallStack).nbCalls;
						(*IterMapCalls).second.averageTime		= (*IterMapCalls).second.totalTime/(*IterMapCalls).second.nbCalls;
					}
					
					else
					{
						tdstGenProfilerData tgt;// = (*IterMap).second;
						if( strstr(tmpString, szThreadId) )
						{
							strcpy( tgt.szBunchCodeName, tmpString );
						}
						else
						{
							sprintf(tgt.szBunchCodeName, "%s%s%s",
									szThreadId,
									_THREADID_NAME_SEPARATOR_,
									tmpString);
						}

						tgt.minTime			= (*IterTmpCallStack).minTime;
						tgt.maxTime			= (*IterTmpCallStack).maxTime;
						tgt.totalTime			= (*IterTmpCallStack).totalTime;
						tgt.averageTime		= (*IterTmpCallStack).averageTime;
						tgt.elapsedTime		= (*IterTmpCallStack).elapsedTime;
						tgt.lastTime			= (*IterTmpCallStack).lastTime;
						tgt.nbCalls			= (*IterTmpCallStack).nbCalls;

						mapCalls.insert( std::make_pair(tmpString, tgt) );
					}
					

					// Copy white space in the string to format the display
					// in function of the hierarchy
					for(i=0;i<nbSeparator;i++) strcat(textLine, "  ");

					// Remove the thred if from the string
					if( strstr(tmpString, _THREADID_NAME_SEPARATOR_) )
					{
						tmpString += strlen(szThreadId)+1;
					}

					// Display the name of the bunch code profiled
					strcat(textLine, tmpString);
					LOG("%s\n", textLine);
				}
		}
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);
		LOG( "\n");
		IterThreadIdsCount++;
	}
	LOG( "\n\n");

	//
	//	DUMP CALLS
	//
	IterThreadIdsCount = ThreadIdsCount.begin();
	for(unsigned long nbThread=0;nbThread<ThreadIdsCount.size();nbThread++)
	{
		sprintf(szThreadId, "%s", IterThreadIdsCount->first.c_str() );

		sprintf(textLine, "DUMP of Thread %s\n", szThreadId);
		LOG(textLine);
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);
		sprintf(textLine, "| Temps total  | Temps Moyen  |  Temps Min   |  Temps Max   | Appels | Nom de l'appel\n");
		LOG(textLine);
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);

		for(IterMapCalls=mapCalls.begin(); IterMapCalls!=mapCalls.end(); ++IterMapCalls)
		{
			tmpString = (*IterMapCalls).second.szBunchCodeName;
			if( strstr(tmpString, szThreadId) )
			{
				sprintf(textLine, "| %12.4f | %12.4f | %12.4f | %12.4f | %6d | %s\n",
						(*IterMapCalls).second.totalTime,
						(*IterMapCalls).second.averageTime,
						(*IterMapCalls).second.minTime,
						(*IterMapCalls).second.maxTime,
						(int)(*IterMapCalls).second.nbCalls,
						(*IterMapCalls).second.szBunchCodeName+strlen(szThreadId)+1);
				LOG(textLine);
			}

		}
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);
		LOG("\n");
		IterThreadIdsCount++;
	}

}

#if 0
void Zprofiler_dumpToFile(const char *filename)
{
	FILE *DumpFile;
	DumpFile	= fopen( filename, "wt");

	// Error on file. Quit
	if( NULL==DumpFile ) return;

		// Thread Id String
		char szThreadId[16];
		char textLine[1024];
		char *tmpString;
		long i;
		long nbTreadIds	= 0;
		long size		= 0;

		// Map for calls
		map<string, tdstGenProfilerData> mapCalls;
		map<string, tdstGenProfilerData>::iterator IterMapCalls;
		mapCalls.clear();

		// Map for Thread Ids
		map<string, int> ThreadIdsCount;
		map<string, int>::iterator IterThreadIdsCount;
		ThreadIdsCount.clear();

		// Vector for callstack
		vector<tdstGenProfilerData> tmpCallStack;
		vector<tdstGenProfilerData>::iterator IterTmpCallStack;
		tmpCallStack.clear();

		// Copy map data into a vector in the aim to sort it
		map<string, tdstGenProfilerData>::iterator	IterMap;
		for(IterMap=mapProfilerGraph.begin(); IterMap!=mapProfilerGraph.end(); ++IterMap)
		{
			strcpy((*IterMap).second.szBunchCodeName, (*IterMap).first.c_str());
			tmpCallStack.push_back( (*IterMap).second );
		}

		// Sort the vector
		std::sort(tmpCallStack.begin(), tmpCallStack.end(), MyDataSortPredicate);

		// Create a map with thread Ids
		for(IterTmpCallStack=tmpCallStack.begin(); IterTmpCallStack!=tmpCallStack.end(); IterTmpCallStack++)
		{
			//// DEBUG
			//fprintf(DumpFile, "%s\n", (*IterTmpCallStack).szBunchCodeName );
			//// DEBUG

			tmpString	= strstr((*IterTmpCallStack).szBunchCodeName, _THREADID_NAME_SEPARATOR_);
			size		= (long)(tmpString-(*IterTmpCallStack).szBunchCodeName);
			strncpy(szThreadId, (*IterTmpCallStack).szBunchCodeName+1, size-1); szThreadId[size-1] = 0;
			ThreadIdsCount[szThreadId]++;
		}

		//// DEBUG
		//fprintf(DumpFile, "====\n");
		//fprintf(DumpFile, "====\n");
		//fprintf(DumpFile, "Number of threadids : %d\n", ThreadIdsCount.size());
		//for(IterThreadIdsCount=ThreadIdsCount.begin(); IterThreadIdsCount!=ThreadIdsCount.end(); ++IterThreadIdsCount)
		//{
		//	char text[256];
		//	sprintf(text, "%d %s\n", IterThreadIdsCount->second, IterThreadIdsCount->first.c_str());
		//	fprintf(DumpFile, text);
		//}
		//fprintf(DumpFile, "====\n");
		//fprintf(DumpFile, "====\n");
		//// DEBUG

		// Retrieve the number of thread ids
		//unsigned long nbThreadIds	= mapProfilerGraph.size();

		IterThreadIdsCount = ThreadIdsCount.begin();
		for(unsigned long nbThread=0;nbThread<ThreadIdsCount.size();nbThread++)
		{
			sprintf(szThreadId, "%s", IterThreadIdsCount->first.c_str() );

			sprintf(textLine, "CALLSTACK of Thread %s\n", szThreadId);
			LOG(textLine);
			sprintf(textLine, "_______________________________________________________________________________________\n");
			LOG(textLine);
			sprintf(textLine, "| Temps total  | Temps Moyen  |   Temps Min  |  Temps Max   | Appels | Nom de l'appel\n");
			LOG(textLine);
			sprintf(textLine, "_______________________________________________________________________________________\n");
			LOG(textLine);

			long nbSeparator = 0;
			for(IterTmpCallStack=tmpCallStack.begin(); IterTmpCallStack!=tmpCallStack.end(); IterTmpCallStack++)
			{
				tmpString	= (*IterTmpCallStack).szBunchCodeName+1;

				if( strstr(tmpString, szThreadId) )
				{
					// Count the number of separator in the string
					nbSeparator	= 0;
					while( *tmpString )
					{
						if( *tmpString++== '|' )
						{
							nbSeparator++;
						}
					}

					// Get times and fill in the dislpay string
					sprintf(textLine, "| %12.4Lf | %12.4Lf | %12.4Lf | %12.4Lf |%6d  | ",
							(*IterTmpCallStack).totalTime,
							(*IterTmpCallStack).averageTime,
							(*IterTmpCallStack).minTime,
							(*IterTmpCallStack).maxTime,
							(*IterTmpCallStack).nbCalls);

					// Get the last start_profile_name in the string
					tmpString = strrchr((*IterTmpCallStack).szBunchCodeName, '|')+1;

					if (!mapCalls.empty())
					{
						IterMapCalls	= mapCalls.find( tmpString );
						if( IterMapCalls!=mapCalls.end() )
						{
							double minTime			= (*IterMapCalls).second.minTime;
							double maxTime			= (*IterMapCalls).second.maxTime;
							double totalTime		= (*IterMapCalls).second.totalTime;
							double averageTime		= (*IterMapCalls).second.averageTime;
							unsigned long nbCalls	= (*IterMapCalls).second.nbCalls;

							if( (*IterTmpCallStack).minTime<minTime )
							{
								(*IterMapCalls).second.minTime	= (*IterTmpCallStack).minTime;
							}
							if( (*IterTmpCallStack).maxTime>maxTime )
							{
								(*IterMapCalls).second.maxTime	= (*IterTmpCallStack).maxTime;
							}
							(*IterMapCalls).second.totalTime		+= (*IterTmpCallStack).totalTime;
							(*IterMapCalls).second.nbCalls			+= (*IterTmpCallStack).nbCalls;
							(*IterMapCalls).second.averageTime		= (*IterMapCalls).second.totalTime/(*IterMapCalls).second.nbCalls;
						}
						else
						{
							tdstGenProfilerData& pdt = (*IterMap).second;
							if( strstr(tmpString, szThreadId) )
							{
								strcpy( pdt.szBunchCodeName, tmpString );
							}
							else
							{
								sprintf(pdt.szBunchCodeName, "%s%s%s",
										szThreadId,
										_THREADID_NAME_SEPARATOR_,
										tmpString);
							}

							pdt.minTime			= (*IterTmpCallStack).minTime;
							pdt.maxTime			= (*IterTmpCallStack).maxTime;
							pdt.totalTime			= (*IterTmpCallStack).totalTime;
							pdt.averageTime		= (*IterTmpCallStack).averageTime;
							pdt.elapsedTime		= (*IterTmpCallStack).elapsedTime;
							pdt.lastTime			= (*IterTmpCallStack).lastTime;
							pdt.nbCalls			= (*IterTmpCallStack).nbCalls;

							mapCalls.insert( std::make_pair(tmpString, pdt) );
						}

						// Copy white space in the string to format the display
						// in function of the hierarchy
						for(i=0;i<nbSeparator;i++) strcat(textLine, "  ");

						// Remove the thred if from the string
						if( strstr(tmpString, _THREADID_NAME_SEPARATOR_) )
						{
							tmpString += strlen(szThreadId)+1;
						}

						// Display the name of the bunch code profiled
						strcat(textLine, tmpString);
						fprintf(DumpFile, "%s\n", textLine);
					}
				}
		}
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);
		fprintf(DumpFile, "\n");
		IterThreadIdsCount++;
	}
	fprintf(DumpFile, "\n\n");

	//
	//	DUMP CALLS
	//
	IterThreadIdsCount = ThreadIdsCount.begin();
	for(unsigned long nbThread=0;nbThread<ThreadIdsCount.size();nbThread++)
	{
		sprintf(szThreadId, "%s", IterThreadIdsCount->first.c_str() );

		sprintf(textLine, "DUMP of Thread %s\n", szThreadId);
		LOG(textLine);
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);
		sprintf(textLine, "| Temps total  | Temps Moyen  |  Temps Min   |  Temps Max   | Appels | Nom de l'appel\n");
		LOG(textLine);
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);

		for(IterMapCalls=mapCalls.begin(); IterMapCalls!=mapCalls.end(); ++IterMapCalls)
		{
			tmpString = (*IterMapCalls).second.szBunchCodeName;
			if( strstr(tmpString, szThreadId) )
			{
				sprintf(textLine, "| %12.4Lf | %12.4Lf | %12.4Lf | %12.4Lf | %6d | %s\n",
						(*IterMapCalls).second.totalTime,
						(*IterMapCalls).second.averageTime,
						(*IterMapCalls).second.minTime,
						(*IterMapCalls).second.maxTime,
						(*IterMapCalls).second.nbCalls,
						(*IterMapCalls).second.szBunchCodeName+strlen(szThreadId)+1);
				LOG(textLine);
			}

		}
		sprintf(textLine, "_______________________________________________________________________________________\n");
		LOG(textLine);
		fprintf(DumpFile, "\n");
		IterThreadIdsCount++;
	}

	// Close the Dump file
	fclose( DumpFile);
}
#endif

//-------------------------------------------------------------------------
// @function ProfilerActivate
// Check if the profiler has been activated in the Commworld.ini file
// Profiler is activated ?
// @return true or false
//-------------------------------------------------------------------------
bool ProfilerActivate()
{
	LOG("*** ProfilerActivate ***\n");

	static int m_ProfilerActivated = -1;
	/*
	if ( m_ProfilerActivated == -1 )
	{
#define CONFIG_BUFFER_SIZE 256
		char	*ProfilerActivatedStr = new char[CONFIG_BUFFER_SIZE];
		GetPrivateProfileString( TEXT("Profiler"),
								 TEXT("ProfilerEnable"),
								 NULL,
								 ProfilerActivatedStr,
								 CONFIG_BUFFER_SIZE,
								 INI_FILENAME );


		if( 0==ProfilerActivatedStr[0] )
		{
			m_ProfilerActivated = 0;
		}
		else
		{
			m_ProfilerActivated = atoi(ProfilerActivatedStr);
		}

		delete [] ProfilerActivatedStr;
	}
	*/
	m_ProfilerActivated = 1;
	return (m_ProfilerActivated)?true:false;
}

////
////	Gestion des timers
////
#if defined(WIN32)

	// Initialize Our Timer (Get It Ready)
	void TimerInit(void)
	{
		memset(&timer, 0, sizeof(timer));

		// Check to see if a performance counter is available
		// If one is available the timer frequency will be updated
		if( !QueryPerformanceFrequency((LARGE_INTEGER *) &timer.frequency) )
		{
			// No performace counter available
			timer.performance_timer	= false;					// Set performance timer to false
			timer.mm_timer_start	= timeGetTime();			// Use timeGetTime() to get current time
			timer.resolution		= 1.0f/1000.0f;				// Set our timer resolution to .001f
			timer.frequency			= 1000;						// Set our timer frequency to 1000
			timer.mm_timer_elapsed	= timer.mm_timer_start;		// Set the elapsed time to the current time
		}
		else
		{
			// Performance counter is available, use it instead of the multimedia timer
			// Get the current time and store it in performance_timer_start
			QueryPerformanceCounter((LARGE_INTEGER *) &timer.performance_timer_start);
			timer.performance_timer			= true;				// Set performance timer to true

			// Calculate the timer resolution using the timer frequency
			timer.resolution				= (float) (((double)1.0f)/((double)timer.frequency));

			// Set the elapsed time to the current time
			timer.performance_timer_elapsed	= timer.performance_timer_start;
		}
	}

	// platform specific get hires times...
	double startHighResolutionTimer()
	{
		__int64 time;

		// Are we using the performance timer?
		if( timer.performance_timer )
		{
			// Grab the current performance time
			QueryPerformanceCounter((LARGE_INTEGER *) &time);

			// Return the current time minus the start time multiplied
			// by the resolution and 1000 (To Get MS)
			return ( (double) ( time - timer.performance_timer_start) * timer.resolution)*1000.0f;
		}
		else
		{
			// Return the current time minus the start time multiplied
			// by the resolution and 1000 (To Get MS)
			return( (double) ( timeGetTime() - timer.mm_timer_start) * timer.resolution)*1000.0f;
		}
	}
/*
	unsigned long endHighResolutionTimer(unsigned long time[2])
	{
		unsigned long ticks=0;
		//__asm__ __volatile__(
		//   "rdtsc\n"
		//   "sub  0x4(%%ecx),  %%edx\n"
		//   "sbb  (%%ecx),  %%eax\n"
		//   : "=a" (ticks) : "c" (time)
		//   );
		return ticks;
	}
*/
#elif defined(MAC_OS)

	// Initialize Our Timer (Get It Ready)
	void TimerInit(void)
	{
	}

	void startHighResolutionTimer(unsigned long time[2])
	{
		UnsignedWide t;
		Microseconds(&t);
		time[0] = t.lo;
		time[1] = t.hi;
	}
/*
	unsigned long endHighResolutionTimer(unsigned long time[2])
	{
		UnsignedWide t;
		Microseconds(&t);
		return t.lo - time[0];
		// given that we're returning a 32 bit integer, and this is unsigned subtraction...
		// it will just wrap around, we don't need the upper word of the time.
		// NOTE: the code assumes that more than 3 hrs will not go by between calls to startHighResolutionTimer() and endHighResolutionTimer().
		// I mean... that damn well better not happen anyway.
	}
*/
#else

	// Initialize Our Timer (Get It Ready)
	void TimerInit(void)
	{
	}

	double startHighResolutionTimer()
	{
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux


		double ms = double(ts.tv_sec*1000LU);
		ms += double(ts.tv_nsec/1000LU)*0.001;


	    return ms;
	}
/*
	unsigned long endHighResolutionTimer(unsigned long time[2])
	{
		return 1;
	}
*/
#endif

	// Gestion des sections critiques
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

