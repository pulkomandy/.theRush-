#ifndef ZENFWVIEWER_H__
#define ZENFWVIEWER_H__

#include "ZenFWNode.h"

#include "../libbase/ZGameResources.h"

class ZMesh;
class ZenFWViewer;
extern ZenFWViewer *GViewer;
class ZenFWViewer : public ZenFWNode
{
public:
	ZenFWViewer() : ZenFWNode("ZenFWViewer")
	{
		GViewer = this;
		mbNewFileToView = false;
		mbAllResourceToScene = false;
		mViewerResources.Attach((ZGameResources*)_New(ZGameResources));
		mCameraBody = NULL;
	}
	virtual ~ZenFWViewer();

	void Init();
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run();

	virtual void SetActive(bool bActive);

	void SetFile(const char *szFile)
	{
		if (mFile != szFile)
		{
			mFile = szFile;
			mbNewFileToView = true;
			mViewerResources->Clear();
		}
	}
protected:
	bool mbNewFileToView;
	bool mbAllResourceToScene;
	tstring mFile;

	bool mbHasFocus;

	smartptr<ZGameResources> mViewerResources;

	//physic
	ZRigidBody *mCameraBody;
	
};

#endif
