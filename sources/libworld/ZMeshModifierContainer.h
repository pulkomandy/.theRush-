#ifndef ZMESHMODIFIERCONTAINER_H__
#define ZMESHMODIFIERCONTAINER_H__

class ZMeshModifier : public ZBaseClass
{
public:
	DECLARE_SERIALIZABLE
public:
	ZMeshModifier();
	virtual ~ZMeshModifier();

	virtual void Tick(float /*aTimeEllapsed*/) { }

};

class ZMeshModifierContainer
{
private:
	ZMeshModifierContainer() {  }
	virtual ~ZMeshModifierContainer() {  }
public:
	static void Tick(float aTimeEllapsed);
	static void Clear()
	{
		mModifiers.clear();
	}
protected:
	static std::list<smartptr<ZMeshModifier> > mModifiers;
	friend class ZMeshModifier;
};

#endif
