///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZLogger.cpp
// Creation : 16/06/2007
// Author : Cedric Guillemet
// Description :
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZLogger.h"
#include "ZSerializator.h"

#if (defined(WIN32) || defined(MAC_OS))
#include "SDL.h"
#else
#include <SDL/SDL.h>
#endif

ZLogger GLOG;


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZLogger::Console(const char *format,...)
{
    if (mConsole.ptr())
    {
        va_list ptr_arg;
        va_start(ptr_arg, format);
        tstring str;
        str.Printf(format, &ptr_arg);
        mConsole->Log(LOG_CONSOLE, str);
        va_end(ptr_arg);
    }
}

void ZLogger::Log(const char *format,...)
{
    if (mLog.ptr())
    {
        va_list ptr_arg;
        va_start(ptr_arg, format);
        tstring str;
        str.Printf(format, &ptr_arg);
        mLog->Log(LOG_LOG, str);
        va_end(ptr_arg);
    }
}

void ZLogger::Warn(const char *format,...)
{
    if (mWarn.ptr())
    {
        va_list ptr_arg;
        va_start(ptr_arg, format);
        tstring str;
        str.Printf(format, &ptr_arg);
        mWarn->Log(LOG_WARN, str);
        va_end(ptr_arg);
    }
}

void ZLogger::Error(const char *format,...)
{
    if (mError.ptr())
    {
        va_list ptr_arg;
        va_start(ptr_arg, format);
        tstring str;
        str.Printf(format, &ptr_arg);
        mError->Log(LOG_ERROR, str);
        va_end(ptr_arg);
		exit(-1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZLoggerFile : public ILogger
{
public:
    DECLARE_SERIALIZABLE

    ZLoggerFile()
    {
        mFile.Open("./Log.txt",ZOPEN_WRITE);
    }
    virtual ~ZLoggerFile()
    {
    }

    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
        mFile.Write(str.c_str(), str.Length());
    }

    virtual tulong GetMemoryUsed() const { return sizeof(ZLoggerFile); }
private:
    ZFile mFile;

};

BEGIN_SERIALIZATION(ZLoggerFile)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZLoggerHTMLFile : public ILogger
{
public:
    DECLARE_SERIALIZABLE

    ZLoggerHTMLFile()
    {
        mFile.Open("./Log.html",ZOPEN_WRITE);
		this->Log(LOG_LOG,"<html><head><title>Zenith Log</title></head><body>");
    }
    virtual ~ZLoggerHTMLFile()
    {
		mFile.Write("</body></html>", (tulong)strlen("</body></html>"));
    }

    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
		mFile.Write(str.c_str(), (tulong)str.Length());
		mFile.Write("<br>", (tulong)strlen("<br>"));
    }

    virtual tulong GetMemoryUsed()  const { return sizeof(ZLoggerHTMLFile); }
private:
    ZFile mFile;

};

BEGIN_SERIALIZATION(ZLoggerHTMLFile)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZLoggerMB : public ILogger
{
public:
    ZLoggerMB() {}
    virtual ~ZLoggerMB() {}

    DECLARE_SERIALIZABLE
public:
    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
        const char *sType;
        switch(level)
        {
        case LOG_CONSOLE: sType = "Console"; break;
            case LOG_LOG: sType = "Log"; break;
            case LOG_WARN: sType = "Warn"; break;
            case LOG_ERROR: sType = "Error"; break;
        }
#ifdef WIN32
        MessageBoxA(NULL, str.c_str(), sType, MB_OK);
#endif
    }
    virtual tulong GetMemoryUsed()  const { return sizeof(ZLoggerMB); }
};


BEGIN_SERIALIZATION(ZLoggerMB)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////


class ZLoggerOutputDebugString : public ILogger
{
public:
    ZLoggerOutputDebugString() {}
    virtual ~ZLoggerOutputDebugString() {}

    DECLARE_SERIALIZABLE
public:
    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
        const char *sType;
        switch(level)
        {
        case LOG_CONSOLE: sType = "Console"; break;
            case LOG_LOG: sType = "Log"; break;
            case LOG_WARN: sType = "Warn"; break;
            case LOG_ERROR: sType = "Error"; break;
        }
        //MessageBox(NULL, str.c_str(), sType, MB_OK);
#ifdef WIN32
        OutputDebugStringA(str.c_str());
#endif
    }
    virtual tulong GetMemoryUsed()  const { return sizeof(ZLoggerOutputDebugString); }
};


BEGIN_SERIALIZATION(ZLoggerOutputDebugString)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////


class ZLoggerPrintf : public ILogger
{
public:
    ZLoggerPrintf() {}
    virtual ~ZLoggerPrintf() {}

    DECLARE_SERIALIZABLE
public:
    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
        const char *sType;
        switch(level)
        {
        case LOG_CONSOLE: sType = "Console"; break;
            case LOG_LOG: sType = "Log"; break;
            case LOG_WARN: sType = "Warn"; break;
            case LOG_ERROR: sType = "Error"; break;
        }
        printf(str.c_str());

    }
    virtual tulong GetMemoryUsed()  const { return sizeof(ZLoggerOutputDebugString); }
};


BEGIN_SERIALIZATION(ZLoggerPrintf)
END_SERIALIZATION()



///////////////////////////////////////////////////////////////////////////////////////////////////


class ZLoggerSTDOUT : public ILogger
{
public:
    ZLoggerSTDOUT() {}
    virtual ~ZLoggerSTDOUT() {}

    DECLARE_SERIALIZABLE
public:
    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
        const char *sType;
        switch(level)
        {
        case LOG_CONSOLE: sType = "Console"; break;
            case LOG_LOG: sType = "Log"; break;
            case LOG_WARN: sType = "Warn"; break;
            case LOG_ERROR: sType = "Error"; break;
        }
        //MessageBox(NULL, str.c_str(), sType, MB_OK);
        printf("%s",str.c_str());
    }
    virtual tulong GetMemoryUsed()  const { return sizeof(ZLoggerSTDOUT); }
};


BEGIN_SERIALIZATION(ZLoggerSTDOUT)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////


void LogFramesPerSecond()
{
    static int T0     = 0;
    static int Frames = 0;

    Frames++;
    {
	int t = SDL_GetTicks();
	if (t - T0 >= 5000) {
	    float seconds = (t - T0) / 1000.0f;
	    float fps = Frames / seconds;
	    LOG("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
	    T0 = t;
	    Frames = 0;
	}
    }
}