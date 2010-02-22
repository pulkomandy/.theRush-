#ifndef ZDDEGAME_H__
#define ZDDEGAME_H__

class ZDDEGame : public ZGame
{
public:
    DECLARE_SERIALIZABLE
public:
	ZDDEGame();
	virtual ~ZDDEGame();
	virtual void Update(float aTimeEllapsed);
protected:
	smartptr<ZGameResources> mWorldRes;
	bool mbAllResourceToScene;
};

#endif