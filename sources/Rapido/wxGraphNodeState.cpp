#include <wx/tglbtn.h>
#include "wxGraphContainer.h"
#include "wxGraphNodeState.h"

wxString CodeToXML(const wxString & str);
wxString XMLToCode(const wxString & str);

wxString wxGraphNodeState::BuildGraphString()
{
	wxString res = wxGraphNode::BuildGraphString();
	std::map<wxString, wxString>::iterator iter = mEventsCode.begin();
	for (;iter != mEventsCode.end() ; ++iter)
	{
		res += "	<Event name=\"";
		res += (*iter).first.Mid(3);
		res += "\" code=\"";
		res += CodeToXML((*iter).second);
		res += "\" />\n";
	}

	return res;
}


int wxGraphNodeState::GetAnchorIndexByName(const char *szName)
{
	int idx = 0;

	std::list<wxToggleButton*>::iterator iter = mButs.begin();
	for (;iter != mButs.end(); ++iter)
	{
		wxString szAnchor = (*iter)->GetLabel().Mid(3);
		if (szAnchor == szName)
			return idx;
		idx ++;
	}
	return -1;
}

void wxGraphNodeState::GetStateAssignments(const char*pSource, std::vector<wxString>& list)
{
	//const char *start = pSource;
	const char *finds = pSource;
	while (finds = strstr(finds, "SetState("))
	{
		finds+=9;
		wxString res ="";
		while ( (*finds != ')') && (*finds != 0))
		{
			res += *finds++;
		}
		list.push_back(res);
	}
}

void wxGraphNodeState::GetAllCodeConnections(std::map<wxString, std::vector<wxString> > & aCon)
{
	std::map<wxString, wxString>::iterator iter = mEventsCode.begin();
	for (;iter != mEventsCode.end() ; ++iter)
	{
		const wxString& code = (*iter).first.Mid(3);
		std::vector<wxString> mAssignsTo;
		GetStateAssignments((*iter).second.c_str(), mAssignsTo);
		for (unsigned int i=0;i<mAssignsTo.size();i++)
		{
			aCon[code].push_back(mAssignsTo[i]);
		}
	}
}

void wxGraphNodeState::OnBtClick(wxEvent& event)
{
	wxToggleButton* pBut = (wxToggleButton*)event.GetEventObject();
	wxGraphNodeState *pParent = (wxGraphNodeState *)pBut->GetParent();
	pParent->LoseFocus();
	pBut->SetValue(true);
	
	pParent->mCurSelectedButton = pBut->GetLabel();
	pParent->mParent->SetSelectedNode(pParent);
}