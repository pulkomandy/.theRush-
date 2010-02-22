#include <wx/wx.h>
#include <wx/window.h>
#include <wx/grid.h>
#include <wx/treectrl.h>
#include <wx/spinctrl.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/image.h>
#include <wx/colordlg.h>
#include <wx/wxhtml.h>
#include <vector>
#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>

#include "wxEdit.h"
#include "ZString.h"

#include "wxEditGraphNode.h"
#include "wxGraphNode.h"
#include "wxGraphContainer.h"
#include "wxGraphNodeState.h"
#include "wxScriptEditPanel.h"

//#include "res.h"
extern wxGraphContainer *mScrollV;

/*

*/


/////
void SetScriptToEdit(const char *szName, wxString* pCode);
void wxZEditNode::BuildInterface(wxGraphNode *pNode)
{
	mEditingNode = pNode;


	BeginPanel();
	if (mEditingNode)
	{
		mNodeName = mEditingNode->GetFunctionName();
		AddFolder("Node");
		AddTextEdit("Name", &mNodeName, 200);
		/*
		AddStatic("Comments");
		AddScriptEdit(pNode->GetComment(), 2);
		/*
		if (pNode->GetType() == GNT_STATE)
		{
			AddFolder("Properties");
		}
		*/
		if (pNode->GetType() == GNT_MESSAGE)
		{
			AddFolder("Source code");
			AddScriptEdit(pNode->GetCode(pNode->GetSubItem()), 2);
		}
		
		if (mEditingNode->GetType() == GNT_STATE)
		{
			wxString info;
			info = mNodeName+" > ";
			info += ((wxGraphNodeState*)mEditingNode)->mCurSelectedButton;
			SetScriptToEdit(info.c_str(), pNode->GetCode(pNode->GetSubItem()));
		}
	}
	else
	{
		// editing tab

		extern wxNotebook * myNotebook;
		
		mNodeName = myNotebook->GetPageText(myNotebook->GetSelection());
		AddFolder("FSM");
		AddTextEdit("Name", &mNodeName, 199);
		AddCheckBox("Add Raknet packet support", &mScrollV->mbRaknetMessage);
		AddCheckBox("Tick has time parameter", &mScrollV->mbTickHasTime);
		AddTextEdit("State base", &mScrollV->mStateEnumBase);
		AddTextEdit("Message base", &mScrollV->mMessageEnumBase);
		AddTextEdit("Output", &mScrollV->mOutputFileName);
	

		std::vector<wxString> mStateList;
		mScrollV->FillNodeList(mStateList, GNT_STATE);
		if (!mStateList.empty())
		{
			wxString mInitialStatesList;
			for (unsigned int i=0;i<mStateList.size()-1;i++)
			{
				mInitialStatesList += mStateList[i]+"|";
			}
			mInitialStatesList += mStateList[mStateList.size()-1];

			if (mScrollV->mInitialState > (mStateList.size()-1) )
				mScrollV->mInitialState = 0;

			AddCombo("Initial State", mInitialStatesList.c_str(), &mScrollV->mInitialState);
		}
		AddFolder("Code");
		AddStatic("Includes");
		AddScriptEdit(mScrollV->GetIncludes(), 14);
		AddStatic("Member Variables");
		AddScriptEdit(mScrollV->GetMemberVariables(), 14);
		AddStatic("Members Init");
		AddScriptEdit(mScrollV->GetMembersInit(), 14);

		extern wxScriptEditPanel *mEditScriptPanel;
		mEditScriptPanel->SetScriptToEdit("", NULL);
	}
	EndPanel();
}

void wxZEditNode::BuildInterfaceForCodeGeneration()
{
	mEditingNode = NULL;


	BeginPanel();


	AddFolder("Generate");


	this->AddFileOpener("Export File","*.h;*.cpp", &mFileExport);
	AddButton("Generate", 101);

	EndPanel();
}

wxString GenerateCPP();

void wxZEditNode::HandleButtonClick(unsigned int aButtonID)
{
	GenerateCPP();
}

void wxZEditNode::HandleLostFocus(unsigned int aID)
{
	if (!mEditingNode)
		return;

	if (mEditingNode->GetType() == GNT_STATE)
	{
		
	}
}

void wxZEditNode::HandleValueChanged(unsigned int aID)
{
	if (aID == 199)
	{
		extern wxNotebook * myNotebook;
		myNotebook->SetPageText(myNotebook->GetSelection(), mNodeName.c_str());
		mScrollV->mGraphName = mNodeName.c_str();
	}
	else

	if (aID == 200)
	{
		// node name
		int aCount = 0;
		char tmps[512];

		strcpy(tmps, mNodeName.c_str());
		/*
		while (mScrollV->GetNodeByName(tmps));
		{
			sprintf(tmps,"%s%03d", mNodeName.c_str(), aCount++);
		};
		
		*/
		if (mEditingNode->GetType() == GNT_MESSAGE)
		{
			mScrollV->ChangeMessageName(mEditingNode->GetFunctionName(), tmps);
		}
		mEditingNode->SetFunctionName(tmps);
		return;
	}
	else

	if (mEditingNode)
	{
		mEditingNode->SetFunctionName(mNodeName);



	}
}


wxString GenerateEnum(std::vector<wxString> & list, const wxString& prefix)
{
	wxString enumList = "";
	for (unsigned int i=0;i<list.size();i++)
	{
		enumList += prefix;
		enumList += list[i];
		if (i != (list.size()-1))
			enumList += ",\n";
		else
			enumList += "\n";
	}
	return enumList;

}

wxString GenerateSwitchCase(std::vector<wxString> & list, const wxString& functionPrefix, 
							const wxString& variableName, const wxString& functionParameter)
{
	wxString res = "	switch("+variableName+")\n	{\n";

	for (unsigned int i=0;i<list.size();i++)
	{
		res += "		case "+list[i]+":\n";
		res += "			"+functionPrefix+list[i]+"("+functionParameter+");\n";
		res += "			break;\n";
	}

	res += "	}";

	return res;
}

wxString BuildMessagesStruct(std::vector<wxString> &list, 
							 std::vector<wxString> &code,
							 std::vector<wxString> &comment)
{
	wxString res;
	for (unsigned int i = 0;i<list.size();i++)
	{
		res += "\n\n\n/*\n"+comment[i]+"\n*/\n";
		res += "typedef struct "+list[i]+"\n{\n";
		res += code[i]+"\n} "+list[i]+";\n\n";
	}

	return res;
}

wxString BuildFunctions(std::vector<wxString> &list, std::vector<wxString> &code,
						const wxString& prefix, const wxString& parameter, const wxString& className)
{
	wxString res;
	for (unsigned int i = 0;i<list.size();i++)
	{
		//res += "\n/*"+""/*comment[i]*/+"*/\n";
		res += "inline void "+prefix+list[i]+"("+parameter+")\n{\n\n";
		res += code[i]+"\n}\n";
	}

	return res;
}

wxString BuildMessagesFunctions(std::vector<wxString> &list, std::vector<wxString> &codeList, std::vector<wxString> &stateList)
{
	wxString res;
	for (unsigned int i = 0;i<list.size();i++)
	{
		res += "void Handle(const "+list[i]+"& mes)\n{\n";
		res += GenerateSwitchCase(stateList, "Handle", "mState", "mes");
		res += "\n}\n\n";
	}
	return res;
}

wxString BuildRaknetPacketRead(std::vector<wxString> &meslist,const wxString& enumPrefix)
{
	wxString res;
	res += "bool HandleRaknetPacket(Packet& packet)\n{\n";
	res += "switch (packet->data[0])\n";
	res += "{\n";
	for (unsigned int i=0;i<meslist.size();i++)
	{
		res += "case "+enumPrefix+meslist[i]+":\n";
		res += "{\n";
		res += meslist[i]+" mes;\n";//		RakNetTime time, dataLength;
		res += "RakNet::BitStream rakStream( packet->data+1, sizeof("+meslist[i]+"), false);\n";
		res += "rakStram.Read(mes);\n";
		res += "Handle(mes);\n";
		res += "return true;\n";
		res += "}\n";
	}
	res += "}\nreturn false;\n}\n";

	return res;
}

wxString MakeAllStatesHandles(std::vector<wxString> &statelist, std::vector<wxString> &meslist)
{
	wxString res;

	for (unsigned int i=0;i<statelist.size();i++)
	{
		for (unsigned int j=0;j<meslist.size();j++)
		{
			res += "void Handle"+statelist[i]+"(const "+meslist[j]+"& mes)\n";
			res += "{\n";

			wxGraphNode *pStateNode = mScrollV->GetNodeByName(statelist[i].c_str());
			wxString onit = "On "+meslist[j];
			res += *pStateNode->GetCode(onit.c_str());
			res += "\n}\n\n";
		}
	}

	return res;
}


wxString GenerateCPP()
{
	std::vector<wxString> mMesList, mStateList, mMesCommentList, mMesCodeList;
	std::vector<wxString> mTickCodeList;
	std::vector<wxString> mEnterCodeList, mLeaveCodeList;
	mScrollV->FillNodeList(mMesList, GNT_MESSAGE);
	mScrollV->FillNodeList(mStateList, GNT_STATE);
	mScrollV->FillCodesFromNodeList(mMesCodeList, GNT_MESSAGE);
	mScrollV->FillCommentsFromNodeList(mMesCommentList, GNT_MESSAGE);


	mScrollV->FillCodesFromNodeList(mTickCodeList, GNT_STATE, "On Tick");
	mScrollV->FillCodesFromNodeList(mEnterCodeList, GNT_STATE, "On Enter");
	mScrollV->FillCodesFromNodeList(mLeaveCodeList, GNT_STATE, "On Leave");
	

	// build messages enum

	wxString enumMessages = GenerateEnum(mMesList, "Type_");
	wxString enumStates = GenerateEnum(mStateList, "");

	wxString strTemplate;
	FILE *fp = fopen("CPPTemplate.template","rb");
	if (fp)
	{
		int len = _filelength(fileno(fp));
		char *tmps = new char [len+1];
		
		fread(tmps, len, 1, fp);
		tmps[len] = 0;
		fclose(fp);

		strTemplate = tmps;
		delete [] tmps;
	}
	else 
		return "CPPTemplate.template NOT FOUND!!!";

	std::map<wxString, wxString> mTokens;

	wxString className = mScrollV->mGraphName.c_str();// gClassName.c_str();
	mTokens["%%STATEMACHINENAME%%"] = className;

	bool bHasFloatTime = true;
	bool bRaknetPacket = false;
	if (bHasFloatTime)
		mTokens["%%HASFLOATTIME%%"] = "float timeEllapsed";
	else
		mTokens["%%HASFLOATTIME%%"] = "";

	mTokens["%%MESSAGESENUM%%"] = enumMessages;

	mTokens["%%TICKSTATECASE%%"] = GenerateSwitchCase(mStateList, "Tick", 
		"mState", bHasFloatTime?"timeEllapsed":"");
	

	mTokens["%%STATESENUM%%"] = enumStates;

	mTokens["%%MESSAGEALLSTRUCTS%%"] = BuildMessagesStruct(mMesList, mMesCodeList, mMesCommentList);

	mTokens["%%ALLTICKFUNCTIONS%%"] = BuildFunctions(mStateList, mTickCodeList, "Tick",
		bHasFloatTime?"float timeEllapsed":"", className);

	mTokens["%%ALLENTERFUNCTIONS%%"] = BuildFunctions(mStateList, mEnterCodeList, "Enter",
		"", className);

	mTokens["%%ALLLEAVEFUNCTIONS%%"] = BuildFunctions(mStateList, mLeaveCodeList, "Leave",
		"", className);


	mTokens["%%SETSTATESWITCH%%"] = GenerateSwitchCase(mStateList, "Leave", 
		"mState", "")+"\n"+
		"mState = newState;"+
		GenerateSwitchCase(mStateList, "Enter", 
		"newState", "");


	mTokens["%%ALLHANDLEMESSAGES%%"] = BuildMessagesFunctions(mMesList, mMesList, mStateList);
	mTokens["%%INITIALSTATE%%"] = mStateList[mScrollV->mInitialState];

	mTokens["%%HANDLERAKNETPACKET%%"] = bRaknetPacket?BuildRaknetPacketRead(mMesList, "Type_"):"";
	mTokens["%%ALLSTATEMESSAGESHANDLE%%"] = MakeAllStatesHandles(mStateList, mMesList);

	mTokens["%%INCLUDES%%"] = *mScrollV->GetIncludes();
	mTokens["%%MEMBERSINIT%%"] = *mScrollV->GetMembersInit();
	mTokens["%%MEMBERSDECLARE%%"] = *mScrollV->GetMemberVariables();

	//wxString *GetMemberVariables() { return mMemberVariables; }

	SYSTEMTIME systime;
	GetSystemTime(&systime);

	int size = ::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &systime, NULL, NULL, 0);
	char *tmpdate = new char [size+1];
	tmpdate[size] = 0;
	::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &systime, NULL, tmpdate, size);


	size = ::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &systime, NULL, NULL, 0);
	char *tmptime = new char [size+1];
	tmptime[size] = 0;
	::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &systime, NULL, tmptime, size);

	mTokens["%%DATE%%"] = tmpdate;
	mTokens["%%TIME%%"] = tmptime;

	delete [] tmpdate;
	delete [] tmptime;
	// replacing
	std::map<wxString, wxString>::iterator iter = mTokens.begin();

	for (; iter != mTokens.end(); ++iter)
		strTemplate.Replace((*iter).first, (*iter).second);

	// write down
	fp = fopen(mScrollV->mOutputFileName.c_str(),"wb");
	if (fp)
	{
		fwrite(strTemplate.c_str(), strTemplate.Len(), 1, fp);
		fclose(fp);
	}

	// Count "\n"
	int aCount = 0;
	for (int cnt = 0;cnt<strTemplate.Len(); cnt++)
	{
		if (strTemplate[cnt] == '\n')
			aCount++;
	}
	wxString GenInfos="Generation of ";
	GenInfos += mScrollV->mOutputFileName.c_str();//"res.h ";
	GenInfos += " OK!\n";
	char tmps[512];
	GenInfos += itoa(aCount, tmps, 10);
	GenInfos += " Lines generated.\n";
	return GenInfos;
	

}
