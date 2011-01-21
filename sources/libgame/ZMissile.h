#ifndef ZMISSILE_H__
#define ZMISSILE_H__

#include "ZGameEntity.h"

#include "../libbase/ZSmartPtr.h"
#include "../libworld/ZTransform.h"

DECLAREZCLASS(ZMissile);
DECLAREZCLASS(ZNetMissile);

class ZMissile : public ZGameEntity
{
public:
    DECLARE_SERIALIZABLE
public:
	ZMissile();
	virtual ~ZMissile();
	void Init(const tmatrix& mat, bool bComputeMissilePhysics);
	virtual void Tick(float timeEllapsed);
protected:
	float mMissileLife;
	bool mbComputeMissilePhysics;
	smartptr<ZTransform> mMissileTransform;
	int maCurIdx;
	tvector3 mGravityDirection;
	tvector3 mPos, mVel;
};

#endif
