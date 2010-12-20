#include "ZAtmosphere.h"

#include "ZMeshBuilder.h"

ZAtmosphere *gAtmosphere = NULL;

BEGIN_SERIALIZATION(ZAtmosphere)
END_SERIALIZATION()

ZAtmosphere::ZAtmosphere()
{
	gAtmosphere = this;
	// Create sky mesh
	mSkyboxMesh.Attach( (ZMeshInstance*)_New(ZMeshInstance));
	mSkyboxMesh->SetMesh(CreateSkyBox());
	ZMaterial *pMat = mSkyboxMesh->GetMaterial(0);
	pMat->setEffect(DirectLoad("./ZenithDatas/Skybox.fx"));
	pMat->addTexture(DirectLoadTex("./ZenithDatas/checker.dds"));

	mDefaultSkyColor = tcolor(0.2f,0.2f,0.8f,1.f);
}

ZAtmosphere::~ZAtmosphere()
{
	if (gAtmosphere == this)
		gAtmosphere = NULL;
	mSkyboxMesh->GetTransform()->SubRef();
}

void ZAtmosphere::LoadSkybox(const char *szSkyboxFileName)
{
	ZMaterial *pMat = mSkyboxMesh->GetMaterial(0);
	pMat->setTexture(0, DirectLoadTex(szSkyboxFileName));
}
