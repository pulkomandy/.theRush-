///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWLoaderNode.h
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


#ifndef ZENFWLOADERNODE_H__
#define ZENFWLOADERNODE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////


class ZenFWNode;

class ZenFWLoaderNode;
extern ZenFWLoaderNode *mGLoaderService;

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenFWLoaderNode : public ZenFWNode
{
public:
	ZenFWLoaderNode() : ZenFWNode("LoaderNode")
	{
		mbIsActive = true;
		mLastFrame = 0;
		mProcessingCount = 0;
		mGLoaderService = this;
		mMetaDatas.Init("./MetaDatas.xml");
	}
	virtual ~ZenFWLoaderNode()
	{
	}


	virtual int GetPreferredLine() const { return DISKIOLANE; }
	virtual void Run();

	//ZBaseClass* CreateInstance(void *pParam, bool bAddToScene = false);
	ZBaseClass* CreateInstance(const char *szName, ZGameResources *pGR = NULL);
	//ZDataResource<DataToLoad_t>& GetDataList() { return mDataList; }
	virtual unsigned int  GetProcessingCount() { return mProcessingCount;}
	bool IsDataListEmpty() { return mDataList.empty(); }
protected:

	ZDataResource<DataToLoad_t> mDataList;

	unsigned int mProcessingCount;
	ZMetaDatas	mMetaDatas;

};

///////////////////////////////////////////////////////////////////////////////////////////////////


#endif
