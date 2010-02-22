#ifndef ZMATERIAL_H__
#define ZMATERIAL_H__

#include "ZFx.h"
#include "ZTexture.h"

DECLAREZCLASS(ZMaterial);

class ZMaterial :  public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZMaterial();
	virtual ~ZMaterial();

	// material building
	void setEffectFileName(const char * aFileName);
	const char * getEffectFileName() const;
	void setEffect(ZFx* aEffect);
	ZFx* getEffect() const;
	void connectEffect(bool aAddNewParams=false, bool aRemoveOldParams=false);
	void addParam(FFxSetParam* aParam);
	void addTexture(ZTexture* aTexture);
	virtual void					setTexture(int idx, ZTexture *aTexture)
	{
		int sz = (getTextureArray().size()-1);
		for (int i = sz; i < idx ; i ++)
		{
			addTexture(aTexture);
			return;
		}

		mTextures[idx] = aTexture;
	}

	const std::vector<smartptr<ZTexture> >& getTextureArray() const { return mTextures; }

	// Queries operations
	int getNumParams() const;
	FFxSetParam* getParam(int aParamIndex) const;
	FFxSetParam* getNamedParam(const char* aName) const;
	ZMaterial *Clone();
	void Clone(ZMaterial *pMatToClone);


	bool open();
	bool open(bool bAlreadyOpen);
	bool close();
	bool setPass(int32 aNum);
	uint32 getNumPasses() const;

	virtual void					SetTechnic(unsigned int aTech) { mTechnic = aTech; }
	virtual unsigned int			GetTechnic() { return mTechnic; }

	// Physics material
	void				setPhysicsMaterialID(uint32 mat)			{ mPhysicsMaterial = mat; }
	uint32				getPhysicsMaterialID() const						{ return mPhysicsMaterial; }

    virtual tulong GetMemoryUsed() const { return tulong(sizeof(ZMaterial)+mParams.size()*sizeof(FFxSetParam*)+mEffectFileName.Length()+mTextures.size()*sizeof(smartptr<ZTexture>)); }

	virtual int						GetTechnicBlend()
	{

		return mEffect->GetBlend(mTechnic, 0);

	}

	// returns true if both material are same
	virtual bool					IsEsqualTo(ZMaterial *pMat);

protected:
	std::vector<FFxSetParam*>	mParams;
	bool						mDisabled;

	tstring						mEffectFileName;
	smartptr<ZFx>						mEffect;
    std::vector<smartptr<ZTexture> >	mTextures;
	uint32						mPhysicsMaterial;
    uint32                      mTexIndex;
	unsigned int				mTechnic;
};

#endif

