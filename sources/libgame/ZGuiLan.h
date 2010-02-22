///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZGuiLan.h
// Creation : 10/04/2009
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

#ifndef GUILan_H__
#define GUILan_H__
#include "CEGUI.h"
class ZProtoGUI;
using namespace CEGUI;
class ZGuiLapChoser;

class GuiLan
{
public:
	GuiLan() :winMgr(WindowManager::getSingleton())
	{
	}
	virtual ~GuiLan();
	void Build(IProtoGUI *pGUI);
	void Tick();
	void Show(const ZSubscriberSlot& onBack);
	void ShowLanServer( const ZSubscriberSlot& onBack );
	void Hide();
protected:
	DefaultWindow* root;
	Font *mFontArial8, *mFontArial24;
	WindowManager& winMgr;
	FrameWindow* AddServerFrm;
	FrameWindow* mMainServerWindow;
	FrameWindow* mLanMainWindow;
	ZProtoGUI *mGUI;
	PushButton* mLanBack;
	ZSubscriberSlot mOnBack;
	Window *srvthmbimg; // thumbnail
	ZGuiLapChoser *lapc;
	Editbox* urlBoxAddServer;
	MultiColumnList* mclFavorites;


	Combobox* cbworld;
	Combobox* cbtrack;
	Combobox* cbgamemode;

	int mMenuCurrentTrackWorld,mMenuCurrentTrackIndex;


	bool handleConnectToServer(const CEGUI::EventArgs& e);
	void ConnectToServer(const char *szURL);
	void CancelConnectToServer();
	bool OnOptionsFromLobby(const CEGUI::EventArgs& e);
	void CleanLanServers();
	void AddLanServer(const char *serverName, int nbPlayers,
					  const char *szStatus, const char *szMapName, const char *szMode, int aPing);
	void AddServer(MultiColumnList* mcl, const char *serverName, int nbPlayers,
						  const char *szStatus, const char *szMapName, const char *szMode, int aPing);
	void CleanLanLobby();
	bool LanBack(const CEGUI::EventArgs& e);
	bool LanJoin(const CEGUI::EventArgs& e);

	bool LanAddServ(const CEGUI::EventArgs& e);
	bool LanAddServOK(const CEGUI::EventArgs& e);
	bool LanAddServCancel(const CEGUI::EventArgs& e);
	void DoLanRefresh();
	bool LanRefresh(const CEGUI::EventArgs& e);
	void BuildLanClient();
	bool LanLaunchServer(const CEGUI::EventArgs& e);
	bool LanCreate(const CEGUI::EventArgs& e);
	void BuildLanServer();

	bool LanServerTrackChanged(const CEGUI::EventArgs& e);
	bool LanServerWorldChanged(const CEGUI::EventArgs& e);

	void BuildChat(Window *parent = NULL);
	void BuildAddServer();

	void BuildChatClient(Window *parent);
	bool handleTextChat(const CEGUI::EventArgs& e);
	bool LanEndRaceBackToMain(const CEGUI::EventArgs& e);

	void BackFromLobbyConfig();
	void RefreshFavorites();
};

#endif
