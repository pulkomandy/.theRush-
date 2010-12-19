#include "ZGameEntitiesManager.h"
#include "ZMissile.h"
#include "ZMine.h"
#include "ZExplosion.h"

//////////////////////////////////////////////////////////////////////////////////////////////

ZGameEntitiesManager::ZGameEntitiesManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZGameEntitiesManager::~ZGameEntitiesManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////

void ZGameEntitiesManager::Tick(float timeEllapsed)
{

	ZGameEntity* aEntitiesToUpdate[128];
	int aEntitiesToUpdateIndex = 0;
	std::list<ZGameEntity* >::iterator iter = mGameEntities.begin();
	for (; iter != mGameEntities.end(); ++iter)
	{
		aEntitiesToUpdate[aEntitiesToUpdateIndex++] = (*iter);
		if (aEntitiesToUpdateIndex == 128)
			break;
	}

	for (int i=0;i<aEntitiesToUpdateIndex ; i ++)
	{
		aEntitiesToUpdate[i]->Tick(timeEllapsed);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZMissile* ZGameEntitiesManager::SpawnMissile(const tmatrix& dropMatrix)
{
	ZMissile * pMissile = (ZMissile*)_New( ZMissile );
	pMissile->Init( dropMatrix, true );
	AddEntity(pMissile);
	return pMissile;
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZMine* ZGameEntitiesManager::SpawnMine(const tmatrix& dropMatrix)
{
	ZMine * pMine = (ZMine*)_New( ZMine );
	pMine->Init( dropMatrix );
	AddEntity( pMine );
	return pMine;
}

//////////////////////////////////////////////////////////////////////////////////////////////

ZExplosion* ZGameEntitiesManager::SpawnExplosion(const tmatrix& dropMatrix)
{
	ZExplosion * pExplosion = (ZExplosion*)_New( ZExplosion );
	AddEntity( pExplosion );
	return pExplosion;
}

//////////////////////////////////////////////////////////////////////////////////////////////
