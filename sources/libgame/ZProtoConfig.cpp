///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZProtoConfig.cpp
// Creation : 03/12/2007
// Author : Cedric Guillemet
// Description :
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tinyxml.h"

#ifndef WIN32
#include <stdlib.h>
#endif


ZProtoConfig *GConfig = NULL;
ZProtoConfig::ZProtoConfig()
{
	GConfig = this;
	// Create default standard controls
	//SetControl("toto",1,2,3,4);

	// default controls
#ifdef WIN32
	// DINput8 version
	SetControl("Accelerate", 0, 0, 119, 0);
	SetControl("AttackBonus", 0, 0, 102, 0);
	SetControl("Booster", 0, 0, 32, 0);
	SetControl("LeftBrake", 0, 0, 113, 0);
	SetControl("LeftTurn", 0, 0, 97, 0);
	SetControl("PauseInGame", 0, 0, 1, 0);
	SetControl("RightBrake", 0, 0, 101, 0);
	SetControl("RightTurn", 0, 0, 100, 0);
	SetControl("ViewChange", 0, 0, 9, 0);
#else
	// SDL Version
	SetControl("Accelerate", 0, 0, 119, 0);
	SetControl("AttackBonus", 0, 0, 102, 0);
	SetControl("Booster", 0, 0, 32, 0);
	SetControl("LeftBrake", 0, 0, 113, 0);
	SetControl("LeftTurn", 0, 0, 97, 0);
	SetControl("PauseInGame", 0, 0, 27, 0);
	SetControl("RightBrake", 0, 0, 101, 0);
	SetControl("RightTurn", 0, 0, 100, 0);
	SetControl("ViewChange", 0, 0, 9, 0);
#endif
	// default Name
	mPlayerName = "PlayerName";

	// default screen values
	SetEnable("CHKBVSYNC", false);
#ifdef WIN32	
	SetEnable("CHKBFS", true);
#else
	SetEnable("CHKBFS", false);
#endif
	SetQuality("Width", 800);
	SetQuality("Height", 600);
	SetQuality("RenderProfile", RENDER_PROFILE_DEFAULT);

	SetQuality("MUSICVOLUME", 50);
	SetQuality("SFXVOLUME", 80);
	SetEnable("CHKMUSIC", true);
	
	ReadConfig();
}


ZProtoConfig::~ZProtoConfig()
{
	if (GConfig == this)
		GConfig = NULL;

	WriteConfig();
}

void ZProtoConfig::ReadConfig()
{
	ZFile file;
	tstring configName = GetHomeDirectoy();
	configName += ".the rush/Game.cfg";
	if (file.Open(configName.c_str(), ZOPEN_READ, false))
	{

		TiXmlDocument xmldoc;
		char *xmlDoc = new char [file.GetSize()];
		file.Read(xmlDoc,file.GetSize());
		xmldoc.Parse((const char*)xmlDoc);

		// control bind
		{
			TiXmlNode* root= xmldoc.FirstChild( "CfgBind" );
			if (root)
			{
				TiXmlElement* itemElement = root->ToElement();

				for (;itemElement;itemElement = itemElement->NextSiblingElement("CfgBind"))
				{
					const char *name = itemElement->Attribute("name");
					const char *altsz = itemElement->Attribute("alt");
					int alt = 0;
					if (altsz)
						alt = atoi(altsz);

					int device = atoi(itemElement->Attribute("device"));
					int button = atoi(itemElement->Attribute("button"));
					int axis   = atoi(itemElement->Attribute("axis"));
					SetControl(name, alt, device, button, axis);

				}
			}
		}
		// enable
		{
			TiXmlNode* root= xmldoc.FirstChild( "Enable" );
			if (root)
			{
				TiXmlElement* itemElement = root->ToElement();

				for (;itemElement;itemElement = itemElement->NextSiblingElement("Enable"))
				{
					const char *name = itemElement->Attribute("name");
					const char *enable = itemElement->Attribute("val");
					SetEnable(name, (strcmp(enable,"true")==0));
				}
			}
		}
		// Quality
		{
			TiXmlNode* root= xmldoc.FirstChild( "Quality" );
			if (root)
			{
				TiXmlElement* itemElement = root->ToElement();

				for (;itemElement;itemElement = itemElement->NextSiblingElement("Quality"))
				{
					const char *name = itemElement->Attribute("name");
					int qual = atoi(itemElement->Attribute("val"));
					SetQuality(name, qual);
				}
			}
		}
		// Favorites
		{
			TiXmlNode* root= xmldoc.FirstChild( "Favorite" );
			if (root)
			{
				TiXmlElement* itemElement = root->ToElement();

				for (;itemElement;itemElement = itemElement->NextSiblingElement("Favorite"))
				{
					const char *url = itemElement->Attribute("url");
					mFavURL.push_back(url);
				}
			}
		}
		// Player name
		{
			TiXmlNode* root= xmldoc.FirstChild( "Player" );
			if (root)
			{
				TiXmlElement* itemElement = root->ToElement();

				for (;itemElement;itemElement = itemElement->NextSiblingElement("Player"))
				{
					const char *name = itemElement->Attribute("name");
					SetPlayerName(name);
				}
			}
		}
		delete [] xmlDoc;
	}
}


void ZProtoConfig::WriteConfig() const
{
	// write
	ZFile cfgFile;
	tstring configName = GetHomeDirectoy();
	configName += ".the rush";//Game.cfg";
#ifdef WIN32	
	_mkdir(configName.c_str());
#else
	mkdir(configName.c_str(), 777);
#endif
	configName += "/Game.cfg";
	if (cfgFile.Open(configName.c_str(), ZOPEN_WRITE, false))
	{
		// controls
		std::map<tstring, ControlEntry>::const_iterator iter = mControls.begin();
		for (;iter != mControls.end(); ++iter)
		{
			tstring fmt;
			fmt.Printf("<CfgBind name=\"%s\" alt=\"0\" device=\"%d\" button=\"%d\" axis=\"%d\" />\n",
				(*iter).first.c_str(),
				(*iter).second.mDevice1,
				(*iter).second.mButton1,
				(*iter).second.mAxis1);
			cfgFile.Write(fmt.c_str(), fmt.Length());

			fmt.Printf("<CfgBind name=\"%s\" alt=\"1\" device=\"%d\" button=\"%d\" axis=\"%d\" />\n",
				(*iter).first.c_str(),
				(*iter).second.mDevice2,
				(*iter).second.mButton2,
				(*iter).second.mAxis2);

			cfgFile.Write(fmt.c_str(), fmt.Length());
		}
		// enables
		std::map<tstring, bool>::const_iterator iter2 = mEnables.begin();
		for (;iter2 != mEnables.end(); ++iter2)
		{
			tstring fmt;
			fmt.Printf("<Enable name=\"%s\" val=\"%s\" />\n",
				(*iter2).first.c_str(),
				(*iter2).second?"true":"false");

			cfgFile.Write(fmt.c_str(), fmt.Length());
		}
		// qualities
		std::map<tstring, int>::const_iterator iter3 = mQualities.begin();
		for (;iter3 != mQualities.end(); ++iter3)
		{
			tstring fmt;
			fmt.Printf("<Quality name=\"%s\" val=\"%d\" />\n",
				(*iter3).first.c_str(),
				(*iter3).second);

			cfgFile.Write(fmt.c_str(), fmt.Length());
		}
		// player name
		tstring fmt;
		fmt.Printf("<Player name=\"%s\" />\n",
			mPlayerName.c_str());
		// fav URL
		for(::uint ui = 0;ui<mFavURL.size();ui++)
		{
			fmt.Printf("<Favorite url=\"%s\" />\n",
				mFavURL[ui].c_str());
		}

		cfgFile.Write(fmt.c_str(), fmt.Length());

	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZProtoConfig)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////
