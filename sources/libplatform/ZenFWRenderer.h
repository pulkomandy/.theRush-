#ifndef ZENFWRENDERER_H__
#define ZENFWRENDERER_H__

#include "../libworld/libworld.h"
#include "../libgame/libgame.h"

class ZSoundManager;

class ZenFWRenderer : public ZenFWNode
{
public:
	ZenFWRenderer() : ZenFWNode("ZenFWRenderer")
	{
		mSoundRenderer = NULL;
	}
	virtual ~ZenFWRenderer();

	void Init();
	void InitForEditor(WindowHandle_t windowHandle);

	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 10; }
	virtual void Run();
protected:
	smartptr<ZCamera> mCam;
	smartptr<ZAtmosphere> mAtmosphere;
	// Gui
	smartptr<IProtoGUI> mProtoGui;
	// Sound
	ZSoundManager* mSoundRenderer;
	int mRenderProfile;
	void PrivateInit(const INITPARAMS& RendererInitParams);
};

#endif
