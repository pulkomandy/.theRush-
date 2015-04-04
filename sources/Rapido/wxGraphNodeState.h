#ifndef WXGRAPHNODESTATE_H__
#define WXGRAPHNODESTATE_H__

#include "wxGraphNode.h"
void SetScriptToEdit(const char *szName, wxString* pCode);
class wxGraphNodeState : public wxGraphNode //,public wxZEdit
{
public:
	wxGraphNodeState(wxGraphContainer* parent) : wxGraphNode(parent) //,wxZEdit(static_cast<wxPanel*>(this))
	{
		mTextForeground = wxColour(240,240,40);
		mBackGround = wxColour(157,87,87);
		mCol1 = wxColour(177,117,117);
		mColSel1 = wxColour(255,255,0,0);
		mColSel2 = wxColour(0,0,0,0);
		mColForeground = wxColour(0,0,0);
		mbHasTumbnail = false;
		mCurSelectedButton = "";


		AddRightPlug("To");
		AddRightPlug("To");
		AddRightPlug("To");

		//AddRightPlug("To");

		AddLeftPlug("In");
		AddLeftPlug("");
		AddLeftPlug("");
		//for (int i=0;i<2;i++)
		{
			AddLeftPlug("In");
			AddRightPlug("To");
		}

		mButCount = 3;


		for (int i=0;i<3;i++)
		{
			static const char *szLibOnEvent[]={"On Enter", "On Tick", "On Leave"};
			wxToggleButton *mButton = new wxToggleButton(this, 2, szLibOnEvent[i], wxPoint(30, 28+i*18), wxSize(120,18));
			mButton->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, (wxObjectEventFunction)&wxGraphNodeState::OnBtClick);
			mButs.push_back(mButton);
		}

		SetSize(-1, -1, 180, 100);
	}
	virtual ~wxGraphNodeState()
	{
	}
	virtual GraphNodeType GetType()
	{
		return GNT_STATE;
	}
	virtual bool OnAddNewConnection(wxGraphNode *pOther)
	{
		if (pOther->GetType() == GNT_MESSAGE)
		{
			int i = mButCount;
			wxString onmachin =  wxString("On ") + wxString(pOther->GetFunctionName());
			// check already present
			std::list<wxToggleButton*>::iterator iter = mButs.begin();
			for (; iter != mButs.end(); ++iter)
			{
				if ( (*iter)->GetLabel() == onmachin)
					return false;
			}

			wxToggleButton *mButton = new wxToggleButton(this, 2, onmachin, wxPoint(30, 28+i*18), wxSize(120,18));
			mButCount ++;
			mButs.push_back(mButton);
			mButton->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, (wxObjectEventFunction)&wxGraphNodeState::OnBtClick);


			AddLeftPlug("In");
			AddRightPlug("To");

			SetSize(-1, -1, GetSize().x, GetSize().y+18);

			return true;
		}
	}

	virtual void ChangeMessageName(const char *szPrevious, const char *szNew)
	{
		wxString prev = "On ";
		prev += szPrevious;
		wxString neww = "On ";
		neww += szNew;
		std::list<wxToggleButton*>::iterator iter = mButs.begin();
		for (; iter != mButs.end(); ++iter)
		{
			if ( prev == (*iter)->GetLabel() )
			{
				(*iter)->SetLabel(neww);
			}
		}
	}

	virtual void MessageHasBeenRemoved(const char *szMessageName)
	{
		wxString prev;
		if ((szMessageName[0] == 'O') && (szMessageName[1] == 'n'))
			prev = "";
		else
			prev = "On ";

		int indx = 0;
		prev += szMessageName;
		std::list<wxToggleButton*>::iterator iter = mButs.begin();
		for (; iter != mButs.end(); ++iter)
		{
			if ( ( prev == (*iter)->GetLabel() ) && (indx>2))
			{
				wxToggleButton *abut = (*iter);
				delete abut;
				mButs.remove( abut );
				mRightPlugs.pop_back();
				mLeftPlugs.pop_back();
				mButCount--;
				return;
			}
			indx ++;
		}
	}

	int mButCount;

	void OnBtClick(wxEvent& event);
	virtual void LoseFocus()
	{
		std::list<wxToggleButton*>::iterator iter = mButs.begin();
		for (;iter != mButs.end(); ++iter)
		{
			(*iter)->SetValue(false);
		}
		mCurSelectedButton = "";
	}
	virtual const char* GetSubItem() { return mCurSelectedButton.c_str(); }
	virtual wxString *GetCode(const char*szSubItem="") 
	{ 
		wxString keyItem = szSubItem;
		return &mEventsCode[keyItem];
	}
	std::list<wxToggleButton*> mButs;
	wxString mCurSelectedButton;
	std::map<wxString, wxString> mEventsCode;

	virtual wxString BuildGraphString();

	virtual int GetAnchorIndexByName(const char *szName);
	virtual void GetAllCodeConnections(std::map<wxString, std::vector<wxString>> & aCon);
	void GetStateAssignments(const char*pSource, std::vector<wxString>& list);
};

#endif