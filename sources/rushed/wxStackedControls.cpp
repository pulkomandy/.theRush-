///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxStackedControls.cpp
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

//#include "wxStdAfx.h"
#include <wx/wx.h>
/*
#include <wx/window.h>
#include <wx/grid.h>
#include <wx/treectrl.h>
#include <wx/spinctrl.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/image.h>
#include <wx/colordlg.h>
#include <wx/wxhtml.h>
*/
#include "wxGradientControl.h"
#include "wxStackedControls.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenTextEditRoot, wxPanel)
EVT_COMMAND_KILL_FOCUS(3, wxZenTextEditRoot::LostFocus)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenSlider, wxPanel)
EVT_SCROLL(wxZenSlider::Scroll)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenCheckBox, wxPanel)
EVT_CHECKBOX(2, wxZenCheckBox::OnCheck)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenCheckBoxBitField, wxPanel)
EVT_CHECKBOX(2, wxZenCheckBoxBitField::OnCheck)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenComboBox, wxPanel)
EVT_COMBOBOX(2, wxZenComboBox::OnSelChange)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(wxZenRadioBox, wxPanel)
EVT_RADIOBOX(2, wxZenRadioBox::OnSelChange)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenColorPicker, wxPanel)
EVT_COLOURPICKER_CHANGED(2, wxZenColorPicker::OnColorChanged)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenFileOpen, wxPanel)
EVT_BUTTON(2, wxZenFileOpen::OnOpen)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenDirectoryOpen, wxPanel)
EVT_BUTTON(2, wxZenDirectoryOpen::OnOpen)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenButton, wxPanel)
EVT_BUTTON(2, wxZenButton::OnClick)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenGradient, wxPanel)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenToggleButton, wxPanel)
EVT_TOGGLEBUTTON(2, wxZenToggleButton::OnToggle)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenToggleButtonPlugin, wxPanel)
EVT_TOGGLEBUTTON(2, wxZenToggleButtonPlugin::OnToggle)
END_EVENT_TABLE()

wxZenToggleButtonPlugin* wxZenToggleButtonPlugin::gLastClickedToggleButton = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxZenTextList, wxPanel)
EVT_LISTBOX_DCLICK(2, wxZenTextList::OnDblClick)
EVT_LISTBOX(2, wxZenTextList::OnClick)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(wxZenTreeView, wxPanel)
EVT_TREE_SEL_CHANGED(2, wxZenTreeView::OnTVSelChanged)
END_EVENT_TABLE()
