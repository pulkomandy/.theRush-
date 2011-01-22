#ifndef WXMAINFRAME_H__
#define WXMAINFRAME_H__

#include <wx/aui/aui.h>
class ZenithConsole;
class wxNavigator;
class wxZEditTrack;
class wx3DPanel;
// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);
	virtual ~MyFrame();
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& );
	void OnSave(wxCommandEvent& );
	void OnSaveAs(wxCommandEvent& );
	void OnFileNew(wxCommandEvent& );
	void RefreshInterface();
	void UpdateTitleBar();
	bool CheckModifiedFlagDocumentForNewDocument();
private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

	void OnCloseFrame(wxCloseEvent&);
	void OnRunGame(wxCommandEvent& WXUNUSED(event));

	wxAuiManager m_mgr;
    ZenithConsole *mConsole;
    wxToolBar *mToolBarEdit;
    wxMenuBar* mainMenu;
    wxNavigator *mNavigator;	
	wxZEditTrack *mEditTrack;
	wx3DPanel *m3DPanel;

	void CreateEditor();

	// TB
	void OnSelect(wxCommandEvent& event);
    void OnMove(wxCommandEvent& event);
    void OnRotate(wxCommandEvent& event);
    void OnScale(wxCommandEvent& event);
    void OnManageSnap(wxCommandEvent& event);


	// Load/Save
	bool mbFileIsNamed;
	wxString mFileName;

	
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT


	
};

#endif