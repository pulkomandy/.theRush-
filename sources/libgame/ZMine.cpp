#include "ZMine.h"

#include "../libgame/ZRushGameContent.h"
#include "../libgame/ZGameTriggers.h"
#include "../libgame/ZBonusParameters.h"
#include "../libgame/ZGameEntitiesManager.h"

BEGIN_SERIALIZATION(ZMine)
END_SERIALIZATION()

ZMine::ZMine()
{
	mMineLife = 0.f;

}

ZMine::~ZMine()
{
#ifdef _DEBUG	
	LOG("ZMine 0x%d destroyed.\n", this);
#endif
	mMineTransform->RemoveFromScene();
}


void ZMine::Init(const tmatrix& mat)
{
	mMineTransform = ((ZTransform*)gRushGameContent->GetRes("Mine"))->Clone();

	// trigger box
	// only for server/solo 
	{
	ZTrigger *pTrig = (ZTrigger*)_New(ZTrigger);
	pTrig->GetOriginVolume().SetBoxMinMax(tvector3(-3,0,-3), tvector3(3,6,3));
	pTrig->GetOriginVolume().ComputeBSphereFromBoxMinMax();
	pTrig->mTriggerType = ZTrigger::TRIGGER_SPHERE;
	pTrig->mFilterGroup = TGMINEGROUP;
	pTrig->mFilterMask = TGMINEFILTER;
	pTrig->SetUserPointer(this);

	mMineTransform->AddChild(pTrig->GetTransform());
	}
	// add to scene
	mMineTransform->AddToScene(GScene);
	mMineTransform->SetWorldMatrix(mat);
	mMineTransform->Update();
}

void ZMine::Tick(float timeEllapsed)
{
	mMineLife += timeEllapsed;
	if (mMineLife >= GBonusParameters->MineTTL)
	{
		mManager->SpawnExplosion(tmatrix::identity);
		mManager->DestroyEntity(this);
	}
}
