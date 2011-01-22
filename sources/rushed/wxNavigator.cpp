///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxNavigator.cpp
// Creation : 08/11/2006
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

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include "wxNavigator.h"
#include "../libbase/LibBase.h"
/*
#include <wx/window.h>
#include <wx/grid.h>

#include <wx/spinctrl.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/image.h>
#include <wx/colordlg.h>
#include <wx/wxhtml.h>
#include <wx/stc/stc.h>
#include <wx/aui/aui.h>
#include "wxNavigator.h"
#include "wxMainFrame.h"
#include "wx3DPanel.h"
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxNavigator, wxPanel)
EVT_TREE_SEL_CHANGED(8, wxNavigator::onSGTreeSelection)
EVT_TREE_BEGIN_DRAG(8, wxNavigator::onStartDrag) 	//Begin dragging with the left mouse button.
EVT_TREE_END_DRAG(8, wxNavigator::onEndDrag)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::map<ZBaseClass *, wxTreeItemId> NodeTreeItemsLinkT;
NodeTreeItemsLinkT mNodeTreeItemsLink;
//extern wx3DPanel *m3D;


class wxTreeItemDataNode : public wxTreeItemData
{
public:
    wxTreeItemDataNode(ZBaseClass *pNode, wxTreeItemId tiID) : wxTreeItemData()
    {
        mNode = pNode;
		mSecondNode = NULL;
        mNodeTreeItemsLink[mNode] = tiID;
    }
    wxTreeItemDataNode(ZBaseClass *pNode, ZBaseClass *pSecondNode, wxTreeItemId tiID) : wxTreeItemData()
    {
        mNode = pNode;
		mSecondNode = pSecondNode;
        mNodeTreeItemsLink[mNode] = tiID;
    }

    virtual ~wxTreeItemDataNode()
    {
        mNodeTreeItemsLink.erase(mNode);
    }

    ZBaseClass *GetNode() { return mNode; }
	ZBaseClass *GetSecondNode() { return mSecondNode; }

protected:
    ZBaseClass *mNode, *mSecondNode;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
wxNavigator *GNav = NULL;
wxNavigator::wxNavigator(wxWindow* parent, wxAuiManager* mgr)
                     : wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
{
	GNav = this;
    wxToolBar *tb = new wxToolBar(this , 4, wxDefaultPosition, wxDefaultSize, wxTB_FLAT + wxTB_NODIVIDER); // --wxTB_FLAT + wxTB_NODIVIDER

    tb->SetToolBitmapSize(wxSize(16,16));

    wxBitmap tb_navigat_scenegraph("ZenithRes/NavigatSceneGraph.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_navigat_resources("ZenithRes/NavigatResources.bmp", wxBITMAP_TYPE_BMP);
    tb->AddTool(101, wxT("SceneGraph"), tb_navigat_scenegraph);
    tb->AddTool(102, wxT("Resources"), tb_navigat_resources);
    tb->Realize();

    mSceneGraphTree = new wxTreeCtrl(this, 8, wxDefaultPosition, wxSize(400, 200));

    // -- Nodes little bitmaps

    wxImageList *nImageList = new wxImageList(16, 16, 1, 4);
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeRoot.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //0      //PSM_NODE_TYPE_ROOT,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeCamera.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //1   //PSM_NODE_TYPE_CAMERA,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeLight.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));   //2   //PSM_NODE_TYPE_LIGHT,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeObject.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //3   //PSM_NODE_TYPE_OBJECT,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeDummy.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));   //4   //PSM_NODE_TYPE_DUMMY,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypePath.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));    //5   //PSM_NODE_TYPE_PATH,
    nImageList->Add(wxBitmap("./ZenithRes/NodeType3DSound.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //6   //PSM_NODE_TYPE_SOUND,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeWidget.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //7   //PSM_NODE_TYPE_WIDGET,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeOctreeNode.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //8//PSM_NODE_TYPE_OCTREENODE,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeOctree.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //9    //PSM_NODE_TYPE_OCTREE,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeLensflare.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //10 //PSM_NODE_TYPE_LENSFLARE,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeBone.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //11     //PSM_NODE_TYPE_BONES,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeSkeleton.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //12  //PSM_NODE_TYPE_SKELETON,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeWaypoint.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //13  //PSM_NODE_TYPE_WAYPOINT,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeTarget.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //14   //PSM_NODE_TYPE_TARGET,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeCloth.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //15    //PSM_NODE_TYPE_CLOTH,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeCluster.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //16  //PSM_NODE_TYPE_CLUSTER,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeOccluder.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //17  //PSM_NODE_TYPE_OCCLUDER,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypePortal.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //18   //PSM_NODE_TYPE_PORTAL,   // 'door' between 2 antiportal
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeAntiportal.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));//19 //PSM_NODE_TYPE_ANTIPORTAL, // room -> not used
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeTeleportal.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));//20 //PSM_NODE_TYPE_TELEPORTAL, // 'stargate'
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeDecal.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));   //21   //PSM_NODE_TYPE_DECAL,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeParticle.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //22  //PSM_NODE_TYPE_PARTICLE_INSTANCE,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeSprite.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197)); //23  //PSM_NODE_TYPE_PARTICLE_INSTANCE,
    nImageList->Add(wxBitmap("./ZenithRes/NodeTypeTerrain.bmp", wxBITMAP_TYPE_BMP), wxColour(197, 197, 197));  //24 //PSM_NODE_TYPE_PARTICLE_INSTANCE,
    mSceneGraphTree->AssignImageList(nImageList);


    // -- Root
	AddRoot();

    mSceneGraphTree->ExpandAll();
    mSceneGraphTree->SetBackgroundColour(wxColour(197, 197, 197));


    wxSizer *mSizer = new wxBoxSizer(wxVERTICAL);
    mSizer->Add(tb, 0, wxEXPAND + wxALL, 0);
    mSizer->Add(mSceneGraphTree, 1, wxEXPAND + wxALL, 0);
    SetSizer(mSizer);
//    PSM_World::SetNodeCallBack(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

wxNavigator::~wxNavigator()
{
//    PSM_World::SetNodeCallBack(NULL);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::AddRoot()
{
    mRootItem = mSceneGraphTree->AddRoot("Root", 0);
    mSceneGraphTree->SetItemData(mRootItem, NULL);
}

/*
void wxNavigator::NewNodeCreated(PSM_Node *pNode)
{
    if (pNode != gCurrentWorld->rootNode)
    {
        wxTreeItemId nItem = mSceneGraphTree->AppendItem(mRootItem, "UnnamedNode");//, int image = -1, int selImage = -1, wxTreeItemData* data = NULL)
        mSceneGraphTree->SetItemData(nItem, new wxTreeItemDataNode(pNode, nItem));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::NewParentedNode(PSM_Node *pNode, PSM_Node *pOldParent, PSM_Node *pNewParent)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::NodeUpdate(PSM_Node *pNode)
{
    if (pNode == gCurrentWorld->rootNode)
    {
        NodeDeleted(pNode);
        return;
    }

    NodeTreeItemsLinkT::iterator iter = mNodeTreeItemsLink.find(pNode);
    if (iter != mNodeTreeItemsLink.end())
    {
        //return (*iter).second;
        // update name
        if (pNode->type == PSM_NODE_TYPE_WIDGET)
        {
            mSceneGraphTree->Delete((*iter).second);
        }
        else
        {
            mSceneGraphTree->SetItemText((*iter).second, pNode->GetName().c_str() );
            mSceneGraphTree->SetItemImage((*iter).second, pNode->type);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::NodeDeleted(PSM_Node *pNode)
{
    NodeTreeItemsLinkT::iterator iter = mNodeTreeItemsLink.find(pNode);
    if (iter != mNodeTreeItemsLink.end())
    {
        mSceneGraphTree->Delete((*iter).second);
        wxTreeItemId curSel = mSceneGraphTree->GetSelection();
        if (curSel.IsOk())
            SelectTreeItem(curSel);
        else
            gMyFrame->SelectNode(NULL);
    }
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::onSGTreeSelection(wxTreeEvent& event)
{
    wxTreeItemId  selItem = event.GetItem();
    SelectTreeItem(selItem);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::SelectTreeItem( wxTreeItemId selItem)
{

    wxTreeItemDataNode* pndData = (wxTreeItemDataNode*)mSceneGraphTree->GetItemData(selItem);

    if (pndData)
    {
        //PSM_Node *pNode = pndData->GetNode();
        //gMyFrame->SelectNode(pNode, MyFrame::NODE_SELECT_SINGLE);
		ZBaseClass *pcls = pndData->GetNode();
		ZBaseClass *pclsSecond = pndData->GetSecondNode();
		TClassID id = pcls->GetClassID();
#ifdef FIXED
		if (id == ClassIDZLight)
		{
			m3D->SelectLight((ZLight*)pcls->QueryInstance());
			GZenithApp->mGizmosRendering.SetSelection((ZLight*)pcls->QueryInstance());
		}
		else if (id == ClassIDZOccluderBox)
		{
			m3D->SelectOccluder((ZOccluderBox*)pcls->QueryInstance());
			GZenithApp->mGizmosRendering.SetSelection((ZOccluderBox*)pcls->QueryInstance());
		} else if (id == ClassIDZCubeSkyDX9)
		{
			m3D->SelectEnv((ISky*)pcls);
			GZenithApp->mGizmosRendering.SetSelection(NULL);
		} else if (id == ClassIDZMetaEmitter)
		{
			m3D->SelectMetaEmitter((IMetaEmitter*)pcls->QueryInstance());
			GZenithApp->mGizmosRendering.SetSelection(NULL);
		} else if (id == ClassIDZParticleSystem)
		{
			m3D->SelectParticleSys((IMetaEmitter*)pclsSecond, (IParticleSystem*)pcls->QueryInstance());
			GZenithApp->mGizmosRendering.SetSelection(((IParticleSystem*)pcls->QueryInstance())->GetMeshInstance());
		}
		else if (id == ClassIDZSoundInstance)
		{
			m3D->SelectSoundInstance((ZSoundInstance*)pcls->QueryInstance());
			GZenithApp->mGizmosRendering.SetSelection(NULL);
		}
#endif
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void wxNavigator::SetSelectedNode(PSM_Node *pNode)
{
    NodeTreeItemsLinkT::iterator iter = mNodeTreeItemsLink.find(pNode);
    if (iter != mNodeTreeItemsLink.end())
    {
        if (mSceneGraphTree->GetSelection() != (*iter).second.m_pItem)
            mSceneGraphTree->SelectItem((*iter).second.m_pItem);
    }
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::onStartDrag(wxTreeEvent& event)
{
    /*
    event.Allow();
    mDragginNode = NULL;
    wxTreeItemDataNode* pndData = (wxTreeItemDataNode*)mSceneGraphTree->GetItemData(event.GetItem());
    if (pndData)
    {
        mDragginNode = pndData->GetNode();
    }
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::onEndDrag(wxTreeEvent& event)
{
    /*
    if (mDragginNode)
    {
        wxTreeItemDataNode* pndData = (wxTreeItemDataNode*)mSceneGraphTree->GetItemData(event.GetItem());
        if (pndData)
        {
            PSM_Node *mOverNode = pndData->GetNode();
            wxMessageBox(wxString(mOverNode->GetName())+" Is now parent of "+wxString(mDragginNode->GetName()));
        }
    }
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::DoUpdateList()
{
	// clear
	mSceneGraphTree->DeleteAllItems();
	AddRoot();
#ifdef FIXED
	// SkyBox/Envirroment
	wxTreeItemId nItem = mSceneGraphTree->AppendItem(mRootItem, "Envirronement", 24);
	mSceneGraphTree->SetItemData(nItem, new wxTreeItemDataNode(GetSky(), nItem));

	// occluder
	ZOccluderBox *pocc = (ZOccluderBox*)FirstInstanceOf(ZOccluderBox);
	while (pocc)
	{
		wxTreeItemId nItem = mSceneGraphTree->AppendItem(mRootItem, pocc->GetName().c_str(), 17);
		mSceneGraphTree->SetItemData(nItem, new wxTreeItemDataNode(pocc, nItem));

		pocc = (ZOccluderBox*)NI(pocc);
	}

	// Lights
	ZLight *plight = (ZLight*)FirstInstanceOf(ZLight);
	while (plight)
	{
		wxTreeItemId nItem = mSceneGraphTree->AppendItem(mRootItem, plight->GetName().c_str(), 2);
		mSceneGraphTree->SetItemData(nItem, new wxTreeItemDataNode(plight, nItem));

		plight = (ZLight*)NI(plight);
	}

	// Particles
	IMetaEmitter *pME = (IMetaEmitter *)FirstInstanceOf(ZMetaEmitter);
	while (pME)
	{
		wxTreeItemId nItem = mSceneGraphTree->AppendItem(mRootItem, pME->GetName().GetFileName().c_str(), 22);
		mSceneGraphTree->SetItemData(nItem, new wxTreeItemDataNode(pME, nItem));


		std::list<superEmit_t >::iterator iter = pME->GetEmittersList().begin();
		for (; iter != pME->GetEmittersList().end(); ++iter)
		{
			if ((*iter).mEmit)
			{
				wxTreeItemId ni = mSceneGraphTree->AppendItem(nItem, (*iter).mEmit->GetName().c_str(), 22);
				mSceneGraphTree->SetItemData(ni, new wxTreeItemDataNode((*iter).mEmit, pME, ni));
			}
			if ((*iter).mChildEmit)
			{
				wxTreeItemId ni = mSceneGraphTree->AppendItem(nItem, (*iter).mChildEmit->GetName().c_str(), 10);
				mSceneGraphTree->SetItemData(ni, new wxTreeItemDataNode((*iter).mChildEmit, pME, ni));
			}
		}

		pME = (IMetaEmitter *)NI(pME);
	}
	// Sounds
	ZSoundInstance *psound = (ZSoundInstance *)FirstInstanceOf(ZSoundInstance);
	while (psound)
	{
		wxTreeItemId nItem = mSceneGraphTree->AppendItem(mRootItem, psound->GetName().GetFileName().c_str(), 6);
		mSceneGraphTree->SetItemData(nItem, new wxTreeItemDataNode(psound, nItem));

		psound = (ZSoundInstance*)NI(psound);
	}
	mSceneGraphTree->ExpandAll();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxNavigator::UpdateList()
{
	//GNav->DoUpdateList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

