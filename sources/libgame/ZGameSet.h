#ifndef ZGAMESET_H__
#define ZGAMESET_H__

#include <stdlib.h>

typedef struct GameSet_t
{
	GameSet_t()
	{
		mCurrentMapName[0] = 0;
		mCurrentWorldName[0] = 0;
		mbHasBonus = true;
		mNbLaps = 3;
	}

	GameSet_t(const tstring& currentWorldName, const tstring& mapName, int aGameType = GAME_TYPE_RAW, bool bHasBonus = true, int aNbLaps = 3)
	{
		//mCurrentMapName = mapName;
		ZASSERT( (mapName.Length()<=255), "Potential security hole.");
		strcpy(mCurrentMapName, mapName.c_str());
		strcpy(mCurrentWorldName, currentWorldName.c_str());
		
		mGameType = aGameType;
		mbHasBonus = bHasBonus;
		mNbLaps = aNbLaps;
	}

	bool operator == (const GameSet_t& other)
	{
		bool bequal = true;

		bequal &= (!stricmp(mCurrentMapName, other.mCurrentMapName));//(other.mCurrentMapName == mCurrentMapName);
		bequal &= (!stricmp(mCurrentWorldName, other.mCurrentWorldName));//(other.mCurrentMapName == mCurrentMapName);
		bequal &= (mbHasBonus == other.mbHasBonus);
		bequal &= (mGameType == other.mGameType);
		bequal &= (mNbLaps == other.mNbLaps);
		return bequal;
	}
	bool operator != (const GameSet_t& other)
	{
		return (!( *this == other));
	}

	void operator = (const GameSet_t& other)
	{
		memcpy(mCurrentMapName, other.mCurrentMapName, 256);
		memcpy(mCurrentWorldName, other.mCurrentWorldName, 256);
		mbHasBonus = other.mbHasBonus;
		mGameType = other.mGameType;
		mNbLaps = other.mNbLaps;
	}
	char mCurrentWorldName[256];
	char mCurrentMapName[256];
	bool mbHasBonus;
	int mGameType;
	int mNbLaps;

	enum GAME_TYPE
	{
		GAME_TYPE_RAW = 0,
		GAME_TYPE_CLASSIC = 1,
		GAME_TYPE_MENU = 2,
		GAME_TYPE_EDITOR = 3
	};

} GameSet_t;

#endif
