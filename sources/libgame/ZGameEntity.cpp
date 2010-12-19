#include "ZGameEntity.h"
#include "ZGameEntitiesManager.h"

BEGIN_SERIALIZATION(ZGameEntity)
END_SERIALIZATION()


ZGameEntity::~ZGameEntity()
{
	mManager->mGameEntities.remove(this);
}
