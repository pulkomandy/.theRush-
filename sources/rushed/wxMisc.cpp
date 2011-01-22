///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxZenDialog.cpp
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
#include "wxMisc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxAboutDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxAboutDialog::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxAboutDialog, wxDialog)


///////////////////////////////////////////////////////////////////////////////////////////////////

wxAboutDialog::wxAboutDialog(wxWindow *parent, const wxString &caption)
: wxDialog(parent, wxID_ANY, caption, wxDefaultPosition, wxSize(432,720),
                            wxDEFAULT_DIALOG_STYLE)
{
    wxBitmapButton *bmpBut = new wxBitmapButton(this, wxID_OK, wxBitmap(wxString(_T("./ZenithRes/ZenithSplash.png")), wxBITMAP_TYPE_PNG),
        wxPoint(0,0), wxSize(432, 720));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxAboutDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
    EndModal(wxID_OK);
}
