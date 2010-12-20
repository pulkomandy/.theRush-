#include "ZPhysicTriMesh.h"

#include <BulletCollision/CollisionShapes/btConcaveShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/GImpact/btGImpactShape.h>

BEGIN_SERIALIZATION(ZPhysicTriMesh)
END_SERIALIZATION()


tulong ZPhysicTriMesh::GetMemoryUsed() const 
{ 
	return sizeof(ZPhysicTriMesh) + sizeof(btConcaveShape) + (sizeof(int)*mNbIndices);
}


ZPhysicTriMesh* ZPhysicTriMesh::Build( ZMesh *pMesh )
{
	if (!pMesh->GetIndexBuffer()) 
		return NULL;




	ZPhysicTriMesh *nMesh = (ZPhysicTriMesh *)_New(ZPhysicTriMesh);
	nMesh->mMesh = pMesh;

	IVertexArray *pva = pMesh->GetVertexBuffer();
	IIndexArray *pia = pMesh->GetIndexBuffer();

	unsigned short *pus = (unsigned short*)pia->Lock(VAL_READONLY);
	int * mIndices = new int [pia->GetIndexCount()];
	for (unsigned int i=0;i<pia->GetIndexCount();i+=3)
	{
		mIndices[i] = pus[i];
		mIndices[i+1] = pus[i+1];
		mIndices[i+2] = pus[i+2];
	}
	nMesh->mIndices = mIndices;
	nMesh->mNbIndices = pia->GetIndexCount();

	
	
	// build trimesh shape
	btTriangleIndexVertexArray* indexVertexArrays = new btTriangleIndexVertexArray(pia->GetIndexCount()/3,
		mIndices,
		3*sizeof(int),
		pva->GetVertexCount(), (float*)pva->Lock(VAL_READONLY), pva->GetVertexSize());
	
	pia->Unlock();
	pva->Unlock();



	btConcaveShape *trimesh; 
	/*btGImpactMeshShape **/ trimesh = new btGImpactMeshShape(indexVertexArrays);
							((btGImpactMeshShape*)trimesh)->updateBound();
							//trimesh = new btBvhTriangleMeshShape(indexVertexArrays );

	

	nMesh->mTrimesh = trimesh;
	/*
	nMesh->mObject.setCollisionShape(trimesh);
	nMesh->mObject.getWorldTransform().setOrigin(btVector3(initialPos.x, initialPos.y, initialPos.z));
	nMesh->mObject.getWorldTransform().setRotation(btQuaternion(0,0,0,1));
*/

	return nMesh;
}

