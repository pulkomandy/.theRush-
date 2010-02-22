///////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "ZPhysicWorldBullet.h"
#include "../libgame/libgame.h"
#include "../libplatform/libplatform.h"

BEGIN_SERIALIZATION(ZPhysicWorldBullet)
END_SERIALIZATION()


DECLAREZCLASS(ZShip);
DECLAREZCLASS(ZNetShip);

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZPhysicWorldBullet::customNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo)
{
	btCollisionObject* colObj0 = (btCollisionObject*)collisionPair.m_pProxy0->m_clientObject;
	btCollisionObject* colObj1 = (btCollisionObject*)collisionPair.m_pProxy1->m_clientObject;
	if (dispatcher.needsCollision(colObj0,colObj1))
	{
		//dispatcher will keep algorithms persistent in the collision pair
		if (!collisionPair.m_algorithm)
		{
			collisionPair.m_algorithm = dispatcher.findAlgorithm(colObj0,colObj1);
		}

		if (collisionPair.m_algorithm)
		{
			btManifoldResult contactPointResult(colObj0,colObj1);

			if (dispatchInfo.m_dispatchFunc == 		btDispatcherInfo::DISPATCH_DISCRETE)
			{
				//discrete collision detection query
				collisionPair.m_algorithm->processCollision(colObj0,colObj1,dispatchInfo,&contactPointResult);
			} else
			{
				//continuous collision detection query, time of impact (toi)
				float toi = collisionPair.m_algorithm->calculateTimeOfImpact(colObj0,colObj1,dispatchInfo,&contactPointResult);
				if (dispatchInfo.m_timeOfImpact > toi)
					dispatchInfo.m_timeOfImpact = toi;

			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PhysCollision_t * ZPhysicWorldBullet::GetCollisionList(int &aNbCollisions)
{
	aNbCollisions = mCollisionsCount[mCollisionBufferBack];
	return &mCollisions[mCollisionBufferFront][0];
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZPhysicWorldBullet::Clear()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// framework
void ZPhysicWorldBullet::TickJobs()
{
	PROFILER_START(TickJobs);
	JobEntry_t aJobs[128];
	unsigned int nbjobs = mJobs.ListToArray(NULL, &aJobs[0], 128);
	for (unsigned int i = 0 ; i < nbjobs ; i ++)
	{
		JobEntry_t *pj = &aJobs[i];
		switch (pj->mType)
		{
		case NEW_COLLISION_MESH_IW:
			mDynamicsWorld->addCollisionObject(&pj->mColMesh->mObject, pj->filterGroup, pj->filterMask);
			pj->mColMesh->mObject.setUserPointer(pj->baseClassUserPtr);
			break;
		case DELETE_COLLISION_MESH_IW:
			mDynamicsWorld->removeCollisionObject(&pj->mColMesh->mObject);
//			mCollisionMeshList.remove(pj->mColMesh);
			delete pj->mColMesh;
			break;
			/*
		case NEWMATRIX_COLLISION_MESH_IW:
			pj->mColMesh->mObject.getWorldTransform().setFromOpenGLMatrix((btScalar*)&pj->mColMesh->mMatrix);
			break;
			*/
		case TOTALYDELETEMODEL_IW:
			mDynamicsWorld->removeCollisionObject(&pj->mColMesh->mObject);
//			mCollisionMeshList.remove(pj->mColMesh);
			delete pj->mColMesh;
			break;
		}
	}
	mJobs.Clear();
	PROFILER_END();
}
void ZPhysicWorldBullet::Tick(float timeEllapsed)
{
	PROFILER_START(PhysicTick);

	if (!mDynamicsWorld)
		return;
	// do job list
	TickJobs();
	
	///step the simulation
	PROFILER_START(SimulationStep);
	mDynamicsWorld->stepSimulation(timeEllapsed);
	PROFILER_END();

	// get collisions
	PROFILER_START(CollisionsAcquire);
	int &acc = mCollisionsCount[mCollisionBufferBack];
	acc = 0;
	int numManifolds = mDynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold = mDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
	
		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			ZBaseClass *mUserPtr1 = (ZBaseClass *)obA->getUserPointer();
			ZBaseClass *mUserPtr2 = (ZBaseClass *)obB->getUserPointer();

			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			if (GGame)
			{
				if (mUserPtr1 && ( (mUserPtr1->GetClassID() == ClassIDZShip) || (mUserPtr1->GetClassID() == ClassIDZNetShip) ) )
				{
					ZRushGame *pGame = (ZRushGame *)GGame->GetServerGame();
					if (pGame) pGame->HandleShipCollision( (ZShip*)mUserPtr1, mUserPtr2, pt.getDistance());
					pGame = (ZRushGame *)GGame->GetClientGame();
					if (pGame) pGame->HandleShipCollision( (ZShip*)mUserPtr1, mUserPtr2, pt.getDistance());
				}
				if (mUserPtr2 && ( (mUserPtr2->GetClassID() == ClassIDZShip) || (mUserPtr2->GetClassID() == ClassIDZNetShip) ) )
				{
					ZRushGame *pGame = (ZRushGame *)GGame->GetServerGame();
					if (pGame) pGame->HandleShipCollision( (ZShip*)mUserPtr2, mUserPtr1, pt.getDistance());
					pGame = (ZRushGame *)GGame->GetClientGame();
					if (pGame) pGame->HandleShipCollision( (ZShip*)mUserPtr2, mUserPtr1, pt.getDistance());
				}
			}

			if (acc<MAX_COLLISIONS)
			{
				PhysCollision_t *pc = &mCollisions[mCollisionBufferBack][acc];
				pc->mDistance = pt.m_distance1;
				pc->mObject1 = mUserPtr1;
				pc->mObject2 = mUserPtr2;

				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptN = pt.m_normalWorldOnB;

				pc->mWorldPosition = tvector3(ptA.x(), ptA.y(), ptA.z());
				pc->mWorldNormal = tvector3(ptN.x(), ptN.y(), ptN.z());

				acc ++;
			}
		}

		//you can un-comment out this line, and then all points are removed
		//contactManifold->clearManifold();	
	}

	++mCollisionBufferBack&=1;
	++mCollisionBufferFront&=1;
	PROFILER_END();

	// raycasts
#if 0
	LockCriticalSection(mRayCastCS);
	std::map<ZBaseClass*, RayCastEntry_t>::iterator iter = mRayCasts.begin();
	for (; iter != mRayCasts.end(); ++iter)
	{
		RayCastEntry_t & pr = (*iter).second;
		
		
		for (int i=0;i<pr.mCount;i++)
		{
			const tvector3 & rayo = pr.pPtr[i].rayOrigin;
			const tvector3 & rayd = pr.pPtr[i].rayEnd;
			
			ClosestNotMe rayCB;

			mDynamicsWorld->rayTest(btVector3(rayo.x, rayo.y, rayo.z), 
				btVector3(rayd.x, rayd.y, rayd.z), rayCB);
			if (rayCB.hasHit())
			{
				const btVector3& hn = rayCB.m_hitNormalWorld;
				const btVector3& hp = rayCB.m_hitPointWorld;
				pr.pPtr[i].hitNormal = tvector3(hn.x(), hn.y(), hn.z());
				pr.pPtr[i].hitPosition =  rayo;
				pr.pPtr[i].hitPosition += ((rayd-rayo)*rayCB.m_closestHitFraction);//tvector3(hp.x(), hp.y(), hp.z());
				pr.pPtr[i].bHasHit = true;
				pr.pPtr[i].t = rayCB.m_closestHitFraction;
			}
			else
				pr.pPtr[i].bHasHit = false;
				
		}
	}
	mRayCasts.clear();
	UnLockCriticalSection(mRayCastCS);
#endif
	PROFILER_END();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZPhysicCollisionMeshBullet::SetMatrix(const tmatrix& mat)
{
	mMatrix = mat;
	ZPhysicWorldBullet::JobEntry_t entry;
	entry.mType = ZPhysicWorldBullet::NEWMATRIX_COLLISION_MESH_IW;
	entry.mColMesh = this;
	//GBullet->mJobs.push_back(entry);
	if (mPhysicWorld)
		mPhysicWorld->mJobs.ArrayToList(NULL, &entry, 1);

}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZPhysicWorldBullet::AddRayCast(ZBaseClass *pOwner, PhysRayCast_t *pRays, int aRayCount)
{
	LockCriticalSection(mRayCastCS);
	mRayCasts[pOwner] = RayCastEntry_t(pRays, aRayCount);
	UnLockCriticalSection(mRayCastCS);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZPhysicWorldBullet::RemoveRayCast(ZBaseClass *pOwner)
{
	LockCriticalSection(mRayCastCS);
	std::map<ZBaseClass*, RayCastEntry_t>::iterator iter = mRayCasts.find(pOwner);
	if (iter != mRayCasts.end())
		mRayCasts.erase(iter);
	UnLockCriticalSection(mRayCastCS);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZPhysicWorldBullet::Delete(IPhysicModel *pModel)
{
	// only ZPhysicCollisionMeshBullet supported
	JobEntry_t entry;
	entry.mType = TOTALYDELETEMODEL_IW;
	entry.mColMesh = (ZPhysicCollisionMeshBullet*)pModel;
	//mJobs.push_back(entry);
	mJobs.ArrayToList(NULL, &entry, 1);
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////


ZRigidBody *ZPhysicWorldBullet::CreateRigidBodySphere(float aRadius, float aMass)
{
	ZRigidBody *nMesh = (ZRigidBody *)_New(ZRigidBody);
	nMesh->mPhysicWorld.Attach(this);

	float mass = aMass;
	btTransform startTransform;
	startTransform.setIdentity();
	btVector3 camPos(0,0,0);
	startTransform.setOrigin(camPos);

	btCollisionShape* colShape = new btSphereShape(1.f);

	bool isDynamic = (aMass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(aMass,localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);

	body->setCcdMotionThreshold(1.);
	body->setCcdSweptSphereRadius(0.2f);
	body->setDamping(0.99f,1.f);
	body->setGravity(btVector3(0,0,0));

	nMesh->mBody = body;
	nMesh->mCollisionShape = colShape;


	return nMesh;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZPhysicWorldBullet::SyncRayCast(const tvector3& rayOrigin, const tvector3& rayEnd, tvector3* pHitPosition, tvector3* pHitNormal, float *pFraction, int aGroupFilter)
{
	PROFILER_START(SyncRayCast);

	const tvector3 & rayo = rayOrigin;
	const tvector3 & rayd = rayEnd;

	ClosestNotMe rayCB(aGroupFilter);

	mDynamicsWorld->rayTest(btVector3(rayo.x, rayo.y, rayo.z), 
		btVector3(rayd.x, rayd.y, rayd.z), rayCB);

	PROFILER_END();

	if (rayCB.hasHit())
	{
		const btVector3& hn = rayCB.m_hitNormalWorld;
		const btVector3& hp = rayCB.m_hitPointWorld;
		*pHitNormal = tvector3(hn.x(), hn.y(), hn.z());
		*pHitPosition =  rayo;
		(*pHitPosition) += ((rayd-rayo)*rayCB.m_closestHitFraction);
		(*pFraction) = rayCB.m_closestHitFraction;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZPhysicWorldBullet::DebugDraw()
{
#if 0
	mDynamicsWorld->debugDrawWorld();

	btScalar m[16];

	glMatrixMode(GL_MODELVIEW);
	glDepthFunc(GL_LEQUAL);
	if (mDynamicsWorld)
	{
		btVector3	worldBoundsMin,worldBoundsMax;
		mDynamicsWorld->getBroadphase()->getBroadphaseAabb(worldBoundsMin,worldBoundsMax);


		int numObjects = mDynamicsWorld->getNumCollisionObjects();
		btVector3 wireColor(1,0,0);
		for (int i=0;i<numObjects;i++)
		{
			btCollisionObject* colObj = mDynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(colObj);

			if (body && body->getMotionState())
			{
				btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
				myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
			} else
			{
				colObj->getWorldTransform().getOpenGLMatrix(m);
			}

			btVector3 wireColor(1.f,1.0f,0.5f); //wants deactivation
			if (i & 1)
			{
				wireColor = btVector3(0.f,0.0f,1.f);
			}
			///color differently for active, sleeping, wantsdeactivation states
			if (colObj->getActivationState() == 1) //active
			{
				if (i & 1)
				{
					wireColor += btVector3 (1.f,0.f,0.f);
				} else
				{
					wireColor += btVector3 (.5f,0.f,0.f);
				}
			}
			if (colObj->getActivationState() == 2) //ISLAND_SLEEPING
			{
				if (i & 1)
				{
					wireColor += btVector3 (0.f,1.f, 0.f);
				} else
				{
					wireColor += btVector3 (0.f,0.5f,0.f);
				}
			}

			m_shapeDrawer.enableTexture(true);
			m_shapeDrawer.drawOpenGL(m,colObj->getCollisionShape(),wireColor,false/*getDebugMode()*/,worldBoundsMin,worldBoundsMax);
		}
	}
#endif
}
ZRigidBody *ZPhysicWorldBullet::CreateRigidBodyShip(ZPhysicTriMesh * aCollisionMesh, float aMass, ZBaseClass *pBaseClass )
{
	ZRigidBody *nMesh = (ZRigidBody *)_New(ZRigidBody);
	nMesh->mPhysicWorld.Attach(this);
	
	float mass = aMass;
	btTransform startTransform;
	startTransform.setIdentity();
	btVector3 camPos(0,0,0);// = getCameraPosition();
	startTransform.setOrigin(camPos);

	//

	btCollisionShape* colShape = aCollisionMesh->mTrimesh;
	bool isDynamic = (aMass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(aMass,localInertia);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	mDynamicsWorld->addRigidBody(body);
	

	body->setCcdMotionThreshold(1.);
	body->setCcdSweptSphereRadius(0.2f);
	body->setDamping(0.99f,0.9999f);

	body->setCollisionFlags(body->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	nMesh->mBody = body;
	nMesh->mCollisionShape = colShape;

	nMesh->mBody->setUserPointer(pBaseClass);
	return nMesh;
}



static bool CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
{
    /*
    // Apply material properties
    if (colObj0->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
    {
        const btCollisionShape* parent0 = colObj0->getRootCollisionShape();
        if(parent0 != 0 && parent0->getShapeType() == MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE)
        {
            btMultimaterialTriangleMeshShape* shape = (btMultimaterialTriangleMeshShape*)parent0;
            const btMaterial * props = shape->getMaterialProperties(partId0, index0);
            cp.m_combinedFriction = calculateCombinedFriction(props->m_friction, colObj1->getFriction());
            cp.m_combinedRestitution = props->m_restitution * colObj1->getRestitution();
        }
    }
    else if (colObj1->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE)
    {
        const btCollisionShape* parent1 = colObj1->getRootCollisionShape();
        if(parent1 != 0 && parent1->getShapeType() == MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE)
        {
            btMultimaterialTriangleMeshShape* shape = (btMultimaterialTriangleMeshShape*)parent1;
            const btMaterial * props = shape->getMaterialProperties(partId1, index1);
            cp.m_combinedFriction = calculateCombinedFriction(props->m_friction, colObj0->getFriction());
            cp.m_combinedRestitution = props->m_restitution * colObj0->getRestitution();
        }
    }
*/
	ZBaseClass *pObj1 = (ZBaseClass*)colObj0->getUserPointer();
	ZBaseClass *pObj2 = (ZBaseClass*)colObj1->getUserPointer();


	// 0 distance only for ship/wall
	if ( (!pObj1) || (!pObj2))
		cp.m_distance1 = 0;

	//LOG("Custom Material\n");
    //this return value is currently ignored, but to be on the safe side: return false if you don't calculate friction
    return true;
}

extern ContactAddedCallback		gContactAddedCallback;


void ZPhysicWorldBullet::Init()
{
	if (mDynamicsWorld)
		return;

	gContactAddedCallback = CustomMaterialCombinerCallback;

	///collision configuration contains default setup for memory, collision setup
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new	btCollisionDispatcher(mCollisionConfiguration);

	mBroadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
	mDynamicsWorld->setGravity(btVector3(0,0,0));

	//btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(m_dynamicsWorld ->getDispatcher());
	btGImpactCollisionAlgorithm::registerAlgorithm(mDispatcher);
	mDispatcher->setNearCallback(customNearCallback);

}


void ZPhysicWorldBullet::RemoveRigidBody(ZRigidBody *pRigidBody)
{
	mDynamicsWorld->removeRigidBody(pRigidBody->mBody);
}

void ZPhysicWorldBullet::Delete(ZRigidBody *pModel)
{
}