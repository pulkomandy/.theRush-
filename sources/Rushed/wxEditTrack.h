///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxZEditTrack.h
// Creation : 08/08/2007
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

#ifndef WXZEDITTRACK_H__
#define WXZEDITTRACK_H__

#include <wx/laywin.h>
#include "foldpanelbar.h"
#include "wxStackedControls.h"
#include "wxEdit.h"
//#include "wxEditTerrain.h"
class Zen3DEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

class wxZEditTrack : public wxZEdit, public wxZEditHandler
{
public:

    wxZEditTrack(wxWindow* parent, wxAuiManager* mgr = NULL) :
      wxZEdit(parent, mgr, this)
      {
            this->Connect(-1, 14000, (wxObjectEventFunction)&wxZEditTrack::OnRefresh);
            worldNumber = 0;
            currentPiece = 0;
			mBonusLeft = 0;
			mBonusRight = 0;
			mBonusDecal = 7.f;

            BuildInterface();
			mReplaceMeshCount = 1;
			mAddMeshAfterCount = 1;
			mAddMeshBeforeCount = 1;
			mRemoveMeshCount = 1;


      }

      virtual ~wxZEditTrack() {}
      void BuildInterface();
	  void Refresh();

	  Zen3DEdit *m3DEdit;
private:

//    PSM_Node *mNode;

    // Particle Edit
    /*
    unsigned int tmpGenerator;
    unsigned int tmpMotion;
    tcolor tmpColor;
    float tmpSize;
    float tmpAlpha;
    unsigned char tmpDisplay;
    wxGradientControl *mCurrentGradientControl;
    */
    unsigned int worldNumber;
    unsigned int prefabNumber;
    unsigned int currentPiece;


    // handler callback
    virtual void HandleButtonClick(unsigned int aButtonID);
    virtual void HandleValueChanged(unsigned int aID);
    virtual void HandleGradientSelChanged(wxGradientControl *pControl);
    void OnRefresh(wxEvent& event);
    void AskRefresh();

    tstring curWorld;
	tstring worldTrack;

	float mBonusDecal;
	unsigned int mBonusLeft, mBonusRight;
	void SetSelection(ZSpatialEntity *pse);

	// bricks count
	unsigned int mReplaceMeshCount;
	unsigned int mAddMeshAfterCount;
	unsigned int mAddMeshBeforeCount;
	unsigned int mRemoveMeshCount;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
