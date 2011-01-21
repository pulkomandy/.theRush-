///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZLocalisation.cpp
// Creation : 14/11/2007
// Author : Cedric Guillemet
// Description :
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZLOCALISATION_H__
#define ZLOCALISATION_H__

#include "../libbase/ZSerializator.h"

#ifdef MAC_OS
CFStringRef supportedLanguages[] = {CFSTR("en"), CFSTR("fr")};
unsigned char supportedCodes[] = {0x09, 0x0C};
#endif

DECLAREZCLASS(ZLocalisation);

///////////////////////////////////////////////////////////////////////////////////////////////////

#define SUBDIR "./Prototype/Common/Localization"

class ZLocalisation : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:
	ZLocalisation();
	virtual ~ZLocalisation();

	void Init();
	void ParseFile(ZFile &file);


	const tstring& GetString(const tstring & szStrKey) const
	{
		if (!mInit)
			return mUninitedString;

		std::map<tstring , tstring>::const_iterator iter = mStrings.find(szStrKey);
		if (iter != mStrings.end())
			return (*iter).second;

		return mUninitedString;
	}

protected:

	bool mInit;
	tstring mUninitedString;
	std::map<tstring , tstring> mStrings;
};

extern ZLocalisation *GLoc;

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif

