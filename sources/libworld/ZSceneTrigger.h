#ifndef ZSCENETRIGGER_H__
#define ZSCENETRIGGER_H__

#include "ZTrigger.h"
#include "../libbase/ZProfiler.h"
#include "../libbase/ZCollisionsUtils.h"

class ZSceneTrigger
{
public:
	ZSceneTrigger()
	{
	}
	virtual ~ZSceneTrigger()
	{
	}
	void RegisterTriggerCallback(unsigned int aGroup1, unsigned int aGroup2, ZTriggerCallback* pcb)
	{
		mCallBacks[aGroup1][aGroup2] = pcb;
		mCallBacks[aGroup2][aGroup1] = pcb;
	}

	typedef struct TriggerCheckPeer_t
	{
		TriggerCheckPeer_t(ZTrigger *aTrig1, ZTrigger *aTrig2)
		{
			mTrig1 = aTrig1;
			mTrig2 = aTrig2;
		}
		bool operator == (const TriggerCheckPeer_t& other)
		{
			return ( (other.mTrig1 == mTrig1) && (other.mTrig2 == mTrig2) );
		}
		ZTrigger *mTrig1, *mTrig2;
	} TriggerCheckPeer_t;

	void CheckTriggers(std::vector<TriggerCheckPeer_t>* aTriggerPeers)
	{
		PROFILER_START(CheckTriggers);
		// sort triggers
		qsort( &(*aTriggerPeers)[0], aTriggerPeers->size(), sizeof(TriggerCheckPeer_t), SortTriggerPeers);
		// handle peers
		for ( unsigned int i = 0; i < aTriggerPeers->size() ; i++ )
		{
			if ( ( i > 0 ) && ( (*aTriggerPeers)[i] == (*aTriggerPeers)[i-1]))
				continue;
			ZTrigger *trig1 = (*aTriggerPeers)[i].mTrig1;
			ZTrigger *trig2 = (*aTriggerPeers)[i].mTrig2;

			if ( (trig1->mTriggerType == ZTrigger::TRIGGER_BOX) && (trig2->mTriggerType == ZTrigger::TRIGGER_SPHERE) )
			{
				CheckTriggerBoxTriggerSphere(trig1, trig2);
			}
			else if ( (trig2->mTriggerType == ZTrigger::TRIGGER_BOX) && (trig1->mTriggerType == ZTrigger::TRIGGER_SPHERE) )
			{
				CheckTriggerBoxTriggerSphere(trig2, trig1);
			}
			else if ( (trig1->mTriggerType == ZTrigger::TRIGGER_SPHERE) && (trig2->mTriggerType == ZTrigger::TRIGGER_SPHERE) )
			{
				CheckTriggerSphereTriggerSphere(trig1, trig2);
			}
		}
		PROFILER_END();
	}

	void UpdateTriggerDependanciesList(ZTrigger *pTrig, std::vector<ZSceneTrigger::TriggerCheckPeer_t> *mTriggerPeers)
	{
		// add it to dirty trigger list
		//
		if (pTrig->IsActive())
		{
			ZCluster *pTriggerCluster = pTrig->GetCluster();
			if ( pTriggerCluster )
			{
				int aTriggersNb = 0;
				ZTrigger *aTriggersList[128];
				GetActiveTriggersRelatedToClusterDownSide( pTriggerCluster, aTriggersList, &aTriggersNb );
				if ( pTriggerCluster->mParentCluster )
					GetActiveTriggersRelatedToClusterUpSide( pTriggerCluster->mParentCluster, aTriggersList, &aTriggersNb );

				if (aTriggersNb)
				{
					for (int lott = 0; lott<aTriggersNb; lott++)
					{
						if ( aTriggersList[lott] != pTrig )
						{
							if ( ( pTrig->mFilterGroup&aTriggersList[lott]->mFilterMask ) &&
								( aTriggersList[lott]->mFilterGroup&pTrig->mFilterMask ) )
							{
								// should check group&filter here
								if (pTrig < aTriggersList[lott]) {
									mTriggerPeers->push_back( TriggerCheckPeer_t( pTrig, aTriggersList[lott]));
								} else {
									mTriggerPeers->push_back( TriggerCheckPeer_t( aTriggersList[lott], pTrig));
								}
							}
						}
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

protected:

	std::map<unsigned int, std::map<unsigned int, ZTriggerCallback*> > mCallBacks;

	ZTriggerCallback* FindCallback(unsigned int aGroup1, unsigned int aGroup2)
	{
		std::map<unsigned int, std::map<unsigned int, ZTriggerCallback*> >::iterator iter = mCallBacks.find(aGroup1);
		if (iter != mCallBacks.end())
		{
			std::map<unsigned int, ZTriggerCallback*>::iterator iter2 = (*iter).second.find(aGroup2);
			if (iter2 != (*iter).second.end())
			{
				return (*iter2).second;
			}
		}
		return NULL;
	}
	void CheckTriggerBoxTriggerSphere(ZTrigger *pTrigBox, ZTrigger *pTrigSphere)
	{
		tmatrix mt;
		mt.Inverse( pTrigBox->GetWorldMatrix() );
		tvector3 sphPos = pTrigSphere->GetBVolume().GetBSphere();
		sphPos.TransformPoint(mt);

		if (AABBOverlapsSphere ( &pTrigBox->GetOriginVolume().mBoxMin.x, 
			&pTrigBox->GetOriginVolume().mBoxMax.x, pTrigSphere->GetOriginVolume().GetBSphere().w, &sphPos.x ))
		{
			ZTriggerCallback *pcb = FindCallback(pTrigBox->mFilterGroup, pTrigSphere->mFilterGroup);
			if (pcb)
				pcb->CallBack(pTrigBox, pTrigSphere);
		}
	}

	void CheckTriggerSphereTriggerSphere(ZTrigger *pTrigSphere1, ZTrigger *pTrigSphere2)
	{
		if (SphereOverlapsSphere (pTrigSphere1->GetBVolume().GetBSphere(), 
			pTrigSphere2->GetBVolume().GetBSphere()))
		{
			ZTriggerCallback *pcb = FindCallback(pTrigSphere1->mFilterGroup, pTrigSphere2->mFilterGroup);
			if (pcb)
				pcb->CallBack(pTrigSphere1, pTrigSphere2);
		}
	}

	static int SortTriggerPeers(const void *_peer1, const void *_peer2)
	{
		TriggerCheckPeer_t *peer1 = (TriggerCheckPeer_t*)_peer1;
		TriggerCheckPeer_t *peer2 = (TriggerCheckPeer_t*)_peer2;

		if (peer1->mTrig1<peer2->mTrig1)
			return -1;
		if (peer1->mTrig1>peer2->mTrig1)
			return 1;
		if (peer1->mTrig2<peer2->mTrig2)
			return -1;
		if (peer1->mTrig2>peer2->mTrig2)
			return 1;
		return 0;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////

	void GetActiveTriggersInCluster( ZCluster *pClust, ZTrigger** pTrigs, int* aNbTrigs )
	{
		std::list<smartptr<ZSpatialEntity> >::iterator ier = pClust->mEntities.begin();
		for (;ier != pClust->mEntities.end(); ++ier)
		{
			ZSpatialEntity *pEnt = (*ier).ptr();
			if ( pEnt->GetClassID() == ClassIDZTrigger)
			{
				ZTrigger *curTrig = (ZTrigger*)pEnt->QueryInstance();
				if (curTrig->IsActive())
				{
					pTrigs[*aNbTrigs] = curTrig;
					(*aNbTrigs)++;
				}
			}
		}
	}

	void GetActiveTriggersRelatedToClusterDownSide( ZCluster *pClust, ZTrigger** pTrigs, int* aNbTrigs )
	{
		GetActiveTriggersInCluster( pClust, pTrigs, aNbTrigs );

		std::list<ZCluster>::iterator iter = pClust->mChildClusters.begin();
		for (;iter != pClust->mChildClusters.end(); ++iter)
		{
			GetActiveTriggersRelatedToClusterDownSide( &(*iter), pTrigs, aNbTrigs);
		}
	}

	void GetActiveTriggersRelatedToClusterUpSide( ZCluster *pClust, ZTrigger** pTrigs, int* aNbTrigs )
	{
		GetActiveTriggersInCluster( pClust, pTrigs, aNbTrigs );

		if (pClust->mParentCluster)
			GetActiveTriggersRelatedToClusterUpSide(pClust->mParentCluster, pTrigs, aNbTrigs);
	}
};

#endif
