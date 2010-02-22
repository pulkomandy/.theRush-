#ifndef WXEDITGRAPHNODE_H__
#define WXEDITGRAPHNODE_H__

#include "wxEdit.h"
#include "ZString.h"
class wxGraphNode;

class wxZEditNode : public wxZEdit, public wxZEditHandler
{
public:

	wxZEditNode(wxWindow* parent, wxAuiManager* mgr = NULL) :
	  wxZEdit(parent,  this)
	  {
		  this->Connect(-1, 14000, (wxObjectEventFunction)&wxZEditNode::OnRefresh);

		  mEditingNode = NULL;


		  mFileExport = "testExport";

		  

	  }

	  virtual ~wxZEditNode() {}
	  void OnRefresh(wxEvent& event) {}

	  void HandleValueChanged(unsigned int);

	  void BuildInterface(wxGraphNode *pNode);
	  void BuildInterfaceForCodeGeneration();

	  tstring mNodeName;
	  wxGraphNode *mEditingNode;


	  tstring mFileExport;
	 

	  


	virtual void HandleButtonClick(unsigned int aButtonID);
	virtual void HandleLostFocus(unsigned int aID);
};

#endif