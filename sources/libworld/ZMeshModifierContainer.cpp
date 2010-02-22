#include "stdafx.h"

ZMeshModifierContainer *GMeshModifierContainer;

BEGIN_SERIALIZATION(ZMeshModifier)
END_SERIALIZATION()

std::list<smartptr<ZMeshModifier> > ZMeshModifierContainer::mModifiers;

ZMeshModifier::ZMeshModifier()
{
	ZMeshModifierContainer::mModifiers.push_back(this);
}

ZMeshModifier::~ZMeshModifier()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZMeshModifierContainer::Tick(float aTimeEllapsed)
{
	PROFILER_START(MeshModifierTick);

	std::list<smartptr<ZMeshModifier> >::iterator iter = mModifiers.begin();
	for (; iter != mModifiers.end(); )
	{
		if ( (*iter)->GetRefCount() <= 1)
			iter = mModifiers.erase(iter);
		else
		{
			(*iter)->Tick(aTimeEllapsed);
			++iter;
		}
	}

	PROFILER_END();
}

	 