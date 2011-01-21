///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFxData.h
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

#include "FFx.h"
#include "libbase/ZLogger.h"


#include <Cg/cg.h>


#ifdef PS3
#define	cgGetMatrixParameterfc cgGLGetMatrixParameterfc

#endif

//---------------------------------------------------------------------------------
// FFxData building
//---------------------------------------------------------------------------------

FFxData::FFxData(CGtype aCGtype) :
mCGtype(aCGtype)
{
}

FFxData::~FFxData()
{
}

void FFxData::setVector(const tvector4& aVector)
{
	LOG("SetVector not implemented for this data type");
	//FASSERT(0);
}
bool FFxData::getVector(tvector4& aVector) const
{
	return false;
}

//---------------------------------------------------------------------------------
// FFxDataVector
//---------------------------------------------------------------------------------

FFxDataVector::FFxDataVector(CGtype aCGtype, const float* aValues, bool bKeepPtr) :
FFxData(aCGtype)
{
	mbKeepPtr = bKeepPtr;
	if (mbKeepPtr)
	{
		mValuesPtr = (float*)aValues;
		return;
	}
	for (int i=0; i<4; i++)
		mValues[i] = aValues[i];

}

void FFxDataVector::setVector(const tvector4& aVector)
{
	mValues[0] = aVector.x;
	mValues[1] = aVector.y;
	mValues[2] = aVector.z;
	mValues[3] = aVector.w;
}

bool FFxDataVector::getVector(tvector4& aVector) const
{
	aVector.x = mValues[0];
	aVector.y = mValues[1];
	aVector.z = mValues[2];
	aVector.w = mValues[3];
	return true;
}

void FFxDataVector::apply(CGparameter aParam)
{
	if (mbKeepPtr)
	{
		switch (mCGtype)
		{
		case CG_FLOAT:
			cgSetParameter1fv(aParam, mValuesPtr);
			break;
		case CG_FLOAT2:
			cgSetParameter2fv(aParam, mValuesPtr);
			break;
		case CG_FLOAT3:
			cgSetParameter3fv(aParam, mValuesPtr);
			break;
		case CG_FLOAT4:
			cgSetParameter4fv(aParam, mValuesPtr);
			break;
		}

		return;
	}
	switch (mCGtype)
	{
	case CG_INT:
	case CG_INT1:
	case CG_FLOAT:
		cgSetParameter1fv(aParam, mValues);
		break;
	case CG_FLOAT2:
	case CG_INT2:
		cgSetParameter2fv(aParam, mValues);
		break;
	case CG_FLOAT3:
	case CG_INT3:
		cgSetParameter3fv(aParam, mValues);
		break;
	case CG_FLOAT4:
	case CG_INT4:
		cgSetParameter4fv(aParam, mValues);
		break;
	}

}


//---------------------------------------------------------------------------------
// FFxDataMatrix
//---------------------------------------------------------------------------------

FFxDataMatrix::FFxDataMatrix(CGtype aCGtype, const float* aValues) :
FFxData(aCGtype)
{
	for (int i=0; i<16; i++)
		mValues[i] = aValues[i];
}

void FFxDataMatrix::apply(CGparameter aParam)
{
    cgSetMatrixParameterfc(aParam, mValues);
}


//---------------------------------------------------------------------------------
// FFxDataMatrix
//---------------------------------------------------------------------------------

FFxDataGlMatrix::FFxDataGlMatrix(uint32_t aGLMatrixID, uint32_t aGLMatrixOp) :
FFxData(CG_FLOAT4x4),
mGLMatrixID(aGLMatrixID),
mGLMatrixOp(aGLMatrixOp)
{
}

void FFxDataGlMatrix::apply(CGparameter aParam)
{
	tmatrix		temp, temp2,temp3;
	tmatrix*	mat;

	switch (mGLMatrixID)
	{
		case ZCG_GL_MODELVIEW_MATRIX:
            temp = GDD->GetWorldMatrix() * GDD->GetViewMatrix();//temp = gRenderState.mWorldViewMatrix;
			mat = &temp;
			break;

        case ZCG_GL_WORLD_MATRIX:
		case 0:	// world
			//mat = &gRenderState.mWorldMatrix;
            temp = GDD->GetWorldMatrix();
            mat = &temp;
			break;

		case 1:	// view
			//mat = &gRenderState.mViewMatrix;
            temp = GDD->GetViewMatrix();
            mat = &temp;
			break;


		case ZCG_GL_MODELVIEW_PROJECTION_MATRIX:
			temp = GDD->GetWorldMatrix() * GDD->GetViewMatrix() * GDD->GetProjectionMatrix();
            //temp.Transpose();
            //temp2.Transpose();
            //temp *= temp2;
			mat = &temp;
			break;

		case ZCG_GL_PROJECTION_MATRIX:
			temp = GDD->GetProjectionMatrix();//&gRenderState.mProjectionMatrix;
            //temp.Transpose();
            mat = &temp;
			break;
        case ZCG_FRESH_SHADOW_MATRIX:
			{
				extern tmatrix mShadowMap;
				temp = mShadowMap;
				mat = &temp;
			}
            break;
        case ZCG_TEXTURE_TRANSFORM_MATRIX:
            temp = GDD->GetTextureTransformMatrix();// * GDD->GetWorldMatrix();
            mat = &temp;
            break;
        case ZCG_PREVIOUS_WORLD_MATRIX:
            temp = GDD->GetPreviousWorldMatrix();// * GDD->GetPreviousViewMatrix() * GDD->GetProjectionMatrix();
            mat = &temp;
            break;
		case ZCG_GL_VIEW_MATRIX:
			temp = GDD->GetViewMatrix();// * GDD->GetWorldMatrix();
            mat = &temp;
			break;
		case ZCG_GL_VIEW_PROJECTION_MATRIX:
			temp = GDD->GetViewMatrix() * GDD->GetProjectionMatrix();
			mat = &temp;
			break;
		default:
			// not supported
			//FASSERT(0);
            LOG("Bad matrix semantic. Setting world matrix instead.\r\n");
            temp = GDD->GetWorldMatrix();
            mat = &temp;
			//mat = &gRenderState.mWorldMatrix;
			break;
	}
	if (mGLMatrixOp & ZCG_GL_MATRIX_INVERSE)
	{
        mat->Inverse();//Invert(temp2, *mat);
		//mat = &temp2;
	}
    // BEWARE need transpose for DX9
    if ((mGLMatrixOp & ZCG_GL_MATRIX_TRANSPOSE))
	{
        mat->Transpose();//Transpose(temp3, *mat);
		//mat = &temp3;
	}
    cgSetMatrixParameterfr(aParam, (const float*)mat->m16);

}

//---------------------------------------------------------------------------------
// FFxDataString
//---------------------------------------------------------------------------------

FFxDataString::FFxDataString(CGtype aCGtype, const tstring& aName) :
FFxData(aCGtype),
mName(aName)
{
}
void FFxDataString::apply(CGparameter aParam)
{
}




//---------------------------------------------------------------------------------
// FFxDataTransformVector
//---------------------------------------------------------------------------------

FFxDataTransformVector::FFxDataTransformVector(ZTransform* aTransform, uint32_t aVectorIndex, uint32_t aSpace) :
FFxData(CG_FLOAT4),
mSpace(aSpace),
mVectorIndex(aVectorIndex)
{
	mTransform.Attach(aTransform);
}

void FFxDataTransformVector::setVector(const tvector4& aVector)
{
	//unused
}

bool FFxDataTransformVector::getVector(tvector4& aVector) const
{
	//unused
	return false;
}

#define FFX_GLOBAL 99
#define FFX_DIRECTION 100

void FFxDataTransformVector::apply(CGparameter aParam)
{
	//$PSGL specific
    if (mTransform.ptr())
	{
		const tmatrix* matrix;
		if (mSpace == FFX_GLOBAL)
			matrix = &mTransform->GetWorldMatrix();
		else
			matrix = &mTransform->GetLocalMatrix();
		//const float* vector = &matrix->right.x+4*mVectorIndex;
		if (mVectorIndex == FFX_DIRECTION)
		{
			const float* v = &matrix->V4.right.x+4*mVectorIndex;
			tvector4	dir = vector4(v[0], v[1], v[2], 0.0f);
            float len = dir.LengthSq();
			if (len - 1.0f > FLOAT_EPSILON*2)
				dir /= sqrtf(len);
	        cgSetParameter4fv(aParam, &dir.x);
		}
		else
		{
			const float* vector = &matrix->V4.right.x+4*mVectorIndex;
	        cgSetParameter4fv(aParam, vector);
		}
	}
}


//---------------------------------------------------------------------------------
// FFxData read
//---------------------------------------------------------------------------------

FFxData* FFxData::create(CGtype aType, const float* aValues, int /*aSize*/, bool bKeepPtr)
{
	FFxData*	data = 0;

	switch (cgGetTypeClass(aType))
	{
		case CG_PARAMETERCLASS_SCALAR:
		case CG_PARAMETERCLASS_VECTOR:
			data = new FFxDataVector(aType, aValues, bKeepPtr);
			break;

		case CG_PARAMETERCLASS_MATRIX:
			data = new FFxDataMatrix(aType, aValues);
			break;

		case CG_PARAMETERCLASS_ARRAY:
		case CG_PARAMETERCLASS_SAMPLER:
			// unsupported
			break;

		case CG_PARAMETERCLASS_OBJECT:
			switch(aType)
			{
				case CG_STRING:
				case CG_TEXTURE:
				case CG_PROGRAM_TYPE:
					// unsupported
					break;
			}
			break;
		default:
			// unsupported
			break;
	}
	return data;
}

//---------------------------------------------------------------------------------
// FFxData read
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
// FFxDataVectorAttr
//---------------------------------------------------------------------------------
FFxDataVectorAttr::FFxDataVectorAttr(float* aFloat1Ptr, float aFloat1, char aOp, float* aFloat2Ptr, float aFloat2) :
FFxData(CG_FLOAT4),
mFloat1Ptr(aFloat1Ptr),
mFloat2Ptr(aFloat2Ptr),
mFloat1(aFloat1),
mFloat2(aFloat2),
mValue(tvector4::zero),
mOp(aOp)
{
}

FFxDataVectorAttr::~FFxDataVectorAttr()
{
}

void FFxDataVectorAttr::setVector(const tvector4& aVector)
{
	mValue = aVector;
}

bool FFxDataVectorAttr::getVector(tvector4& aVector) const
{
	aVector = mValue;
	return true;
}

void FFxDataVectorAttr::apply(CGparameter aParam)
{
//$PSGL specific
//	tvector4 value2;
	float	value2;

	if (mFloat1Ptr)
		mValue = vector4(mFloat1Ptr[0], mFloat1Ptr[1], mFloat1Ptr[2]) * mFloat1;
	else
		mValue = vector4(mFloat1, mFloat1, mFloat1);
	if (mOp)
	{
		/*
		if (mFloat2Ptr)
			value2 = tvector4(mFloat2Ptr[0],mFloat2Ptr[1], mFloat2Ptr[2])  * mFloat2;
		else
			value2 = tvector4(mFloat2, mFloat2, mFloat2);
			*/
		if (mFloat2Ptr)
			value2 = mFloat2Ptr[0]  * mFloat2;
		else
			value2 = mFloat2;
		if (mOp == '*')
			mValue*= value2;
		else if (mOp == '/')
			mValue /= value2;
/*
		else if (mOp == '+')
			value += value2;
		else if (mOp == '-')
			value -= value2;
*/
	}
    cgSetParameter4fv(aParam, &mValue.x);
}


//---------------------------------------------------------------------------------
// FFxDataFloatAttr
//---------------------------------------------------------------------------------

FFxDataFloatAttr::FFxDataFloatAttr(float* aFloat1Ptr, float aFloat1, char aOp, float* aFloat2Ptr, float aFloat2) :
FFxData(CG_FLOAT4),
mFloat1Ptr(aFloat1Ptr),
mFloat2Ptr(aFloat2Ptr),
mFloat1(aFloat1),
mFloat2(aFloat2),
mValue(tvector4::zero),
mOp(aOp)
{
}

FFxDataFloatAttr::~FFxDataFloatAttr()
{
}

void FFxDataFloatAttr::setVector(const tvector4& aVector)
{
	mValue = aVector;
}

bool FFxDataFloatAttr::getVector(tvector4& aVector) const
{
	aVector = mValue;
	return true;
}

void FFxDataFloatAttr::apply(CGparameter aParam)
{
//$PSGL specific
//	float vector[4];
	float value2;

	if (mFloat1Ptr)
		mValue.x = *mFloat1Ptr * mFloat1;
	else
		mValue.x = mFloat1;
	if (mOp)
	{
		if (mFloat2Ptr)
			value2 = *mFloat2Ptr * mFloat2;
		else
			value2 = mFloat2;
		if (mOp == '*')
			mValue.x *= value2;
		else if (mOp == '/')
			mValue.x /= value2;
		else if (mOp == '+')
			mValue.x += value2;
		else if (mOp == '-')
			mValue.x -= value2;
	}
    cgSetParameter4fv(aParam, &mValue.x);
}


//---------------------------------------------------------------------------------
// FFxData creator
//---------------------------------------------------------------------------------

FFxData* FFxData::create(CGparameter parameter)
{
	// ensure the specified parameter is a program parameter (not an effect parameter or any intermediate)
	parameter = FFxParam::getLeaf(parameter);

	CGtype type = cgGetParameterType(parameter);
	const char* name = cgGetParameterName(parameter);
	FFxData*	data= 0;

	switch (cgGetTypeClass(type))
	{
		case CG_PARAMETERCLASS_SAMPLER:
			break;
		case CG_PARAMETERCLASS_SCALAR:
		case CG_PARAMETERCLASS_VECTOR:
			{
				float v[4] = {0,0,0,0};
//					cgGLGetParameter4f(parameter, v);

				//int nvalues = 0;
				//int i = 0;
				//const double* dvalues = cgGetParameterValues(parameter, CG_CURRENT, &nvalues);
				/*
				for (i=0; i<4; i++)
				{
					v[i] = (float) dvalues[i];
				}
				LOG ("creating data from '%s'=(%f,%f,%f,%f)\n", name, v[0],v[1],v[2],v[3]);
				*/
				data = new FFxDataVector(type, v);
			}
			break;
		case CG_PARAMETERCLASS_MATRIX:
			{
				float v[16];
	            cgGetMatrixParameterfc(parameter, v);
				LOG ("creating data from '%s'\n", name);
				data = new FFxDataMatrix(type, v);
			}
			break;
		case CG_PARAMETERCLASS_ARRAY:
			LOG ("ERROR: array not handled\n");
			break;
		case CG_PARAMETERCLASS_STRUCT:
			LOG ("ERROR: struct not handled\n");
			break;
		case CG_PARAMETERCLASS_OBJECT:
			LOG ("ERROR: object not handled\n");
			break;
		default:
			LOG ("unhandled parameter class for '%s'\n", name);
			break;
	}
	return data;
}
