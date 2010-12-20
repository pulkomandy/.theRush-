#ifndef ZHASHNAME_H__
#define ZHASHNAME_H__

#include <inttypes.h>

typedef uint32_t HASH;

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
