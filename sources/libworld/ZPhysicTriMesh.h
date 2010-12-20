#ifndef ZPHYSICTRIMESH_H__
#define ZPHYSICTRIMESH_H__

#include "..\libbase\ZSmartPtr.h"
#include "..\libworld\ZMesh.h"
class ZMesh;

class btConcaveShape;

class ZPhysicTriMesh : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZPhysicTriMesh()
	{
		mIndices = NULL;
		mTrimesh = NULL;
		mNbIndices = NULL;
	}
	virtual ~ZPhysicTriMesh()
	{
		if (mIndices)
			delete [] mIndices;
	}

	static ZPhysicTriMesh* Build( ZMesh *pMesh );

	virtual tulong GetMemoryUsed() const;

	
	virtual ZMesh *GetMesh() const { return mMesh.ptr(); }
//	btCollisionObject mObject;
	int *mIndices;
	int mNbIndices;
	smartptr<ZMesh> mMesh;
	//smartptr<ZMesh> mMesh;
	btConcaveShape * mTrimesh;

};


#endif
