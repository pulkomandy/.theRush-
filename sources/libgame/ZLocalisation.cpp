///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZLocalisation.cpp
// Creation : 14/11/2007
// Author : Cedric Guillemet
// Description :
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tinyxml.h"

ZLocalisation *GLoc = NULL;
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZLocalisation)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

ZLocalisation::ZLocalisation()
{
	GLoc = this;
	mInit = false;
	mUninitedString = "NO LOCALS";
	Init();
}

ZLocalisation::~ZLocalisation()
{
	if (GLoc == this)
		GLoc = NULL;
}


void ZLocalisation::Init()
{
#ifdef MAC_OS
	unsigned char lngId = 0x09;
	bool bFound = false;
	int supportedLanCount = 2;
	CFArrayRef languages = CFLocaleCopyPreferredLanguages();
	for (CFIndex idx = 0 ; idx < CFArrayGetCount(languages) && !bFound ; idx++)
	{
		CFStringRef lang = (CFStringRef) CFArrayGetValueAtIndex(languages, idx);

		for (int jdx = 0 ; jdx < supportedLanCount && !bFound ; jdx++)
		{
			if (CFStringCompare(lang, supportedLanguages[jdx], 0) == kCFCompareEqualTo)
			{
				bFound = true;
				lngId = supportedCodes[jdx];
			}
		}
	}

	CFRelease(languages);
#elif defined(WIN32)
	LANGID lngId = GetSystemDefaultLangID()&0xFF;
#else
	unsigned char lngId = 0x09;
#endif
	tstring xmlLang;
	xmlLang.Printf("%s/Lang0x%02x.xml", SUBDIR, lngId);
	ZFile afile;
	if (afile.Open(xmlLang.c_str(), ZOPEN_READ, false))
	{
		ParseFile(afile);
	}
	else
	{
		// english standard
		xmlLang.Printf("%s/Lang0x%02x.xml", SUBDIR, 9);
		if (afile.Open(xmlLang.c_str(), ZOPEN_READ, false))
		{
			ParseFile(afile);
		}

	}


	mInit = true;
}

void ZLocalisation::ParseFile(ZFile &file)
{
	TiXmlDocument xmldoc;
	char *xmlDoc = new char[file.GetSize()+1];
	xmlDoc[file.GetSize()] = 0;

	file.Read(xmlDoc,file.GetSize());
	xmldoc.Parse((const char*)xmlDoc);

	TiXmlNode* root= xmldoc.FirstChild( "STR" );
	TiXmlElement* itemElement = root->ToElement();

	for (;itemElement;itemElement = itemElement->NextSiblingElement("STR"))
	{
		const char *id = itemElement->Attribute("id");
		const char *lib = itemElement->Attribute("lib");
		/*
		OutputDebugString(id);
		OutputDebugString(lib);
		OutputDebugString("\r\n");
		*/
		mStrings.insert(std::make_pair(id, lib));
	}


	delete [] xmlDoc;
}
