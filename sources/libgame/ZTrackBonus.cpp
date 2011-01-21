#include "../libbase/ZSerializator.h"
#include "../libgame/ZTrackBonus.h"
#include "../libgame/ZGameTriggers.h"
#include "../libgame/ZShip.h"
#include "../libworld/ZTrigger.h"

BEGIN_SERIALIZATION(ZTrackBonus)
END_SERIALIZATION()


ZTrackBonus::~ZTrackBonus()
{
	mBonusTransform->RemoveFromScene();
}

ZTrackBonus* ZTrackBonus::BuildTrackBonus(const tmatrix& mt, ZTransform *pOriginalTransform, TBTYPE aType)
{
	ZTrackBonus* nBonus = (ZTrackBonus*)_New(ZTrackBonus);

	ZTransform *pSpeedBonusTransform = pOriginalTransform->Clone();

	pSpeedBonusTransform->SetLocalMatrix( mt );

	// trigger
	ZTrigger *pTrig = (ZTrigger*)_New(ZTrigger);
	pTrig->GetOriginVolume().SetBoxMinMax(tvector3(-3,0,-3), tvector3(3,6,3));
	pTrig->GetOriginVolume().ComputeBSphereFromBoxMinMax();
	pTrig->mTriggerType = ZTrigger::TRIGGER_BOX;
	pTrig->mFilterGroup = TGTRACKBONUSGROUP;
	pTrig->mFilterMask = TGTRACKBONUSFILTER;
	pTrig->SetUserPointer(nBonus);

	pSpeedBonusTransform->AddChild(pTrig->GetTransform());

	pSpeedBonusTransform->AddToScene(GScene);
	pSpeedBonusTransform->Update();


	nBonus->mFSMTrackBonus.mBonusTransform = pSpeedBonusTransform;
	nBonus->mBonusTransform.Attach(pSpeedBonusTransform);
	nBonus->mType = aType;
	return nBonus;
}

void ZTrackBonus::Trigger(ZShip *pShip)
{
	ShipTrigger shpTrig;
	shpTrig.pShip = pShip;
	if (mType == TYPE_BONUS)
	{
		pShip->GetGameItBelongsTo()->GiveBonusToShip(rand()%pShip->GetAvailableBonusCount(), pShip);
	}
	mFSMTrackBonus.Handle(shpTrig);
}
