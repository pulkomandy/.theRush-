///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWDecoderNode.h
// Creation : 02/05/2009
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


#ifndef ZENFWDECODERNODE_H__
#define ZENFWDECODERNODE_H__

#include "ZenFWNode.h"


///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenFWDecoderNode;
extern ZenFWDecoderNode* mGDecoderService;

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenFWDecoderNode : public ZenFWNode
{
public:
	ZenFWDecoderNode() : ZenFWNode("DecoderNode")
	{
		mProcessingCount = 0;
		mGDecoderService = this;
	}
	virtual ~ZenFWDecoderNode()
	{
	}

	virtual void Run();


	ZDataResource<DataToLoad_t>& GetDataList() { return mDataList; }
	virtual unsigned int  GetProcessingCount() { return mProcessingCount;}
	virtual	bool HasJobTodo() { return (!mDataList.empty()); }
protected:
	unsigned int mProcessingCount;
	ZDataResource<DataToLoad_t> mDataList;
};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif
