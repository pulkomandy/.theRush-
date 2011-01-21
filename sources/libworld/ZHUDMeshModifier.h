#ifndef ZHUDMESHMODIFIER_H__
#define ZHUDMESHMODIFIER_H__

#include "../libbase/ZSerializator.h"
#include "../libbase/ZBaseMaths.h"
#include "ZMeshModifierContainer.h"
#include "ZMeshInstance.h"

DECLAREZCLASS(ZHUDMeshModifier);
class ZRushGame;

class ZHUDMeshModifier : public ZMeshModifier
{
public:
	DECLARE_SERIALIZABLE
public:
	ZHUDMeshModifier();
	virtual ~ZHUDMeshModifier();
	virtual void Tick(float aTimeEllapsed);

	// Quads
	void StartQuads();
	void PushQuad(int Qnb, const tvector2& pos, const tvector2& size, const tmatrix& mt, const tcolor& col = tcolor::white, float fact=1.f);
	void EndQuads();

	// Times
	void PushTime(float aTime) { mTimes[mNbTimes++] = aTime; }
	void ClearTimes()
	{
		mNbTimes = 0;
		mCurrentTime = 0;
		mUpdateTime = 100;

	}
	int GetNbTimes() { return mNbTimes; }
	float GetTime(int aIdx)
	{
		if ((aIdx<0) || (aIdx >(mNbTimes-1)))
			return 0;
		return mTimes[aIdx];
	}

	ZMeshInstance * GetMeshInstance() { return mHUDMesh; }

	float mCurrentTime;
	int mNbTimes;
	float mTimes[16];
	float mUpdateTime;

	// vertices
	typedef struct guiMeshVT
	{
		tvector3 pos;
		unsigned int color;
		tvector2 uv;

	} guiMeshVT;

	unsigned int mNbVt;
	guiMeshVT *mVts;
	guiMeshVT *mCurVts;
	float mEsp, mScX, mScY, mRotY, mRotZ;

	void SetRectSource(int rectNb, const tvector4 &vt)
	{
		UVSources[rectNb] = vt;
	}
	tvector4 UVSources[99];

	// Mesh
	ZMeshInstance *mHUDMesh;

	void DecompNumber(float aspeed, float &centaine, float &dizaine, float &unit)
	{
		centaine = floorf(aspeed/100);
		dizaine = floorf( (aspeed-centaine*100)/10);
		unit = floorf( (aspeed-(centaine*100) - (dizaine*10)));
	}
	void DecompTime(float atime, float &minutes, float &secondes, float &dixiemes)
	{
		minutes = floorf(atime/6000.f);
		secondes = floorf( (atime-(minutes*6000)))/100;
		dixiemes = atime;// --math.floor(atime-(minutes*6000) -(secondes*100));

	}

	virtual void Show(bool bShow)
	{
		mHUDMesh->GetTransform()->SetVisible(bShow);
	}

	void SetGame(ZRushGame *pGame) { mGame = pGame; }
	ZRushGame *mGame;

	virtual tulong GetMemoryUsed() const { return sizeof(ZHUDMeshModifier); }
};

#endif
