#ifndef ZIMPORTEXPORT_H__
#define ZIMPORTEXPORT_H__

#include "../libbase/ZBaseClass.h"
#include "..\libbase\ZSerializator.h"

class ZMeshInstance;
class ZGameResources;

class  IExporter : public ZBaseClass
{
public:
    
    virtual bool Export(const char *szPath, ZGameResources *pGR) = 0;
};
class IImporter : public ZBaseClass
{
public:
    virtual bool Import(const char *szPath, const char *pBuf, 
		ZMeshInstance *pFirst, bool bKeepVBOInRam = false, ZGameResources *pGR = NULL) = 0;
};

DECLAREZCLASS(ZZenExporter);
DECLAREZCLASS(ZZenImporter);
DECLAREZCLASS(ZImporterCollada);

#endif
