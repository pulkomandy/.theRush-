#ifndef ZSHIPAI_H__
#define ZSHIPAI_H__

#include "../libbase/ZPIDCtrl.h"
#include "../libbase/ZSerializator.h"

class ZShip;
struct keysStruct;

DECLAREZCLASS(ZShipAI);

class ZShipAI : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZShipAI()
	{
	}
	virtual ~ZShipAI()
	{
	}
	void Init(ZShip *pShip);
	void ComputeAI(float aTimeEllapsed, keysStruct* res);
protected:
	ZShip *mShip;
public:
	ZPID<float> mPID;
};


#endif
