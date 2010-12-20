#ifndef ZMETADATAS_H__
#define ZMETADATAS_H__

#include "ZHashName.h"

#include <map>

typedef struct MetaData_t
{
	enum DataType
	{
		AUDIO,
		TEXTURE
	};
	DataType mType;
	union 
	{
		struct 
		{
			unsigned is3DSound : 1;
			unsigned isStreaming : 1;
		} Audio;
		struct 
		{
			unsigned hasAlphaTest : 1;
		} Texture;
	};
	MetaData_t(bool a3DSound, bool aStreaming)
	{
		mType = AUDIO;
		Audio.is3DSound = a3DSound;
		Audio.isStreaming = aStreaming;
	}
} MetaData_t;

class ZMetaDatas
{
public:
	ZMetaDatas()
	{
	}
	virtual ~ZMetaDatas()
	{
	}
	void Init(const char *szPathTooXML);
	const MetaData_t* GetMetaData(HASH resourceHash) const;
protected:
	std::map<HASH, MetaData_t> mMetaDatas;
};

#endif
