#include "ZRenderQueue.h"

#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"
#include "libworld/ZAtmosphere.h"
#include "libgame/ZRushGame.h"
#include "libworld/ZHUDMeshModifier.h"


void ZRenderQueue::Begin()
{
	mSolids.clear();
}

void ZRenderQueue::End()
{
}

int frameNb = 0;
void ZRenderQueue::Rasterize(int aProfile)
{
	PROFILER_START(Rasterize);

	//RenderCascadedShadowMaps(mShadowMapsBuffer);


	GDD->SetProjectionMatrix(mProjMat);
	GDD->SetViewMatrix(mViewMat);
		
	if (!mSolids.empty())
	{
		
		std::vector<MeshEntry_t>::iterator iter = mSolids.begin();
		for (; iter != mSolids.end(); ++iter)
		{
			MeshEntry_t & pt = (*iter);
			GDD->SetWorldMatrix(pt.mt);
			
			if (!pt.mMI->GetMaterial(0)->getTextureArray().empty())
				pt.mMI->GetMaterial(0)->getTextureArray()[0]->Bind(0);
				
			//gAtmosphere->GetSkybox()->GetMaterial(0)->getTextureArray()[0]->Bind(0);
				
			

			pt.mMI->GetMesh()->Draw(0);
			
		}
	
	}

	// Sky

	if (aProfile != RENDER_PROFILE_NETBOOK)
		DrawMeshInstance(gAtmosphere->GetSkybox());

	// 2D gui
	ZMeshInstance *mgMesh = (ZMeshInstance*)FirstInstanceOf(ZMeshInstance);
	while (mgMesh)
	{
		if (mgMesh->IsGui2DMesh()&&mgMesh->IsVisible())
		{
			GDD->SetWorldMatrix(mgMesh->GetTransform()->GetWorldMatrix());
			DrawMeshInstance(mgMesh);
		}

		mgMesh = (ZMeshInstance*)mgMesh->GetNext();
	}
	// temporary HUD
	if (GGame && GGame->GetClientGame() )
	{
		ZGame *pGame = GGame->GetClientGame();
		if ( (pGame->GetClassID() == ClassIDZRushNetGameClient ) || (pGame->GetClassID() == ClassIDZRushGame) )
		{
			if (((ZRushGame*)GGame->GetClientGame())->GetHUD())
			{
				mgMesh = ((ZRushGame*)GGame->GetClientGame())->GetHUD()->GetMeshInstance();
				if (mgMesh->IsVisible())
				{
					GDD->SetWorldMatrix(mgMesh->GetWorldMatrix());
					DrawMeshInstance(mgMesh);
				}
			}
		}
	}
	// unbind streams
	if (mPreviousIA)
	{
		mPreviousIA->Unbind();
		mPreviousIA->SubRef();
		mPreviousIA = NULL;
	}

	if (mPreviousVA)
	{
		mPreviousVA->Unbind();
		mPreviousVA->SubRef();
		mPreviousVA = NULL;
	}
	// clean shaders
	CleanPasses();

	//DisplayShadowMaps();

	PROFILER_END();
}


void ZRenderQueue::DrawMeshInstance(ZMeshInstance *mgMesh) const
{

	//uint nbsu = mgMesh->GetMesh()->GetNbSubMeshes();
	for (unsigned int iasm = 0;iasm<mgMesh->GetMesh()->GetNbSubMeshes(); iasm++)
	{

		ZFx *nFX = mgMesh->GetMaterial(iasm)->getEffect();
		nFX->selectTechniqueByIndex(mgMesh->GetMaterial(iasm)->GetTechnic());


		if (mgMesh->GetMaterial(iasm)->open())
		{
			bool bUseScisor = false;
			/*
			if (mgMesh->GetClipperParent())
			{
				GDD->SetScissorRect(mgMesh->GetClipperParent()->GetPosx(),
					mgMesh->GetClipperParent()->GetPosy(),
					mgMesh->GetClipperParent()->GetSizex(),
					mgMesh->GetClipperParent()->GetSizey());
				GDD->EnableScissor(true);
				bUseScisor = true;
			}
			*/


			//mgMesh->GetMaterial(iasm)->open();

			//int nbPass = nFX->getNumPasses();

			nFX->setPassState(0);

			IOcclusionQuery *pOQuery = mgMesh->GetOcclusionQuery();
			if (pOQuery)
			{
				//
				pOQuery->StartOcclusionQuery();
				mgMesh->GetMesh()->Draw(iasm);
				pOQuery->EndOcclusionQuery();

				mgMesh->SetLastFrameNumberOfPixelsRendered(pOQuery->GetNumberOfPixelsRendered());

			}
			else
			{
				mgMesh->GetMesh()->Draw(iasm);
			}

/*
			if (bUseScisor)
			{
				GDD->EnableScissor(false);
			}
			*/
		}
		else
		{
			mgMesh->GetMesh()->Draw(iasm);
		}

	} // submesh
}
