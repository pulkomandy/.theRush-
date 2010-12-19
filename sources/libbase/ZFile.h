///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZFile.h
// Creation : 07/07/2006
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

#ifndef ZH__
#define ZH__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZString.h"
#include "ZBaseDefs.h"

// Constants //////////////////////////////////////////////////////////////////////////////////////

typedef tuchar ZOPEN_FLAG ;

const ZOPEN_FLAG    ZOPEN_WRITE    = 1 << 0;
const ZOPEN_FLAG    ZOPEN_READ     = 1 << 1;
const ZOPEN_FLAG    ZOPEN_APPEND   = 1 << 2;
const ZOPEN_FLAG    ZOPEN_ENCRYPT  = 1 << 3;
const ZOPEN_FLAG    ZOPEN_TEXT     = 1 << 4;
const ZOPEN_FLAG    ZOPEN_BINARY   = 1 << 5;
const ZOPEN_FLAG    ZOPEN_BUFFERED = 1 << 6;

class ZBaseClass;

enum    ZSEEK_ORIGIN
{
    ZSEEK_BEGIN        = SEEK_SET,
    ZSEEK_END            = SEEK_END,
    ZSEEK_CURRENT    = SEEK_CUR
};

// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZFile
{
    friend class ZPackage;

protected:
    bool        mBuffered;
    tuchar *     mBuffer;
    tstring      mFullName;
    tulong       mSize;
    tulong       mPackStartOffset;
    tulong       mPackFileSize;
    tulong       mCurrentPos;
    FILE *       mHandle;
    ZFile *   mPackFile;
    bool        mFreeBuffer;
    bool        mDelFile;

    tstring      BuildFlags(const ZOPEN_FLAG flags);

public:
    ZFile();
    virtual ~ZFile();

public:
    static bool CanOpen(const tstring & fileName, const ZOPEN_FLAG flags = ZOPEN_READ);
    tstring&     GetFullName() { return mFullName; }
    bool        Open(const tstring & path, const tstring & name, const ZOPEN_FLAG flags = ZOPEN_READ);
    bool        Open(const tstring & fullName, const ZOPEN_FLAG flags = ZOPEN_READ, bool bStopOnError = true);
    bool        OpenMem(tuchar * buffer, tulong bufferSize=0);
#ifdef ZWIN32
    bool        OpenRes(HINSTANCE hInst, LPCTSTR lpResourceName, LPCTSTR lpType);
#endif
    virtual bool Close();
    bool        Seek(const tlong pos, ZSEEK_ORIGIN origin = ZSEEK_BEGIN);

    bool        Write(const void * buffer, const tulong sizeInBytes);
    bool        Write(const tstring & string);
    bool        Printf(const char *format,...);

    tulong       Read(void * buffer, const tulong sizeInBytes);
    bool        Read(tstring & a);

    tchar        GetChar()
    {
        tchar car;
        Read(&car, 1);
        return car;
    }

    FILE *       GetHandle() { return mHandle; }
    tlong        GetSize();

    tlong        GetPos();

    static void GetFilesList(std::vector<tstring>& aList, const char *szPath, const char *szWild, bool bRecurs, bool bDirectoriesInsteadOfFiles, bool bCompletePath);

    template <class T> void operator << ( T& type)
    {
        fwrite(&type, sizeof(T), 1, mHandle);
    }
/*
    template <class T> void operator << ( const T& type)
    {
        <class T> typeCopy;
        typeCopy=type;
        fwrite(&typeCopy, sizeof(T), 1, mHandle);
    }
*/
    template <class T> void operator >> ( T& type)
    {
        Read(&type, sizeof(T));
    }
	tstring ReadFileString();
};

// Inlines ////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
