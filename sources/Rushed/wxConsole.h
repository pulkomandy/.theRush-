///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxConsole.h
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

#ifndef WXCONSOLE_H__
#define WXCONSOLE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

class ZenithConsole : public wxPanel
{
public:

    ZenithConsole(wxWindow* parent, wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     wxAuiManager* mgr = NULL);

    virtual ~ZenithConsole();

    wxAuiDockArt* GetDockArt();
    void DoUpdate();
    void AddText(const char* pszText);
private:

    void OnEraseBackground(wxEraseEvent& event);
    void OnCommandEnter(wxCommandEvent& event);
    void OnIdle(wxIdleEvent &event);


private:
    wxTextCtrl *mTextOut;
    wxComboBox *mCombo;
    wxBoxSizer *mSizer;

    wxTextAttr *logAttr, *cmdAttr, *errAttr;

    DECLARE_EVENT_TABLE();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
