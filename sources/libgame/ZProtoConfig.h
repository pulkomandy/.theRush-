///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZProtoConfig.h
// Creation : 03/12/2007
// Author : Cedric Guillemet
// Description :
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZPROTOCONFIG_H__
#define ZPROTOCONFIG_H__

#include "..\libbase\ZSerializator.h"
#include "../librender/IDisplayDevice.h"

DECLAREZCLASS(ZProtoConfig);

///////////////////////////////////////////////////////////////////////////////////////////////////
class ZProtoConfig : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZProtoConfig();
	virtual ~ZProtoConfig();

	 void SetControl(const char *szCtrlName, int Alternate, int aDevice, int aButton, int aAxis)
	{
		std::map<tstring, ControlEntry>::iterator iter = mControls.find(szCtrlName);
		if (iter != mControls.end())
		{
			ControlEntry &pent = (*iter).second;
			if (Alternate == 0)
			{
				pent.mDevice1 = aDevice;
				pent.mButton1 = aButton;
				pent.mAxis1   = aAxis;
			}
			else
			{
				pent.mDevice2 = aDevice;
				pent.mButton2 = aButton;
				pent.mAxis2   = aAxis;
			}
		}
		else
		{
			if (Alternate == 0)
			{
				mControls[szCtrlName].mDevice1 = aDevice;
				mControls[szCtrlName].mButton1 = aButton;
				mControls[szCtrlName].mAxis1   = aAxis;
			}
			else
			{
				mControls[szCtrlName].mDevice2 = aDevice;
				mControls[szCtrlName].mButton2 = aButton;
				mControls[szCtrlName].mAxis2   = aAxis;
			}
		}
	}

	 bool GetControl(const char *szCtrlName, int Alternate, int &aDevice, int &aButton, int &aAxis) const
	{
		std::map<tstring, ControlEntry>::const_iterator iter = mControls.find(szCtrlName);
		if (iter != mControls.end())
		{
			if (Alternate == 0)
			{
				aDevice = (*iter).second.mDevice1;
				aButton = (*iter).second.mButton1;
				aAxis   = (*iter).second.mAxis1;
			}
			else
			{
				aDevice = (*iter).second.mDevice2;
				aButton = (*iter).second.mButton2;
				aAxis   = (*iter).second.mAxis2;
			}
			return true;
		}
		aDevice = 0;
		aButton = 0;
		aAxis =0;
		return false;
	}

	 void ReadConfig();
	 void WriteConfig() const;
	 void SetEnable(const char *szFeatureName, bool bEnable)
	{
		mEnables[szFeatureName] = bEnable;
	}
	 bool IsEnable(const char *szFeatureName) const
	{
		std::map<tstring, bool>::const_iterator iter = mEnables.find(szFeatureName);
		if ( iter != mEnables.end())
			return (*iter).second;
		return false;
	}

	 void SetQuality(const char *szFeatureName, int aQuality)
	{
		mQualities[szFeatureName] = aQuality;
	}
	 int  GetQuality(const char *szFeatureName) const
	{
		std::map<tstring, int>::const_iterator iter = mQualities.find(szFeatureName);
		if ( iter != mQualities.end())
			return (*iter).second;
		return 0;
	}
	 void SetPlayerName(const char *szName) { mPlayerName = szName; }
	 const char * GetPlayerName() const { return mPlayerName.c_str(); }

	 void AddFavoriteURL(const char *szURL)
	{
		for (unsigned int i=0;i<mFavURL.size();i++)
		{
			if (!strcmp(mFavURL[i], szURL))
				return;
		}
		mFavURL.push_back(szURL);
	}
	 int GetNbFavoriteURL() { return (int)mFavURL.size(); }
	 const char *GetFavoriteURL(int idx) { return mFavURL[idx]; }

	 void BuildInitParams(INITPARAMS *pToFill)
	{
		pToFill->idAdapter = 0;
		pToFill->refreshRate60hz = false;
		pToFill->vSync = IsEnable("CHKBVSYNC");
		pToFill->fullscreen = IsEnable("CHKBFS");
		pToFill->width = GetQuality("Width");
		pToFill->height = GetQuality("Height");
		pToFill->RenderProfile = GetQuality("RenderProfile");
		pToFill->antiAlias = false;
		pToFill->bpp = 32;
	}

protected:
	typedef struct ControlEntry
	{
		ControlEntry()
		{
			mDevice1 = mButton1 = mAxis1 =
			mDevice2 = mButton2 = mAxis2 = 0;
		}
		int mDevice1, mButton1, mAxis1;
		int mDevice2, mButton2, mAxis2;
	}ControlEntry;
	std::map<tstring, ControlEntry> mControls;
	std::map<tstring, bool> mEnables;
	std::map<tstring, int> mQualities;
	std::vector<tstring> mFavURL;
	tstring mPlayerName;
};

extern ZProtoConfig *GConfig;
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
