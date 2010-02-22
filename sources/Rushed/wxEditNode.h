///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxZEditNode.h
// Creation : 08/03/2007
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

#ifndef WXZEDITNODE_H__
#define WXZEDITNODE_H__
/*
#include <wx/laywin.h>
#include "foldpanelbar.h"
*/
#include "wxStackedControls.h"
#include "wxEdit.h"
//#include "wxEditTerrain.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

//class PSM_Node;

///////////////////////////////////////////////////////////////////////////////////////////////////

class wxZEditNode : public wxZEdit, public wxZEditHandler
{
public:

    wxZEditNode(wxWindow* parent, wxAuiManager* mgr = NULL) :
      wxZEdit(parent, mgr, this)
      {
            this->Connect(-1, 14000, (wxObjectEventFunction)&wxZEditNode::OnRefresh);
      }

      virtual ~wxZEditNode() {}


//    void SetSelectedNode(PSM_Node *aNode);
private:

//    PSM_Node *mNode;

    // Particle Edit
    unsigned int tmpGenerator;
    unsigned int tmpMotion;
    tcolor tmpColor;
    float tmpSize;
    float tmpAlpha;
    unsigned char tmpDisplay;
    wxGradientControl *mCurrentGradientControl;
    // Terrain

//    wxEditTerrain mEditTerrain;

    // handler callback
    virtual void HandleButtonClick(unsigned int aButtonID);
    virtual void HandleValueChanged(unsigned int aID);
    virtual void HandleGradientSelChanged(wxGradientControl *pControl);
    void OnRefresh(wxEvent& event);
    void AskRefresh();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
