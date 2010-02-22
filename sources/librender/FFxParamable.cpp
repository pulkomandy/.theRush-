///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : FFxParamable.cpp
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

#include "stdafx.h"
#include "FFx.h"



//---------------------------------------------------------------------------------
// FFxParamable
//---------------------------------------------------------------------------------

FFxParamable::~FFxParamable()
{
	// delete params
	foreach (param, mParams, FFxParam*)
	{
		delete (*param);
	}

	// delete params
	foreach (param, mMatrixParams, FFxParam*)
	{
		delete (*param);
	}
}

FFxParam*	FFxParamable::getNamedParam(const tstring& aName) const
{
	foreach_const (param, mParams, FFxParam*)
		if ((*param)->mName == aName)
			return *param;
	return 0;
}

void	FFxParamable::addParam(FFxParam* aParam)
{
	mParams.push_back(aParam);
}

FFxParam*	FFxParamable::getNamedMatrixParam(const tstring& aName) const
{
	foreach_const (param, mMatrixParams, FFxParam*)
		if ((*param)->mName == aName)
			return *param;
	return 0;
}

void	FFxParamable::addMatrixParam(FFxParam* aParam)
{
	mMatrixParams.push_back(aParam);
}
