///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFX.h
// Creation : 28/06/2007
// Author : Cedric Guillemet
// Description :
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _FFX_H_
#define _FFX_H_
#include "../libbase/LibBase.h"
/*
#include <Cg/cg.h>
#ifdef WIN32
#include <Cg/cgD3D9.h>
#endif
#include <Cg/cgGL.h>
*/
#ifdef WIN32
#include <d3d9.h>
#include <d3dx9core.h>
#endif

extern CGcontext			gDefaultCGcontext;



//typedef uint32 GLenum;

class ZMaterial;
class IShader;
class ZTexture;
class ZFx;


enum
 {
  ZCG_GL_MATRIX_IDENTITY = 0,
  ZCG_GL_MATRIX_TRANSPOSE = 1,
  ZCG_GL_MATRIX_INVERSE = 2,
  ZCG_GL_MATRIX_INVERSE_TRANSPOSE = 3,

  ZCG_GL_MODELVIEW_MATRIX,
  ZCG_GL_PROJECTION_MATRIX,
  ZCG_GL_TEXTURE_MATRIX,
  ZCG_GL_MODELVIEW_PROJECTION_MATRIX,
  ZCG_GL_VIEW_MATRIX,
  ZCG_GL_WORLD_MATRIX,
  ZCG_GL_VIEW_PROJECTION_MATRIX
 };

//---------------------------------------------------------------------------------
// abstract parameter class
//---------------------------------------------------------------------------------

struct FFxParam
{
	FFxParam();
	FFxParam(const char* aName);
	virtual ~FFxParam();

	virtual void	apply() = 0;
	virtual void	init(CGcontext aContext) = 0;

	static	CGparameter getLeaf(CGparameter parameter);

	tstring			mName;
	CGparameter		mCGparam;
};


//---------------------------------------------------------------------------------
// variant data
//---------------------------------------------------------------------------------
struct FFxData
{
	enum Type
	{
		Unknown,
		Vector,
		Matrix,
		String
	};

	FFxData(CGtype aType);
	virtual ~FFxData();
	virtual void apply(CGparameter aParam) = 0;

	virtual void setVector(const tvector4& aVector);
	virtual bool getVector(tvector4& aVector) const;

	static	FFxData* create(CGtype aType, const float* aValues, int aSize, bool bKeepPtr = false);
	static	FFxData* create(CGparameter parameter);

	CGtype	mCGtype;
};

struct FFxDataVector : public FFxData
{
	FFxDataVector(CGtype aCGtype, const float* aValues, bool bKeepPtr = false);
	void apply(CGparameter aParam);
	void setVector(const tvector4& aVector);
	bool getVector(tvector4& aVector) const;

	float *mValuesPtr;
	float mValues[4];
	bool mbKeepPtr;
};

struct FFxDataMatrix : public FFxData
{
	FFxDataMatrix(CGtype aCGtype, const float* aValues);
	void apply(CGparameter aParam);

	float mValues[16];
};

struct FFxDataGlMatrix : public FFxData
{
	FFxDataGlMatrix(uint32 aGLMatrixID, uint32 aGLMatrixOp);
	void apply(CGparameter aParam);


	uint32	mGLMatrixID;
	uint32	mGLMatrixOp;
};

struct FFxDataString : public FFxData
{
	FFxDataString(CGtype aCGtype, const tstring& aName);
	void apply(CGparameter aParam);


	tstring mName;
};

struct FFxDataTransformVector : public FFxData
{
	FFxDataTransformVector(ZTransform*,  uint32 aVectorIndex, uint32 aSpace);
	void apply(CGparameter aParam);

	void setVector(const tvector4& aVector);
	bool getVector(tvector4& aVector) const;

	smartptr<ZTransform>	mTransform;
	uint32			mSpace;
	uint32			mVectorIndex;
};

struct FFxDataFloatAttr : public FFxData
{
	FFxDataFloatAttr(float* aFloat1Ptr, float aFloat1, char aOp, float* aFloat2Ptr, float aFloat2);
	~FFxDataFloatAttr();
	void apply(CGparameter aParam);

	void setVector(const tvector4& aVector);
	bool getVector(tvector4& aVector) const;

	tvector4			mValue;
	float*			mFloat1Ptr;
	float*			mFloat2Ptr;
	float			mFloat1;
	float			mFloat2;
	char			mOp;
};

struct FFxDataVectorAttr : public FFxData
{
	FFxDataVectorAttr(float* aFloat1Ptr, float aFloat1, char aOp, float* aFloat2Ptr, float aFloat2);
	~FFxDataVectorAttr();
	void apply(CGparameter aParam);

	void setVector(const tvector4& aVector);
	bool getVector(tvector4& aVector) const;

	tvector4			mValue;
	float*			mFloat1Ptr;
	float*			mFloat2Ptr;
	float			mFloat1;
	float			mFloat2;
	char			mOp;
};

//---------------------------------------------------------------------------------
// structure holding an array of parameters
//---------------------------------------------------------------------------------

class FFxParamable //: public IFxParamable
{
public:
	FFxParamable() {};
	virtual ~FFxParamable();

	void			addParam(FFxParam* aParam);
	FFxParam*		getNamedParam(const tstring& aName) const;
	void			addMatrixParam(FFxParam* aParam);
	FFxParam*		getNamedMatrixParam(const tstring& aName) const;


	std::vector<FFxParam*> mParams;
	std::vector<FFxParam*> mMatrixParams;
};

//---------------------------------------------------------------------------------
// global uniform CG parameters defined by the user (tweakables)
//---------------------------------------------------------------------------------
struct FFxEffectParam : public FFxParam
{
	FFxEffectParam();
	FFxEffectParam(const tstring& aName, const tstring& aSemantic, FFxData* aData);

	void			init(CGcontext aContext);
	void			apply();


	FFxData*		mData;			// default value
	tstring			mSemantic;		// semantic name
};

//---------------------------------------------------------------------------------
// parameter connection pair
//---------------------------------------------------------------------------------
struct FFxParamConnection
{
	FFxParamConnection();
	FFxParamConnection(const tstring& aFrom, const tstring &To);
	tstring			mFrom;			// new effect param
	tstring			mTo;			// program param
};

//---------------------------------------------------------------------------------
// uniform CG parameters binded to the vertex/fragment programs (not tweakable)
//---------------------------------------------------------------------------------
struct FFxBindParam
{
	FFxBindParam() {};
	~FFxBindParam() {};

	CGenum			mClass;
	uint32			mMatrixID;
	uint32			mMatrixOp;
};

//---------------------------------------------------------------------------------
// instance of Effect parameter value hold by a material
//---------------------------------------------------------------------------------
struct FFxSetParam : public FFxParam //, IFFxSetParam
{
	FFxSetParam(const tstring& aName, FFxData* aData)
	{
		mName = aName;
		mData = aData;
	}
	~FFxSetParam()
	{
	}

    virtual void setVector(const tvector4& aVector)
    {
        mData->setVector(aVector);
    }


	void			init(CGcontext /*aContext*/)
	{
	}

	FFxData*		mData;

	void apply()
	{
		if (mData && mCGparam)
			mData->apply(mCGparam);
	}

	virtual void Apply()
	{
		apply();
	}
	virtual void getVector(tvector4& aVector) const
	{
		mData->getVector(aVector);
	}
};

//---------------------------------------------------------------------------------
// FFxSamplerParam
//---------------------------------------------------------------------------------
struct FFxSamplerParam
{
	FFxSamplerParam() : mCGparam(0), mTexture(NULL), mTextureIndex(-1) {};
	FFxSamplerParam(CGparameter	p, int i, int smpidx, ZTexture* tex) : mCGparam(p), mTextureIndex(i), mSamplerIndex(smpidx), mTexture(tex) {};

	CGparameter	mCGparam;			// sampler param
  ZTexture* mTexture;
	uint32		mTextureIndex;		// texture index
	int32		mSamplerIndex;
};

//---------------------------------------------------------------------------------
// This class keeps track of the data related to a VertexProgram or FragmentProgram
// that cannot be retrieved usign from CGprogram.
// This includes the .cgfx or .cg filename, entry name and args used to generate the
// program.
// After creating a program, the parameters are analysed and the corresponding plugs
// are initialized
//---------------------------------------------------------------------------------

class FFxProgram //: public FFxParamable
{
public:
	FFxProgram();
	FFxProgram(CGdomain aProfileDomain, const char* aFileName, const char* aEntryName, const char** args);
	virtual ~FFxProgram();

	// building
	void				addParamConnection(const tstring& aFrom, const tstring& aTo);

	// runtime init
	bool				init(CGcontext aContext);

	// rendering
	void				bind();
	void				unbind();

protected:
	void				useDefaultProgram(CGcontext aContext);

public:
	uint32			mProgramFlags;		// flags for the program
	CGprogram			mProgram;			// vertex or shader program
	tstring				mFileName;			// filename source or binary (detected by extension)
	tstring				mEntry;				// entry name
	tstring				mArgs;				// args used at compile time
	CGprofile			mProfile;			// profile
	CGdomain			mProfileDomain;		// profile
	const char*			mSourceBuffer;
	std::vector<FFxParamConnection> mConnections;

	static CGprogram	sDefaultVertexProgram;
	static CGprogram	sDefaultFragmentProgram;
	//LPD3DXBUFFER		mDXShader;
#ifdef DX9
    LPDIRECT3DVERTEXSHADER9            mVertexShader;
    LPDIRECT3DPIXELSHADER9            mPixelShader;
#endif
	tulong GetMemoryUsed() const 
	{
		return sizeof(FFxProgram) + mConnections.size()*sizeof(FFxParamConnection);
	}
};


//---------------------------------------------------------------------------------
// FFxPass
//---------------------------------------------------------------------------------
struct FFxPass
{
	FFxPass();
	~FFxPass();

	// building
	void					addProgram(FFxProgram* aProgram);
	void					addSamplerParam(ZFx *pFx, FFxSamplerParam* aSamplerParam);


	// runtime init
	void					init(CGcontext aContext);

	// rendering
	void					setPassState(ZMaterial* aMaterial);

	tstring					mName;
	std::vector<FFxProgram*>		mPrograms;
	std::vector<FFxSamplerParam>	mSamplerParams;
    int                     mShadowSamplerIndex;
    int                     mRenderTargetSampler, mFrameBufferTexture;
	int						mMiniFrameBufferTexture, mZBufferTexture;
	int mJitterSamplerIndex;
	RenderingState_t mRenderingState;

	tulong GetMemoryUsed() const 
	{
		tulong mu = sizeof(FFxPass);
		
		mu += mSamplerParams.size()*sizeof(FFxSamplerParam);
		for (unsigned int i=0;i<mPrograms.size();i++)
			mu += mPrograms[i]->GetMemoryUsed();
		return mu;
	}
	void UnbindPrograms();
};

//---------------------------------------------------------------------------------
// FFxTechnique
//---------------------------------------------------------------------------------
struct FFxTechnique : public FFxParamable
{
	FFxTechnique();
	~FFxTechnique();

	// building
	void					addPass(FFxPass* aPass);
	FFxPass*				getNamedPass(const tstring& aName);
	uint32					registerSemanticParameter(CGparameter aParam, const char* aSemantic);

	// runtime init
	void					init(CGcontext aContext);

	// rendering
	void					beginMaterial(ZMaterial* aMaterial);
	void					setPassState(uint aPassNum);
	void					setPassState(int aPass, RenderingState_t rs);

	tstring					mName;
	std::vector<FFxPass*>		mPasses;
	ZMaterial*				mCurMaterial;
	FFxTechnique			*mHWITechnic;

	tulong GetMemoryUsed() const 
	{
		tulong mu = sizeof (FFxTechnique);
		for (unsigned int i=0;i<mPasses.size();i++)
		{
			mu += mPasses[i]->GetMemoryUsed();
		}
		return mu;
	}
};

//---------------------------------------------------------------------------------
// custom Semantics
//---------------------------------------------------------------------------------

enum
{
    ZCG_FRESH_SHADOW_MATRIX = 100,
    ZCG_TEXTURE_TRANSFORM_MATRIX = 101,
    ZCG_PREVIOUS_WORLD_MATRIX = 102

};



#endif
