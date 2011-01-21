///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZFile.cpp
// Creation : 07/07/2006
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

#include "ZFile.h"
#include "ZLogger.h"
#include <cassert>
#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

ZFile::ZFile()
{
	mHandle             = NULL;
	mBuffer             = NULL;
	mBuffered           = false;
	mSize               = 0;
	mFreeBuffer         = false;
	mSize               = 0;
	mPackFile           = NULL;
	mDelFile            = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZFile::~ZFile()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tstring ZFile::BuildFlags(const ZOPEN_FLAG flags)
{
	tstring accessMode;
	if(flags & ZOPEN_APPEND)
	{
		if(flags & ZOPEN_READ)
			accessMode = "a+";
		else
			accessMode = "a";
	}
	else
	{
		if(flags & ZOPEN_WRITE)
		{
			if(flags & ZOPEN_READ)
				accessMode = "r+";
			else
				accessMode = "w";
		}
		else
		{
			if(flags & ZOPEN_READ)
				accessMode = "r";
		}
	}

	// Set text or binary mode
	if(flags & ZOPEN_TEXT)
		accessMode += "t";
	else
		accessMode += "b";

	if(flags & ZOPEN_BUFFERED)
		mBuffered = true;

	return accessMode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::CanOpen(const tstring & fileName, const ZOPEN_FLAG p_flags)
{
	ZFile fptmp;
	if (fptmp.Open(fileName, p_flags, false))
	{
		fptmp.Close();
		return true;
	}
	return false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Open(const tstring & path, const tstring & name, const ZOPEN_FLAG p_flags)
{
	return Open(path + name, p_flags);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//ZFile *FindInBigFile(const char *szName, tulong &start, tulong &mPackFileSize, const tstring& accessMode, bool delfile);

bool ZFile::Open(const tstring & p_fullName, const ZOPEN_FLAG p_flags, bool bStopOnError)
{
	mFullName    = p_fullName;

	//mFullName.SimplifyPath();

	Close();

	tstring accessMode = BuildFlags(p_flags);
	if (p_flags&ZOPEN_WRITE)
		mPackFile = NULL;
	/*
	else
	mPackFile = FindInBigFile(mFullName.GetFileName(), mPackStartOffset, mPackFileSize, accessMode, mDelFile);
	*/
	// Init the access mode
	if (!mPackFile)
	{
		mHandle = fopen(mFullName.c_str(), accessMode);
		if (bStopOnError)
		{
			char tmps[512];
			GetCurrentDirectory(512, tmps);
			if (mHandle == 0)
			{
				// file not found
				LOG("Err loading file %s \n",mFullName.c_str());
				assert(false);
			}
			tstring errOpening("Cannot open file " + mFullName);
			ZASSERT( (mHandle != 0) , errOpening.c_str() );
		}
		if (!mHandle) return false;
		if (p_flags&ZOPEN_WRITE) return true;
	}

	mSize = GetSize();

	if(mBuffered)
	{
		mBuffer            = (tuchar*)malloc(mSize);
		mFreeBuffer        = true;
		Read(mBuffer, mSize);
	}
	mCurrentPos = 0;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

bool    ZFile::OpenMem(tuchar * p_memBuffer, tulong p_bufferSize)
{
	mBuffer              =    p_memBuffer;
	mFreeBuffer        =    false;
	mBuffered            =    true;
	mCurrentPos = 0;
	mSize = 0xFFFFFFFF;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PSM_WIN32
/*
bool    ZFile::OpenRes(HINSTANCE hInst, LPCTSTR lpResourceName, LPCTSTR lpType)
{
m_FreeBuffer    =    true;
buffered            =    true;

HRSRC hSrc = FindResource(hInst, lpResourceName, lpType);

if(hSrc)
{
BYTE * hResData = (unsigned char*)LoadResource(hInst, hSrc);
if(hResData)
{
LPVOID lpResData = LockResource(hResData);
if(lpResData)
{
DWORD dwSize = SizeofResource(hInst, hSrc);
if(dwSize)
{
buffer = (BYTE*)MALLOC(dwSize);
if(buffer)
{
bufferSize    =    dwSize;
CopyMemory(buffer, lpResData, dwSize);
return true;
}
}
}
}
}

return false;

}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Close()
{
	if(mHandle)
	{
		fclose(mHandle);
		mHandle        = NULL;
	}
	if (mFreeBuffer&&mBuffer)
	{
		free(mBuffer);
		mBuffer = NULL;
	}

	if (mDelFile)
	{
		if (mPackFile)
			free(mPackFile);
		mPackFile = NULL;
		mDelFile = false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Seek(const tlong pos, ZSEEK_ORIGIN origin)
{
	if(mHandle)
	{
		if (mBuffered)
		{
			switch (origin)
			{
			case SEEK_SET: mCurrentPos = pos; break;
			case SEEK_END: mCurrentPos = mSize-pos; break;
			case SEEK_CUR: mCurrentPos += pos; break;
			}
			return true;
		}
		if( fseek(mHandle, pos, origin ) != 0)
		{
			ZASSERT(false, "Cannot seek into file");
			return false;
		}
		return true;
	}
	if (mPackFile)
	{
		mCurrentPos = pos;
		return mPackFile->Seek(pos+mPackStartOffset, origin)?true:false;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Write(const void * p_buffer, const tulong sizeInBytes)
{
	if(mHandle)
	{
		if( fwrite(p_buffer, sizeInBytes, 1, mHandle) == 1)
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Write(const tstring & string)
{
	uint32_t strl = string.Length();
	*this << strl;
	Write(string.c_str(), strl);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Read(tstring & string)
{
	uint32_t strl;
	*this >> strl;
	string.stringData = (tchar *)malloc(strl+1);
	Read(string.stringData, strl);
	string.stringData[strl] = 0;
	string.length = strl;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZFile::Printf(const char *format,...)
{
	tchar strToWrite[1000];

	va_list ptr_arg;
	va_start(ptr_arg, format);
	vsprintf(strToWrite,format, ptr_arg);    // on ecrit dans le buffer
	if(!Write(strToWrite))
	{
		return false;
	}
	va_end(ptr_arg);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

tulong ZFile::Read(void * p_buffer, const tulong sizeInBytes)
{
	if (mBuffered)
	{
		if ((mCurrentPos+sizeInBytes) < mSize)
		{
			memcpy(p_buffer, &mBuffer[mCurrentPos], sizeInBytes);
			mCurrentPos += sizeInBytes;
			return sizeInBytes;
		}
		else
		{
			memcpy(p_buffer, &mBuffer[mCurrentPos], mSize-mCurrentPos);
			mCurrentPos += mSize-mCurrentPos;
			return (mSize-mCurrentPos);
		}
	}
	else
		if (mHandle)
		{
			return (tulong)fread(p_buffer, 1, sizeInBytes, mHandle);
		}
		else
			if (mPackFile)
			{
				if (mCurrentPos>(tulong)GetSize())
					return 0;
				// streaming from pack
				if ( (mPackStartOffset + mCurrentPos) != (unsigned int)mPackFile->GetPos())
					mPackFile->Seek( (mPackStartOffset + mCurrentPos) );
				long ret = mPackFile->Read(p_buffer, sizeInBytes);
				mCurrentPos += ret;
				if (mCurrentPos>(tulong)GetSize())
					ret -= (mCurrentPos-GetSize());

				return ret;
			}

			return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
tstring ZFile::Read()
{
char str[255];
tstring retour;

D_ASSERT((handle != 0) || (readFromPack && buffer), "Cannot Read : File not Open");

if(handle|| buffer)
{
if(handle)
{
fscanf(handle, "%s", str);
retour = str;
}
else
{
while(buffer[posInBuffer] != '\n' && buffer[posInBuffer] != ' ')
{
retour += buffer[posInBuffer];
posInBuffer++;
}
}
}
else
{
retour = "";
}

return retour;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

tlong ZFile::GetSize()
{
	if (mBuffered)
		return mSize;
	else
		if (mPackFile)
			return mPackFileSize;
		else
			if (mHandle)
			{
				if (!mSize)
				{
					fseek(mHandle, 0, SEEK_END );
					mSize = ftell(mHandle);
					fseek(mHandle, 0, SEEK_SET );
				}
				return mSize;
			}
			return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tlong ZFile::GetPos()
{
	if (mBuffered)
	{
		return mCurrentPos;
	}
	else
		if (mPackFile)
		{
			return mCurrentPos;//mPackFile->GetPos();
		}
		else
			if (mHandle)
			{
				return ftell(mHandle);
			}
			return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


void ZFile::GetFilesList(std::vector<tstring>& aList, const char *szPath, const char *szWild, bool bRecurs, bool bDirectoriesInsteadOfFiles, bool bCompletePath)
{
#ifdef WIN32

	_finddata_t fileinfo;
	tstring path = szPath;
	tstring wildc = "*";

	long fret;
	intptr_t fndhand = _findfirst(path+wildc, &fileinfo);
	if (fndhand != -1)
	{
		do
		{
			if (strcmp(fileinfo.name,".") && strcmp(fileinfo.name,"..") )
			{
				tstring wildc2;
				wildc2=szPath;
				wildc2+=fileinfo.name;
				if (!(fileinfo.attrib&_A_HIDDEN))
				{
					if (fileinfo.attrib&_A_SUBDIR)
					{
						if (bDirectoriesInsteadOfFiles)
						{
							if (bCompletePath)
							{
								aList.push_back(wildc2);
							}
							else
							{
								aList.push_back(fileinfo.name);
							}
						}

						wildc2+="/";
						if (bRecurs)
							GetFilesList(aList, wildc2.c_str(), szWild, bRecurs, bDirectoriesInsteadOfFiles, bCompletePath);
					}
					else if (!bDirectoriesInsteadOfFiles)
					{
						if (strstr(wildc2.c_str(), szWild))
						{
							if (bCompletePath)
							{
								aList.push_back(wildc2);
							}
							else
							{
								aList.push_back(fileinfo.name);
							}
						}
					}
				}
			}
			fret=_findnext(fndhand,&fileinfo);
		}
		while (fret!=-1);
	}
	_findclose(fndhand);

	// BigFile
	//BFDumpList(aList, szPath, szWild);
#else

	struct dirent *lecture;	
	DIR *rep;
	rep = opendir(szPath);	
	struct stat entry_info;

	while ((lecture = readdir(rep)))
	{
		if ( lecture->d_name[0] != '.' )
		{
			tstring wildc2;
			wildc2 = szPath;
			wildc2 += lecture->d_name;
			stat(wildc2, &entry_info);
			if (S_ISDIR(entry_info.st_mode))
			{
				if (bDirectoriesInsteadOfFiles)
				{
					if (bCompletePath)
					{
						aList.push_back(wildc2);
					}
					else
					{
						aList.push_back(lecture->d_name);
					}
				}

				wildc2+="/";
				if (bRecurs)
					GetFilesList(aList, wildc2.c_str(), szWild, bRecurs, bDirectoriesInsteadOfFiles, bCompletePath);
			}
			else if (S_ISREG(entry_info.st_mode))
			{
				if (!bDirectoriesInsteadOfFiles)
					if (strstr(wildc2.c_str(), szWild))
					{
						if (bCompletePath)
						{
							aList.push_back(wildc2);
						}
						else
						{
							aList.push_back(lecture->d_name);
						}
					}
			}
		}
	}

	closedir(rep);	

#endif


}

///////////////////////////////////////////////////////////////////////////////////////////////////


tstring ZFile::ReadFileString()
{
	int size = this->GetSize();
	char *psz = new char [size+1];
	Read(psz, size);
	psz[size]=0;
	tstring res(psz);
	delete [] psz;
	return res;
}
