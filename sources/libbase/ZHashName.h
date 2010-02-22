#ifndef ZHASHNAME_H__
#define ZHASHNAME_H__

#ifdef WIN32
typedef unsigned __int32 HASH;
#else
typedef unsigned int HASH;
#endif
inline HASH GetHash(const char *pszStr)
{
	HASH ret = 0;
	while (*pszStr != 0)
	{
		ret += (*pszStr) * 65599;
		pszStr ++;
	}
	return ret;
}

#endif