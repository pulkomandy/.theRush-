#ifndef ZTRACKBONUS_H__
#define ZTRACKBONUS_H__
class ZShip;
#include "FSMTrackBonus.h"
#include "../librender/FFx.h"

DECLAREZCLASS(ZTrackBonus);


class ZTrackBonus : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZTrackBonus() : mType(TYPE_SPEED)
	{
	}
	virtual ~ZTrackBonus();

	enum TBTYPE
	{
		TYPE_SPEED,
		TYPE_BONUS
	};

	static ZTrackBonus* BuildTrackBonus(const tmatrix& mt, ZTransform *pOriginalTransform, TBTYPE aType);
	void Tick(float aTimeEllapsed)
	{
		mFSMTrackBonus.Tick(aTimeEllapsed);
	}
	void Trigger(ZShip *pShip);


	TBTYPE GetType() const { return mType; }
protected:
	smartptr<ZTransform> mBonusTransform;
	FFxParam *mShaderParam;
	FSMTrackBonus mFSMTrackBonus;
	TBTYPE mType;
};

#endif
