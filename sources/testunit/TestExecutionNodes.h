
class FakeRender : public ZenFWNode
{
public:
	FakeRender() : ZenFWNode("FakeRender")
	{
	}
	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 10; }
	virtual void Run() 
	{ 
		ZSleep(5);	
	}
};

class FakePresentation : public ZenFWNode
{
public:
	FakePresentation() : ZenFWNode("FakePresentation")
	{
		mCount = 0;
	}
	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 1; }
	virtual void Run() 
	{ 
		ZSleep(1);	
		mCount++;
		if (mCount >= 20)
		{
			ZManager->AskToQuit();
		}

	}

	int mCount;
};
class FakePresentationNoQuit : public ZenFWNode
{
public:
	FakePresentationNoQuit() : ZenFWNode("FakePresentationNoQuit")
	{
	}
	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 1; }
	virtual void Run() 
	{ 
		ZSleep(1);	
		
	}

	int mCount;
};

class FakeScenegraph : public ZenFWNode
{
public:
	FakeScenegraph() : ZenFWNode("FakeScenegraph")
	{
	}
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep(2);	
	}
};

class FakeGameplay : public ZenFWNode
{
public:
	FakeGameplay() : ZenFWNode("FakeGameplay")
	{
	}
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep(4);	
	}
};

class FakeWindowInput : public ZenFWNode
{
public:
	FakeWindowInput() : ZenFWNode("FakeWindowInput")
	{
	}
	virtual int GetPreferredLine() const { return 0; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep(1);	
	}
};


class FakeSound : public ZenFWNode
{
public:
	FakeSound() : ZenFWNode("FakeSound")
	{
	}
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep(1);	
	}
};

class FakeAnimation : public ZenFWNode
{
public:
	FakeAnimation() : ZenFWNode("FakeAnimation")
	{
	}
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep(2);	
	}
};

class FakeParticlesAndFX : public ZenFWNode
{
public:
	FakeParticlesAndFX() : ZenFWNode("FakeParticlesAndFX")
	{
	}
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep(3);	
	}
};

class FakeDecoder : public ZenFWNode
{
public:
	FakeDecoder() : ZenFWNode("FakeDecoder")
	{
	}
	virtual int GetPreferredLine() const { return -1; }
	virtual unsigned int GetPriority() const { return 5; }
	virtual void Run() 
	{ 
		ZSleep((rand()&1)?4:0);	
	}
};