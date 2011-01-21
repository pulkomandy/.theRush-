#include "ZBaseDefs.h"
#include "targetver.h"
#include <cstdio>
#ifdef WIN32
#include <shlobj.h>
#endif

__inline const char * GetPictureDirectory()
{
#ifdef LINUX
	return "~/";
#else 
	#ifdef MAC_OS
	return "~/pictures/";
#else	// WIN32
	wchar_t szLongPath[MAX_PATH] = { 0 };
	LPITEMIDLIST pidlDocFiles;
	/*HRESULT hr =*/ SHGetFolderLocation(NULL, 
		CSIDL_MYPICTURES, 
		NULL,
		0,
		&pidlDocFiles);
	SHGetPathFromIDListW(pidlDocFiles, szLongPath);
	// Free the ID list allocated by ParseDisplayName
	LPMALLOC pMalloc = NULL;
	SHGetMalloc(&pMalloc);
	pMalloc->Free(pidlDocFiles);
	pMalloc->Release();
	static char imgPath[MAX_PATH];
	sprintf(imgPath, /*MAX_PATH,*/ "%ls",szLongPath);
	return imgPath;
#endif
#endif
}

__inline const char * GetHomeDirectory()
{
#ifdef LINUX
	return "~/";
#else 
	#ifdef MAC_OS
	return "~/";
#else	// WIN32
	WCHAR szLongPath[MAX_PATH] = { 0 };
	LPITEMIDLIST pidlDocFiles;
	/*HRESULT hr =*/ SHGetFolderLocation(NULL, 
		CSIDL_PERSONAL, 
		NULL,
		0,
		&pidlDocFiles);
	SHGetPathFromIDListW(pidlDocFiles, szLongPath);
	// Free the ID list allocated by ParseDisplayName
	LPMALLOC pMalloc = NULL;
	SHGetMalloc(&pMalloc);
	pMalloc->Free(pidlDocFiles);
	pMalloc->Release();
	static char imgPath[MAX_PATH];
	sprintf(imgPath/*, MAX_PATH*/, "%ls",szLongPath);
	strcat(imgPath, "/");
	return imgPath;
#endif
#endif
}
