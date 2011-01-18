#ifndef ZENFWSTDNODES_H__
#define ZENFWSTDNODES_H__

#include "../libworld/ZMeshModifierContainer.h"
#include "../libbase/ZTimer.h"

class ZenFWRenderQueueBuilder;
extern ZenFWRenderQueueBuilder *GRQBNode;
class ZenFWRenderQueueBuilder : public ZenFWNode
{
public:
	ZenFWRenderQueueBuilder() : ZenFWNode("ZenFWRenderQueueBuilder")
	{
		GRQBNode = this;
	}
	virtual ~ZenFWRenderQueueBuilder()
	{
	}

	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run()
	{
		if (GDD->GetClassID() == ClassIDZDisplayDeviceNULL)
			GScene->Tick();
		else
			GScene->BuildRenderingQueue(GDD->GetQueueToBuild(), GetActiveCamera());
	}
};
/*
class ZenFWPhysic : public ZenFWNode
{
public:
	ZenFWPhysic() : ZenFWNode("ZenFWPhysic")
	{
	}
	virtual ~ZenFWPhysic()
	{
	}

	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 6; }
	virtual void Run()
	{
		//GBullet->Tick(gTimer.GetEllapsed());
	}
};
*/
class ZenFWPresent : public ZenFWNode
{
public:
	ZenFWPresent() : ZenFWNode("ZenFWPresent")
	{
	}
	virtual ~ZenFWPresent()
	{
	}

	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 1; }
	virtual void Run()
	{
		GDD->EndScene();
		GDD->Present();
		LogFramesPerSecond();	
	}
};

class ZenFWSleep : public ZenFWNode
{
public:
	ZenFWSleep(int aNbms) : ZenFWNode("ZenFWSleep")
	{
		mNbMs =	aNbms;
	}
	virtual ~ZenFWSleep()
	{
	}

	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run()
	{
		ZSleep(mNbMs);
	}
protected:
	int mNbMs;
};

class ZenFWMeshModifiers : public ZenFWNode
{
public:
	ZenFWMeshModifiers() : ZenFWNode("ZenFWMeshModifiers")
	{
	}
	virtual ~ZenFWMeshModifiers()
	{
		ZMeshModifierContainer::Clear();
	}

	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run()
	{
		ZMeshModifierContainer::Tick(gTimer.GetEllapsed());
	}
protected:

};

#endif
