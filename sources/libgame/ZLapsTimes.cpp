//////////////////////////////////////////////////////////////////////////////////////////////
	
#include "stdafx.h"
#include "BitStream.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "StringTable.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "ReplicaManager2.h"
using namespace RakNet;
#include "ZNetMessages.h"
#include "ZNetShip.h"
#include "ZRushNetGame.h"

//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct sortResults_t
{
	float totalTime;
	ZNetShip *pShip;
	SystemAddress addr;
	char bestLap;
} sortResults_t;

//////////////////////////////////////////////////////////////////////////////////////////////

void ZLapsTimes::BroadCast(RakPeerInterface *rakInterface)
{
	BitStream bitStream;
	unsigned char msgid = ZNMSG_GAMELAPSTIMES;
	bitStream.Write(msgid);
	unsigned int nbEntries = mTotalTime.size();
	bitStream.Write(nbEntries);

	std::map<ZShip*, float>::const_iterator iter = mTotalTime.begin();
	for (; iter != mTotalTime.end(); ++iter)
	{
		bitStream.Write( ((ZNetShip*)(*iter).first)->mNetworkOwner);
		bitStream.Write( (*iter).second );
		char bestLap = (pBestLapTimeShip == (*iter).first)? 1:0;
		bitStream.Write( bestLap );
	}

	rakInterface->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}

//////////////////////////////////////////////////////////////////////////////////////////////

int ZLapsTimes::sortRes(const void *p1, const void *p2)
{
	const sortResults_t *r1 = (const sortResults_t*)p1;
	const sortResults_t *r2 = (const sortResults_t*)p2;

	if (r2->totalTime < r1->totalTime)
		return 1;
	if (r2->totalTime > r1->totalTime)
		return -1;
	return 0;
}

void ZLapsTimes::Read(ZRushNetGame *pGame, BitStream& bitStream)
{
	mTotalTime.clear();

	unsigned int nbEntries;
	bitStream.Read(nbEntries);

	for (unsigned int i = 0 ; i < nbEntries; i ++)
	{
		SystemAddress addr;
		float totalTime;
		char bestLap;
		

		bitStream.Read(addr);
		bitStream.Read(totalTime);
		bitStream.Read(bestLap);

		ZShip *pShip = pGame->GetShipBySystemAddress(addr);
		if (pShip)
		{
			mTotalTime[pShip] = totalTime;
			if (bestLap)
			{
				pBestLapTimeShip = pShip;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

tstring ZLapsTimes::BuildEndResults(ZRushGame *pGame) const
{
	tstring res;

	unsigned int nbEntries = mTotalTime.size();
	sortResults_t *pres = new sortResults_t[nbEntries];

	unsigned int i = 0;
	std::map<ZShip*, float>::const_iterator iter = mTotalTime.begin();
	for (; iter != mTotalTime.end(); ++iter, i++)
	{
		pres[i].pShip = (ZNetShip*)(*iter).first;
		pres[i].bestLap = ((*iter).first == pBestLapTimeShip)?1:0;
		pres[i].totalTime = (*iter).second;
	}
	qsort(pres, nbEntries, sizeof(sortResults_t), sortRes);

	for ( i = 0 ; i < nbEntries; i ++)
	{
		char tmps[512];
		int min = int(pres[i].totalTime)/60;
		int sec = (int(pres[i].totalTime)%60);
		int cent = (int(pres[i].totalTime*100)%100);
		sprintf_s(tmps, 512, ".%02d %d:%02d.%02d %s %s\n", i+1, min, sec, cent, pGame->GetNickName(pres[i].pShip), (pres[i].bestLap?"*":""));
		res += tmps;

	}

	delete [] pres;

	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////

int ZLapsTimes::GetShipRanking(ZShip *pShip) const
{
	std::map<ZShip*, float>::const_iterator iter;

	iter = mTotalTime.find(pShip);
	if ( iter == mTotalTime.end())
		return 999;

	int aPos = 1;
	float totTimeForShip = (*iter).second;
	iter = mTotalTime.begin();
	for (; iter != mTotalTime.end(); ++iter)
	{
		if ( (*iter).second < totTimeForShip)
			aPos ++;
	}
	return aPos;
}

//////////////////////////////////////////////////////////////////////////////////////////////

const std::vector<float>* ZLapsTimes::GetTimesForShip(ZShip* pShip) const
{
	std::map<ZShip*, std::vector<float> >::const_iterator iter = mTimes.find(pShip);
	if ( iter != mTimes.end())
		return & ((*iter).second);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////