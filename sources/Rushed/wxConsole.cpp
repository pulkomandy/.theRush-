///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxConsole.cpp
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
#include <wx/aui/aui.h>
#include "wxConsole.h"
#include "../libbase/LibBase.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(ZenithConsole, wxPanel)
    EVT_ERASE_BACKGROUND(ZenithConsole::OnEraseBackground)
    EVT_TEXT_ENTER(2, ZenithConsole::OnCommandEnter)
    EVT_IDLE(ZenithConsole::OnIdle)

END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZenithConsole* mConsoleInstance = NULL;
bool bServerOK = false;

void AddConsoleText(const char *pszText)
{
    if (mConsoleInstance)
    {
        mConsoleInstance->AddText(pszText);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////

class ZLoggerwxConsole : public ILogger
{
public:
    DECLARE_SERIALIZABLE

    ZLoggerwxConsole()
    {
    }
    ~ZLoggerwxConsole()
    {
    }

    virtual void Log(LOG_LEVEL level, const tstring& str)
    {
        AddConsoleText(str);
    }

    virtual tulong GetMemoryUsed()  const { return sizeof(ZLoggerwxConsole); }

};
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZLoggerwxConsole)
END_SERIALIZATION()

extern "C" {
void LUARedirectPrintf(const char *format, ...)
{
    char strToWrite[4000];

    va_list ptr_arg;
    va_start(ptr_arg, format);
    vsprintf(strToWrite,format, ptr_arg);    // on ecrit dans le buffer

	AddConsoleText(strToWrite);

    va_end(ptr_arg);
}
}

void InitServer()
{
    /*
    if (!bServerOK)
    {
        AddConsoleText(" ##### Starting Server #####\r\n");
        bServerOK = true;
        //dlg->mStatus.SetWindowText("Started.");
        FLuaLib::Init(NULL,NULL);
	    InitINet();
        FLuaLib::DoFile("ServerAutoExec.lua");
        //dlg->mButStart.EnableWindow(FALSE);
        //dlg->mButStop.EnableWindow(TRUE);
        //dlg->mRestart.EnableWindow(TRUE);
        AddConsoleText(" ##### Server Started #####\r\n");

    }
    */
}

void UninitServer()
{
    /*
    if (bServerOK)
    {
        AddConsoleText(" ##### Closing Server #####\r\n");
        bServerOK = false;
        //dlg->mStatus.SetWindowText("Stopped.");
        UnInitINet();
        FLuaLib::Uninit();
        //dlg->mButStart.EnableWindow(TRUE);
        //dlg->mButStop.EnableWindow(FALSE);
        //dlg->mRestart.EnableWindow(FALSE);
        AddConsoleText(" ##### Server Terminated #####\r\n");
    }
    */
}


///////////////////////////////////////////////////////////////////////////////////////////////////

ZenithConsole::ZenithConsole(wxWindow* parent, wxWindowID id ,
                     const wxPoint& pos ,
                     const wxSize& size ,
                     wxAuiManager* mgr)
                     : wxPanel(parent, id, pos, size, wxNO_BORDER)
{

    mTextOut = new wxTextCtrl(this, 1, _T(""), wxPoint(0,0), wxSize(size.x, size.y-20),
        wxTE_RICH2 |wxTE_RICH |wxTE_MULTILINE|wxTE_READONLY);
	mTextOut->SetMaxLength(1000000);
    mCombo = new wxComboBox(this, 2);//wxTextCtrl(this, -1, "HO!", wxPoint(0,size.y-20), wxSize(size.x, 20));

    logAttr = new wxTextAttr(wxColour(0,0,0), *wxWHITE, wxFont(10, wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    cmdAttr = new wxTextAttr(wxColour(0,0,0), *wxWHITE, wxFont(10, wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    errAttr = new wxTextAttr(wxColour(255,10,10), *wxWHITE, wxFont(10, wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    mSizer = new wxBoxSizer(wxVERTICAL);
    mSizer->Add(mTextOut, 1, wxEXPAND + wxALL, 0);
    mSizer->Add(mCombo, 0, wxEXPAND + wxALL, 0);
    SetSizer(mSizer);
    mConsoleInstance = this;
    mCombo->SetFocus();
    mTextOut->SetBackgroundColour(wxColour(197, 197, 197));
    mCombo->SetBackgroundColour(wxColour(197, 197, 197));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZenithConsole::~ZenithConsole()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenithConsole::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenithConsole::OnCommandEnter(wxCommandEvent& event)
{
    if (event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER)
    {
        if (!mCombo->GetValue().IsEmpty())
        {
		    //mTextOut->SetDefaultStyle(*cmdAttr);
		    mTextOut->AppendText(mCombo->GetValue()+_T("\n"));
		    //mTextOut->SetDefaultStyle(*logAttr);
#ifdef FIXED
            LUA()->DoString(mCombo->GetValue().c_str(), "Console");
#endif
            mCombo->SetValue(_T(""));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenithConsole::OnIdle(wxIdleEvent &event)
{
    /*
    if (bServerOK)
    {
        // App idle for longer amount of time
        gINet->Tick();
        FLuaLib::Tick();
    }
    */
#ifdef WIN32
    Sleep(1);
#else
    sleep(0.001f);
#endif
    event.RequestMore();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZenithConsole::AddText(const char* pszText)
{
    const char *szText = pszText;
    if (szText[0] == 1)
    {
        szText++;
        mTextOut->SetDefaultStyle(*errAttr);
    }
    //mTextOut->SetDefaultStyle(*logAttr);
    
	if (mTextOut)
		mTextOut->AppendText(wxString(szText));
    
    //mTextOut->SetDefaultStyle(*logAttr);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

/*

DS.Consol = nil

function	CreateConsol(parentWnd,ID)

	------------------------------------------------------------------------------
	-- Parameters ----------------------------------------------------------------
	DS.Consol 						= wx.wxPanel(parentWnd,ID)
	DS.Consol.chilID 			= 	{	DSC_TXTDISP = 110, --text display ID
															DSC_INSTANTEDIT = 111 --intant edit ID
														}
	DS.Consol.Display 		= wx.wxTextCtrl(DS.Consol,DS.Consol.chilID.DSC_TXTDISP,"",wx.wxDefaultPosition,wx.wxDefaultSize, wx.wxFULL_REPAINT_ON_RESIZE + wx.wxVSCROLL + wx.wxTE_READONLY + wx.wxTE_MULTILINE + wx.wxTE_RICH + wx.wxTE_RICH2)
	DS.Consol.logtext			= wx.wxTextAttr(wx.wxColour(0,0,0),wx.wxWHITE,wx.wxFont(10,wx.wxFONTFAMILY_DEFAULT,wx.wxFONTSTYLE_NORMAL,wx.wxFONTWEIGHT_NORMAL))	--text attribut for log
	DS.Consol.errortext		= wx.wxTextAttr(wx.wxRED,wx.wxWHITE,wx.wxFont(10,wx.wxFONTFAMILY_DEFAULT,wx.wxFONTSTYLE_NORMAL,wx.wxFONTWEIGHT_BOLD)) 		--text attribut for error
	DS.Consol.cmdtext			= wx.wxTextAttr(wx.wxColour(10,10,250),wx.wxWHITE,wx.wxFont(10,wx.wxFONTFAMILY_DEFAULT,wx.wxFONTSTYLE_NORMAL,wx.wxFONTWEIGHT_NORMAL)) 	--text attribut for command msg
	--set default style
	DS.Consol.Display:SetDefaultStyle(DS.Consol.logtext)
	DS.Consol.Edit 				= wx.wxComboBox(DS.Consol,DS.Consol.chilID.DSC_INSTANTEDIT)
	DS.Consol.history 		= {}
	DS.Consol.histoPos		= -1
	DS.Consol.paneInfo		= wx.wxPaneInfo()
	DS.Consol.name				= "Consol"
--DS.Consol.dropTarget	= wx.wxDropTarget()
	-------------------------------------------------------------------------------

	-------------------------------------------------------------------------------
	-- Functions ------------------------------------------------------------------
	--function DS.Consol.dropTarget:OnDragEnter(x,y,def)
	--	print("plop")
	--end

	function DS.Consol.OnKeyDown(event)
		local key = event:GetKeyCode()
		local n		= table.getn(DS.Consol.history)
		if key == wx.WXK_UP then
			DS.Consol.histoPos = DS.Consol.histoPos+1
			if DS.Consol.histoPos > n-1 then
				DS.Consol.histoPos = 0
			end
			DS.Consol.Edit:SetValue(DS.Consol.history[n-DS.Consol.histoPos])
		elseif key == wx.WXK_DOWN then
			DS.Consol.histoPos = DS.Consol.histoPos-1
			if DS.Consol.histoPos < 0 then
				DS.Consol.histoPos = n-1
			end
			DS.Consol.Edit:SetValue(DS.Consol.history[n-DS.Consol.histoPos])
		end
		event:Skip()
	end

	function DS.Consol.OnEnter(event)
			local str = DS.Consol.Edit:GetValue()
			--if command is empty
			if str == "" then
				DS.Error("empty command")
				return
			end
			-- insert the command in the history
			if table.getn(DS.Consol.history) >= 30 then
				table.remove(DS.Consol.history,1)
			end
			table.insert(DS.Consol.history,str)
			DS.Consol.histoPos = -1
			-- display the command in the log
			DS.Consol.Display:SetDefaultStyle(DS.Consol.cmdtext)
			DS.Consol.Display:AppendText(str.."\n")
			DS.Consol.Display:SetDefaultStyle(DS.Consol.logtext)
			DS.Consol.Display:ScrollLines(-1)
			-- send the command to the target
			DS.sendCmd(str)
			DS.Consol.Edit:SetValue("")
	end

	function DS.Consol.Enable(flag)
		if flag == true then
			DS.Consol:DragAcceptFiles(true)
		else
			DS.Consol:DragAcceptFiles(false)
		end
	end

	function DS.Consol.OnDropFile(event)
		local t = event:GetFiles()
		for k,v in pairs(t) do
			if DS.Utils.FindExt(v) == "lua" then
				--load the lua script
				DS.sendCmd('lua_run("'..v..'")')
			elseif DS.Utils.FindExt(v) == "pak" then
				--merge the .pak
				v = v:gsub("\\","/")
				DS.sendCmd('merge("'..v..'")\n')
			else
				DS.Error("Format unsupported")
			end
		end
	end

	function DS.Consol.Init()

	end
	-------------------------------------------------------------------------------


	-------------------------------------------------------------------------------
	--	set content  --------------------------------------------------------------
	local globalSizer = wx.wxBoxSizer(wx.wxVERTICAL)
	globalSizer:AddWindow(DS.Consol.Display,1,wx.wxGROW + wx.wxALL, 0)
	globalSizer:AddWindow(DS.Consol.Edit,0,wx.wxGROW + wx.wxALL, 0)
	DS.Consol:SetSizer(globalSizer)

	--------------------------------------------------------------------------------
	-- Add to manager --------------------------------------------------------------
	DS.Consol.paneInfo:Name(DS.Consol.name)
	DS.Consol.paneInfo:Center()
	DS.Consol.paneInfo:CaptionVisible(false)
	DS.Consol.paneInfo:DestroyOnClose(false)

	DS.Mainfrm.frmManager:AddPaneWithInfo(DS.Consol,DS.Consol.paneInfo)
  DS.Mainfrm.frmManager:Update()
	--------------------------------------------------------------------------------

	--------------------------------------------------------------------------------
	-- Connect Event ---------------------------------------------------------------
	DS.Consol.Edit:Connect(wx.wxEVT_KEY_DOWN, DS.Consol.OnKeyDown)
	DS.Consol.Edit:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, DS.Consol.OnEnter)
	DS.Consol:Connect(wx.wxEVT_DROP_FILES,DS.Consol.OnDropFile)
	--register for the connection event
	table.insert(DS.freshcom.callback,DS.Consol.Enable)
	--------------------------------------------------------------------------------
end
*/
