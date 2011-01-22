// ----------------------------------------------------------------------------
// main frame
#include <wx/wx.h>
#include <wx/confbase.h>
#include <wx/treectrl.h>

#include "wxMainFrame.h"
#include "wxNavigator.h"
#include "wxConsole.h"
#include "wxEditTrack.h"
#include "wx3DPanel.h"

// ----------------------------------------------------------------------------

enum
{
    wxID_ENV = 10201,
    wxID_ViewConsole,
    wxID_ViewToolBar,
    wxID_ViewGrid,
    wxID_ViewEditPanel,
    wxID_ViewScripts,
    wxID_ViewNavigator,
    wxID_NEW_PARTICLE,
    wxID_ViewEditTrack,
    wxID_ViewEditParticles,
	wxID_RunGame,
	wxID_PhotoMode,
	wxID_ViewEditEnv,
	wxID_NEW_POINTLIGHT,
	wxID_NEW_SPOTLIGHT,
	wxID_NEW_OCCLUDERBOX,
	wxID_NEW_3DSOUND,
	wxID_SHIP0,

	wxID_NEXTITEM = wxID_SHIP0+100,
};

enum
{
    IDM_TOOLBAR_SELECT = 200,
    IDM_TOOLBAR_MOVE ,
    IDM_TOOLBAR_ROTATE ,
    IDM_TOOLBAR_SCALE ,

    IDM_TOOLBAR_SNAPMOVE ,
    IDM_TOOLBAR_SNAPROTATE ,
    IDM_TOOLBAR_SNAPSCALE ,
    IDM_TOOLBAR_COMBOLOCATE,
};

enum
{

    wxID_MERGE = wxID_HIGHEST +1,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_CLOSE(MyFrame::OnCloseFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)

	EVT_MENU(IDM_TOOLBAR_SELECT, MyFrame::OnSelect)
	EVT_MENU(IDM_TOOLBAR_MOVE, MyFrame::OnMove)
	EVT_MENU(IDM_TOOLBAR_ROTATE, MyFrame::OnRotate)
	EVT_MENU(IDM_TOOLBAR_SCALE, MyFrame::OnScale)
	EVT_MENU(IDM_TOOLBAR_SNAPMOVE, MyFrame::OnManageSnap)
	EVT_MENU(IDM_TOOLBAR_SNAPROTATE, MyFrame::OnManageSnap)
	EVT_MENU(IDM_TOOLBAR_SNAPSCALE, MyFrame::OnManageSnap)

	EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
	EVT_MENU(wxID_SAVE, MyFrame::OnSave)
	EVT_MENU(wxID_SAVEAS, MyFrame::OnSaveAs)
	EVT_MENU(wxID_NEW, MyFrame::OnFileNew)
	EVT_MENU(wxID_RunGame, MyFrame::OnRunGame)
	


END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{

	mbFileIsNamed = false;
	mFileName = "";

	m_mgr.SetManagedWindow(this);

	CreateEditor();


	wxConfigBase *pConfig = wxConfigBase::Get();
	wxString appLayout = pConfig->Read(_T("/Zenith/AUIPerspective"), "");
	if (!appLayout.IsEmpty())
	{
		m_mgr.LoadPerspective(appLayout);
	}
	wxString appWinState = pConfig->Read(_T("/Zenith/MainWinState"), "");
	if (!appWinState.IsEmpty())
	{
		wxSize appSize;
		wxPoint appPos;
		sscanf(appWinState.c_str(), "%d,%d,%d,%d",&appSize.x, &appSize.y, &appPos.x, &appPos.y);


		SetSize(appSize);
		SetPosition(appPos);
	}

	m_mgr.Update();

	/*
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T(".rushed//Ready"));
#endif // wxUSE_STATUSBAR
	*/
}


void MyFrame::CreateEditor()
{

    mToolBarEdit = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
    mToolBarEdit->SetToolBitmapSize(wxSize(22,22));

    /*new wxToolBar(this, -1, wxDefaultPosition, wxSize(800,-1),
    wxNO_BORDER|wxHORIZONTAL|wxTB_FLAT| wxADJUST_MINSIZE);*/


    wxBitmap tb_edit_select("ZenithRes/TBSelect.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_edit_move("ZenithRes/TBMove.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_edit_rotate("ZenithRes/TBRotate.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_edit_scale("ZenithRes/TBScale.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_edit_snap_move("ZenithRes/TBSnapMove.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_edit_snap_rotate("ZenithRes/TBSnapRotate.bmp", wxBITMAP_TYPE_BMP);
    wxBitmap tb_edit_snap_scale("ZenithRes/TBSnapScale.bmp", wxBITMAP_TYPE_BMP);

	wxBitmap tb_edit_newoccluder("ZenithRes/NodeTypeOccluder.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap tb_edit_newparticle("ZenithRes/NodeTypeParticle.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap tb_edit_newspotlight("ZenithRes/NodeTypeLight.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap tb_edit_newomnZLight("ZenithRes/NodeTypeOmniLight.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap tb_edit_new3dsound("ZenithRes/NodeType3DSound.bmp", wxBITMAP_TYPE_BMP);


    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_SELECT, wxT("Select"), tb_edit_select);


    mToolBarEdit->AddSeparator();

    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_MOVE, wxT("Move"), tb_edit_move);
    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_ROTATE, wxT("Rotate"), tb_edit_rotate);
    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_SCALE, wxT("Scale"), tb_edit_scale);
    mToolBarEdit->AddSeparator();

    wxComboBox *nCombo = new wxComboBox(mToolBarEdit, IDM_TOOLBAR_COMBOLOCATE, wxEmptyString, wxDefaultPosition, wxDefaultSize, NULL, wxCB_DROPDOWN|wxCB_READONLY);
    nCombo->Append(_T("View"));
    nCombo->Append(_T("World"));
    nCombo->Append(_T("Local"));
    nCombo->SetSelection(0);

    mToolBarEdit->AddControl(nCombo);

    mToolBarEdit->AddSeparator();

    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_SNAPMOVE, wxT("Snap Move"), tb_edit_snap_move);
    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_SNAPROTATE, wxT("Snap Rotate"), tb_edit_snap_rotate);
    mToolBarEdit->AddCheckTool(IDM_TOOLBAR_SNAPSCALE, wxT("Snap Scale"), tb_edit_snap_scale);
    mToolBarEdit->AddSeparator();
	mToolBarEdit->AddTool(wxID_NEW_OCCLUDERBOX, wxT("New Occluder"), tb_edit_newoccluder);
	mToolBarEdit->AddTool(wxID_NEW_PARTICLE, wxT("New Particle"), tb_edit_newparticle);
	mToolBarEdit->AddTool(wxID_NEW_POINTLIGHT, wxT("New Omni Light"), tb_edit_newomnZLight);
	mToolBarEdit->AddTool(wxID_NEW_SPOTLIGHT, wxT("New Spot Light"), tb_edit_newspotlight);
	mToolBarEdit->AddTool(wxID_NEW_3DSOUND, wxT("New 3D Sound"), tb_edit_new3dsound);



    //mToolBarEdit->SetSize(wxSize(800,22));
    //mToolBarEdit->SetMargins( 2, 2 );
    mToolBarEdit->Realize();

    m_mgr.AddPane(mToolBarEdit, wxAuiPaneInfo().
        Name(wxT("tb_edit")).Caption(wxT("Edit Bar")).
        ToolbarPane().Top().Row(1).
        LeftDockable(false).RightDockable(false));






    // create menu
    mainMenu = new wxMenuBar;
    // File
    wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_NEW, _("New\tCtrl+N"));
    file_menu->Append(wxID_OPEN, _("Open\tCtrl+O"));
    file_menu->Append(wxID_SAVE, _("Save\tCtrl+S"));
    file_menu->Append(wxID_SAVEAS, _("Save As"));
    //file_menu->Append(wxID_MERGE, _("Merge\tCtrl+M"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _("Exit\tAlt+F4"));


    mainMenu->Append(file_menu, _("File"));
    // Edit
    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(wxID_NEW_PARTICLE, _("New Particle"));
	edit_menu->Append(wxID_NEW_POINTLIGHT, _("New Point Light"));
	edit_menu->Append(wxID_NEW_SPOTLIGHT, _("New Spot Light"));
	edit_menu->Append(wxID_NEW_OCCLUDERBOX, _("New Occluder Box"));
	edit_menu->Append(wxID_NEW_3DSOUND, _("New 3D Sound"));

    edit_menu->AppendSeparator();
    edit_menu->Append(wxID_DELETE, _("Delete Selection"));
    mainMenu->Append(edit_menu, _("Edit"));
    // Tools
    wxMenu* Tools_menu = new wxMenu;
    Tools_menu->Append(wxID_ENV, _("Env. Rendering\tCtrl+E"));
    mainMenu->Append(Tools_menu, _("Tools"));
    // View
    wxMenu* View_menu = new wxMenu;
    View_menu->Append(wxID_ViewConsole, _("Console"));
    View_menu->Append(wxID_ViewToolBar, _("Tool bar"));
    View_menu->Append(wxID_ViewScripts, _("Scripts Edit"));
    //View_menu->Append(wxID_ViewEditPanel, _("Edit Panel"));
    View_menu->Append(wxID_ViewNavigator, _("Navigator"));
    View_menu->Append(wxID_ViewEditTrack, _("Edit Track"));
    //View_menu->Append(wxID_ViewEditParticles, _("Edit Particles"));
	View_menu->Append(wxID_ViewEditEnv, _("Edit"));

    View_menu->AppendSeparator();
    View_menu->Append(wxID_ViewGrid, _("3D Grid"));
    mainMenu->Append(View_menu, _("View"));
	// Game
    wxMenu* Game_menu = new wxMenu;
    Game_menu->Append(wxID_RunGame, _("Run\tF5"));
	Game_menu->Append(wxID_PhotoMode, _("Photo Mode\tF6"));
	mainMenu->Append(Game_menu, _("Game"));

    // Help
    wxMenu* Help_menu = new wxMenu;
    Help_menu->Append(wxID_HELP, _("Engine Help\tF1"));
    Help_menu->AppendSeparator();
    Help_menu->Append(wxID_ABOUT, _("About"));
    mainMenu->Append(Help_menu, _("Help"));

	


	


    SetMenuBar(mainMenu);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));


/*
	mNavigator = new wxNavigator(this, &m_mgr);
    m_mgr.AddPane(mNavigator, wxAuiPaneInfo().Name(wxT("Navigator")).Caption(wxT("Navigator")).Right());
*/
	
	ZRushTrack::UseEditingGizmo = true;

	mConsole = new ZenithConsole(this, -1, wxDefaultPosition, wxSize(400, 200), &m_mgr);
    m_mgr.AddPane(mConsole, wxAuiPaneInfo().Name(wxT("Console")).Caption(wxT("Console")).Bottom());



	wxSize sz2 = GetClientSize();
    m3DPanel = new wx3DPanel( this, -1, wxDefaultPosition, sz2, &m_mgr );

    m3DPanel->SetLocate( LOCATE_VIEW );

    m_mgr.AddPane(m3DPanel, wxAuiPaneInfo().
        Name(wxT("3D")).Caption(wxT("3D")).
        CenterPane());
		
	mEditTrack = new wxZEditTrack(this, &m_mgr);//-1, wxDefaultPosition, wxSize(400, 200), &m_mgr);
	mEditTrack->m3DEdit = m3DPanel;
	m3DPanel->mEditTrack = mEditTrack;
	m_mgr.AddPane(mEditTrack, wxAuiPaneInfo().Name(wxT("EditTrack")).Caption(wxT("EditTrack")).Right());

	m3DPanel->SetFocusIgnoringChildren();


	// ship names
	wxMenu* ships_menu = new wxMenu;


	int idxship = 0;
	for (int i=0;i<gRushGameContent->GetTeams().size();i++)
	{
		const tstring& szteam = gRushGameContent->GetTeams()[i];

		for (int j=0;j<gRushGameContent->GetShipsNames(szteam).size();j++)
		{
			const tstring& szship = gRushGameContent->GetShipsNames(szteam)[j];

			wxMenuItem *item = ships_menu->AppendRadioItem(wxID_SHIP0+idxship, _(szship.c_str()));
			if (!_stricmp(szship,"feisar"))
				item->Check(true);
/*
			 mShipsCreationMethods.push_back(shipsel_t(gContentManager->GetShipCreationString(i, j),
			 i,j, item));
			 */
			 idxship++;

		}

	}
	Game_menu->AppendSubMenu(ships_menu, _("Ships"));
}

MyFrame::~MyFrame()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	wxString myPerspect = m_mgr.SavePerspective();
	pConfig->Write(_T("/Zenith/AUIPerspective"), myPerspect);

	wxString myWinState;
	myWinState = wxString::Format("%d,%d,%d,%d",GetSize().x, GetSize().y, GetPosition().x, GetPosition().y);
	pConfig->Write(_T("/Zenith/MainWinState"), myWinState);

	m_mgr.UnInit();
}

// event handlers


///////////////////////////////////////////////////////////////////////////////////////////////////

void MyFrame::OnSelect(wxCommandEvent& event)
{
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SELECT, true);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_MOVE, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_ROTATE, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SCALE, false);
    m3DPanel->SetEditMode(EDIT_MODE_SELECT);
    //gMyFrame->SetCurrentPlugin(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MyFrame::OnMove(wxCommandEvent& event)
{
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SELECT, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_MOVE, true);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_ROTATE, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SCALE, false);
    m3DPanel->SetEditMode(EDIT_MODE_MOVE);
    //gMyFrame->SetCurrentPlugin(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MyFrame::OnRotate(wxCommandEvent& event)
{
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SELECT, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_MOVE, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_ROTATE, true);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SCALE, false);
    m3DPanel->SetEditMode(EDIT_MODE_ROTATE);
    //gMyFrame->SetCurrentPlugin(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MyFrame::OnScale(wxCommandEvent& event)
{
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SELECT, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_MOVE, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_ROTATE, false);
    mToolBarEdit->ToggleTool( IDM_TOOLBAR_SCALE, true);
    m3DPanel->SetEditMode(EDIT_MODE_SCALE);
    //gMyFrame->SetCurrentPlugin(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MyFrame::OnManageSnap(wxCommandEvent& event)
{
    bool bSnapRotate = mToolBarEdit->GetToolState(IDM_TOOLBAR_SNAPROTATE);
    bool bSnapScale = mToolBarEdit->GetToolState(IDM_TOOLBAR_SNAPSCALE);
    bool bSnapMove = mToolBarEdit->GetToolState(IDM_TOOLBAR_SNAPMOVE);

    m3DPanel->SetGizmoSnap(bSnapMove, bSnapRotate, bSnapScale);
}

void MyFrame::RefreshInterface()
{
	mEditTrack->BuildInterface();
}

void MyFrame::OnCloseFrame(wxCloseEvent& event)
{
	if (CheckModifiedFlagDocumentForNewDocument())
		event.Skip();

}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	if (!CheckModifiedFlagDocumentForNewDocument())
		return;

    // true is to force the frame to close
    Close(true);
}
void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	if (!CheckModifiedFlagDocumentForNewDocument())
		return;

	bool mbForOpening = true;
	wxFileDialog fDialog(GetParent(), "Choose a file", "", "", "Track|*.track",
		mbForOpening?(wxFD_OPEN|wxFD_FILE_MUST_EXIST):(wxFD_SAVE|wxFD_OVERWRITE_PROMPT));

	if (fDialog.ShowModal() == wxID_OK)
	{
		const char* fname = fDialog.GetPath().mb_str();
		//gContentManager->SaveTrack(fname);//LoadGameResources(mCurrentMapName.c_str(), true);//"./ProtoType/Worlds/Earth/Tracks/track1.Track");
		tstring localpath = fname;
		localpath.ComputeRelativeWD();
		mbFileIsNamed = true;
		mFileName = fname;

		m3DPanel->LoadWorld(localpath.c_str());
	}
}

void MyFrame::OnSave(wxCommandEvent& event)
{
	if (!mbFileIsNamed)
	{
		OnSaveAs(event);
	}
	else
	{
		m3DPanel->SaveWorld(mFileName.c_str());
		m3DPanel->ClearDocumentModifiedFlag();
	}
}

void MyFrame::OnSaveAs(wxCommandEvent& event)
{
	bool mbForOpening = false;
	wxFileDialog fDialog(GetParent(), "Choose a file", "", "", "Track|*.track",
		mbForOpening?(wxFD_OPEN|wxFD_FILE_MUST_EXIST):(wxFD_SAVE|wxFD_OVERWRITE_PROMPT));

	if (fDialog.ShowModal() == wxID_OK)
	{
		const char* fname = fDialog.GetPath().mb_str();
		//gContentManager->SaveTrack(fname);//LoadGameResources(mCurrentMapName.c_str(), true);//"./ProtoType/Worlds/Earth/Tracks/track1.Track");

		mbFileIsNamed = true;
		mFileName = fname;

		OnSave(event);
	}
}

// return true if can continue processing. False means don't do anything
bool MyFrame::CheckModifiedFlagDocumentForNewDocument()
{
	if (m3DPanel->IsDocumentModified())
	{
		// Message Box
		int res = wxMessageBox("Document has been modified. Do you want to save it?","Document modified", wxYES_NO|wxCANCEL);
		if (res == wxYES)
		{
			OnSave(wxCommandEvent());
			return true;
		}
		if (res == wxCANCEL)
			return false;
	}
	return true;
}
void MyFrame::OnFileNew(wxCommandEvent& WXUNUSED(event))
{
	if (!CheckModifiedFlagDocumentForNewDocument())
		return;

	mbFileIsNamed = false;
	mFileName = "";

	// not very proud of this : 
	/*
	static int fakeNewInt = 0;
	tstring fakeNew;
	fakeNew.Printf("Default.track0x%x", ++fakeNewInt);
	m3DPanel->LoadWorld(fakeNew.c_str());
	*/
	GTrack->RemoveTrackPiecesFromScene();
	GTrack->CreateDefaultTrack();
	GTrack->Compute();
	m3DPanel->SetSelection(NULL);

	m3DPanel->ClearDocumentModifiedFlag();
}

void MyFrame::OnRunGame(wxCommandEvent& WXUNUSED(event))
{
	GTrack->ShowEditGizmos(false);

	m3DPanel->RunGame();
}



void MyFrame::UpdateTitleBar()
{
	wxString titleBar = ".rushed//";
	if (mbFileIsNamed)
		titleBar+=mFileName.c_str();
	else
		titleBar+="Unnamed.track";

	if (m3DPanel->IsDocumentModified())
		titleBar+=" *";
	this->SetTitle(titleBar);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T(".rushed\n")
                    _T("\n")
                    _T(".the rush Track editor using wxWidgets %s\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About .rushed"),
                 wxOK | wxICON_INFORMATION,
                 this);
}

