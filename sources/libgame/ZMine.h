#ifndef ZMINE_H__
#define ZMINE_H__

#include "ZGameEntity.h"

DECLAREZCLASS(ZMine);
DECLAREZCLASS(ZNetMine);

class ZMine : public ZGameEntity
{
public:
    DECLARE_SERIALIZABLE
public:
	ZMine();
	virtual ~ZMine();
	void Init(const tmatrix& mat);
	virtual void Tick(float timeEllapsed);
protected:
	float mMineLife;
	smartptr<ZTransform> mMineTransform;
};

#endif