#include "tinyxml.h"
#include "ZMetaDatas.h"
#include "ZFile.h"
#include "ZLogger.h"

void ZMetaDatas::Init(const char *szPathTooXML)
{
	ZFile file;
	if (file.Open(szPathTooXML))
	{
		tstring doc = file.ReadFileString();

		TiXmlDocument xmlDoc;
		if (xmlDoc.Parse(doc.c_str()))
		{
			TiXmlNode* root = xmlDoc.FirstChild( "MetaDatas" );
			
			TiXmlElement* itemElement = root->ToElement();
			
root = itemElement->FirstChild( "Meta" );
			itemElement = root->ToElement();

			//itemElement = root->ToElement();
			for (;itemElement;itemElement = itemElement->NextSiblingElement("Meta"))
			{
				/*
				<Meta File="./Prototype/Common/Sounds/three.wav" Type="Audio" Is3DSound="FALSE" />
<Meta File="./Prototype/Common/Sounds/WallCollisionDefault.wav" Type="Audio" Is3DSound="TRUE" />
*/
				const char *fileName = itemElement->Attribute("File");
				if (!fileName)
				{
					ERR("No file name in MetaDatas description.");
				}
				HASH fileHash = GetHash(fileName);
				if (GetMetaData(fileHash))
				{
					ERR("Hash Collision with %s.", fileName);
				}

				tstring fileType = itemElement->Attribute("Type");
				if (fileType == "Audio")
				{
					const char *a3DSound = itemElement->Attribute("Is3DSound");
					bool is3DSound = a3DSound?(!stricmp(a3DSound,"TRUE")):true;
					
					mMetaDatas.insert(std::make_pair(fileHash, MetaData_t(is3DSound, false)));
				}
				else
				{
					ERR("Bad type in MetaDatas description.");
				}

			}
			

		}
		else
			ERR("Can't pase MetaDatas. Fix it please.\n");

		return;
	}
	ERR("Error Opening MetaDatas. Fix it please.\n");
}

const MetaData_t* ZMetaDatas::GetMetaData(HASH resourceHash) const
{
	std::map<HASH, MetaData_t>::const_iterator iter = mMetaDatas.find(resourceHash);
	if ( iter == mMetaDatas.end())
		return NULL;

	return &(*iter).second;
}

//std::map<HASH, MetaData_t> mMetaDatas;
