#include "stdafx.h"

BEGIN_SERIALIZATION(ZRigidBody)
END_SERIALIZATION()

ZRigidBody::~ZRigidBody()
{
	mPhysicWorld->RemoveRigidBody(this);//mDynamicsWorld->removeRigidBody(body);
}
