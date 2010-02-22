#include "stdafx.h"
#include "ZExplosion.h"

BEGIN_SERIALIZATION(ZExplosion)
END_SERIALIZATION()

ZExplosion::ZExplosion()
{
	mExplosionLife = 0.f;
}

ZExplosion::~ZExplosion()
{
#ifdef _DEBUG	
	LOG("Explosion 0x%d destroyed.\n", this);
#endif
}

void ZExplosion::Tick(float timeEllapsed)
{
	mExplosionLife += timeEllapsed;
	if (mExplosionLife >= 5.f)
	{
		mManager->DestroyEntity(this);
	}
}