#ifndef ZEXPLOSION_H__
#define ZEXPLOSION_H__

#include "ZGameEntity.h"

DECLAREZCLASS(ZExplosion);
DECLAREZCLASS(ZNetExplosion);

class ZExplosion : public ZGameEntity
{
public:
    DECLARE_SERIALIZABLE
public:
	ZExplosion();
	virtual ~ZExplosion();
	virtual void Tick(float timeEllapsed);
protected:
	float mExplosionLife;
};

#endif