///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZenFWGizmosNode.cpp
// Creation : 02/06/2009
// Author : Cedric Guillemet
// Description : 
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../libplatform/ZenFWManager.h"
#include "../libplatform/ZenFWVRAMService.h"
#include "../libplatform/ZenFWLoaderNode.h"
#include "../libplatform/ZenFWGame.h"

#include "../libbase/LibBase.h"

#include "ZenFWGizmosNode.h"
#include "../libworld/libworld.h"
#include "../LibEdit/LibEdit.h"
#include "wxZ3DEdit.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
extern IGizmo *        m3DEdit->mGizmoEdit;
extern ZBaseClass *m3DEdit->mCurrentBaseClassEditing;
extern tmatrix mMetaEmitEditMatrix;
extern int mEditAnimationKey;
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

ZenFWGizmosNode::~ZenFWGizmosNode()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWGizmosNode::RenderChildClusters(ZCluster *pClust)
{
#ifdef WIN32
	const tvector4& wsph = pClust->GetBVolume().mBSphere;
	if (GetActiveCamera()->GetFrustum()->SphereInFrustum(wsph))
	{
		tmatrix mt(wsph.w, 0, 0, 0,
			0, wsph.w, 0, 0,
			0, 0, wsph.w, 0,
			wsph.x, wsph.y, wsph.z, 1);
		GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mt.m16 );
//		GDD->SetWorldMatrix(mt);
		wDummysph->Draw(0);

		std::list<ZCluster>& lc = pClust->mChildClusters;
		std::list<ZCluster>::iterator iter = lc.begin();
		for (; iter != lc.end(); ++iter)
		{
			RenderChildClusters( &(*iter) );
		}
	}
#endif
}

void ZenFWGizmosNode::RenderSEBVolumes(ZCluster *pClust)
{
#ifdef WIN32
	const tvector4& wsph = pClust->GetBVolume().mBSphere;
	if (GetActiveCamera()->GetFrustum()->SphereInFrustum(wsph))
	{


		std::list<smartptr<ZSpatialEntity> >::iterator iteres = pClust->mEntities.begin();
		for (;iteres != pClust->mEntities.end(); ++iteres)
		{
			ZSpatialEntity *pse = (*iteres);
			const tvector4& mws = pse->GetBVolume().GetBSphere();


			tmatrix mt(mws.w, 0, 0, 0,
				0, mws.w, 0, 0,
				0, 0, mws.w, 0,
				mws.x, mws.y, mws.z, 1);
			GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mt.m16 );
			//		GDD->SetWorldMatrix(mt);
			wDummysph->Draw(0);

		}



		

		std::list<ZCluster>& lc = pClust->mChildClusters;
		std::list<ZCluster>::iterator iter = lc.begin();
		for (; iter != lc.end(); ++iter)
		{
			RenderSEBVolumes( &(*iter) );
		}
	}
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWGizmosNode::SetSelection(ZSpatialEntity *pse)
{
	if (!pse)
	{
		if (m3DEdit->mGizmoEdit)
			m3DEdit->mGizmoEdit->SetEditTransform( NULL );

		m3DEdit->mCurrentBaseClassEditing = NULL;
		if (mWireSelectionBox)
			mWireSelectionBox->SetVisible(false);
		mSelectedSE = NULL;
		return;
	}
	if (mWireSelectionBox)
		mWireSelectionBox->SetVisible(true);
	mSelectedSE = pse;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWGizmosNode::Init()
{
	// selection box
    ZMesh * wbox = CreateFacetedBox(tvector3(1,1,1));
	 wirebox = CreateBox(tvector3(2,2,2), tcolor(1,1,1,0.5));
    mWireSelectionBox = (ZMeshInstance*)_New(ZMeshInstance);
    mWireSelectionBox->SetMesh(wbox);
    mWireSelectionBox->SetVisible(false);

    wDummysph = CreateDummySphere();
	wDummyCone = CreateDummyCone();

	mSelectionTexture = DirectLoadTex("./ZenithDatas/SelectionBoxTexture.dds");

/*
		miDummySph = (ZMeshInstance*)_New(ZMeshInstance);

		miDummySph->SetMesh(wDummysph);
		miDummySph->GetMaterial(0)->setEffect(DirectLoad("./ZenithDatas/Debug.fx"));
*/


	SetActive(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWGizmosNode::DrawGizmo(ZCamera *pCam)
{
    if ((m3DEdit->mGizmoEdit))
    {
        m3DEdit->mGizmoEdit->SetCameraMatrix(pCam->GetTransform()->GetWorldMatrix(), pCam->GetProjectionMatrix());
        m3DEdit->mGizmoEdit->SetCam(pCam);
		m3DEdit->mGizmoEdit->SetLocation(IGizmo::LOCATE_LOCAL);

		if (m3DEdit->mCurrentBaseClassEditing)
		{
			TClassID clsID = m3DEdit->mCurrentBaseClassEditing->GetClassID();
			if (clsID == ClassIDZOccluderBox)
			{
				m3DEdit->mGizmoEdit->SetEditTransform(((ZOccluderBox*)m3DEdit->mCurrentBaseClassEditing->QueryInstance())->GetTransform());
			}
#ifdef FIXED
			else if (clsID == ClassIDZMetaEmitter)
			{
				mMetaEmitEditMatrix = ((IMetaEmitter*)m3DEdit->mCurrentBaseClassEditing)->GetGenerationMatrix();
				m3DEdit->mGizmoEdit->SetEditMatrix(&mMetaEmitEditMatrix);
			}
#endif
			else if (clsID == ClassIDZMeshInstance)
			{
				m3DEdit->mGizmoEdit->SetEditTransform(((ZMeshInstance*)m3DEdit->mCurrentBaseClassEditing->QueryInstance())->GetTransform());
			}
			else if (clsID == ClassIDZLight)
			{
				m3DEdit->mGizmoEdit->SetEditTransform(((ZLight*)m3DEdit->mCurrentBaseClassEditing->QueryInstance())->GetTransform());
			}
			else if (clsID == ClassIDZAnimation)
			{
				ZAnimation* dt = (ZAnimation*)m3DEdit->mCurrentBaseClassEditing->QueryInstance();
				ZAnimationTrack<tvector3>* zat = (ZAnimationTrack<tvector3>*)dt->m_AnimTrackList[0];
				ZAnimationTrack<tquaternion>* zar = (ZAnimationTrack<tquaternion>*)dt->m_AnimTrackList[1];
				m3DEdit->mGizmoEdit->SetEditVQS(&zat->m_KeyFrameList[m3DEdit->mEditAnimationKey].m_Value,
					&zar->m_KeyFrameList[m3DEdit->mEditAnimationKey].m_Value,
					NULL);
			}
			else if (clsID == ClassIDZSoundInstance)
			{
				ZTransform *pTrans = ((ZSoundInstance*)m3DEdit->mCurrentBaseClassEditing)->getTransform();
				if ( pTrans )
				{
					m3DEdit->mGizmoEdit->SetEditTransform( pTrans );
				}
			}

			
			GDD->SetWorldMatrix(tmatrix::identity);//(PSM_TRANSFORM_WORLD, IDMATRIX);
			m3DEdit->mGizmoEdit->Draw();
		}

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenFWGizmosNode::Run()
{
	if (IsActive())
	{

		ZenFWNode::Run();


		// dummy lights/gizmo
		if (GTrack->IsDebugEditGizmoVisible())
		{
#ifdef WIN32
			tmatrix mat;
			mat.Identity();
			GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mat.m16 );
			mat = GDD->GetViewMatrix();
			GDD->GetD3D9Device()->SetTransform(D3DTS_VIEW, (const D3DMATRIX *)mat.m16 );
			mat = GDD->GetProjectionMatrix();
			GDD->GetD3D9Device()->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX *)mat.m16 );

			// gizmo

			ZCamera *pCam = GetActiveCamera();
			DrawGizmo(pCam);
			GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
			GDD->GetD3D9Device()->SetTexture(0,NULL);
			GDD->GetD3D9Device()->SetTexture(1,NULL);
			GDD->GetD3D9Device()->SetTexture(2,NULL);
			GDD->GetD3D9Device()->SetTexture(3,NULL);
			GDD->GetD3D9Device()->SetTexture(4,NULL);
			GDD->GetD3D9Device()->SetTexture(5,NULL);
			GDD->GetD3D9Device()->SetTexture(6,NULL);
			GDD->GetD3D9Device()->SetTexture(7,NULL);
			GDD->GetD3D9Device()->SetRenderState(D3DRS_COLORVERTEX , true);
			//GDD->GetD3D9Device()->SetRenderState(D3DRS_LIGHTING , true);
			GDD->GetD3D9Device()->SetRenderState(D3DRS_AMBIENT  , 0xFFFFFFFF);
			GDD->GetD3D9Device()->SetVertexShader(NULL);
			GDD->GetD3D9Device()->SetPixelShader(NULL);

			// lights

			if (mbDrawLights)
			{
				ZLight* pLight = (ZLight*)FirstInstanceOf(ZLight);
				while (pLight)
				{
					if (pLight->GetLightType() == LT_POINTLIGHT)
					{
						const tvector3& ps = pLight->GetTransform()->GetWorldMatrix().V4.position;
						tmatrix mt(pLight->GetRadius(), 0, 0, 0,
							0, pLight->GetRadius(), 0, 0,
							0, 0, pLight->GetRadius(), 0,
							ps.x, ps.y, ps.z, 1);

						GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mt.m16 );
						wDummysph->Draw(0);
					} else if (pLight->GetLightType() == LT_SPOT)
					{

						float valsp1 = pLight->GetLength()*pLight->GetSpotAngle2();
						tmatrix mt(valsp1, 0, 0, 0,
							0, valsp1, 0, 0,
							0, 0, pLight->GetLength(), 0,
							0, 0, 0, 1);
						mt *= pLight->GetTransform()->GetWorldMatrix();

						GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mt.m16 );
						wDummyCone->Draw(0);
					}


					pLight = (ZLight*)NI(pLight);
				}
			}
			// clusters
			if (mbDrawClusters)
				RenderChildClusters((GScene)->GetRootCluster());

			if (mbDrawSEBVolume)
				RenderSEBVolumes((GScene)->GetRootCluster());
			// occluders;
			GDD->GetD3D9Device()->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
			GDD->GetD3D9Device()->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
			GDD->GetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			GDD->GetD3D9Device()->SetRenderState(D3DRS_ZENABLE, false);

			if (mbDrawOccluders)
			{
				ZOccluderBox *pocc = (ZOccluderBox*)FirstInstanceOf(ZOccluderBox);
				while (pocc)
				{
					//GDD->SetWorldMatrix(pocc->GetTransform()->GetWorldMatrix());
					GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)pocc->GetTransform()->GetWorldMatrix().m16 );
					wirebox->Draw(0);
					pocc = (ZOccluderBox*)NI(pocc);
				}
			}

			// 	selected track mesh

			if (mWireSelectionBox->IsVisible())
			{
				mSelectionTexture->Bind(0);
				//GDD->SetWorldMatrix(mWireSelectionBox->GetTransform()->GetWorldMatrix());
				tmatrix mat;
				tvector3 vtptmin, vtptmax;
				vtptmin = mSelectedSE->GetBVolume().GetBoxMin();
				vtptmax = mSelectedSE->GetBVolume().GetBoxMax();

				mat.Translation((vtptmin+vtptmax)*0.5f);
				mat.m16[0]= vtptmax.x-vtptmin.x;
				mat.m16[5]= vtptmax.y-vtptmin.y;
				mat.m16[10]= vtptmax.z-vtptmin.z;
				mWireSelectionBox->GetTransform()->SetLocalMatrix(mat);
				mWireSelectionBox->GetTransform()->Update();

				GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mWireSelectionBox->GetTransform()->GetWorldMatrix().m16 );
				mWireSelectionBox->GetMesh()->Draw(0);
			}
			GDD->GetD3D9Device()->SetRenderState(D3DRS_ZENABLE, true);

			// 3D sound instances
			if (mbDrawSound)
			{
				ZSoundInstance* p3DSnd = (ZSoundInstance*)FirstInstanceOf(ZSoundInstance);
				while (p3DSnd)
				{
					if (p3DSnd->isPlaying()&&p3DSnd->getTransform())
					{
						const tvector3& ps = p3DSnd->getTransform()->GetWorldMatrix().V4.position;
						tmatrix mt(p3DSnd->getInnerAttenuationRadius(), 0, 0, 0,
							0, p3DSnd->getInnerAttenuationRadius(), 0, 0,
							0, 0, p3DSnd->getInnerAttenuationRadius(), 0,
							ps.x, ps.y, ps.z, 1);

						GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mt.m16 );
						wDummysph->Draw(0);
						tmatrix mt2(p3DSnd->getOuterAttenuationRadius(), 0, 0, 0,
							0, p3DSnd->getOuterAttenuationRadius(), 0, 0,
							0, 0, p3DSnd->getOuterAttenuationRadius(), 0,
							ps.x, ps.y, ps.z, 1);

						GDD->GetD3D9Device()->SetTransform(D3DTS_WORLD, (const D3DMATRIX *)mt2.m16 );
						wDummysph->Draw(0);
					} 

					p3DSnd = (ZSoundInstance*)p3DSnd->mNextInstance;
				}
			}

			// mesh debug
			/*
			ZMeshInstance *pInst = (ZMeshInstance*)FirstInstanceOf(ZMeshInstance);
			while (pInst)
			{
			const tvector4& wsph = pInst->GetWorldBSphere();
			const tvector3& ps = wsph;
			tmatrix mt(wsph.w, 0, 0, 0,
			0, wsph.w, 0, 0,
			0, 0, wsph.w, 0,
			wsph.x, wsph.y, wsph.z, 1);

			GDD->SetWorldMatrix(mt);
			wDummysph->Draw(0);

			pInst = (ZMeshInstance*)pInst->mNextInstance;
			}
			*/
			//RenderChildClusters((GScene)->GetRootCluster());

			extern RenderingState_t GRenderingState;
			GRenderingState.mAlphaTestEnable = 0;
			GRenderingState.mZWriteEnable = 1;
			GRenderingState.mBlending = 2;
			GRenderingState.mZTestType = 1;
#endif
		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
