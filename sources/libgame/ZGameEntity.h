#ifndef ZGAMEENTITY_H__
#define ZGAMEENTITY_H__

#include "../libbase/ZBaseClass.h"
#include "../libbase/ZSerializator.h"

class ZGameEntitiesManager;

class ZGameEntity : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZGameEntity()
	{
	}
	virtual ~ZGameEntity();

	virtual void BroadcastDestruction() {}
	virtual void Tick(float /*timeEllapsed*/) {}

	ZGameEntitiesManager *mManager;
};

#endif
