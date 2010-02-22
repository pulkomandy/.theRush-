#include "stdafx.h"
#include "ZGameEntity.h"

BEGIN_SERIALIZATION(ZGameEntity)
END_SERIALIZATION()


ZGameEntity::~ZGameEntity()
{
	mManager->mGameEntities.remove(this);
}