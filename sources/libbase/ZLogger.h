///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZLogger.h
// Creation : 16/06/2007
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

#ifndef ZLOGGER_H__
#define ZLOGGER_H__

// levels:
// console : Infos returned by scripting commands
// Log   : Infos from engine (init, close,...)
// warn  : error that won't cause crash
// Error : error that'll cause crash
enum LOG_LEVEL
{
    LOG_CONSOLE,
    LOG_LOG,
    LOG_WARN,
    LOG_ERROR
};

class ILogger : public ZBaseClass
{
public:
    virtual void Log(LOG_LEVEL level, const tstring& str) = 0;
};

DECLAREZCLASS(ZLoggerMB);
DECLAREZCLASS(ZLoggerFile);
DECLAREZCLASS(ZLoggerPrintf);
#ifdef _WIN32
DECLAREZCLASS(ZLoggerOutputDebugString);
#endif
DECLAREZCLASS(ZLoggerHTMLFile);
DECLAREZCLASS(ZLoggerSTDOUT);


class ZLogger
{
private:

    smartptr<ILogger> mConsole;
    smartptr<ILogger> mLog;
    smartptr<ILogger> mWarn;
    smartptr<ILogger> mError;

public:
    ZLogger() {}
    virtual ~ZLogger() {}

    void SetConsole(ILogger *aLog) { mConsole.Attach(aLog); }
    void SetLog(ILogger *aLog)
	{
		mLog.Attach(aLog);
	}
    void SetWarn(ILogger *aLog) { mWarn.Attach(aLog); }
    void SetError(ILogger *aLog) { mError.Attach(aLog); }

    void Console(const char *format,...);
    void Log(const char *format,...);
    void Warn(const char *format,...);
    void Error(const char *format,...);

};

extern ZLogger GLOG;

#define CONSOLE GLOG.Console
#define LOG GLOG.Log
#define WARN GLOG.Warn
#define ERR GLOG.Error




#ifdef _WIN32
#define ZENITH_HALT() do { __debugbreak(); } while(0)
#else
#define ZENITH_HALT() do { __asm int 3 } while(0)
#endif

#if defined(_DEBUG) || defined(ZENITH_ASSERT_ENABLED)
#define ZENITH_ASSERT(x, y) \
	do { if  ((!(x))) {\
	GLOG.Error("Assertion in " __FILE__ " Line %d : %s.", __LINE__, y); \
	ZENITH_HALT();} } while(0)
#else
#define ZENITH_ASSERT(x, y) \
	do { (void)sizeof(x); } while(0)
#endif


#ifdef _DEBUG
    #define ZASSERT(x , y) if (!x) {GLOG.Error("Assertion in " __FILE__ " Line %d : %s.", __LINE__, y); Zexit(-1);  }
#else
    #define ZASSERT(x , y)
#endif

#endif
