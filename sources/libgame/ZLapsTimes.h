#ifndef ZLAPSTIMES_H__
#define ZLAPSTIMES_H__

#include "BitStream.h"
#include "RakPeerInterface.h"
using namespace RakNet;
class ZRushGame;
class ZRushNetGame;
class ZNetShip;


class ZLapsTimes
{
public:
	ZLapsTimes()
	{
		Clear();
	}

	~ZLapsTimes()
	{
	}

	void PushTime(ZShip *pShip, float aTime)
	{
		mTimes[pShip].push_back(aTime);
		std::map<ZShip*, float>::iterator iter = mTotalTime.find(pShip);
		if (iter != mTotalTime.end())
			(*iter).second += aTime;
		else
			mTotalTime.insert( std::make_pair(pShip, aTime) );

		if (aTime<aBestLapTime)
		{
			pBestLapTimeShip = pShip;
			aBestLapTime = aTime;
		}
	}

	void Clear()
	{
		mTimes.clear();
		aBestLapTime = 99999.f;
		pBestLapTimeShip = NULL;
	}

	std::map<ZShip*, std::vector<float> > mTimes;
	std::map<ZShip*, float> mTotalTime;
	ZShip *pBestLapTimeShip;
	float aBestLapTime;

	void BroadCast(RakPeerInterface *rakInterface);
	void Read(ZRushNetGame *pGame, BitStream& bitStream);

	tstring BuildEndResults(ZRushGame *pGame) const;

	int GetShipRanking(ZShip *pShip) const;
	static int sortRes(const void *p1, const void *p2);

	const std::vector<float>* GetTimesForShip(ZShip* pShip) const;
};

#endif