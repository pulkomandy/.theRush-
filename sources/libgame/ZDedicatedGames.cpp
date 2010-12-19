#include "ZDedicatedGames.h"
#include "../libbase/ZFile.h"
#include "tinyxml.h"

ZDedicatedGames *GDedicatedGames = NULL;
ZDedicatedGames::ZDedicatedGames()
{
	GDedicatedGames = this;

	mGameIndex = 0;
	mPort = 49888;
	ZFile file;
	if (file.Open("./DedicatedGames.xml"))
	{
		tstring doc = file.ReadFileString();

		TiXmlDocument xmlDoc;
		if (xmlDoc.Parse(doc.c_str()))
		{
			TiXmlNode* root = xmlDoc.FirstChild( "Server" );
			
			TiXmlElement* itemElement = root->ToElement();
			if (itemElement && itemElement->Attribute("Port"))
				mPort = atoi(itemElement->Attribute("Port"));


			root = xmlDoc.FirstChild( "Game" );
			itemElement = root->ToElement();
			for (;itemElement;itemElement = itemElement->NextSiblingElement("Game"))
			{
				const char *trk = itemElement->Attribute("Track");
				const char *world = itemElement->Attribute("World");
				bool aBonus = true;

				if (itemElement->Attribute("Bonus"))
					aBonus = (atoi(itemElement->Attribute("Bonus")) != 0);

				int aGameMode = 0;
				if (itemElement->Attribute("GameType"))
					aGameMode = atoi(itemElement->Attribute("GameType"));

				if (trk)
					mGames.push_back(GameSet_t(world, trk, aGameMode, aBonus ));
			}
			

			if (mGames.empty())
				ERR("No games found in DedicatedGames.xml. Fix it please.\n");
		}
		else
			ERR("Can't parse DedicatedGames.xml. Fix it please.\n");

		return;
	}
	ERR("Error Opening DedicatedGames.xml. Fix it please.\n");
	
}
	
