#ifndef ZSHIPAI_H__
#define ZSHIPAI_H__

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