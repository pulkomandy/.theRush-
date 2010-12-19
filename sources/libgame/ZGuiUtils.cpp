///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiUtils.cpp
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

#include "ZGuiUtils.h"
#include "ZProtoGUI.h"


///////////////////////////////////////////////////////////////////////////////////////////////////

void CEGUICBSel(Combobox *pcb, int idx)
{
	if (pcb->getItemCount())
	{
		ListboxTextItem *psel = (ListboxTextItem *)pcb->getListboxItemFromIndex(idx);
		pcb->setItemSelectState(psel, true);
		pcb->getEditbox()->setText(psel->getText());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FillWorldListComboBox(Combobox *pcb)
{
	if (gRushGameContent)
	if (!gRushGameContent->GetWorlds().empty())
	{
		pcb->resetList();
		for (unsigned int i=0;i<gRushGameContent->GetWorlds().size();i++)
		{
			CEGUI::ListboxTextItem *it = new ListboxTextItem(gRushGameContent->GetWorlds()[i].c_str());
			//int * iPtr = new int(i);
			//it->setUserData((void*) iPtr);
			pcb->addItem (it);
		}
		pcb->setItemSelectState ((size_t )0,true);
		pcb->setText(gRushGameContent->GetWorlds()[0].c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FillTrackListComboBox(Combobox *pcb, int aWorldNb)
{
	pcb->resetList();

	if (gRushGameContent)
	{
		const std::vector<tstring>& trckv = gRushGameContent->GetTracksNames(gRushGameContent->GetWorlds()[aWorldNb]);
		for (unsigned int i=0;i<trckv.size();i++)
		{
			tstring trackn = trckv[i];		
			CEGUI::ListboxTextItem *it = new ListboxTextItem(trackn.GetBaseName().c_str());
			CEGUI::String *filepath = new String(trackn.c_str());
			//it->setUserData ((void*)filepath);
			pcb->addItem (it);
		}
		pcb->setItemSelectState ((size_t )0,true);
		//pcb->setText(tstring(gContentManager->GetTrackName(aWorldNb, 0)).GetBaseName().c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ThumbnailChange(Window *pWnd, int worldIdx, int trackIdx)
{
	WindowManager &winMgr = WindowManager::getSingleton();

	const tstring& sworld = gRushGameContent->GetWorlds()[worldIdx];
	const tstring& strk = gRushGameContent->GetTracksNames(sworld)[trackIdx];

	tstring trackname;
	trackname.Printf("./Prototype/Worlds/%s/Tracks/%s", sworld.c_str(), strk.c_str());
	trackname.Replace(".track", "Thumb.tga");
//	if (ImagesetManager::getSingleton().isImagesetPresent("ThumbImageImageSet"))
//		ImagesetManager::getSingleton().destroyImageset("ThumbImageImageSet");

	ImagesetManager::getSingleton().createFromImageFile("ThumbImageImageSet", trackname.c_str());

	pWnd->setProperty("Image", "set:ThumbImageImageSet image:full_image");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LeaveQuestion(const ZSubscriberSlot& sltYes, const ZSubscriberSlot& sltNo)
{
	String lib = ".99 ";
	lib += GLoc->GetString("LEAVEGAME").c_str();
	String Yes = ".01 ";
	Yes += GLoc->GetString("YES").c_str();
	String No = ".02 ";
	No += GLoc->GetString("NO").c_str();

	((ZProtoGUI*)GProtoGui)->mMessageboxGui.Show(lib.c_str(), Yes.c_str(), sltYes, 
		No.c_str(), sltNo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FillCBWithTeams(Combobox *pcb)
{
	pcb->resetList();

	for (unsigned int i=0;i<gRushGameContent->GetTeams().size();i++)
	{
		pcb->addItem (new ListboxTextItem(gRushGameContent->GetTeams()[i].c_str()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void FillCBWithShips(Combobox *pcb, int teamIdx)
{
	pcb->resetList();
	
	//tstring maketeam = "Team|";
	//maketeam += GetGameContent()->GetEntry("Team", teamIdx);
	const std::vector<tstring>& shipsN = gRushGameContent->GetShipsNames(gRushGameContent->GetTeams()[teamIdx]);
	for (unsigned int j=0;j< shipsN.size() ; j++)
	{
		pcb->addItem (new ListboxTextItem(shipsN[j].c_str() ));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LoadSelShipShip(unsigned int aTeam, unsigned int aShip)
{
	
	if (GGame)
	{
		ZRushGame *pGame = ((ZRushGame*)GGame->GetClientGame());
		if (pGame && pGame->GetClientShip() )
		{
			pGame->GetClientShip()->Init( pGame, pGame->GetPhysicWorld(), 
				gRushGameContent->GetShip( aTeam, aShip), -1, false );
			gCameraMotion.SetCameraOrbitingClose(pGame->GetClientShip()->GetTransform());
			//tstring cs = gContentManager->GetShipCreationString(aTeam, aShip);
			//LUA()->CallShipCreationMethod(GClientGame->mClientShip, cs.c_str());
		}
		else
		{
			if (!pGame->GetShips().empty())
			{
				ZShip *pShip = (*pGame->GetShips().begin()).ptr();
				pShip->Init( pGame, pGame->GetPhysicWorld(), 
					gRushGameContent->GetShip( aTeam, aShip), -1, false );
				gCameraMotion.SetCameraOrbitingClose(pShip->GetTransform());
			}
		}
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tstring GetTrackFileName(int worldIdx, int trackIdx)
{
	const tstring& sworld = gRushGameContent->GetWorlds()[worldIdx];
	const tstring& strk = gRushGameContent->GetTracksNames(sworld)[trackIdx];

	tstring trackname;
	trackname.Printf("./Prototype/Worlds/%s/Tracks/%s", sworld.c_str(), strk.c_str());

	return trackname;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Combobox *BuildGameModesCombo(const char *szCBName)
{
	WindowManager &winMgr = WindowManager::getSingleton();

	// Creation
	Combobox * cbgamemode = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox", szCBName));

	// Items
	ListboxTextItem *lbgm = new ListboxTextItem(GLoc->GetString("SIMPLERACE").c_str());
	cbgamemode->addItem (lbgm);

	//cbgamemode->addItem (new ListboxTextItem(GLoc->GetString("SURVIVOR").c_str()));

	// default selection
	cbgamemode->setItemSelectState ((size_t )0, true);
	cbgamemode->setText(GLoc->GetString("SIMPLERACE").c_str());

	// properties
	cbgamemode->setReadOnly(true);

	return cbgamemode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int GetGameMode(Combobox *pcb)
{
	int modeType	=  pcb->getItemIndex(pcb->getSelectedItem());

	int aGameType = GameSet_t::GAME_TYPE_RAW;
	switch (modeType)
	{
	case 0:
		aGameType = GameSet_t::GAME_TYPE_CLASSIC;
		break;
	default:
		break;
	}
	return aGameType;
}
