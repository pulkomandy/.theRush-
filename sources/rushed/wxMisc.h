///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxZenDialog.h
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

#ifndef WXMISC_H__
#define WXMISC_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

class wxAboutDialog : public wxDialog
{
public:

    wxAboutDialog(wxWindow* parent, const wxString &caption);

    ~wxAboutDialog() {}


private:
    void OnOK(wxCommandEvent& WXUNUSED(event) );

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxAboutDialog)
    DECLARE_NO_COPY_CLASS(wxAboutDialog)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
