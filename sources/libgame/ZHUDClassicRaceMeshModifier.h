#ifndef ZHUDCLASSICRACEMESHMODIFIER_H__
#define ZHUDCLASSICRACEMESHMODIFIER_H__

#include "libworld/ZHUDMeshModifier.h"

DECLAREZCLASS(ZHUDClassicRaceMeshModifier);

class ZRushGame;

class ZHUDClassicRaceMeshModifier : public ZHUDMeshModifier
{
public:
	DECLARE_SERIALIZABLE
public:
	ZHUDClassicRaceMeshModifier();
	virtual ~ZHUDClassicRaceMeshModifier();

	
	virtual void Tick(float aTimeEllapsed);
protected:
	ZTexture * mHUDBaseTexture;

	tmatrix computePanelMatrix(float atrx, float atry, float ngy, float ngz);

	float mEsp, mScX, mScY, mRotY, mRotZ;
	float mCurrentLifeEnergy;
	virtual tulong GetMemoryUsed() const { return sizeof(ZHUDClassicRaceMeshModifier); }
	
	char mCurrentBonus;
};

#endif
