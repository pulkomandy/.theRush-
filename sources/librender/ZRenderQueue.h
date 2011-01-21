#ifndef ZRENDERQUEUE_H__
#define ZRENDERQUEUE_H__

#include "../libworld/ZMesh.h"
#include "../libworld/ZMeshInstance.h"
#include "../libworld/ZCascadedShadows.h"

#include "../libbase/ZFrustum.h"

class ZRenderQueue
{
public:
	ZRenderQueue()
	{
		mProjMat.PerspectiveFovLH2(1.f, 1.f, 0.1f, 1.f);
		mViewMat.Identity();
		memset(mShadowMapsBuffer, 0, sizeof(shadowmap_t) * 4);
	}
	virtual ~ZRenderQueue()
	{
	}

	void AddSolid(ZMeshInstance *pMesh)
	{
		mSolids.push_back(MeshEntry_t(pMesh, pMesh->GetWorldMatrix()));
	}
	void Rasterize(int aProfile);

	void Begin();
	void End();
	void SetViewProjection(const tmatrix& viewMat, const tmatrix& projMat)
	{
		mViewMat = viewMat;
		mProjMat = projMat;
	}
	void SetFrustum(const ZFrustum & frus)
	{
		mFrustum = frus;
	}
	const ZFrustum & GetFrustum()
	{
		return mFrustum;
	}

	shadowmap_t * GetShadowMapBuffer() { return mShadowMapsBuffer; }
protected:
	shadowmap_t mShadowMapsBuffer[4];

	typedef struct MeshEntry_t
	{
		MeshEntry_t(ZMeshInstance *pmi, const tmatrix& amt)
		{
			mMI.Attach(pmi);
			mt = amt;
		}

		smartptr<ZMeshInstance> mMI;
		tmatrix mt;
	} MeshEntry_t;
	std::vector<MeshEntry_t> mSolids;

	// frame view infos
	tmatrix mViewMat, mProjMat;
	ZFrustum mFrustum;

	void DrawMeshInstance(ZMeshInstance *mgMesh) const;

};

#endif
