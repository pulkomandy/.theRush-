#ifndef ZRIGIDBODYBULLET_H__
#define ZRIGIDBODYBULLET_H__

#include "..\libbase\ZSmartPtr.h"

DECLAREZCLASS(ZRigidBody);

class ZRigidBody : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZRigidBody()
	{
	}
	virtual ~ZRigidBody();

	void GetMatrix(tmatrix* pMat) const
	{
		mBody->getWorldTransform().getOpenGLMatrix(pMat->m16);
	}
	void SetMatrix(const tmatrix& mat)
	{
		mBody->getWorldTransform().setFromOpenGLMatrix(mat.m16);
	}
	void SetGravity(const tvector3& aGrav)
	{
		mBody->setGravity(btVector3(aGrav.x, aGrav.y, aGrav.z));
	}
	void ApplyForce(const tvector3& aForce)
	{
		mBody->activate(true);
		mBody->applyCentralImpulse(btVector3(aForce.x, aForce.y, aForce.z));
	}
	void ApplyTorque(const tvector3&aForce)
	{
		mBody->activate(true);
		mBody->applyTorque(btVector3(aForce.x, aForce.y, aForce.z));
	}

	tquaternion GetOrientationQuaternion() const
	{
		btQuaternion quat = mBody->getWorldTransform().getRotation();
		return tquaternion(quat.x(), quat.y(), quat.z(), quat.w());
	}
	void SetOrientationQuaternion(const tquaternion &quat)
	{
		btQuaternion btquat(quat.x, quat.y, quat.z, quat.w);
		return mBody->getWorldTransform().setRotation(btquat);
	}

	tvector3 GetLinearVelocity() const
	{
		const btVector3& vel = mBody->getLinearVelocity();
		return tvector3(vel.getX(), vel.getY(), vel.getZ());
	}

	void SetLinearVelocity(const tvector3 &aVelocity)
	{
		mBody->setLinearVelocity(btVector3(aVelocity.x, aVelocity.y, aVelocity.z));
	}

	void SetAngularVelocity(const tvector3 &aAngularVelocity)
	{
		mBody->setAngularVelocity(btVector3(aAngularVelocity.x, aAngularVelocity.y, aAngularVelocity.z));
	}

	tvector3 GetAngularVelocity() const
	{
		const btVector3& vel = mBody->getAngularVelocity();
		return tvector3(vel.getX(), vel.getY(), vel.getZ());
	}

	void ResetForceAndTorque()
	{
		mBody->setAngularVelocity(btVector3(0,0,0));
		mBody->setLinearVelocity(btVector3(0,0,0));
	}
	void SetDamping(float aLinear, float aAngular)
	{
		mBody->setDamping(aLinear, aAngular);
	}

	tulong GetMemoryUsed() const 
	{ 
		return sizeof(ZRigidBody);
	}

	float GetMass() const
	{
		return (1.f/mBody->getInvMass());
	}
	float GetInvMass() const
	{
		return mBody->getInvMass();
	}
	btRigidBody* mBody;
	btCollisionShape *mCollisionShape;
	smartptr<ZPhysicWorldBullet> mPhysicWorld;
};

#endif
