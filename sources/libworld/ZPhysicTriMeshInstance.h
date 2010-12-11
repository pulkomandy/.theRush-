#ifndef ZPHYSICTRIMESHINSTANCE_H__
#define ZPHYSICTRIMESHINSTANCE_H__

#include "ZPhysicWorldBullet.h"

DECLAREZCLASS(ZPhysicTriMeshInstance);

class ZPhysicTriMeshInstance : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZPhysicTriMeshInstance()
	{
	}
	virtual ~ZPhysicTriMeshInstance()
	{
	}

	void Init(ZPhysicWorldBullet *aPhysicWorld, ZPhysicTriMesh *pMesh, tmatrix *pMat = NULL)
	{
		mPhysicTriMesh = pMesh;
		mPhysicWorld = aPhysicWorld;
		mObject.setCollisionShape(pMesh->mTrimesh);	
		if (!pMat)
		{
			mObject.getWorldTransform().setOrigin(btVector3(0,0,0));
			mObject.getWorldTransform().setRotation(btQuaternion(0,0,0,1));
		}
		else
		{
			mObject.getWorldTransform().setFromOpenGLMatrix(pMat->m16);
		}
	}

	void GetMatrix(tmatrix *pMat)
	{
		mObject.getWorldTransform().getOpenGLMatrix((float*)pMat->m16);
	}

	void SetMatrix(const tmatrix& aMat)
	{
		mObject.getWorldTransform().setFromOpenGLMatrix((float*)aMat.m16);
	}

	ZPhysicWorldBullet * GetWorld() const
	{
		return mPhysicWorld.ptr();
	}

	ZPhysicTriMesh *GetTriMesh() const
	{
		return mPhysicTriMesh.ptr();
	}
	virtual tulong GetMemoryUsed() const 
	{ 
		return sizeof(ZPhysicTriMeshInstance);
	}

protected:
	btCollisionObject mObject;
	smartptr<ZPhysicTriMesh> mPhysicTriMesh;
	smartptr<ZPhysicWorldBullet> mPhysicWorld;
	friend class ZPhysicWorldBullet;
};


#endif
