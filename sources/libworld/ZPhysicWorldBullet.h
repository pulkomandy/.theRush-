///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ZPHYSICWORLDBULLET_H__
#define ZPHYSICWORLDBULLET_H__

#include "../libbase/LibBase.h"
#include "../libbase/ZDatasResource.h"


#include "LinearMath/btVector3.h"
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btAlignedObjectArray.h"

#include "LinearMath/btMotionState.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btDefaultMotionState.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"

#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"


#if 0
#include "GL_ShapeDrawer.h"
#endif
//#include "../LibPlatform/ZDatasProcessor.h"

#include "../libbase/ZBaseMaths.h"
#include "..\libbase\ZSerializator.h"

#include <vector>
#include <map>


typedef struct PhysCollision_t
{
	ZBaseClass *mObject1, *mObject2; // user pointers
	tvector3 mWorldPosition;
	tvector3 mWorldNormal;
	float mDistance;
} PhysCollision_t;

typedef struct PhysRayCast_t
{
	tvector3 rayOrigin, rayEnd;
	bool bHasHit;
	tvector3 hitPosition, hitNormal;
	float t;
} PhysRayCast_t;


#define MAX_COLLISIONS 128
class ZPhysicWorldBullet;

///////////////////////////////////////////////////////////////////////////////////////////////////

class ClosestNotMe : public btCollisionWorld::ClosestRayResultCallback
{
public:
	ClosestNotMe (int aGroupFilter) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_collisionFilterGroup = aGroupFilter;// world + objects
		this->m_collisionFilterMask = aGroupFilter;
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
		return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace
	);
}
};

DECLAREZCLASS(ZPhysicCollisionMeshBullet);
DECLAREZCLASS(ZPhysicWorldBullet);


///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZRigidBodyBullet.h"
#include "ZPhysicTriMesh.h"
#include "ZPhysicTriMeshInstance.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZPhysicWorldBullet : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	
	ZPhysicWorldBullet()
	{
		mDynamicsWorld = NULL;
		mCollisionsCount[0] = 0;
		mCollisionsCount[1] = 0;
		mCollisionBufferFront = 0;
		mCollisionBufferBack = 1;
		mRayCastCS = NewCriticalSection();
	}
	virtual ~ZPhysicWorldBullet()
	{
		TickJobs();
		DestroyCriticalSection(mRayCastCS);
		
	}

	virtual void Init();
	virtual void Uninit()
	{
		if (!mDynamicsWorld)
			return;

		
		delete mDynamicsWorld;

		delete mSolver;

		delete mBroadphase;

		delete mDispatcher;

		delete mCollisionConfiguration;
		mDynamicsWorld = NULL;
	}

	virtual void AddRayCast(ZBaseClass *pOwner, PhysRayCast_t *pRays, int aRayCount);
	virtual void RemoveRayCast(ZBaseClass *pOwner);
	bool SyncRayCast(const tvector3& rayOrigin, const tvector3& rayEnd, tvector3* pHitPosition, tvector3* pHitNormal, float *pFraction, int aGroupFilter);

	ZRigidBody *CreateRigidBodySphere(float aRadius = 1.f, float aMass = 1.f);
	ZRigidBody *CreateRigidBodyShip(ZPhysicTriMesh * aCollisionMesh, float aMass = 1.f, ZBaseClass *pBaseClass = NULL);

	ZPhysicTriMeshInstance* AddPhysicModelToWorld(ZPhysicTriMesh * nMesh, ZBaseClass *pBase, unsigned int aFilterGroup = 1, unsigned int aFilterMask = 255, tmatrix *pMat = NULL)
	{
		ZPhysicTriMeshInstance *nTMInstance = (ZPhysicTriMeshInstance*)_New(ZPhysicTriMeshInstance);

		// job
		JobEntry_t entry;	
		entry.mType = NEW_COLLISION_MESH_IW;
		entry.mColMesh = nTMInstance;
		entry.filterMask = aFilterMask;
		entry.filterGroup = aFilterGroup;
		entry.baseClassUserPtr = pBase;
		nTMInstance->Init(this, nMesh, pMat);

		//nMesh->mPhysicWorld.Attach(this);
		//mJobs.push_back(entry);
		mJobs.ArrayToList(NULL, &entry, 1);

		return nTMInstance;
	}

	virtual void RemoveCollisionMesh(ZPhysicTriMeshInstance* pMesh)
	{
		JobEntry_t entry;
		entry.mType = DELETE_COLLISION_MESH_IW;
		entry.mColMesh = pMesh;
		pMesh->AddRef();
		mJobs.ArrayToList(NULL, &entry, 1);
	}
	
	// fills dest buffer and returns the actual number of collisions
	virtual PhysCollision_t * GetCollisionList(int &aNbCollisions);
	virtual void Clear();

	// framework
	void Tick(float timeEllapsed);

	void DebugDraw();

	void Delete(ZRigidBody *pModel);
	virtual tulong GetMemoryUsed() const 
	{ 
		return sizeof(ZPhysicWorldBullet) /*+ mCollisionMeshList.size()*sizeof(ZPhysicCollisionMeshBullet*)*/+
			mRayCasts.size()*(sizeof(ZBaseClass*) + sizeof(RayCastEntry_t));
	}
	void RemoveRigidBody(ZRigidBody *pRigidBody);
public:
	btDynamicsWorld*		mDynamicsWorld;
	btBroadphaseInterface*	mBroadphase;
	btCollisionDispatcher*	mDispatcher;
	btConstraintSolver*	mSolver;
	btDefaultCollisionConfiguration* mCollisionConfiguration;

	//std::list<ZPhysicCollisionMeshBullet*> mCollisionMeshList;

	enum PWJOB
	{
		NEW_COLLISION_MESH_IW,
		DELETE_COLLISION_MESH_IW,
		NEWMATRIX_COLLISION_MESH_IW,
		TOTALYDELETEMODEL_IW,

	};

	typedef struct JobEntry_t
	{
		JobEntry_t() : baseClassUserPtr(NULL)
		{
		}
		
		PWJOB mType;
		ZPhysicTriMeshInstance *mColMesh;
		int filterMask, filterGroup;
		ZBaseClass *baseClassUserPtr;

	}JobEntry_t ;

	ZDataResource<JobEntry_t> mJobs;
	//std::vector<JobEntry_t> mJobs;

	// collisions report
	PhysCollision_t mCollisions[2][MAX_COLLISIONS];
	int mCollisionsCount[2];
	int mCollisionBufferFront, mCollisionBufferBack;

	// Debug Draw
#if 0
	GL_ShapeDrawer	m_shapeDrawer;
#endif
	static void customNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo);

	// raycasts
	typedef struct RayCastEntry_t
	{
		RayCastEntry_t()
		{
		}

		RayCastEntry_t(PhysRayCast_t *_src, int _count)
		{
			pPtr = _src;
			mCount = _count;
		}
		PhysRayCast_t *pPtr;
		int mCount;
	}RayCastEntry_t;

	std::map<ZBaseClass*, RayCastEntry_t> mRayCasts;
	ZCriticalSection_t *mRayCastCS;

	void TickJobs();
};

#endif
