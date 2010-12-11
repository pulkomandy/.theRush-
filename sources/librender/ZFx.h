#ifndef ZFX_H__
#define ZFX_H__

#undef __STRICT_ANSI__
#include <string.h>

#include <Cg/cg.h>

#include "FFx.h"

DECLAREZCLASS(ZFx);

class ZFx : public ZBaseClass , public FFxParamable
{
public:
    DECLARE_SERIALIZABLE
public:
	ZFx();
	virtual ~ZFx();

	// building
	void					setEffectFileName(const char* aFileName);
	void					addTechnique(FFxTechnique* aTechnique);
	bool					importProgramFromCgfx(const char* aCgfxFileName, const char* aVertexEntry, const char* aShaderEntry);
	bool					importProgramFromCg(const char* aVertexShader, const char* aFragmentShader);
	void					setAsDefault();
	void					clearAll();

	// runtime init
	void					init(CGcontext aContext, bool aAddNewParams);
	void					connectProgramParameters(bool bAddNewParams);
	void					connectTechnique(FFxTechnique* technique, bool bAddNewParams);

	// queries
	FFxEffectParam*			getNamedEffectParameter(const tstring& aName) const;
	FFxTechnique*			getNamedTechnique(const tstring& aName);

	// rendering
	FFxTechnique*			selectTechniqueByIndex(uint32 aTechniqueNum);
	FFxTechnique*			selectTechniqueByIndex(uint32 aNum, ZMaterial *pMat);
	FFxTechnique*			selectTechniqueByName(const tstring& aName);

	bool					beginMaterial(ZMaterial* aMaterial);
	int						getNumPasses() const;
	void					setPassState(int aPassNum);
	void					setPassState(int aPass, RenderingState_t rs);

	void					setViewDependentMatrices();

    void			        addParam(FFxParam* aParam)
	{
		FFxParamable::addParam(aParam);
	}

	virtual int					doesBlend() { return mDoesBlend; }
	virtual void					doesBlend(int iVal) { mDoesBlend = iVal; }
	virtual int						GetBlend(int aTechnic, int aPass)
	{
		return mTechniques[aTechnic]->mPasses[aPass]->mRenderingState.mBlending;

	}


    const std::vector<FFxParam*> & getParams() const
	{
		return mParams;
	}
	virtual void					AddSamplerParameter(const samplerparam_t& smpprm)
	{
		mSamplerParams.push_back(smpprm);
	}
	virtual int			GetSamplerByName(const char*szName)
	{
		for (uint i =0;i<mSamplerParams.size();i++)
		{
			if (!stricmp(mSamplerParams[i].szSamplerName, szName))
				return i;
		}
		return -1;
	}
	virtual samplerparam_t& GetSamplerParam(unsigned int idx) { return mSamplerParams[idx]; }

	virtual bool					NeedsMiniFrame() { return mbNeedMiniFrame; }
	virtual void					SetNeedsMiniFrame(bool bNeed)
	{
		mbNeedMiniFrame = bNeed;
	}
	virtual bool					NeedsZBuffer() { return mbNeedZBuffer; }
	virtual void					SetNeedsZBuffer(bool bNeed)
	{
		mbNeedZBuffer = bNeed;
	}

	virtual bool					HasZWrite() { return mbHasZWrite; }
	virtual void					SetZWriteEnable(bool bEnable) { mbHasZWrite = bEnable; }
    //static ZFx*                     getDefaultFX();
	virtual void					EnableHWI(bool bEnable) { mbHWIEnable = bEnable; }
	virtual tulong GetMemoryUsed() const 
	{ 
		tulong mu = sizeof(ZFx) + sizeof(samplerparam_t)*mSamplerParams.size()+
			sizeof(FFxTechnique*)*mTechniques.size();

		for (unsigned int i=0;i<mTechniques.size();i++)
			mu += mTechniques[i]->GetMemoryUsed();

		return mu;
	}
protected:
	void					connectParameter(const tstring& aEffectParamName, CGparameter parameter, bool bAddNewParams);
	FFxParam*				createEffectParam(CGparameter parameter, bool& bMatrixParam);
public:
	bool					mbNeedMiniFrame;
	bool					mbHasZWrite;
	bool					mbNeedZBuffer;
	tstring					mSourceFileName;
	CGcontext				mCGcontext;
	std::vector<FFxTechnique*>	mTechniques;
	FFxTechnique*			mCurTechnique;
	static float			mShaderTime;
	int					mDoesBlend;


	std::vector<samplerparam_t> mSamplerParams;
	static float mCameraInfos[2];
	bool mbHWIEnable;
};

void CleanPasses();
ZFx *DirectLoad(const char *filename);

#endif
