#ifndef ZATMOSPHERE_H__
#define ZATMOSPHERE_H__

DECLAREZCLASS(ZAtmosphere);

class ZAtmosphere : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZAtmosphere();
	virtual ~ZAtmosphere();

	void LoadSkybox(const char *szSkyboxFileName);
	ZMeshInstance* GetSkybox() const { return mSkyboxMesh.ptr(); }

	tvector3* GetSunVector() { return &mSunVector; }
	tcolor* GetDefaultSkyColor() { return &mDefaultSkyColor; }
protected:
	smartptr<ZMeshInstance> mSkyboxMesh;
	tvector3 mSunVector;
	tcolor mDefaultSkyColor;
};

extern ZAtmosphere *gAtmosphere;
#endif
