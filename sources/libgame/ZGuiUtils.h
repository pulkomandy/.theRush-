///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiUtils.h
// Creation : 13/04/2009
// Author : Cedric Guillemet
// Description :
// Reliability :
//
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

#ifndef ZGUIUTILS_H__
#define ZGUIUTILS_H__

class ZProtoGUI;
#include "CEGUI.h"
using namespace CEGUI;

void CEGUICBSel(Combobox *pcb, int idx);
void LeaveQuestion(const ZSubscriberSlot& sltYes, const ZSubscriberSlot& sltNo);

void FillWorldListComboBox(Combobox *pcb);
void FillTrackListComboBox(Combobox *pcb, int aWorldNb);
void ThumbnailChange(Window *pWnd, int worldIdx, int trackIdx);

void FillCBWithTeams(Combobox *pcb);
void FillCBWithShips(Combobox *pcb, int teamIdx);
void LoadSelShipShip(unsigned int aTeam, unsigned int aShip);

tstring GetTrackFileName(int worldIdx, int trackIdx);
Combobox *BuildGameModesCombo(const char *szCBName);
int GetGameMode(Combobox *pcb);

#endif
