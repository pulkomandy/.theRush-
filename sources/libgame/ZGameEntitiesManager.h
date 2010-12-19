#ifndef ZGAMEENTITIESMANAGER_H__
#define ZGAMEENTITIESMANAGER_H__

#include "ZGameEntity.h"
#include "..\libbase\ZBaseMaths.h"

class ZMissile;
class ZMine;
class ZExplosion;

DECLAREZCLASS(ZMine);
DECLAREZCLASS(ZNetMine);

class ZGameEntitiesManager
{
public:
	ZGameEntitiesManager();
	virtual ~ZGameEntitiesManager();

	virtual ZMissile* SpawnMissile(const tmatrix& dropMatrix);
	virtual ZMine* SpawnMine(const tmatrix& dropMatrix);
	virtual ZExplosion* SpawnExplosion(const tmatrix& dropMatrix);

	void AddEntity(ZGameEntity *pEnt) { mGameEntities.push_back(pEnt); pEnt->mManager = this; }
	virtual void DestroyEntity(ZGameEntity *pEnt) { delete pEnt; }

	virtual void Tick(float timeEllapsed);
	
private:
	// Game Entities (missile, mines, explosion,...)
	std::list<ZGameEntity* > mGameEntities;
	friend class ZGameEntity;
	void RemoveEntity(ZGameEntity *pEnt) { mGameEntities.remove(pEnt); }
};

#endif
