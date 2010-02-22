#ifndef ZSHIPEFFECTS_H__
#define ZSHIPEFFECTS_H__

DECLAREZCLASS(ZShipEffectsNULL);
DECLAREZCLASS(ZShipEffects);
class ZShip;

class IShipEffects : public ZBaseClass
{
public:

	virtual void SetShip(ZShip *pShip) = 0;

	// collision with ship or wall
	virtual void Collision() = 0;
};

class ZShipEffectsNULL : public IShipEffects
{
public:
    DECLARE_SERIALIZABLE
public:
	ZShipEffectsNULL()
	{
	}
	virtual ~ZShipEffectsNULL()
	{
	}
	virtual void SetShip(ZShip *pShip) {}
	virtual void Collision() {}
};

class ZShipEffects : public IShipEffects
{
public:
    DECLARE_SERIALIZABLE
public:
	ZShipEffects();
	virtual ~ZShipEffects();

	virtual void SetShip(ZShip *pShip) { mShip = pShip; }
	virtual void Collision();
protected:
	ZShip *mShip;
};

#endif