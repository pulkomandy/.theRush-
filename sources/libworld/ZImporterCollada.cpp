///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZImporter3DS.h
// Creation : 15/06/2007
// Author : Cedric Guillemet
// Description :
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../libbase/LibBase.h"
#include "../librender/librender.h"
//#include "../libplatform/ZenFWVRAMService.h"


#include "tinyxml.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
    A supporter:
    Les animations
    Les skins
    Les hierarchies
    Les FX
*/
class ZFx;

void ReadFloat(float & vt, tstring const& str);
void ReadVector2(tvector2 & vt, tstring const& str);
void ReadVector3(tvector3 & vt, tstring const& str);
void ReadVector4(tvector4 & vt, tstring const& str);

class ZImporterCollada : public IImporter
{
public:
    DECLARE_SERIALIZABLE
public:
    ZImporterCollada()
    {
		mFirstMeshInstance = NULL;
    }

    virtual ~ZImporterCollada() {}
    bool Import(const char *szPath, const char *pBuf, ZMeshInstance *pFirst,
		bool bKeepVBOInRam = false, ZGameResources *pGR = NULL);
    virtual tulong GetMemoryUsed() const { return sizeof(ZImporterCollada); }

protected:

    void LoadCameras(TiXmlElement* pElt);
    void LoadImages(TiXmlElement* pElt, const char *szPath);
    void LoadGeometry(TiXmlElement* pElt);
    void LoadSceneGraph(TiXmlElement* pElt, ZGameResources *pGR);
    void LoadControllers(TiXmlElement* pElt);
    void AddSceneGraphNode(TiXmlElement* pNodeElem, ZTransform *parent, ZGameResources *pGR);
    void LoadMaterials(TiXmlElement* pElt, const char *szPath);
    void LoadEffects(TiXmlElement* pElt, const char *szPath);
    void SetBindedMaterial(TiXmlElement* pElt, ZMeshInstance *pMesh);
    void LoadAnimations(TiXmlElement* pLibraryElem);

    bool mbZToY;
	bool mbKeepVBORam;
	ZMeshInstance * mFirstMeshInstance;

	typedef struct effectParameters
	{

		effectParameters(const effectParameters& tocpy)
		{
			mName = tocpy.mName;
			mFloatValue = tocpy.mFloatValue;
			pTex = tocpy.pTex;
		}


		effectParameters()
		{
			pTex = NULL;
		}
		std::string mName;
		tvector4 mFloatValue;
		ZTexture *pTex;
	} effectParameters;

    // collada struct
	typedef struct colladaCam
	{
		float xfov;
		float ratio;
		float znear, zfar;
	} colladaCam;


    typedef struct mixArray
    {
        mixArray(std::vector<float>* afloats, uint aNbToCopy, uint aoffset, uint astride, uint VAF)
        {
            mfloats = afloats;
            mNbToCopy = aNbToCopy;
            offset = aoffset;
            stride = astride;
            mVAF = VAF;
        }

        std::vector<float>* mfloats;
        uint mNbToCopy;
        uint offset;
        uint stride;
        uint mVAF;
    } mixArray;


	typedef struct explodedAnim_t
	{
		explodedAnim_t()
		{
			mTimeArray = NULL;
			mTranslationArray = NULL;
			mScalingArray = NULL;
			mRotationArray_x = NULL;
			mRotationArray_y = NULL;
			mRotationArray_z = NULL;
			mAnim = NULL;
			mbNeedResetBaseTranslationX =
			mbNeedResetBaseTranslationY =
			mbNeedResetBaseTranslationZ = false;
		}


		std::vector<float> *mTimeArray;
		std::vector<float> *mTranslationArray;
		std::vector<float> *mScalingArray;
		std::vector<float> *mRotationArray_x;
		std::vector<float> *mRotationArray_y;
		std::vector<float> *mRotationArray_z;
		bool mbNeedResetBaseTranslationX;
		bool mbNeedResetBaseTranslationY;
		bool mbNeedResetBaseTranslationZ;
		ZAnimation *mAnim;
	} explodedAnim_t;

	// libraries
    typedef struct tImportDatas
    {
	    std::map<tstring , colladaCam> mColladaCameras;
	    std::map<tstring , std::vector<float> > mColladaFloatArray;
        std::map<tstring , ZMesh * > mColladaMeshes;
        std::map<tstring , unsigned int > mArrayStride;
        std::map<tstring , ZTexture* > mTextures;
        std::map<tstring , tstring > mMaterials;
		std::map<tstring , std::list<ZTexture*> > mEffectTextures;
		std::map<tstring , std::vector<float> > mAnimFloatArray;
		std::map<tstring , ZAnimation*> mAnimationNode;
		std::map<tstring , explodedAnim_t> mAnimationExplodedDatas;

		std::map<tstring , ZFx*> mEffectEffect;
		std::map<tstring , std::vector<effectParameters> > mEffectParameters;

    } tImportDatas;

    tImportDatas *mMI;

    typedef std::map<tstring , std::vector<float> > tfloatarray;
    typedef std::map<tstring , ZMesh * > tmesharray;
	typedef std::map<tstring , std::list<ZTexture*> > ttexarray;
    typedef std::map<tstring , ZTexture* > ttex;

	typedef std::map<tstring , ZFx*> tfxarray;

    typedef std::map<tstring , unsigned int > tstride;
	typedef std::map<tstring , std::vector<effectParameters> > EffectParameters_t;



    inline void ConvertVector3(tvector3 *pvec)
    {
        if (mbZToY)
        {
            pvec->x = -pvec->x;
			float temp = pvec->y;
			pvec->y = pvec->z;
			pvec->z = temp;
        }

    }

    static int sortMixArray(const void *arg1, const void *arg2 )
    {
        if ( ((mixArray*)arg1)->mVAF<((mixArray*)arg2)->mVAF ) return -1;
        if ( ((mixArray*)arg1)->mVAF>((mixArray*)arg2)->mVAF ) return 1;
        return 0;
    }
};


BEGIN_SERIALIZATION(ZImporterCollada)
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::LoadImages(TiXmlElement* pElt, const char *szPath)
{
    TiXmlElement* pImgElem = pElt->FirstChildElement("image");
	for(;pImgElem;pImgElem = pImgElem->NextSiblingElement("image"))
	{
        const char *imgId = pImgElem->Attribute("id");
        const char *name = pImgElem->Attribute("name");
        ZTexture *pTex = (ZTexture *)FindInstance(ZTexture, name);

		const char *surfaceName = pImgElem->FirstChildElement("init_from")->FirstChild()->ToText()->Value();
        if (pTex)
        {
            mMI->mTextures.insert(std::make_pair(imgId, pTex));
        }
        else
        {
            tstring fnames = szPath;
            fnames += surfaceName;
			if (strstr(surfaceName,"file:///"))
			{
				fnames = surfaceName+8;
			}
			if (strstr(surfaceName,"file://"))
			{
				fnames = szPath;
				fnames += tstring(surfaceName).GetFileName();
			}

			ZTexture *pTex = (ZTexture *)FindInstance(ZTexture, fnames);
			if (pTex)
			{
				mMI->mTextures.insert(std::make_pair(imgId, pTex));
			}
			else
			{
				pTex = DirectLoadTex(fnames.c_str());
				mMI->mTextures.insert(std::make_pair(imgId, pTex));
			}
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////


void ZImporterCollada::LoadMaterials(TiXmlElement* pElt, const char *szPath)
{
	/* a faire:
		association material /FX
		appliquer le fx et les parametres ligne 1078
	*/
    TiXmlElement* pMatElem = pElt->FirstChildElement("material");
	for(;pMatElem;pMatElem = pMatElem->NextSiblingElement("material"))
	{
        const char *imgId = pMatElem->Attribute("id");
        //const char *name = pMatElem->Attribute("name");

        TiXmlElement* pInstance = pMatElem->FirstChildElement("instance_effect");
        if (pInstance)
        {
            mMI->mMaterials.insert(std::make_pair(imgId, tstring(pInstance->Attribute("url")+1)));

			TiXmlElement* pMatElemp = pInstance->FirstChildElement("setparam");
			for(;pMatElemp;pMatElemp = pMatElemp->NextSiblingElement("setparam"))
			{
				effectParameters popp;
				effectParameters *param = &popp;


				const char *matparamRef = pMatElemp->Attribute("ref");
				param->mName = matparamRef;
				TiXmlElement* stpmv;
				if (stpmv= pMatElemp->FirstChildElement("float"))
				{
					ReadFloat((float&)param->mFloatValue.x, tstring(stpmv->FirstChild()->ToText()->Value()));
				}
				else
				if (stpmv= pMatElemp->FirstChildElement("float2"))
				{
					ReadVector2((tvector2&)param->mFloatValue, tstring(stpmv->FirstChild()->ToText()->Value()));
				}
				else
				if (stpmv= pMatElemp->FirstChildElement("float3"))
				{
					ReadVector3((tvector3&)param->mFloatValue, tstring(stpmv->FirstChild()->ToText()->Value()));
				}
				else
				if (stpmv= pMatElemp->FirstChildElement("float4"))
				{
					ReadVector4((tvector4&)param->mFloatValue, tstring(stpmv->FirstChild()->ToText()->Value()));
				}
				else
				if (stpmv= pMatElemp->FirstChildElement("int"))
				{
					ReadFloat((float&)param->mFloatValue.x, tstring(stpmv->FirstChild()->ToText()->Value()));
				}
				else
				{
					if (stpmv = pMatElemp->FirstChildElement("surface"))
					{
						if (stpmv = stpmv->FirstChildElement("init_from"))
						{
							tstring fnlo = szPath;
							fnlo += stpmv->FirstChild()->ToText()->Value();

							tstring tnb = fnlo;
							tnb.ToUpper();
							/* FIXME
							if (tnb.Find("PUB.BMP"))
							{
								IAdvertising *pAdv = (IAdvertising*)FirstInstanceOf(ZAdvertising);
								param->pTex = pAdv->GetAdvertisingTexture();
							}
							else
							*/
							{
								param->pTex = DirectLoadTex(fnlo.c_str());
							}
						}
					}
				}
				mMI->mEffectParameters[imgId].push_back(popp);

				/*
<setparam ref="g_NormalTexture">
          <surface type="2D">
            <init_from>bloc01normal.tga</init_from>
            <format>A8R8G8B8</format>
          </surface>
        </setparam>
*/
				/*
				if (pMatElemp->FirstChildElement()->ToText() && pMatElemp->FirstChildElement()->ToText()->Value())
				{
					effectParameters param;
					if (!stricmp(pMatElemp->FirstChildElement()->ToText()->Value(),"Float3") )
					{
						ReadVector3((tvector3&)param.mFloatValue, tstring(pMatElemp->FirstChildElement()->ToText()->Value()));
					}
				}
				//<float3>0 0 0</float3>
				*/


			}
        }
		// save param node
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::LoadEffects(TiXmlElement* pElt, const char *szPath)
{
    TiXmlElement* pFXElem = pElt->FirstChildElement("effect");
	for(;pFXElem;pFXElem = pFXElem->NextSiblingElement("effect"))
	{
        //const char *id = pFXElem->Attribute("id");
        const char *name = pFXElem->Attribute("name");
#if 0
        TiXmlElement* pFXProfileCommonElem = pFXElem->FirstChildElement("profile_COMMON");
	    for(;pFXProfileCommonElem;pFXProfileCommonElem = pFXProfileCommonElem->NextSiblingElement("profile_COMMON"))
	    {

            TiXmlElement* pFXParam = pFXProfileCommonElem->FirstChildElement("newparam");
	        for(;pFXParam;pFXParam = pFXParam->NextSiblingElement("newparam"))
	        {
                TiXmlElement* pFXSurface = pFXParam->FirstChildElement("surface");
	            for(;pFXSurface;pFXSurface = pFXSurface->NextSiblingElement("surface"))
	            {
                    const char *surfaceName = pFXSurface->FirstChildElement("init_from")->FirstChild()->ToText()->Value();

                    ttex::iterator ittex= mMI->mTextures.find(surfaceName);
                    if (ittex == mMI->mTextures.end())
                        ERR("Cannot find texture %s", surfaceName);

					mMI->mEffectTextures[name].push_back((*ittex).second);
					/*
                    ttexarray::iterator itt = mMI->mEffectTextures.find(name);
                    if (itt == mMI->mEffectTextures.end())
                    {
                        std::vector<ZTexture*> avals;
                        avals.push_back((*ittex).second);
                        mMI->mEffectTextures.insert(std::make_pair(name, avals));
                        //ERR("Cannot find Effect textures '%s'\r\n",symbol);
                    }
                    else
                    {
                    //ZTexture *pTex = (*ittex).second;
                    //mMI->mEffectTextures.insert(std::make_pair(name, pTex));
                        (*itt).second.push_back((*ittex).second);
                    }
					*/
                        //[name].push_back(pTex);
                }
            }
        }
		// extra for .fx
#endif
        TiXmlElement* pFXProfileExtraElem = pFXElem->FirstChildElement("extra");
	    for(;pFXProfileExtraElem;pFXProfileExtraElem = pFXProfileExtraElem->NextSiblingElement("extra"))
	    {
			TiXmlElement* pFXProfileExtraTech = pFXProfileExtraElem->FirstChildElement("technique");
			for(;pFXProfileExtraTech;pFXProfileExtraTech = pFXProfileExtraTech->NextSiblingElement("technique"))
			{
				const char *szprofile = pFXProfileExtraTech->Attribute("profile");
				if (!strcmp(szprofile,"NV_import"))
				{
					TiXmlElement* pFXProfileExtraTechimp = pFXProfileExtraTech->FirstChildElement("import");
					for(;pFXProfileExtraTechimp;pFXProfileExtraTechimp = pFXProfileExtraTechimp->NextSiblingElement("import"))
					{
						const char *szurl = pFXProfileExtraTechimp->Attribute("url");
						char tmpfxurl[512];
						const char * ptmpfxurl = tmpfxurl;
						tstring zapsl = szurl;
						zapsl.Replace("%5C","\\");

#ifdef WIN32
						sprintf_s(tmpfxurl, 512, szurl);
#else
						snprintf(tmpfxurl, 512, szurl);
#endif
						if (tmpfxurl[0] == '.') ptmpfxurl++;
						//const char *szimpprofile = pFXProfileExtraTechimp->Attribute("profile");

						tstring fnames = szPath;
						fnames += zapsl;

						ZFx *pfx = DirectLoad(fnames.c_str());
						tfxarray::iterator itt = mMI->mEffectEffect.find(name);
						if (itt == mMI->mEffectEffect.end())
						{
							mMI->mEffectEffect.insert(std::make_pair(name, pfx));
						}

					}
				}
			}
		}
		/*
      <extra>
        <technique profile="MAX3D">
          <faceted>0</faceted>
          <double_sided>0</double_sided>
          <wireframe>0</wireframe>
          <face_map>0</face_map>
        </technique>
        <technique profile="NV_import">
          <import profile="d3dfx" url=".%5Ctlens.fx"/>
        </technique>
      </extra>
	  */
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::LoadCameras(TiXmlElement* pElt)
{
    TiXmlElement* pCamElem = pElt->FirstChildElement("camera");
	for(;pCamElem;pCamElem = pCamElem->NextSiblingElement("camera"))
	{
        //PSM_Camera *pCam = PSM_RenderDevice::GetInterface().GetCamera();
        //pCam->SetName();

        TiXmlElement* pOptic = pCamElem->FirstChildElement("optics");
        if (pOptic)
        {
            TiXmlElement* pTech = pOptic->FirstChildElement("technique_common");
            if (pTech)
            {

                TiXmlElement* pPers = pTech->FirstChildElement("perspective");
                if (pPers)
                {
					colladaCam colCam;
                    colCam.xfov = (float)atof(pPers->FirstChildElement("xfov")->FirstChild()->ToText()->Value());
					colCam.ratio = (float)atof(pPers->FirstChildElement("aspect_ratio")->FirstChild()->ToText()->Value());
					colCam.zfar = (float)atof(pPers->FirstChildElement("zfar")->FirstChild()->ToText()->Value());
                    colCam.znear = (float)atof(pPers->FirstChildElement("znear")->FirstChild()->ToText()->Value());
					const char *colCamID = pCamElem->Attribute("id");
					mMI->mColladaCameras[colCamID] = colCam;
                }
            }
        }

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadFloatArray(std::vector<float>& array, tstring &str)
{
    str.Replace(' ','\0');
    const char *psz = str.c_str();
    for (unsigned int i=0;i<array.size(); i++)
    {
        array[i] = (float)atof(psz);
        psz+=(strlen(psz)+1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadUI16Array(std::vector<unsigned short>& array, tstring &str)
{
    str.Replace(' ','\0');
    const char *psz = str.c_str();
    for (unsigned int i=0;i<array.size(); i++)
    {
        array[i] = (uint16)atof(psz);
        psz+=(strlen(psz)+1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadUndefinedUI16Array(std::vector<unsigned short>& array, tstring &str)
{
    int32 idx = str.Length();
    str.Replace(' ','\0');
    const char *psz = str.c_str();

    while (idx>0) //for (unsigned int i=0;i<array.size(); i++)
    {
        array.push_back((uint16)atof(psz));
        uint32 clulen = (uint32)(strlen(psz)+1);
        idx -= clulen;
        psz += clulen;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadVector3(tvector3 & vt, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
    vt.x = (float)atof(psz);
    psz+=(strlen(psz)+1);
    vt.y = (float)atof(psz);
    psz+=(strlen(psz)+1);
    vt.z = (float)atof(psz);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadFloat(float & vt, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
    vt = (float)atof(psz);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadVector2(tvector2 & vt, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
    vt.x = (float)atof(psz);
    psz+=(strlen(psz)+1);
    vt.y = (float)atof(psz);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadVector4(tvector4 & vt, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
    vt.x = (float)atof(psz);
    psz+=(strlen(psz)+1);
    vt.y = (float)atof(psz);
    psz+=(strlen(psz)+1);
    vt.z = (float)atof(psz);
    psz+=(strlen(psz)+1);
    vt.w = (float)atof(psz);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadQuaternion(tquaternion & qt, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
    qt.x = (float)atof(psz);
    psz+=(strlen(psz)+1);
    qt.y = (float)atof(psz);
    psz+=(strlen(psz)+1);
    qt.z = (float)atof(psz);
    psz+=(strlen(psz)+1);
    qt.w = (float)atof(psz);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadColor(tcolor & col, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
    col.r = (float)atof(psz);
    psz+=(strlen(psz)+1);
    col.g = (float)atof(psz);
    psz+=(strlen(psz)+1);
    col.b = (float)atof(psz);
    psz+=(strlen(psz)+1);
    col.a = (float)atof(psz);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ReadMatrix(tmatrix & mat, tstring const& str)
{
	tstring temp(str);
    temp.Replace(' ','\0');
    const char *psz = temp.c_str();
	for (int mi = 0;mi<16;mi++)
	{
		mat.m16[mi] = (float)atof(psz);
		psz+=(strlen(psz)+1);
	}
	mat.Transpose();

	float tmps = mat.m16[13];
	mat.m16[13] = mat.m16[14];
	mat.m16[14] = tmps;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tmatrix BuildMatrix(const char *tr, const char *rt, const char *sc)
{
	tmatrix ret;
	ret.Identity();

	if (tr)
	{
		tmatrix trm;
		tvector3 trv;
		ReadVector3(trv, tstring(tr));

		trm.Translation(trv);
		ret *= trm;
	}

	if (rt)
	{
		tmatrix rtm;
		tquaternion rtv;
		ReadQuaternion(rtv, tstring(rt));

		rtm.RotationQuaternion(rtv);
		ret *= rtm;
	}

	if (sc)
	{
		tmatrix scm;
		tvector3 scv;
		ReadVector3(scv, tstring(sc));

		scm.Scaling(scv);
		ret *= scm;
	}
	return ret;
}


///////////////////////////////////////////////////////////////////////////////////////////////////


void ZImporterCollada::LoadGeometry(TiXmlElement* pElt)
{
    std::map<tstring, tstring> mSemantics;

    TiXmlElement* pGeomElem = pElt->FirstChildElement("geometry");
	for(;pGeomElem;pGeomElem = pGeomElem->NextSiblingElement("geometry"))
	{
        bool mbFirstIteration = true;
        const char *geometryID = pGeomElem->Attribute("id");

        TiXmlElement* pMeshElem = pGeomElem->FirstChildElement("mesh");
	    for(;pMeshElem;pMeshElem = pMeshElem->NextSiblingElement("mesh"))
	    {
            // per mesh data
            std::vector<uint8> mixVT;
            IVertexArray * pVA = GDD->NewVertexArray();
			DPVA(pVA);
            ZMesh *mgMesh = (ZMesh*)_New(ZMesh);
            mgMesh->SetVertexBuffer(pVA);
            //mMI->mColladaMeshes[geometryID] = mgMesh;
            mMI->mColladaMeshes.insert(std::make_pair(geometryID, mgMesh));

            tvector3 aBMin, aBMax;
            uint32 mVAFormat = 0;

            unsigned int aColladaVTSize = 0;


            TiXmlElement* pSrcElem = pMeshElem->FirstChildElement();
	        for(;pSrcElem;pSrcElem = pSrcElem->NextSiblingElement())
	        {
				tstring sValue(pSrcElem->Value());

				if ( sValue == "source")
				{
					int count = 0;
                    const char *srcID = pSrcElem->Attribute("id");
					//const char *streamVal = NULL;
					TiXmlElement* pfloatArrElem = pSrcElem->FirstChildElement("float_array");
					if (pfloatArrElem)
					{
						count = atoi(pfloatArrElem->Attribute("count"));
                        const char *arrayID = pSrcElem->Attribute("id");//pfloatArrElem->Attribute("id");

						tstring streamVal = pfloatArrElem->FirstChild()->ToText()->Value();
						//srcID = pfloatArrElem->Attribute("id");

                        //




                        std::vector<float> FA;
						FA.resize(count);
						ReadFloatArray(FA, streamVal);

                        mMI->mColladaFloatArray.insert(std::make_pair(arrayID, FA));
					}

					TiXmlElement* pfloatTechElem = pSrcElem->FirstChildElement("technique_common");
					if (pfloatTechElem)
					{
					    TiXmlElement* pfloatAccessorElem = pfloatTechElem->FirstChildElement("accessor");
					    if (pfloatAccessorElem)
					    {
                            int stride = atoi(pfloatAccessorElem->Attribute("stride"));
                            //const char *arrayID = pfloatAccessorElem->Attribute("source")+1;
                            mMI->mArrayStride.insert(std::make_pair(srcID, stride));

                        }
                    }

				}
				else if ( sValue == "vertices")
				{
                    tstring verticesid = pSrcElem->Attribute("id");
                    TiXmlElement* pSemElem = pSrcElem->FirstChildElement("input");
					if (pSemElem)
					{
                        tstring semantic = pSemElem->Attribute("semantic");
                        mSemantics[verticesid] = (pSemElem->Attribute("source")+1);
                    }
				}
				else if ( sValue == "triangles")
				{



                    aColladaVTSize = 0;
                    unsigned int nbInputs = 0;

                    std::vector<mixArray> mInputArray;
                    int maxoffset = -1;




					TiXmlElement* pTriElem = pSrcElem->FirstChildElement();
					for(;pTriElem;pTriElem = pTriElem->NextSiblingElement())
					{
						tstring sTriValue(pTriElem->Value());
						if (sTriValue == "input")
						{
                            //semantic="VERTEX" source="#Box01-mesh-vertices" offset="0"/>
                            tstring semantic = pTriElem->Attribute("semantic");
                            tstring sourceid = pTriElem->Attribute("source")+1;
                            int offset = atoi(pTriElem->Attribute("offset"));
                            if (offset>maxoffset) maxoffset = offset;
                            unsigned int set = pTriElem->Attribute("set")?atoi(pTriElem->Attribute("set")):0;
                            if (semantic == "VERTEX")
                            {
                                const tstring & keyval = mSemantics[sourceid];
                                tfloatarray::iterator iter = mMI->mColladaFloatArray.find(keyval);
                                if (iter == mMI->mColladaFloatArray.end())
                                    ERR("Cannot find float array %s", keyval.c_str());

                                std::vector<float>* pvect = &(*iter).second;//&mMI->mColladaFloatArray[keyval];

                                tstride::iterator itst = mMI->mArrayStride.find(keyval);
                                if ( itst == mMI->mArrayStride.end())
                                    ERR("Cannot find stride %s",keyval.c_str());
                                int strideval = (*itst).second;

                                mInputArray.push_back(mixArray(pvect, 3*sizeof(float), set, strideval, VAF_XYZ));
                                aColladaVTSize+=3*sizeof(float);
                                nbInputs++;
                                mVAFormat |= VAF_XYZ;
                                // switch Y/Z
                                if (mbFirstIteration)
                                {
                                    aBMin = aBMax = *(tvector3*)&(*pvect)[0];
                                    ConvertVector3(&aBMin);
                                    ConvertVector3(&aBMax);
                                    for (uint sw = 0;sw<pvect->size(); sw+=3)
                                    {
                                        ConvertVector3((tvector3*)&(*pvect)[sw]);
                                        aBMin.Minimize(*(tvector3*)&(*pvect)[sw]);
                                        aBMax.Maximize(*(tvector3*)&(*pvect)[sw]);
                                    }

                                }
                            }
                            else if (semantic == "NORMAL")
                            {
                                tfloatarray::iterator iter = mMI->mColladaFloatArray.find(sourceid);
                                if (iter == mMI->mColladaFloatArray.end())
                                    ERR("Cannot find float array %s", sourceid.c_str());

                                tstride::iterator itst = mMI->mArrayStride.find(sourceid);
                                if ( itst == mMI->mArrayStride.end())
                                    ERR("Cannot find stride %s",sourceid.c_str());
                                int strideval = (*itst).second;

                                mInputArray.push_back(mixArray(&(*iter).second/*mMI->mColladaFloatArray[sourceid]*/, 3*sizeof(float), offset, strideval, VAF_NORMAL));
                                std::vector<float>* pvect = &(*iter).second;//&mMI->mColladaFloatArray[sourceid];
                                aColladaVTSize+=3*sizeof(float);
                                nbInputs++;
                                mVAFormat |= VAF_NORMAL;
                                if (mbFirstIteration)
                                {
                                    for (uint sw = 0;sw<pvect->size(); sw+=3)
                                    {
                                        ConvertVector3((tvector3*)&(*pvect)[sw]);
                                    }
                                }
                            }
                            else if (semantic == "TEXCOORD")
                            {
                                /*
                                std::map<tstring , std::vector<float> >::iterator itertexc =  mMI->mColladaFloatArray.begin();
                                for (; itertexc != mMI->mColladaFloatArray.end(); ++itertexc)
                                    LOG(" %s\r\n", (*itertexc).first.c_str());
                                    */
								if (set == 1)
                                {
									tfloatarray::iterator iter = mMI->mColladaFloatArray.find(sourceid);
									if (iter == mMI->mColladaFloatArray.end())
										ERR("Cannot find float array %s", sourceid.c_str());

									tstride::iterator itst = mMI->mArrayStride.find(sourceid);
									if ( itst == mMI->mArrayStride.end())
										ERR("Cannot find stride %s",sourceid.c_str());
									int strideval = (*itst).second;

									std::vector<float>* pvect = &(*iter).second;//mMI->mColladaFloatArray[sourceid];
									mInputArray.push_back(mixArray(&(*iter).second/*&mMI->mColladaFloatArray[sourceid]*/, 2*sizeof(float), offset, strideval, VAF_TEX0));

                                    aColladaVTSize+=2*sizeof(float);
                                    nbInputs++;
                                    mVAFormat |= VAF_TEX0;
                                    if (mbFirstIteration)
                                    {
                                        for (uint sw = 1;sw<pvect->size(); sw+=3)
                                        {
                                            (*pvect)[sw] = 1-(*pvect)[sw];
                                        }
                                    }
                                }
								else if (set == 2)
                                {
									tfloatarray::iterator iter = mMI->mColladaFloatArray.find(sourceid);
									if (iter == mMI->mColladaFloatArray.end())
										ERR("Cannot find float array %s", sourceid.c_str());

									tstride::iterator itst = mMI->mArrayStride.find(sourceid);
									if ( itst == mMI->mArrayStride.end())
										ERR("Cannot find stride %s",sourceid.c_str());
									int strideval = (*itst).second;

									std::vector<float>* pvect = &(*iter).second;//mMI->mColladaFloatArray[sourceid];
									mInputArray.push_back(mixArray(&(*iter).second/*&mMI->mColladaFloatArray[sourceid]*/, 2*sizeof(float), offset, strideval, VAF_TEX1));

                                    aColladaVTSize+=2*sizeof(float);
                                    nbInputs++;
                                    mVAFormat |= VAF_TEX1;

                                    if (mbFirstIteration)
                                    {
                                        for (uint sw = 1;sw<pvect->size(); sw+=3)
                                        {
                                            (*pvect)[sw] = 1-(*pvect)[sw];
                                        }
                                    }
                                }
                            }
                            else if (semantic == "TEXTANGENT")
                            {
                                if (set == 1)
                                {
                                tfloatarray::iterator iter = mMI->mColladaFloatArray.find(sourceid);
                                if (iter == mMI->mColladaFloatArray.end())
                                    ERR("Cannot find float array %s", sourceid.c_str());

                                tstride::iterator itst = mMI->mArrayStride.find(sourceid);
                                if ( itst == mMI->mArrayStride.end())
                                    ERR("Cannot find stride %s",sourceid.c_str());
                                int strideval = (*itst).second;

                                mInputArray.push_back(mixArray(&(*iter).second/*mMI->mColladaFloatArray[sourceid]*/, 3*sizeof(float), offset, strideval, VAF_BITANGENT));
								std::vector<float>* pvect = &(*iter).second;
                                aColladaVTSize+=3*sizeof(float);
                                nbInputs++;
                                mVAFormat |= VAF_BITANGENT;
                                if (mbFirstIteration)
                                {
                                    for (uint sw = 0;sw<pvect->size(); sw+=3)
                                    {
                                        ConvertVector3((tvector3*)&(*pvect)[sw]);
                                    }
                                }
                                }
                            }
                            else if (semantic == "TEXBINORMAL")
                            {
                                if (set == 1)
                                {
                                tfloatarray::iterator iter = mMI->mColladaFloatArray.find(sourceid);
                                if (iter == mMI->mColladaFloatArray.end())
                                    ERR("Cannot find float array %s", sourceid.c_str());

                                tstride::iterator itst = mMI->mArrayStride.find(sourceid);
                                if ( itst == mMI->mArrayStride.end())
                                    ERR("Cannot find stride %s",sourceid.c_str());
                                int strideval = (*itst).second;

                                mInputArray.push_back(mixArray(&(*iter).second/*mMI->mColladaFloatArray[sourceid]*/, 3*sizeof(float), offset, strideval, VAF_BINORMAL));
								std::vector<float>* pvect = &(*iter).second;
                                aColladaVTSize+=3*sizeof(float);
                                nbInputs++;
                                mVAFormat |= VAF_BINORMAL;
                              if (mbFirstIteration)
                                {
                                    for (uint sw = 0;sw<pvect->size(); sw+=3)
                                    {
                                        ConvertVector3((tvector3*)&(*pvect)[sw]);
                                    }
                                }
                                }

                            }
						}
						else if (sTriValue == "p")
						{
                            mbFirstIteration = false;

                            // sort input based on their VAF
                            mbFirstIteration = false;
                            qsort(&mInputArray[0], mInputArray.size(), sizeof(mixArray), sortMixArray);

                            // get tris indices

							tstring triIndices = pTriElem->FirstChild()->ToText()->Value();
							std::vector<unsigned short> localIndices;
							ReadUndefinedUI16Array(localIndices, triIndices);
                            uint nbIdx = (uint)localIndices.size() ;

                            // mix vertex array

                            int stIdx = (int)mixVT.size()/aColladaVTSize;
                            // set submeshs start index & submesh count !!!!
                            // stIdx,
                            int smCnt = (nbIdx / nbInputs);
                            mgMesh->AddSubMesh(stIdx, smCnt);

                            mixVT.resize( ((aColladaVTSize * (stIdx + smCnt) )) );
                            uint8 * mixVT2 = &mixVT[stIdx * aColladaVTSize];
                            for (unsigned int j=0;j<nbIdx;j+=(maxoffset+1))
                            {
                                for (unsigned int i=0;i<nbInputs;i++)
                                {
                                    uint16 p =localIndices[j+mInputArray[i].offset];

                                    //std::vector<float>* pvect = mInputArray[i].mfloats;
                                    int tmpstrd = mInputArray[i].stride;
                                    float *pfptr = &(*mInputArray[i].mfloats)[p*tmpstrd];

                                    memcpy(mixVT2, pfptr, mInputArray[i].mNbToCopy);
                                    mixVT2+=mInputArray[i].mNbToCopy;
                                }
                            }
                            // Create Mesh

                            mgMesh->SetPrimitive(PRIM_TRIANGLELIST);
						}
					}
				}
            }


            // end of mesh, create arrays here
            int nbVts = (int)mixVT.size()/aColladaVTSize;
            //pVA->Init(mVAFormat, nbVts, mbKeepVBORam);
			int GVTS = GetVertexSizeFromFormat(mVAFormat);

            // optimise array before

            uint16 *nInd = (uint16*)malloc((nbVts * sizeof(uint16)));
            uint8 *newVtArray = (uint8*)malloc(GVTS * nbVts * sizeof(uint8));
            int nNbInd;
            int newNbVt;
            mgMesh->RemoveRedundancy(&mixVT[0], GVTS, nbVts, nInd, &nNbInd,
                newVtArray,
                &newNbVt);

			IIndexArray * pIA = GDD->NewIndexArray();
			DPIA(pIA);
			mgMesh->SetIndexBuffer(pIA);
			mgMesh->GetBVolume().SetBoxMinMax(aBMin, aBMax);
			
			


            // reinit
            pVA->Init(mVAFormat, newNbVt, mbKeepVBORam);

            // set index array

            pIA->Init(nNbInd);
            memcpy(pIA->Lock(VAL_WRITE), nInd, nNbInd*sizeof(uint16));
            pIA->Unlock();

            // Create vertex array
            float *mptr = (float*)pVA->Lock(VAL_WRITE);
            memcpy(mptr, newVtArray, pVA->GetVertexSize() * pVA->GetVertexCount());
            pVA->Unlock();
			free(newVtArray);
			free(nInd);
			

			mgMesh->ComputeBSphere();

			// vertex reordering
#ifndef _DEBUG
			//VertexCacheHelper::OptimizeIndicesOrder2 (nInd, nNbInd, (char*)newVtArray, newNbVt, GVTS);
#endif
			// --
			/*
			if (VRAMService)
			{
				VRAMService->AddVertexArrayJob(mgMesh, pVA, true, newVtArray, newNbVt, true, mVAFormat, true);
				VRAMService->AddIndexArrayJob(mgMesh, pIA, true, nInd, nNbInd*sizeof(uint16), true);
			}
			*/
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::SetBindedMaterial(TiXmlElement* pElt, ZMeshInstance *pMesh)
{
//	IAdvertising *pAdv = (IAdvertising*)FirstInstanceOf(ZAdvertising);

    TiXmlElement* pBindMat = pElt->FirstChildElement("bind_material");
	for(;pBindMat;pBindMat = pBindMat->NextSiblingElement("bind_material"))
    {
        TiXmlElement* pBindTech = pBindMat->FirstChildElement("technique_common");
	    for(;pBindTech;pBindTech = pBindTech->NextSiblingElement("technique_common"))
        {
            int nbSubMesh = 0;
            TiXmlElement* pBindInst = pBindTech->FirstChildElement("instance_material");
	        for(;pBindInst;pBindInst = pBindInst->NextSiblingElement("instance_material"))
            {
                const char *symbol = pBindInst->Attribute("symbol");

                ttexarray::iterator itt = mMI->mEffectTextures.find(symbol);
                if (itt == mMI->mEffectTextures.end())
                {
					// look for material
					EffectParameters_t::iterator itertpa = mMI->mEffectParameters.find(symbol);
					if (itertpa != mMI->mEffectParameters.end())
					{

						tfxarray::iterator fxitl = mMI->mEffectEffect.find(symbol);
						if (fxitl != mMI->mEffectEffect.end())
						{
							pMesh->GetMaterial(nbSubMesh)->setEffect( (*fxitl).second );
							if ( (*fxitl).second->doesBlend() )
								pMesh->SetTransparent(true);
						}

						pMesh->GetMaterial(nbSubMesh)->connectEffect(true, false);
						for (uint zzb = 0; zzb < (*itertpa).second.size(); zzb++)
						{
							effectParameters& efp = (*itertpa).second[zzb];
							if (efp.pTex)
							{

								pMesh->GetMaterial(nbSubMesh)->addTexture(efp.pTex);
							}
							else
							{
								FFxSetParam *ppra = pMesh->GetMaterial(nbSubMesh)->getNamedParam(efp.mName.c_str());
								if (ppra)
								{
									ppra->setVector(efp.mFloatValue);
									ppra->Apply();
								}
							}
						}


						nbSubMesh++;
						continue;
					}
					// look for material



                    LOG("Cannot find Effect textures '%s'\r\n",symbol);
					ZTexture *pTex = DirectLoadTex("./ZenithDatas/checker.dds");
					/*GDD->NewTexture();//(*)NewClass(ZTexture, ZTexture);
					pTex->LoadFile("./ZenithDatas/checker.dds");
					*/
					


					pMesh->GetMaterial(nbSubMesh)->addTexture(pTex);

					continue;
                }

				//std::map<tstring , slist<ZTexture*> >::m
                //return mEffectTextures[symbol];
				bool materialIsAd = false;
				//bool materialIsStartupTexture = false;
				std::list<ZTexture*>::iterator juh = (*itt).second.begin();
                for (;juh != (*itt).second.end();++juh) // i<mMI->mEffectTextures[symbol].size(); i++)
                {
					/*
					if(pAdv && pAdv->IsAnAdvertTexture(juh->mItem->GetName()))//  (*itt).second[i]->GetName()))
						materialIsAd = true;
					if (strstr(juh->mItem->GetName(), "StartupMovieTexture"))
						materialIsAd = true;
						*/

                    pMesh->GetMaterial(nbSubMesh)->addTexture((*juh));

				}

/*
				if (materialIsStartupTexture)
				{
					pMesh->GetMaterial(nbSubMesh)->setEffect(pAdv->GetBigScreenShader());
					pMesh->GetMaterial(nbSubMesh)->addTexture(pAdv->GetBigScreenDotTexture());
				}
				else
				*/
				if (materialIsAd)
				{
					/*
					if (pAdv)
					{

						pMesh->GetMaterial(nbSubMesh)->setEffect(pAdv->GetBigScreenShader());
						pMesh->GetMaterial(nbSubMesh)->addTexture(pAdv->GetBigScreenDotTexture());

					}
					*/
				}
				else
				{
					// fx
					tfxarray::iterator itt = mMI->mEffectEffect.find(symbol);
					if (itt != mMI->mEffectEffect.end())
					{

						pMesh->GetMaterial(nbSubMesh)->setEffect((*itt).second);
						if ((*itt).second->doesBlend())
							pMesh->SetTransparent(true);

					}
				}

                nbSubMesh++;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::AddSceneGraphNode(TiXmlElement* pNodeElem, ZTransform *parent, ZGameResources *pGR)
{
    const char *nodeID = pNodeElem->Attribute("id");

	const char *nodeName;

	nodeName = pNodeElem->Attribute("name");

    //const char *nodetype = pNodeElem->Attribute("type");

    //LOG("New node %s\r\n",nodeName);

	if (!nodeName)
	{
		WARN("Collada importer found an unnamed graph node. Please correct it.\n");
		return;

	}
	bool bIsCollisionGeom = false;
	if (strstr(nodeName, "_collision"))
	{
		LOG("New collision mesh %s\r\n", nodeName);
		bIsCollisionGeom = true;
	}
	else 
		LOG("New node %s\r\n", nodeName);

    ZTransform *aTr = NULL;

    tmatrix trMat, scMat, rtMat;
    trMat.Identity();
    scMat.Identity();
    rtMat.Identity();
	tmatrix matpivot = tmatrix::identity;

	tvector3 scTR;
	tvector3 scVT;
	tquaternion scRT;

	scVT = tvector3::one;
	scTR = tvector3::zero;
	scRT.Identity();

	bool bHasMatrix = false;
    // node instances/ info
    TiXmlElement* pNodeDetails = pNodeElem->FirstChildElement();
    for (;pNodeDetails;pNodeDetails = pNodeDetails->NextSiblingElement())
    {
        tstring sValue(pNodeDetails->Value());

        if (sValue == "instance_camera")
        {
            /*
            PSM_Camera *pCam = PSM_RenderDevice::GetInterface().GetCamera();
            pCam->SetName(nodeName);
            pNode = pCam;
            */
            ZCamera *pCam = (ZCamera*)_New(ZCamera);
            aTr = pCam->GetTransform();

            // reset camera params
            const char *nodeUrl = pNodeDetails->Attribute("url");
            nodeUrl ++; // bypass #
            pCam->SetRatio(mMI->mColladaCameras[nodeUrl].ratio);
            pCam->SetVFov(mMI->mColladaCameras[nodeUrl].xfov);
            pCam->SetPlanes(mMI->mColladaCameras[nodeUrl].znear, mMI->mColladaCameras[nodeUrl].zfar);

        }
        else if (sValue == "instance_geometry")
        {
            ZMeshInstance * zzmi;
			if (mFirstMeshInstance)
			{
				zzmi = mFirstMeshInstance;
				mFirstMeshInstance = NULL;
			}
			else
			{
				if (!bIsCollisionGeom)
					zzmi = (ZMeshInstance*)_New(ZMeshInstance);
			}
			ZMesh *toassmesh = NULL;

			const char *nodeUrl = pNodeDetails->Attribute("url");
			nodeUrl ++; // bypass #

			tmesharray::iterator meshiter = mMI->mColladaMeshes.find(nodeUrl);
			if (meshiter == mMI->mColladaMeshes.end())
				ERR("ERR : Cannot find mesh %s \n", nodeUrl);
			else
				toassmesh = (*meshiter).second;


				

			if (!bIsCollisionGeom)
			{
				zzmi->SetShadowCaster(true);
				aTr = zzmi->GetTransform();//NewClass(ZTransform);


				zzmi->SetMesh(toassmesh);// mMI->mColladaMeshes[nodeUrl]);

				zzmi->SetName(nodeName);

				SetBindedMaterial(pNodeDetails, zzmi);
			}
			else
			{
				// Collision mesh
				if (parent)
				{
					ZSpatialEntity *pse = parent->GetSpatialEntity();
					if (pse && pse->GetClassID() == ClassIDZMeshInstance)
					{ 
						ZMeshInstance *pmi = (ZMeshInstance *)pse->QueryInstance();
						pmi->GetMesh()->SetPhysicTriMesh(ZPhysicTriMesh::Build(toassmesh));
						continue; // no child supported for physic model (leaf)
					}
				}
				continue;

			}

        }
		else if (sValue == "translate")
		{
			tvector3 trVT;
			ReadVector3(trVT, tstring(pNodeDetails->FirstChild()->ToText()->Value()));
			ConvertVector3(&trVT);
			trMat.Translation(trVT);
			scTR = trVT;
		}
		else if (sValue == "rotate")
		{
			tvector4 rtV;
			ReadVector4(rtV, tstring(pNodeDetails->FirstChild()->ToText()->Value()));

			tvector3 axis(rtV.x, rtV.y, rtV.z);
			ConvertVector3(&axis);
			rtMat.RotationAxis(axis, rtV.w * DEGTORAD );
			tquaternion scRTTemp;
			scRTTemp.RotationAxis(axis, rtV.w * DEGTORAD );
			scRTTemp.Normalize();
			scRT *= scRTTemp;
		}
		else if (sValue == "scale")
		{
			ReadVector3(scVT, tstring(pNodeDetails->FirstChild()->ToText()->Value()));
			std::swap(scVT.y, scVT.z);
			scMat.Scaling(scVT);
		}
        else if (sValue == "matrix")
        {
			ReadMatrix(matpivot, tstring(pNodeDetails->FirstChild()->ToText()->Value()));
			bHasMatrix = true;

		}
    }
	if (!aTr)
	{
		aTr = (ZTransform*)_New(ZTransform);
		//aTr->AddRef();
	}
	aTr->SetName(nodeName);
    // set transform
    if (aTr)
    {

		aTr->SetBaseScale(scVT);
		aTr->SetBaseTranslation(scTR);
		aTr->SetBaseRotation(scRT);
/*
		if ((bSeekPivot)&&(bMatPivotFound))
			aTr->SetPivotMatrix(matpivot);
*/
		if (bHasMatrix)
			aTr->SetPivot(matpivot);

        // parenting
        aTr->SetParent(parent);

		// got animation?
		std::map<tstring, ZAnimation*>::iterator iteranim = mMI->mAnimationNode.find(nodeID);
		if (iteranim != mMI->mAnimationNode.end())
		{
			/* FIXME
			aTr->GetAnimCtrl()->SetOwnerTransform(aTr);
			aTr->GetAnimCtrl()->Play((*iteranim).second);
			*/
		}
	}

	// waiting for next node to be pivot
		// add childs
	TiXmlElement* pNodeElemChild = pNodeElem->FirstChildElement("node");

	for(;pNodeElemChild;pNodeElemChild = pNodeElemChild->NextSiblingElement("node"))
	{
		AddSceneGraphNode(pNodeElemChild, aTr, pGR);
	}


	aTr->Update();

	// resources

	if ((!parent)&&(pGR))
		pGR->AddResource(aTr);
}

void ZImporterCollada::LoadSceneGraph(TiXmlElement* pElt, ZGameResources *pGR)
{
    TiXmlElement* pSceneElem = pElt->FirstChildElement("visual_scene");
	for(;pSceneElem;pSceneElem = pSceneElem->NextSiblingElement("visual_scene"))
	{

        TiXmlElement* pNodeElem = pSceneElem->FirstChildElement("node");
	    for(;pNodeElem;pNodeElem = pNodeElem->NextSiblingElement("node"))
	    {
            AddSceneGraphNode(pNodeElem, NULL, pGR);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::LoadControllers(TiXmlElement* pElt)
{
    TiXmlElement* pctrlElem = pElt->FirstChildElement("controller");
	for(;pctrlElem;pctrlElem = pctrlElem->NextSiblingElement("controller"))
	{
        TiXmlElement* pskinElem = pctrlElem->FirstChildElement("skin");
	    for(;pskinElem;pskinElem = pskinElem->NextSiblingElement("skin"))
	    {
            //const char *src = pskinElem->Attribute("source");

            //const char *bindShapeMatrix = pskinElem->FirstChildElement("bind_shape_matrix")->FirstChild()->ToText()->Value();

            // source
            TiXmlElement* pskinSource = pskinElem->FirstChildElement("source");
	        for(;pskinSource;pskinSource = pskinSource->NextSiblingElement("source"))
	        {
                // float array
                TiXmlElement* pskinSourceFA = pskinSource->FirstChildElement("float_array");
	            for(;pskinSourceFA;pskinSourceFA = pskinSourceFA->NextSiblingElement("float_array"))
	            {
                }
                // name array
                TiXmlElement* pskinSourceNA = pskinSource->FirstChildElement("float_array");
	            for(;pskinSourceNA;pskinSourceNA = pskinSourceNA->NextSiblingElement("float_array"))
	            {
                }
                // technique common
                TiXmlElement* pskinSourceTC = pskinSource->FirstChildElement("technique_common");
	            for(;pskinSourceTC;pskinSourceTC = pskinSourceTC->NextSiblingElement("technique_common"))
	            {
                }
            }

            // joints
			/*
            TiXmlElement* pskinJoints = pskinElem->FirstChildElement("joints");
	        for(;pskinJoints;pskinJoints = pskinJoints->NextSiblingElement("joints"))
	        {
                TiXmlElement* pskinJointsInput = pskinJoints->FirstChildElement("input");
	            for(;pskinJointsInput;pskinJointsInput = pskinJointsInput->NextSiblingElement("input"))
	            {
                    //<input semantic="JOINT" source="#Mmerc01-mesh-skin-joints"/>
                    const char *semantic = pskinJointsInput->Attribute("semantic");
                    const char *source = pskinJointsInput->Attribute("source");
                }
            }
			 */

            // weights
			/*
            TiXmlElement* pskinWeights = pskinElem->FirstChildElement("vertex_weights");
	        for(;pskinWeights;pskinWeights = pskinWeights->NextSiblingElement("vertex_weights"))
	        {
                const char *szCount = pskinWeights->Attribute("count");
                TiXmlElement* pskinWeightsInput = pskinWeights->FirstChildElement("input");
	            for(;pskinWeightsInput;pskinWeightsInput = pskinWeightsInput->NextSiblingElement("input"))
	            {
                    //<input semantic="JOINT" source="#Mmerc01-mesh-skin-joints"/>
                    const char *semantic = pskinWeightsInput->Attribute("semantic");
                    const char *source = pskinWeightsInput->Attribute("source");
                    const char *offset = pskinWeightsInput->Attribute("offset");
                }
                const char *szV = pskinWeights->FirstChildElement("v")->FirstChild()->ToText()->Value();
                const char *szVcount = pskinWeights->FirstChildElement("vcount")->FirstChild()->ToText()->Value();
            }
			 */

        }

    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZImporterCollada::LoadAnimations(TiXmlElement* pElt)
{
	bool bDeleteTransArray = false;
    TiXmlElement* pctrlAnim = pElt->FirstChildElement("animation");
	for(;pctrlAnim;pctrlAnim = pctrlAnim->NextSiblingElement("animation"))
	{
		//
		ZAnimation *pAnim = NULL;
		//

		std::vector<float> *mWorkingArray;
		std::vector<float> *mTimeArray;

		// sources
        TiXmlElement* pAnimSrc = pctrlAnim->FirstChildElement("source");
	    for(;pAnimSrc;pAnimSrc = pAnimSrc->NextSiblingElement("source"))
	    {
			// source id

			//const char *sourceID = pAnimSrc->Attribute("id");

 			TiXmlElement* pfloatArrElem = pAnimSrc->FirstChildElement("float_array");
			if (pfloatArrElem)
			{
				int count = atoi(pfloatArrElem->Attribute("count"));
                const char *arrayID = pfloatArrElem->Attribute("id");

				tstring streamVal = pfloatArrElem->FirstChild()->ToText()->Value();

                std::vector<float> FA;
				FA.resize(count);
				ReadFloatArray(FA, streamVal);

                mMI->mAnimFloatArray.insert(std::make_pair(arrayID, FA));
			}

			// technique
			TiXmlElement* pAnimTech = pAnimSrc->FirstChildElement("technique_common");
			for(;pAnimTech;pAnimTech = pAnimTech->NextSiblingElement("technique_common"))
			{
				TiXmlElement* pAnimAccess = pAnimTech->FirstChildElement("accessor");
				for(;pAnimAccess;pAnimAccess = pAnimAccess->NextSiblingElement("accessor"))
				{
					const char *accessorSource = pAnimAccess->Attribute("source")+1;

					TiXmlElement* pAnimParam = pAnimAccess->FirstChildElement("param");
					for(;pAnimParam;pAnimParam = pAnimParam->NextSiblingElement("param"))
					{
						const char *paramName = pAnimParam->Attribute("name");
						//const char *paramType = pAnimParam->Attribute("type");
/*
						mWorkingArray =
						if (!strcmp(paramName, "TIME"))
						{
						*/
							std::map<tstring , std::vector<float> >::iterator iter = mMI->mAnimFloatArray.find(accessorSource);
							if (iter != mMI->mAnimFloatArray.end())
								mWorkingArray = &(*iter).second;

							if (!strcmp(paramName, "TIME"))
								mTimeArray = &(*iter).second;
							/*
						}
						else if (!strcmp(paramName, "X"))
						{
							std::map<tstring , std::vector<float> >::iterator iter = mMI->mAnimFloatArray.find(accessorSource);
							if (iter != mMI->mAnimFloatArray.end())
								mTranslationArray = &(*iter).second;
						}
						else if (!strcmp(paramName, "ANGLE"))
						{
							std::map<tstring , std::vector<float> >::iterator iter = mMI->mAnimFloatArray.find(accessorSource);
							if (iter != mMI->mAnimFloatArray.end())
								mRotationArray[mRotationArrayIndex++] = &(*iter).second;
						}
						*/
					}
				}
			}
		}
		// samplers
		/*
        TiXmlElement* pAnimSamp = pctrlAnim->FirstChildElement("sampler");
	    for(;pAnimSamp;pAnimSamp = pAnimSamp->NextSiblingElement("sampler"))
	    {
			const char * samplerID = pAnimSamp->Attribute("id");

		}
		 */

		// channel
        TiXmlElement* pAnimChan = pctrlAnim->FirstChildElement("channel");
	    for(;pAnimChan;pAnimChan = pAnimChan->NextSiblingElement("channel"))
	    {
			//const char * chanSource = pAnimChan->Attribute("source");
			const char * chanTarget = pAnimChan->Attribute("target");
			char NodeDest[512];
			char NodeDestChan[512];
			NodeDest[0] = 0;
			NodeDestChan[0] = 0;
			// get destination
			bool p2 = false;
			for (uint ic=0; ic < strlen(chanTarget); ic++)
			{
				char tmpc[]={0,0};
				tmpc[0] = chanTarget[ic];
				if (chanTarget[ic] == '/')
					p2 = true;
				else
				if (p2)
					strcat_s(NodeDestChan, 512, tmpc);
				else
					strcat_s(NodeDest, 512, tmpc);
			}

			explodedAnim_t *mWEA;

			std::map<tstring, explodedAnim_t>::iterator iterwea = mMI->mAnimationExplodedDatas.find(NodeDest);
			if (iterwea != mMI->mAnimationExplodedDatas.end())
				mWEA = &(*iterwea).second;
			else
			{
				mMI->mAnimationExplodedDatas.insert(std::make_pair(NodeDest, explodedAnim_t()));

				iterwea = mMI->mAnimationExplodedDatas.find(NodeDest);
				mWEA = &(*iterwea).second;
			}

/*
			mTimeArray = NULL;
			mTranslationArray = NULL;
			mScalingArray = NULL;
			mRotationArray_x = NULL;
			mRotationArray_y = NULL;
			mRotationArray_z = NULL;
*/

			mWEA->mTimeArray = mTimeArray;
			if (!strcmp(NodeDestChan, "RotZ.ANGLE"))
			{
				mWEA->mRotationArray_z = mWorkingArray;

			}
			else if (!strcmp(NodeDestChan, "RotY.ANGLE"))
			{
				mWEA->mRotationArray_y = mWorkingArray;
			}
			else if (!strcmp(NodeDestChan, "RotX.ANGLE"))
			{
				mWEA->mRotationArray_x = mWorkingArray;
			}
			else if (!strcmp(NodeDestChan, "translation"))
			{
				mWEA->mTranslationArray = mWorkingArray;
				bDeleteTransArray=false;
			}
			else if (!strcmp(NodeDestChan, "translation.Z"))
			{
				if (!mWEA->mTranslationArray)
					mWEA->mTranslationArray = new std::vector<float>;
				bDeleteTransArray = true;
				mWEA->mTranslationArray->resize(mWorkingArray->size()*3);
				for (uint convz = 0;convz<mWorkingArray->size();convz++)
				{
					(*mWEA->mTranslationArray)[convz*3+2] = (*mWorkingArray)[convz];
				}
			}
			else if (!strcmp(NodeDestChan, "translation.X"))
			{
				if (!mWEA->mTranslationArray)
					mWEA->mTranslationArray = new std::vector<float>;
				bDeleteTransArray = true;
				mWEA->mTranslationArray->resize(mWorkingArray->size()*3);
				for (uint convz = 0;convz<mWorkingArray->size();convz++)
				{
					(*mWEA->mTranslationArray)[convz*3] = (*mWorkingArray)[convz];
				}
			}
			else if (!strcmp(NodeDestChan, "translation.Y"))
			{
				if (!mWEA->mTranslationArray)
					mWEA->mTranslationArray = new std::vector<float>;
				bDeleteTransArray = true;
				mWEA->mTranslationArray->resize(mWorkingArray->size()*3);
				for (uint convz = 0;convz<mWorkingArray->size();convz++)
				{
					(*mWEA->mTranslationArray)[convz*3+1] = (*mWorkingArray)[convz];
				}
			}
			std::map<tstring, ZAnimation*>::iterator iteri = mMI->mAnimationNode.find(NodeDest);
			if (iteri != mMI->mAnimationNode.end())
			{
				// already got an animation
				pAnim = (*iteri).second;
			}
			else
			{

				pAnim = (ZAnimation*)_New(ZAnimation);
				const char * animID = pctrlAnim->Attribute("id");
				pAnim->SetName(animID);

				mMI->mAnimationNode.insert(std::make_pair(NodeDest, pAnim));
			}
			mWEA->mAnim = pAnim;

		}
	}

	std::map<tstring, explodedAnim_t>::iterator iterwea = mMI->mAnimationExplodedDatas.begin();
	for (;iterwea != mMI->mAnimationExplodedDatas.end(); ++iterwea)
	{
		explodedAnim_t *it = &(*iterwea).second;
		if (it->mTranslationArray)
		{
			ZAnimationTrack<tvector3> *nTrack = new ZAnimationTrack<tvector3>;
			for (unsigned int i=0;i<it->mTimeArray->size();i++)
			{
				tvector3 transVt((*it->mTranslationArray)[i*3], (*it->mTranslationArray)[i*3+1], (*it->mTranslationArray)[i*3+2]);
				ConvertVector3(&transVt);
				nTrack->AddKey((*it->mTimeArray)[i], transVt);
			}
			((ZAnimation*)it->mAnim)->AddAnimationTrack(nTrack, ZANIMATION_INTERP_LINEAR);
			it->mAnim->SetType(ANIM_TRANSLATION);
		}
		//
		if ((bDeleteTransArray)&&(it->mTranslationArray))
		{
			delete it->mTranslationArray;

		}
		// rotation
		if (it->mRotationArray_x||it->mRotationArray_y||it->mRotationArray_z)
		{
			ZAnimationTrack<tquaternion> *nTrack = new ZAnimationTrack<tquaternion>;
			for (unsigned int i=0;i<it->mTimeArray->size();i++)
			{
				tmatrix res;
				float rtx=0, rty=0, rtz=0;
				/*
				rtx.Identity();
				rty.Identity();
				rtz.Identity();
				*/

				if (it->mRotationArray_x)
					rtx = DEGTORAD * (*it->mRotationArray_x)[i];
				if (it->mRotationArray_y)
					rtz = DEGTORAD * (*it->mRotationArray_y)[i];
				if (it->mRotationArray_z)
					rty = DEGTORAD * (*it->mRotationArray_z)[i];

				tquaternion resq2;
				resq2.RotationAxis(tvector3::YAxis, rty);

				tquaternion resq3;
				resq3.RotationAxis(tvector3::XAxis, rtx);

				tquaternion resq4;
				resq4.RotationAxis(tvector3::ZAxis, rtz);

				resq2 *= resq3;
				resq2 *= resq4;

				resq2.Normalize();

				nTrack->AddKey((*it->mTimeArray)[i], resq2);
			}
			((ZAnimation*)it->mAnim)->AddAnimationTrack(nTrack, ZANIMATION_INTERP_LINEAR);
			it->mAnim->SetType(it->mAnim->GetType()+ANIM_ROTATION);
		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZImporterCollada::Import(const char *szPath, const char *pBuf, ZMeshInstance *pFirst, 
							  bool bKeepVBOInRam, ZGameResources *pGR)
{
	PROFILER_START(ColladaImport);
    /////////////////////////////////////////////////
    // LOAD THE DOCUMENT

	mbKeepVBORam = bKeepVBOInRam;
    tstring mfn = szPath;
	mFirstMeshInstance = pFirst;
	/*
    mfn += szName;
	ZFile mXMLFile;
	if (!mXMLFile.Open(mfn))
	{
        ERR("Couldn't load Collada XML file '%s'!\r\n",mfn.c_str());
		PROFILER_END();
        return false;
	}
	char *xmlbuf = new char [mXMLFile.GetSize()+1];
	mXMLFile.Read(xmlbuf, mXMLFile.GetSize());
	xmlbuf[mXMLFile.GetSize()] =0;
*/
	TiXmlDocument pXmlDoc;
	if (!pXmlDoc.Parse(pBuf))
	{
		//delete [] xmlbuf;
        ERR("Couldn't parse Collada XML file '%s'!\r\n",mfn.c_str());
		PROFILER_END();
        return false;
	}
		/*
    TiXmlDocument pXmlDoc(mfn.c_str());
    if(pXmlDoc.LoadFile()==false)
    {
        ERR("Couldn't load Collada XML file '%s'!\r\n",mfn.c_str());
        return false;
    }
	*/
    //LOG("Importing Collada XML File : '%s'.\r\n",mfn.c_str());
    //Get the root.
    TiXmlElement* pRootElem = pXmlDoc.RootElement();

    /////////////////////////////////////////////////
    // FIRST SETUP

    //Set up axis as Y.
    mbZToY = false;

    tImportDatas aMI;
    mMI = &aMI;

    //Load assets element.
    TiXmlElement *pAssetElem = pRootElem->FirstChildElement("asset");
    if(pAssetElem)
    {
        TiXmlElement *pUpAxisElem = pAssetElem->FirstChildElement("up_axis");
        if(pUpAxisElem)
        {
            TiXmlText *pAxisText = pUpAxisElem->FirstChild()->ToText();

            if(tstring(pAxisText->Value())=="Z_UP"){
                mbZToY = true;
                LOG("Z IS UP.\r\n");
            }
        }

    }
    else
    {
        WARN("Couldn't find asset element!\n");
    }



    /////////////////////////////////////////////////
    // ITERATE THE LIBRARIES BEFORE SCENE LIBRARIES
    TiXmlElement* pLibraryElem = pRootElem->FirstChildElement();
    while(pLibraryElem)
    {
        tstring sType(pLibraryElem->Attribute("type"));
        tstring sValue(pLibraryElem->Value());

        // Lights
        if((sType == "LIGHT" || sValue == "library_lights") /*&& apColladaLightVec*/)
        {
            LOG("Loading type: %s\n",sType.c_str());
            //LoadLights(pLibraryElem,*apColladaLightVec);
            //Log(" --- \n");
        }

        pLibraryElem = pLibraryElem->NextSiblingElement();
    }

    //////////////////////////////////////////////
    // LOAD SCENE
    //if(apColladaScene)
    {
        TiXmlElement* pSceneElem = pRootElem->FirstChildElement("library_visual_scenes");
        if(pSceneElem)
        {
            pSceneElem = pSceneElem->FirstChildElement("visual_scene");
            //if(pSceneElem==NULL) ;//Warning("No visual scene element found!\n");
        }

        if(pSceneElem==NULL) pSceneElem = pRootElem->FirstChildElement("scene");

        if(pSceneElem==NULL)
        {
            //Warning("No scene element found!\n");
        }
        else
        {
            //Get start and end time (MAYA ONLY!)
            TiXmlElement* pExtraElem = pSceneElem->FirstChildElement("extra");
            if(pExtraElem)
            {
                // Get techniques
                TiXmlElement* pExtraTechElem = pExtraElem->FirstChildElement("technique");
                for(;pExtraTechElem; pExtraTechElem = pExtraTechElem->NextSiblingElement("technique"))
                {
                    //Check for maya profile
                    tstring sProfile(pExtraTechElem->Attribute("profile"));
                    if(sProfile == "MAYA")
                    {
                        //Iterate params
                        TiXmlElement *pParam = pExtraTechElem->FirstChildElement();
                        for(;pParam; pParam = pParam->NextSiblingElement())
                        {
                            tstring sName(pParam->Attribute("name"));
                            if(sName=="")
                            {
                                sName = pParam->Value();
                            }

                            //sName = cString::ToLowerCase(sName);
                            sName.ToLower();

							/*
                            TiXmlText *pText = pParam->FirstChild()->ToText();
                            float fValue = (float)atof(pText->Value());
                            if(sName == "start_time") apColladaScene->mfStartTime = fValue;
                            else if(sName == "end_time") apColladaScene->mfEndTime = fValue;
                            */
                        }
                    }

//                    apColladaScene->mfDeltaTime = apColladaScene->mfEndTime - apColladaScene->mfStartTime;
                }

                //LOG("Anim Start: %f End: %f\n",apColladaScene->mfStartTime, apColladaScene->mfEndTime);
            }
            else
            {
                WARN("No 'extra scene' element found!\n");
            }

            //Load all nodes in the scene.
            TiXmlElement* pNodeElem = pSceneElem->FirstChildElement("node");
            while(pNodeElem)
            {
                //LoadColladaScene(pNodeElem, &apColladaScene->mRoot, apColladaScene, apColladaLightVec);

                pNodeElem = pNodeElem->NextSiblingElement("node");
            }
        }
    }

    /////////////////////////////////////////////////
    // ITERATE THE AFTER SCENE LIBRARIES

    pLibraryElem = pRootElem->FirstChildElement();
    while(pLibraryElem)
    {
        tstring sType(pLibraryElem->Attribute("type"));
        tstring sValue(pLibraryElem->Value());

        // Image
        if ( (sValue == "library_cameras"))
        {
            LoadCameras(pLibraryElem);
        }
        else
        if((sType == "IMAGE" || sValue == "library_images") )
        {
            //Loads all images files available
            LoadImages(pLibraryElem, szPath);
        }
        // Texture
        else if((sType == "TEXTURE" || sValue == "library_effects"))
        {
            //Loads all texture and their settings
            LoadEffects(pLibraryElem, szPath);//, *apColladaTextureVec);
        }
        // Material
        else if((sType == "MATERIAL" || sValue == "library_materials") )
        {
            //All materials used are loaded
            LoadMaterials(pLibraryElem, szPath);//, *apColladaMaterialVec);
        }
        // Geometry
        else if((sType == "GEOMETRY" || sValue == "library_geometries"))
        {
            //geometry for all objects in the scene are loaded.
            LoadGeometry(pLibraryElem);
        }
        // Animation
        else if((sType == "ANIMATION" || sValue == "library_animations") )
        {
            LoadAnimations(pLibraryElem);//, *apColladaAnimVec, apColladaScene);
        }
        // Controller
        else if((sType == "CONTROLLER" || sValue == "library_controllers") )
        {
            LoadControllers(pLibraryElem);
        }
		else if ( sValue == "library_visual_scenes")
		{
			LoadSceneGraph(pLibraryElem, pGR);
		}

        pLibraryElem = pLibraryElem->NextSiblingElement();
    }

	//delete [] xmlbuf;

	PROFILER_END();
    return true;
}

