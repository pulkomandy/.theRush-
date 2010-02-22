///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZProfiler.h
// Creation : 30/01/2008
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

#ifndef ZPROFILER_H__
#define ZPROFILER_H__

bool Zprofiler_enable();
void Zprofiler_disable();
void Zprofiler_start( const char *profile_name );
void Zprofiler_end( );
//void Zprofiler_dumpToFile(const char *filename);

void LogProfiler();

void TimerInit(void);



//defines
#define USE_PROFILER
#ifdef USE_PROFILER

#define PROFILER_ENABLE Zprofiler_enable()
#define PROFILER_DISABLE Zprofiler_disable()
#define PROFILER_START(x) Zprofiler_start(QUOTE(x))
#define PROFILER_END() Zprofiler_end()

#else

#define PROFILER_ENABLE
#define PROFILER_DISABLE
#define PROFILER_START(x)
#define PROFILER_END()

#endif

#endif	// _ZPROFILER_
