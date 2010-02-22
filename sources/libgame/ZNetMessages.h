#ifndef ZNETMESSAGES_H__
#define ZNETMESSAGES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
enum ZNETMESSAGE
{
	ZNMSG_ASKSLOT  = ID_USER_PACKET_ENUM,
	ZNMSG_USERINPUT,
	ZNMSG_SERVERMAPLOADINGDONE,
	ZNMSG_CLIENTMAPLOADINGDONE,
	ZNMSG_GAMESET,
	ZNMSG_TIMEDTEST,
	ZNMSG_SHIPSYNC,

	ZNMSG_RACESTARTSINNSECONDS,
	ZNMSG_COUNTDOWNSTARTS,
	ZNMSG_SLOTSAVAILABLE,
	ZNMSG_RACEENDSINNSECONDS,
	ZNMSG_NEXTMAPINNSECONDS,
	ZNMSG_SHIPLAPTIME,
	ZNMSG_GAMELAPSTIMES,
	ZNMSG_USEBONUS
};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ZNMSG_AskSlot
{
	ZNMSG_AskSlot()
	{
	}

}ZNMSG_AskSlot;

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ZNMSG_ServerMapLoadingDone
{
	ZNMSG_ServerMapLoadingDone()
	{
	}

}ZNMSG_ServerMapLoadingDone;

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ZNMSG_UserInput
{
	ZNMSG_UserInput()
	{
	}

	keysStruct mInput;
}ZNMSG_UserInput;

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ZNMSG_GameSet
{
	ZNMSG_GameSet()
	{
	}

	ZNMSG_GameSet(const GameSet_t& gameSet)
	{
		mGameSet = gameSet;
	}

	GameSet_t mGameSet;
} ZNMSG_GameSet;

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ZNMSG_TimedTest
{
	RakNetTime mTime;
}ZNMSG_TimedTest;

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ZNMSG_ShipSync
{
	ZNMSG_ShipSync()
	{
	}

	ZNMSG_ShipSync(const ShipSync_t& ss, const keysStruct& ks)
	{
		mSync = ss;
		mInput = ks;
	}
	ShipSync_t mSync;
	keysStruct mInput;
} ZNMSG_ShipSync;

typedef struct ZNMSG_ClientMapLoadingDone
{
} ZNMSG_ClientMapLoadingDone;


//	ZNMSG_RACESTARTSINNSECONDS
typedef struct ZNMSG_RaceStartsInNSeconds
{
	ZNMSG_RaceStartsInNSeconds() {}
	ZNMSG_RaceStartsInNSeconds(float aSeconds)
	{
		mSecondsBeforeRaceStarts = aSeconds;
	}
	float mSecondsBeforeRaceStarts;
}ZNMSG_RaceStartsInNSeconds;

//	ZNMSG_COUNTDOWNSTARTS
typedef struct ZNMSG_CountDownStarts
{
}ZNMSG_CountDownStarts;

//	ZNMSG_SLOTSAVAILABLE
typedef struct ZNMSG_SlotsAvailable
{
	ZNMSG_SlotsAvailable() {}
	ZNMSG_SlotsAvailable(int aSlots)
	{
		mNbSlotsAvailable = aSlots;
	}
	int mNbSlotsAvailable;
}ZNMSG_SlotsAvailable;
	
// ZNMSG_RACEENDSINNSECONDS
typedef struct ZNMSG_RaceEndsInNSeconds
{
	ZNMSG_RaceEndsInNSeconds() {}
	ZNMSG_RaceEndsInNSeconds(float aSeconds)
	{
		mSecondsBeforeRaceEnds = aSeconds;
	}
	float mSecondsBeforeRaceEnds;
}ZNMSG_RaceEndsInNSeconds;

//ZNMSG_NEXTMAPINNSECONDS	
typedef struct ZNMSG_NextMapInNSeconds
{
	ZNMSG_NextMapInNSeconds() {}
	ZNMSG_NextMapInNSeconds(float aSeconds)
	{
		mSecondsBeforeNextMap = aSeconds;
	}
	float mSecondsBeforeNextMap;
}ZNMSG_NextMapInNSeconds;

//ZNMSG_SHIPLAPTIME
typedef struct ZNMSG_ShipLapTime
{
	ZNMSG_ShipLapTime() {}
	ZNMSG_ShipLapTime(float aSeconds)
	{
		mShipLapTime = aSeconds;
	}
	float mShipLapTime;
}ZNMSG_ShipLapTime;

// Bonus
typedef struct ZNMSG_UseBonus
{
	ZNMSG_UseBonus()
	{
	}
}ZNMSG_UseBonus;

///////////////////////////////////////////////////////////////////////////////////////////////////
#define VIRTUALZNETMESSAGE(x,y) virtual void SendNetMessage(const x&msg, bool bBroadcast = true) {}\
	virtual void SendNetMessageFast(const x&msg, bool bBroadcast = true) {}\
	virtual void SendNetMessage(const SystemAddress& addr, const x&msg) {}

#define ZNETMESSAGE(x,y) virtual void SendNetMessage(const x&msg, bool bBroadcast = true)\
{\
	if (!rakInterface) \
	{\
		WARN("Can't send message for "QUOTE(y));\
		return;\
	}\
	BitStream bitStream;\
	unsigned char msgid = y;\
	bitStream.Write(msgid);\
	if (sizeof(x) >0) \
		bitStream.Write(msg);\
	rakInterface->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, bBroadcast);\
}\
virtual void SendNetMessageFast(const x&msg, bool bBroadcast = true)\
{\
	if (!rakInterface) \
	{\
		WARN("Can't send message for "QUOTE(y));\
		return;\
	}\
	BitStream bitStream;\
	unsigned char msgid = y;\
	bitStream.Write(msgid);\
	if (sizeof(x) >0) \
		bitStream.Write(msg);\
	rakInterface->Send(&bitStream, LOW_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, bBroadcast);\
}\
virtual void SendNetMessage(const SystemAddress& addr, const x&msg)\
{\
	if (!rakInterface) \
	{\
		WARN("Can't send message for "QUOTE(y));\
		return;\
	}\
	BitStream bitStream;\
	unsigned char msgid = y;\
	bitStream.Write(msgid);\
	if (sizeof(x) >0) \
		bitStream.Write(msg);\
	rakInterface->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, addr, false);\
}

#endif