///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxNavigator.h
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

#ifndef WXNAVIGATOR_H__
#define WXNAVIGATOR_H__

#include <wx/treectrl.h>

class wxTreeCtrl;
class wxTreeItemId;

///////////////////////////////////////////////////////////////////////////////////////////////////

class wxNavigator : public wxPanel //, public PSM_NodeCallBack
{
public:

    wxNavigator(wxWindow* parent, wxAuiManager* mgr = NULL);

    virtual ~wxNavigator();
/*
    virtual void NewNodeCreated(PSM_Node *pNode);
    virtual void NewParentedNode(PSM_Node *pNode, PSM_Node *pOldParent, PSM_Node *pNewParent);
    virtual void NodeUpdate(PSM_Node *pNode);
    virtual void NodeDeleted(PSM_Node *pNode);

    void SetSelectedNode(PSM_Node *pNode);
    */

	static void UpdateList();
private:

    DECLARE_EVENT_TABLE();
    wxTreeCtrl *mSceneGraphTree;
    wxTreeItemId mRootItem;

    void onSGTreeSelection(wxTreeEvent& event);
    void SelectTreeItem( wxTreeItemId selItem);
    void onStartDrag(wxTreeEvent& event);
    void onEndDrag(wxTreeEvent& event);
	void DoUpdateList();

	void AddRoot();


//    PSM_Node * mDragginNode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
