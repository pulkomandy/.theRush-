
#ifndef ZRUSHGAMECONTENT_H__
#define ZRUSHGAMECONTENT_H__

DECLAREZCLASS(ZRushGameContent);

class ZRushGameContent : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRushGameContent();
	virtual ~ZRushGameContent();

	/*
		loading ships, persistant common files,..
		Done @ game init time
	*/
	void LoadPersistantDatas();
	/*
		
	*/
	const std::vector<tstring>& GetTeams();
	const std::vector<tstring>& GetWorlds();
	const std::vector<tstring>& GetShipsNames(const tstring& sTeam);
	const std::vector<tstring>& GetTracksNames(const tstring& sWorld);


	/* 
		Get Standard resource from menu, bonus,...
	*/
	ZBaseClass * GetRes(const tstring& sResName);

	/*
		Get Ship
	*/
	ZTransform * GetShip(const tstring& sShipName);
	ZTransform * GetRandomShip();
	const tstring& GetRandomShipName();
	const tstring& GetShip(int teamIdx, int shipIdx);
	/*
		Load a map:
		Decors , brick and .track
	*/
	void LoadWorld(const tstring& sWorldName);
	void LoadMap(const tstring& sWorldName, const tstring& sMapName);
	const char* GetShipDiskPath(const tstring& sShipName);
	ZGameResources* GetWorldBricksRes() const { return mMapBricks.ptr(); }

	// musics
	const std::vector<tstring>& GetMusics() const { return mMusicsFileNames; }

	// World Bricks
	int GetWorldBricksCount() const { return mMapBricks->GetUnamedRes().size(); }
	const char* GetBrickName(int idx) const 
	{ 
		if (idx <0) 
			return NULL;
		return mMapBricks->GetUnamedRes()[idx]->GetName(); 
	}

protected:
	std::vector<tstring> mMusicsFileNames;
	std::vector<tstring> mTeams;
	std::vector<tstring> mWorlds;
	std::map<tstring, std::vector<tstring> > mTeamsShipsNames;
	std::map<tstring, std::vector<tstring> > mWorldsTracksNames;
	smartptr<ZGameResources> mCommonRes;
	smartptr<ZGameResources> mShips;
	smartptr<ZGameResources> mDecors;
	smartptr<ZGameResources> mMapBricks;
	tstring mCurrentWorld, mCurrentMap;
	std::map<tstring, ZTransform* > mNamedShips;
	std::map<tstring, tstring> mNamedShipsDiskPath;
	ZCriticalSection_t* mCSLoadMap;

	bool mbBaseDatasLoaded;
	void LoadBaseDatas();
};

extern ZRushGameContent *gRushGameContent;
#endif
