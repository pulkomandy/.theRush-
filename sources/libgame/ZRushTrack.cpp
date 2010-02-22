///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../libplatform/libplatform.h"
#include "../libworld/libworld.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION(ZRushTrack)
END_SERIALIZATION()

ZRushTrack* GTrack = NULL;
bool ZRushTrack::UseEditingGizmo = false;

///////////////////////////////////////////////////////////////////////////////////////////////////

ZRushTrack::ZRushTrack()
{
	if (GTrack)
		delete GTrack;
	GTrack = this;

	mTrandBandTransform = NULL;
	mbGizmosVisible = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZRushTrack::~ZRushTrack()
{
	RemoveCollisionMesh();

	if (GTrack == this)
		GTrack = NULL;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::Load(ZFile &mFile)
{
	// fake loading
	// default anim test

	mTrackBandTexture.Attach(DirectLoadTex("./ZenithDatas/TrackBandArrow.dds"));
	mKeyTexture.Attach(DirectLoadTex("./ZenithDatas/fushia.dds"));

	if (this->GetName().Find("Default"))
	{
		CreateDefaultTrack();
	}
	else
	{

		unsigned char format;
		mFile >> format;
		LoadTrack(mFile);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::CreateDefaultTrack()
{
	ZAnimation *pAnim = (ZAnimation*)_New(ZAnimation);
	ZAnimationTrack<tvector3> *nTrack = new ZAnimationTrack<tvector3>;
	ZAnimationTrack<tquaternion> *nTrackq = new ZAnimationTrack<tquaternion>;

	tquaternion nn;
	nn.RotationAxis(tvector3::ZAxis, -45*DEGTORAD);
	tquaternion nn2;
	nn2.RotationAxis(tvector3::ZAxis, 45*DEGTORAD);
	tquaternion nn3;
	nn3.RotationAxis(tvector3::ZAxis, 180*DEGTORAD);
	tquaternion nn4;
	nn4.RotationAxis(tvector3::ZAxis, 270*DEGTORAD);
	tquaternion nn5;
	nn5.RotationAxis(tvector3::ZAxis, -180*DEGTORAD);

	tquaternion nnhl1, nnhl2;
	nnhl1.RotationAxis(tvector3::ZAxis, -20*DEGTORAD);
	nnhl2.RotationAxis(tvector3::ZAxis, 20*DEGTORAD);

	int NbBricks;

	nTrack->AddKey(0, tvector3(0,4,100));
	nTrack->AddKey(1, tvector3(0,4,200));
	nTrack->AddKey(2, tvector3(100,34,300));
	nTrack->AddKey(3, tvector3(200,4,200));
	nTrack->AddKey(4, tvector3(200,4,-200));
	nTrack->AddKey(5, tvector3(100,34,-300));
	nTrack->AddKey(6, tvector3(0,4,-200));
	nTrack->AddKey(7, tvector3(0,40,0));
	nTrack->AddKey(8, tvector3(0,4,100));






	nTrackq->AddKey(0, tquaternion(0,0,0,1));
	nTrackq->AddKey(1, tquaternion(0,0,0,1));
	nTrackq->AddKey(2, nnhl1);
	nTrackq->AddKey(3, tquaternion(0,0,0,1));
	nTrackq->AddKey(4, tquaternion(0,0,0,1));
	nTrackq->AddKey(5, nnhl2);
	nTrackq->AddKey(6, tquaternion(0,0,0,1));
	nTrackq->AddKey(7, tquaternion(0,0,0,1));
	nTrackq->AddKey(8, tquaternion(0,0,0,1));
	NbBricks = 10;

	/*
	else
	{
	nTrack->AddKey(0, tvector3(40,0,50)*tvector3(5,1,5));
	nTrack->AddKey(1, tvector3(40,0,90)*tvector3(5,1,5));
	nTrack->AddKey(2, tvector3(60,10,110)*tvector3(5,1,5));
	nTrack->AddKey(3, tvector3(50,20,140)*tvector3(5,1,5));
	nTrack->AddKey(4, tvector3( 0,17.5,130)*tvector3(5,1,5));
	nTrack->AddKey(5, tvector3( 0,5,90)*tvector3(5,1,5));
	nTrack->AddKey(6, tvector3(40,10,70)*tvector3(5,1,5));
	nTrack->AddKey(7, tvector3(70,5,60)*tvector3(5,1,5));
	nTrack->AddKey(8, tvector3(70,0,40)*tvector3(5,1,5));
	nTrack->AddKey(9, tvector3(55,10,45)*tvector3(5,1,5));
	nTrack->AddKey(10, tvector3(45,7.5f, 40)*tvector3(5,1,5));
	nTrack->AddKey(11, tvector3(40,0,50)*tvector3(5,1,5));


	nTrackq->AddKey(0, tquaternion(0,0,0,1));
	nTrackq->AddKey(1, nn);
	nTrackq->AddKey(2, nn2);
	nTrackq->AddKey(3, nn2);
	nTrackq->AddKey(4, tquaternion(0,0,0,1));
	nTrackq->AddKey(5, tquaternion(0,0,0,1));
	nTrackq->AddKey(6, tquaternion(0,0,0,1));
	nTrackq->AddKey(7, nn);
	nTrackq->AddKey(8, nn);
	nTrackq->AddKey(9, nn2);
	nTrackq->AddKey(10, tquaternion(0,0,0,1));
	nTrackq->AddKey(11, tquaternion(0,0,0,1));
	NbBricks = 20;

	}
	*/
	pAnim->AddAnimationTrack(nTrack, ZANIMATION_INTERP_HERMITE);
	pAnim->AddAnimationTrack(nTrackq);
	mAnimation.Attach(pAnim);

	// fake bricks
	mBricks.clear();
	PushBrick("Startup");
	/*
	char *brickNamesForDefaultCreation[]={"Holed","Winged","Chamfered","Flat","Border_Holed","Border_Chamfered","Border_Flat","Default"};

	srand(998);
	for (int i=0;i<NbBricks;i++)
	{
		int rndB = rand()%8;
		for (int j=0;j<10;j++)
			PushBrick(brickNamesForDefaultCreation[rndB]);
	}
	*/
	for (int i=0;i<50;i++)
	{
		PushBrick("Default");
	}

	mTrackBonusEdit.resize(mBricks.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::DeformMesh(ZMesh *meshSrc, const tmatrix & srcMeshMat, ZMesh *meshDst, int i, 	
							const std::vector<float>& meshLengths, const std::vector<float>& meshMin)
{
	int aNbStackedMeshes = mBricks.size();

	uint8 *mvtSrc = (uint8 *)meshSrc->GetVertexBuffer()->Lock(VAL_READONLY);
	tvector3 vtMin(99999999.f, 99999999.f, 99999999.f), vtMax(-99999999.f, -99999999.f, -99999999.f);

	tmatrix ares;

	unsigned int mcnt = meshSrc->GetVertexBuffer()->GetVertexCount();
	int mvs = meshSrc->GetVertexBuffer()->GetVertexSize();

	meshDst->GetVertexBuffer()->Init(meshSrc->GetVertexBuffer()->GetFormat(), mcnt, true);
	uint8 *mvtDst = (uint8 *)meshDst->GetVertexBuffer()->Lock(VAL_WRITE);



	// copy vertices
	memcpy(mvtDst, mvtSrc, mvs * mcnt);

	float oneoL = 1.0f/meshLengths[i];
	for (uint j = 0;j<mcnt;j++)
	{
		tmatrix ares;

		((tvector3*)mvtDst)->TransformVector(srcMeshMat);
		((tvector3*)(mvtDst+12))->TransformVector(srcMeshMat);

		if (meshSrc->GetVertexBuffer()->GetFormat() & VAF_BINORMAL)
		{
			((tvector3*)(mvtDst+24))->TransformVector(srcMeshMat);
			((tvector3*)(mvtDst+36))->TransformVector(srcMeshMat);
		}

		((tvector3*)mvtDst)->z -= meshMin[i];


		float interplv = (((tvector3*)mvtDst)->z*oneoL);
		if (interplv>0.5f)
		{
			InterpolateMatrix(ares, MATS[i], MATS[i+1], interplv-0.5f );
		}
		else
		{
			InterpolateMatrix(ares, MATS[((i-1)+aNbStackedMeshes)%aNbStackedMeshes], MATS[i], interplv+0.5f );
		}


		InterpolateMatrix2(ares, MATS[i], MATS[i+1], interplv);


		ares.m16[15] = 1;
		//ares.OrthoNormalize();

		// --

		((tvector3*)mvtDst)->z = 0;


		((tvector3*)mvtDst)->TransformPoint(ares);
		vtMin.Minimize(*(tvector3*)mvtDst);
		vtMax.Maximize(*(tvector3*)mvtDst);


		((tvector3*)(mvtDst+12))->TransformVector(ares);
		((tvector3*)(mvtDst+12))->Normalize();

		// no color assumed

		if (meshSrc->GetVertexBuffer()->GetFormat() & VAF_BINORMAL)
		{

			((tvector3*)(mvtDst+24))->TransformVector(ares);
			((tvector3*)(mvtDst+24))->Normalize();
			((tvector3*)(mvtDst+36))->TransformVector(ares);
			((tvector3*)(mvtDst+36))->Normalize();

		}


		mvtDst += mvs;
	}


	meshDst->GetBVolume().SetBoxMinMax(vtMin, vtMax);



	meshSrc->GetVertexBuffer()->Unlock();
	meshDst->GetVertexBuffer()->Unlock();

	meshDst->ComputeBSphere();
}


void ZRushTrack::DeformMeshForPhysic(ZMesh *meshSrc, const tmatrix & srcMeshMat, tvector3 *mvtDst, int i, 	
									 const std::vector<float>& meshLengths, const std::vector<float>& meshMin)
{
	int aNbStackedMeshes = mBricks.size();

	uint8 *mvtSrc = (uint8 *)meshSrc->GetVertexBuffer()->Lock(VAL_READONLY);

	tmatrix ares;

	unsigned int mcnt = meshSrc->GetVertexBuffer()->GetVertexCount();
	int mvs = meshSrc->GetVertexBuffer()->GetVertexSize();



	float oneoL = 1.0f/meshLengths[i];
	for (uint j = 0;j<mcnt;j++)
	{
		tmatrix ares;
		*mvtDst = *(tvector3*)(&mvtSrc[j*mvs]);

		mvtDst->TransformVector(srcMeshMat);

		mvtDst->z -= meshMin[i];


		float interplv = (((tvector3*)mvtDst)->z*oneoL);
		if (interplv>0.5f)
		{
			InterpolateMatrix(ares, MATS[i], MATS[i+1], interplv-0.5f );
		}
		else
		{
			InterpolateMatrix(ares, MATS[((i-1)+aNbStackedMeshes)%aNbStackedMeshes], MATS[i], interplv+0.5f );
		}


		InterpolateMatrix2(ares, MATS[i], MATS[i+1], interplv);


		ares.m16[15] = 1;
		//ares.OrthoNormalize();

		// --

		mvtDst->z = 0;


		mvtDst->TransformPoint(ares);

		mvtDst ++;
	}

	meshSrc->GetVertexBuffer()->Unlock();
}

ZPhysicTriMesh * ZRushTrack::MergePhysicMeshes(std::vector<PhysicMeshToMerge_t>& mPhysicMeshesToMerge, int mBigPhysNbIndices, int mBigPhysNbVertices)
{
	unsigned int i;
	tvector3 *mBigPhysVA = new tvector3 [mBigPhysNbVertices];
	unsigned short *mBigPhysIA = new unsigned short [mBigPhysNbIndices];

	int mBigPhysVAAV = 0;
	int mBigPhysIAAV = 0;
	int mBigPhysIADecal = 0;
	for (i= 0;i<mPhysicMeshesToMerge.size();i++)
	{
		PhysicMeshToMerge_t& entry = mPhysicMeshesToMerge[i];
		for (int j=0;j< entry.mBigPhysVASize/entry.mBigPhysVAVTSize;j++)
		{
			memcpy(&mBigPhysVA[mBigPhysVAAV], entry.mBigPhysVertices[j], sizeof(tvector3));
			mBigPhysVAAV ++;
		}
		for (int j=0;j< entry.mBigPhysIASize;j++)
		{
			mBigPhysIA[mBigPhysIAAV++] = entry.mBigPhysIndices[j]+mBigPhysIADecal;
		}
		mBigPhysIADecal += entry.mBigPhysVASize/entry.mBigPhysVAVTSize;
		delete [] entry.mBigPhysVertices;

	}


	int mNewVTNb = 0;
	int nind = 0;
	tvector3 *ReducedVertices = new tvector3 [mBigPhysVAAV];
	ZMesh::RemoveRedundancy2(mBigPhysVA, mBigPhysVAAV, sizeof(tvector3), mBigPhysIA, mBigPhysIAAV, ReducedVertices, &mNewVTNb);


	// create physic mesh
	IIndexArray *mBigPhysIndexArray = GDD->NewIndexArray();
	mBigPhysIndexArray->Init(mBigPhysIAAV);
	IVertexArray *mBigPhysVertexArray = GDD->NewVertexArray();
	mBigPhysVertexArray->Init(VAF_XYZ, mNewVTNb, true);



	memcpy(mBigPhysIndexArray->Lock(VAL_WRITE), mBigPhysIA, mBigPhysIAAV*sizeof(unsigned short));
	memcpy(mBigPhysVertexArray->Lock(VAL_WRITE), ReducedVertices, mNewVTNb*sizeof(tvector3));
	mBigPhysIndexArray->Unlock();
	mBigPhysVertexArray->Unlock();

	// free things
	delete [] ReducedVertices;

	ZMesh *pmodmeshDst = (ZMesh*)_New(ZMesh);


	pmodmeshDst->SetIndexBuffer(mBigPhysIndexArray);
	pmodmeshDst->SetVertexBuffer(mBigPhysVertexArray);

	// add physic mesh to physic world
	ZPhysicTriMesh *newcol = ZPhysicTriMesh::Build(pmodmeshDst);
	mPhysicMeshes.push_back(newcol);



	delete [] mBigPhysVA;
	delete [] mBigPhysIA;

	mPhysicMeshesToMerge.clear();

	return newcol;
}

void ZRushTrack::RemoveCollisionMesh()
{

	std::vector<smartptr<ZPhysicTriMeshInstance> >::iterator iter = mPhysicMeshesInstances.begin();
	for (; iter != mPhysicMeshesInstances.end() ; ++iter)
	{
		if ((*iter)->GetWorld())
			(*iter)->GetWorld()->RemoveCollisionMesh( (*iter).ptr() );
	}

	mPhysicMeshesInstances.clear();
}

void ZRushTrack::Compute()
{
	PROFILER_START(TrackComputation);
	static const int keyGranul = 32;
	unsigned int i, j;

	mDeformedMeshesTransforms.clear();

	// physic mesh instances
	RemoveCollisionMesh();

	//mTrackBonuses.clear();

	// get lengths

	std::vector<float> meshLengths;
	std::vector<float> meshMin;

	float aStackedMeshesLength = 0;
	for (unsigned int ogi=0;ogi<mBricks.size(); ogi++)
	{


		if (!mBricks[ogi].mBrickMesh)
		{
			ZTransform *pTrans = (ZTransform*)gRushGameContent->GetWorldBricksRes()->GetClassByName(mBricks[ogi].mBrickName, ClassIDZTransform);
			if (!pTrans)
				continue;
			mBricks[ogi].mBrickMesh = (ZMeshInstance*)pTrans->GetSpatialEntity()->QueryInstance();

		}

		ZMesh *meshSrc = mBricks[ogi].mBrickMesh->GetMesh();

		//const ZBoundingVolume & bvol = meshSrc->GetBVolume();
		

		tvector3 bmin = mBricks[ogi].mBrickMesh->GetMesh()->GetBVolume().GetBoxMin();
		tvector3 bmax = mBricks[ogi].mBrickMesh->GetMesh()->GetBVolume().GetBoxMax();
		bmin.TransformVector(mBricks[ogi].mBrickMesh->GetTransform()->GetWorldMatrix());
		bmax.TransformVector(mBricks[ogi].mBrickMesh->GetTransform()->GetWorldMatrix());
		tvector3 wmin,wmax;
		wmin.Minimize(bmin, bmax);
		wmax.Maximize(bmin, bmax);

		meshLengths.push_back(wmax.z - wmin.z);
		meshMin.push_back(wmin.z);


		aStackedMeshesLength += wmax.z - wmin.z;//bvol.mBoxMax.z - bvol.mBoxMin.z;

		if (!stricmp(mBricks[ogi].mBrickName, "Startup"))
		{
			mStartupHalfLength = (wmax.z - wmin.z) * 0.5f;
		}
	}

	// Compute spline

	// get keys
	ZAnimation *dt = mAnimation.ptr();
	ZAnimationTrack<tvector3>* zat = (ZAnimationTrack<tvector3>*)dt->m_AnimTrackList[0];
	ZAnimationTrack<tvector4>* zar = (ZAnimationTrack<tvector4>*)dt->m_AnimTrackList[1];

	zat->m_KeyFrameList[zat->m_KeyFrameList.size()-1].m_Value = zat->m_KeyFrameList[0].m_Value;
	zar->m_KeyFrameList[zar->m_KeyFrameList.size()-1].m_Value = zar->m_KeyFrameList[0].m_Value;

	int nbKeys = int(zat->m_KeyFrameList.size()-1);

	samplesPos.resize(nbKeys * keyGranul);
	samplesRot.resize(nbKeys * keyGranul);
	float *lng = new float[nbKeys * keyGranul ];

	for (i=0;i< uint(nbKeys * keyGranul);i++)
	{
		tvector3 res;
		tvector4 resq;
		tlong curkey=0;

		int keytimegra = int(i/float(keyGranul));
		float timeSample = LERP(zat->m_KeyFrameList[keytimegra].m_Time, zat->m_KeyFrameList[keytimegra+1].m_Time, float(i&(keyGranul-1))/float(keyGranul) );
		dt->m_AnimTrackList[0]->Interpolate(&res,
			timeSample,
			curkey);
		samplesPos[i] = res;
		curkey=0;
		dt->m_AnimTrackList[1]->Interpolate(&resq,
			timeSample,
			curkey);
		samplesRot[i] = resq;


	}



	lng[0] = 0;
	float aCurveLength = 0;
	for (i=1;i< uint(nbKeys * keyGranul);i++)
	{
		float stln = Distance(samplesPos[i], samplesPos[i-1]);
		lng[i] = stln + lng[i-1];
		aCurveLength += stln;
	}
	// compute length per keys
	std::vector<float> mPerKeyLength;
	std::vector<float> mPerKeyDistance;
	mPerKeyLength.resize(nbKeys);
	mPerKeyDistance.resize(nbKeys);
	float mpkcurDistance = 0;
	float prevLng = 0;
	for (i=0;i< uint(nbKeys );i++)
	{
		float mpkcurLength = 0;

		for (int j=0;j<keyGranul;j++)
		{
			float newLng = lng[i*keyGranul + j] - prevLng;
			mpkcurDistance += newLng;
			mpkcurLength += newLng;
			prevLng = lng[i*keyGranul + j];
		}
		mPerKeyLength[i] = mpkcurLength;
		if (i != (uint(nbKeys )-1))
			mPerKeyDistance[i+1] = mpkcurDistance;
	}
	// --

	float mulMeshRatio = (aCurveLength/aStackedMeshesLength);
	LOG("Length of stack Meshes : %5.2f Curve length : %5.2f. mulmesh by %5.2f\r\n",aStackedMeshesLength,
		aCurveLength,
		mulMeshRatio);

	// build matrices
	tvector3 localYUpVector(tvector3::YAxis);
	//mMeshOnLineIndex.resize(mBricks.size());




	//std::vector<int> meshlngIdx;
	std::vector<tvector3> resampledPositions;
	std::vector<tvector4> resampledRotations;

	MATS.resize(mBricks.size()+1);
	/*
	for (i=0;i<mBricks.size(); i++)
	{

	}
*/
	// --
	float curlen = 0;
	MATS[0].Translation(samplesPos[0]);
	MATS[mBricks.size()].Translation(samplesPos[0]);
	curlen += meshLengths[0] * mulMeshRatio;
	//meshlngIdx.push_back(0);

	resampledPositions.push_back(samplesPos[0]);
	resampledRotations.push_back(samplesRot[0]);

	j=0;
	for (i = 1 ; i < mBricks.size() ; i++)
	{
		// get key from curlen in key length array
		unsigned int respK = 0;

		for (unsigned int vf = mPerKeyLength.size()-1;vf>=0;vf--)
		{
			if (curlen>mPerKeyDistance[vf])
			{
				respK = vf;
				break;
			}
		}

		float ratioInKey = (curlen-mPerKeyDistance[respK])/mPerKeyLength[respK];
		//LOG("brk %d -> len %5.2f k %d r %5.2f\n",i, curlen, respK, ratioInKey);
		// recompute position/rotation
		tvector3 res;
		tvector4 resq;
		tlong curkey=0;

		//int keytimegra = int(i/float(keyGranul));
		float timeSample = LERP(zat->m_KeyFrameList[respK].m_Time, zat->m_KeyFrameList[respK+1].m_Time, ratioInKey );
		dt->m_AnimTrackList[0]->Interpolate(&res,
			timeSample,
			curkey);
		//samplesPos[i] = res;
		curkey=0;
		dt->m_AnimTrackList[1]->Interpolate(&resq,
			timeSample,
			curkey);
		//samplesRot[i] = resq;

		resampledPositions.push_back(res);
		resampledRotations.push_back(resq);



		/*
		while (lng[j]<curlen)
		j++;
		meshlngIdx.push_back(j);
		*/
		MATS[i].Translation(res);
		curlen += meshLengths[i]*mulMeshRatio;


	}



	// manage rotation shits
	tvector3 curUp(0,1,0);
	tvector3 oldright;
	for (i = 0 ; i < mBricks.size() ; i++)
	{
		tmatrix rotq;

		tvector3 dt1 = resampledPositions[i];//samplesPos[meshlngIdx[i]];
		tvector3 dt2 = resampledPositions[(i+1)%mBricks.size()];//samplesPos[meshlngIdx[  ]];
		tvector3 dir = dt2-dt1;

		dir.Normalize();

		tvector3 right;
		right.Cross(tvector3::YAxis, dir);
		right.Normalize();

		tvector3 up;
		up.Cross(dir, right);
		up.Normalize();


		rotq.RotationAxis(dir, resampledRotations[i].z);



		right.TransformVector(rotq);
		up.TransformVector(rotq);

		// test retournement
		if (i)
		{
			if (right.Dot(oldright)<0)
			{
				right = -right;
				up = -up;
			}
		}

		oldright = right;

		// --
		MATS[i].V4.right = right;
		MATS[i].V4.up = up;
		MATS[i].V4.dir = dir;

	}



	// loopback
	MATS[mBricks.size()] = MATS[0];
	unsigned int mBigPhysNbIndices = 0;
	unsigned int mBigPhysNbVertices = 0;


	int aDeformedMeshToAttachPhysic = 0;

	std::vector<PhysicMeshToMerge_t> mPhysicMeshesToMerge;

	// -- test grouping
	/*
	typedef struct groupmesh_t
	{
	std::vector<ZMeshInstance*> mSources;
	} groupmesh_t;
	std::vector<groupmesh_t> mGroups;
	mGroups.push_back(groupmesh_t());
	groupmesh_t* curgrp = &mGroups[mGroups.size()-1];
	curgrp->mSources.push_back(mBricks[0].mBrickMesh);

	for (i=1;i<mBricks.size(); i++)
	{
	ZMeshInstance *pLast = curgrp->mSources[0];
	ZMeshInstance *pCurrent = mBricks[i].mBrickMesh;
	if ( (pLast->GetMaterial(0)->IsEsqualTo(pCurrent->GetMaterial(0))) && 
	(pLast->GetMesh()->GetVertexBuffer()->GetFormat() == pCurrent->GetMesh()->GetVertexBuffer()->GetFormat()) )
	{
	curgrp->mSources.push_back(pCurrent);
	}
	else
	{
	mGroups.push_back(groupmesh_t());
	curgrp = &mGroups[mGroups.size()-1];
	curgrp->mSources.push_back(pCurrent);
	}
	}
	*/


	// --
	mDeformedMeshInstances.clear();
	for (i=0;i<mBricks.size(); i++)
	{
		ZMesh *meshSrc = mBricks[i].mBrickMesh->GetMesh();



		// build mesh instance
		ZMeshInstance *mi = (ZMeshInstance*)_New(ZMeshInstance);
		mi->SetVisible(true);
		ZMesh *meshDst = (ZMesh*)_New(ZMesh);
		meshDst->SetPrimitive(meshSrc->GetPrimitive());

		for (j=0;j<meshSrc->GetNbSubMeshes();j++)
		{
			uint32 aStart, aCount;
			meshSrc->GetSubMesh(j, &aStart, &aCount);
			meshDst->AddSubMesh(aStart, aCount);
		}

		mi->SetMesh(meshDst);

		// materials
		for (j=0;j<meshSrc->GetNbSubMeshes();j++)
		{
			mi->GetMaterial(j)->Clone(mBricks[i].mBrickMesh->GetMaterial(j));
		}

		meshDst->SetIndexBuffer(meshSrc->GetIndexBuffer());
		IVertexArray * nvt = GDD->NewVertexArray();
		meshDst->SetVertexBuffer(nvt);
		mi->SetShadowCaster(true);

#ifdef _DEBUG
		tstring fmtnn;
		fmtnn.Printf("GeneratedTrackMesh%d", i);
		mi->SetName(fmtnn);
#endif




		// deform it
		DeformMesh(meshSrc, mBricks[i].mBrickMesh->GetTransform()->GetWorldMatrix(), meshDst, i, meshLengths, meshMin);


		// check physic model

		ZPhysicTriMesh *pMod = (ZPhysicTriMesh*)mBricks[i].mBrickMesh->GetMesh()->GetPhysicTriMesh();
		if (pMod)
		{
			ZMesh *pmodmesh = pMod->GetMesh();
			mBigPhysNbIndices += pmodmesh->GetIndexBuffer()->GetIndexCount();
			mBigPhysNbVertices += pmodmesh->GetVertexBuffer()->GetVertexCount();

			tvector3 *mPhysDeformVertices = new tvector3[pmodmesh->GetVertexBuffer()->GetVertexCount()];
			DeformMeshForPhysic(pmodmesh, mBricks[i].mBrickMesh->GetTransform()->GetWorldMatrix(), mPhysDeformVertices, i, meshLengths, meshMin);

			mPhysicMeshesToMerge.push_back(PhysicMeshToMerge_t((unsigned short*)pmodmesh->GetIndexBuffer()->Lock(VAL_READONLY),
				mPhysDeformVertices, 
				pmodmesh->GetVertexBuffer()->GetVertexSize(),
				pmodmesh->GetVertexBuffer()->GetVertexSize() * pmodmesh->GetVertexBuffer()->GetVertexCount(),
				pmodmesh->GetIndexBuffer()->GetIndexCount() ) );


			pmodmesh->GetIndexBuffer()->Unlock();

			if (mBigPhysNbVertices> 60000)
			{
				// build physic big mesh
				ZPhysicTriMesh *pmergedmodel = MergePhysicMeshes(mPhysicMeshesToMerge, mBigPhysNbIndices, mBigPhysNbVertices);
				//mDeformedMeshInstances[aDeformedMeshToAttachPhysic++]->SetPhysicModel(pmergedmodel);
				mBigPhysNbIndices = 0;
				mBigPhysNbVertices = 0;

			}
		}


		mi->GetTransform()->AddToScene(GScene);
		mi->GetTransform()->Update();
		mDeformedMeshInstances.push_back(mi);
		mDeformedMeshesTransforms.push_back(mi->GetTransform()->GetRootTransform());
	}


	if (mBigPhysNbVertices)
	{
		ZPhysicTriMesh *pmergedmodel = MergePhysicMeshes(mPhysicMeshesToMerge, mBigPhysNbIndices, mBigPhysNbVertices);
		//		mDeformedMeshInstances[aDeformedMeshToAttachPhysic++]->set(pmergedmodel);
	}



	//
	delete [] lng;

	BuildSpawnMatrices();

	ComputeAILine();

	BuildBonus();

	if (UseEditingGizmo)
	{
		BuidEditingKeyBoxes();
		ComputeTrackBand(&mAIPoints[0], uint(mBricks.size()*SPLITCOUNT));
	}


	PROFILER_END();
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::BuildBonus()
{
	mTrackBonuses.clear();
	unsigned int i;
	for (i = 0 ; i < mBricks.size() ; i++)
	{
		const TrackBonusEdit_t& tbe = mTrackBonusEdit[i];
		tmatrix bonusMat;
		const AIPoint_t& bpt = mAIPoints[i*SPLITCOUNT + (SPLITCOUNT>>1)];
		bonusMat.V4.right = bpt.mAIRight;
		bonusMat.V4.up = bpt.mAIUp;
		tvector3 matdir;
		matdir.Cross(bpt.mAIRight, bpt.mAIUp);
		bonusMat.V4.dir = matdir;
		bonusMat.V4.position = vector4(bpt.mAIPos.x, bpt.mAIPos.y, bpt.mAIPos.z, 1);


		if (tbe.mRightBonus > 0)
		{

			ZTransform *pTrans = (ZTransform*)gRushGameContent->GetWorldBricksRes()->GetClassByName( (tbe.mRightBonus==1)?"Speed":"Bonus", ClassIDZTransform);
			tmatrix mt;
			mt.Translation( tbe.mBonusDecal, 0, 0);
			mTrackBonuses.push_back(ZTrackBonus::BuildTrackBonus( (mt * bonusMat ), pTrans, (tbe.mRightBonus==1)?ZTrackBonus::TYPE_SPEED:ZTrackBonus::TYPE_BONUS));

		}
		if (tbe.mLeftBonus > 0)
		{
			ZTransform *pTrans = (ZTransform*)gRushGameContent->GetWorldBricksRes()->GetClassByName( (tbe.mLeftBonus==1)?"Speed":"Bonus" , ClassIDZTransform);
			tmatrix mt;
			mt.Translation( -tbe.mBonusDecal, 0, 0);
			mTrackBonuses.push_back(ZTrackBonus::BuildTrackBonus( (mt * bonusMat ), pTrans, (tbe.mLeftBonus==1)?ZTrackBonus::TYPE_SPEED:ZTrackBonus::TYPE_BONUS));

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::BuildSpawnMatrices()
{
	mSpawnMatrices.clear();
	for (unsigned int ogi=0;ogi<mBricks.size(); ogi++)
	{
		if (strstr(mBricks[ogi].mBrickName.c_str(), "Startup") )
		{
			tmatrix mt,mt2;
			mt = MATS[ogi];
			mt2.Translation(-6, 0 ,mStartupHalfLength);
			mSpawnMatrices.push_back(mt * mt2);
			mt2.Translation(6, 0, mStartupHalfLength);
			mSpawnMatrices.push_back(mt * mt2);

		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZRushTrack::GetClosestSampledPoint(const tvector3& mobilePoint,
										tmatrix& trackMatrix,
										tvector3& trackMiddle,
										int &aaCurIdx,
										float &rightLeftFactor,
										const tvector3& lastKnownGravity,
										float &upDownFactor,
										float& distanceToBorder)
{
	//

	if (mAIPoints.empty()) return false;
	if (mBricks.size()<2) return false;
	float tlagStart = gTimer.ForceGetTime();

	PROFILER_START(GetClosestSampledPoint);
	// point 0
	if (lastKnownGravity.Dot(tvector3::XAxis)>0.9f)
	{
		mProjX.Cross(lastKnownGravity, tvector3::ZAxis);
		mProjX.Normalize();
		mProjY.Cross(mProjX, lastKnownGravity);
		mProjY.Normalize();
	}
	else
	{
		mProjX.Cross(lastKnownGravity, tvector3::XAxis);
		mProjX.Normalize();
		mProjY.Cross(mProjX, lastKnownGravity);
		mProjY.Normalize();
	}

	uint loopSC = uint(mBricks.size()*SPLITCOUNT);
	int loopthru = loopSC;

	tvector3 ptCol;
	uint i;
	uint localIdx;

	//uint halfss = 10;

	if (aaCurIdx > loopSC)
	{
		loopthru = loopSC;
		localIdx =0;
	}
	else
	{
		loopthru = 20;
		localIdx = (aaCurIdx-(loopthru/2)+loopSC)%loopSC;
	}


	float oocurdist = 99999999.f;
	int oocurpt = -1;



	tvector3 amobilePoint = projectWithGrav(mobilePoint);

	for (i=0;i<(uint)loopthru;i++)
	{
		tvector3 tv1 = projectWithGrav(mAIPoints[(localIdx+i)%loopSC].mRailRight);
		tvector3 tv2 = projectWithGrav(mAIPoints[(localIdx+i)%loopSC].mRailLeft);


		tvector3 tmpptCol;
		if (CollisionClosestPointOnSegment( amobilePoint, tv1, tv2, tmpptCol))
		{
			float nDist = SquaredDistance(tmpptCol, amobilePoint);
			if (nDist<oocurdist)
			{
				oocurdist = nDist;
				oocurpt = (localIdx+i)%loopSC;
				ptCol = tmpptCol;
			}
		}
	}
	if (oocurpt == -1)
	{
		// pb pour sly N°1
		PROFILER_END();
		return false;
		//DebugBreak();
	}

	aaCurIdx = oocurpt;
	localIdx = oocurpt;
	i = 0;

	float nDistp1=9999999999.f, nDistm1=9999999999.f;
	tvector3 ptColp1, ptColm1;
	// get neighbour segs
	bool nneigfound = false;
	int neidDecal = 1;
	while ((!nneigfound)&&(neidDecal<20))
	{



		tvector3 tv1 = projectWithGrav(mAIPoints[(localIdx+neidDecal)%loopSC].mRailRight);
		tvector3 tv2 = projectWithGrav(mAIPoints[(localIdx+neidDecal)%loopSC].mRailLeft);


		if (CollisionClosestPointOnSegment( amobilePoint, tv1, tv2, ptColp1 ))
		{
			nDistp1 = SquaredDistance(ptColp1, amobilePoint);
			nneigfound = true;
		}

		tv1 = projectWithGrav(mAIPoints[(localIdx-neidDecal+loopSC)%loopSC].mRailRight);
		tv2 = projectWithGrav(mAIPoints[(localIdx-neidDecal+loopSC)%loopSC].mRailLeft);


		if (CollisionClosestPointOnSegment( amobilePoint, tv1, tv2, ptColm1 ))
		{
			nDistm1 = SquaredDistance(ptColm1, amobilePoint);
			nneigfound = true;
		}
		if (!nneigfound)
			neidDecal++;

	}

	if (!nneigfound)
	{
		// pb pour sly N°2
		//DebugBreak();
		PROFILER_END();
		return false;
	}


	int idx2;
	float ndistrt;
	tvector3 ptcolision;
	//tvector4 gqt, gqt2;
	bool bDirInv = false;
	if (nDistp1<nDistm1)
	{
		idx2 = ((localIdx+neidDecal)%loopSC);
		ndistrt = sqrtf(nDistp1);
		ptcolision = ptColp1;

	}
	else
	{
		idx2 = ((localIdx-neidDecal+loopSC)%loopSC);
		ndistrt = sqrtf(nDistm1);
		ptcolision = ptColm1;
		bDirInv  =true;
	}
	//gqt = samplesRot[oocurpt];
	//gqt2 = samplesRot[idx2];
	// lerp1
	float lerp1= 0;//Distance(ptcolision, ptCol);
	lerp1 = sqrtf(oocurdist)/Distance(ptcolision, ptCol);

	// --

	// lerp2
	tvector3 vtlong = projectWithGrav(mAIPoints[(localIdx)%loopSC].mRailLeft - mAIPoints[(localIdx)%loopSC].mRailRight);
	tvector3 vtcourt = ptCol- projectWithGrav(mAIPoints[(localIdx)%loopSC].mRailRight);

	float lerp2 = vtcourt.Length()/ vtlong.Length();
	rightLeftFactor = lerp2;
	//LOG("%5.4f\r\n",rightLeftFactor);
	/*
	if ((lerp1<0)||(lerp1>1)||(lerp2<0)||(lerp2>1))
	{
	//DebugBreak();
	return false;
	}
	*/
	// quaternion avec le lerp1
	// position avec bilerp
	// le up avec lerp1 entre les mAIUp
	// dir =vecteur entre les 2
	// point on track = lerp1

	//tvector4 goodQt;
	tvector3 goodUp, goodDir, goodPointOnTrack, goodProjectedPosition;
	//goodQt.Lerp(gqt, gqt2, lerp1);
	goodUp.Lerp(mAIPoints[oocurpt].mAIUp, mAIPoints[idx2].mAIUp, lerp1);

	if (bDirInv)
	{
		goodDir = mAIPoints[oocurpt].mAIPos - mAIPoints[idx2].mAIPos;

	}
	else
	{
		goodDir = mAIPoints[idx2].mAIPos - mAIPoints[oocurpt].mAIPos;
	}
	goodDir.Normalize();

	goodPointOnTrack.Lerp(mAIPoints[oocurpt].mAIPos, mAIPoints[idx2].mAIPos, lerp1);

	goodProjectedPosition.Lerp( LERP(mAIPoints[oocurpt].mRailRight, mAIPoints[oocurpt].mRailLeft, lerp2),
		LERP(mAIPoints[idx2].mRailRight, mAIPoints[idx2].mRailLeft, lerp2), lerp1);

	upDownFactor = lerp1;

	tvector3 goodRight;
	/*
	goodRight.Cross(goodDir, goodUp);
	goodRight.Normalize();
	*/
	goodRight = LERP((mAIPoints[oocurpt].mRailRight- mAIPoints[oocurpt].mRailLeft), (mAIPoints[idx2].mRailRight- mAIPoints[idx2].mRailLeft), lerp1);
	goodRight.Normalize();

	trackMatrix.V4.dir = goodDir;
	trackMatrix.V4.right = goodRight;
	trackMatrix.V4.up = goodUp;
	trackMatrix.V4.position = goodProjectedPosition;
	trackMatrix.m16[15] = 1;



	/*
	// test mat 2
	int nbSplit = int(mMeshes.size() * 8);

	tvector3 m1right = mRailRight[oocurpt]-mAIPos[oocurpt];
	m1right.Normalize();
	tvector3 m1dir = mAIPos[idx2]-mAIPos[oocurpt];
	m1dir.Normalize();
	tvector3 m1up;
	m1up.Cross(m1dir, m1right);
	m1up.Normalize();

	tvector3 m2right = mRailRight[idx2]-mAIPos[idx2];
	m2right.Normalize();
	tvector3 m2dir = mAIPos[(idx2+1)%nbSplit]-mAIPos[idx2];
	m2dir.Normalize();
	tvector3 m2up;
	m2up.Cross(m2dir, m2right);
	m2up.Normalize();


	tmatrix trckm1(m1right, m1up, m1dir, mAIPos[oocurpt]);
	tmatrix trckm2(m2right, m2up, m2dir, mAIPos[idx2]);

	InterpolateMatrix2(trackMatrix, trckm1, trckm2, lerp1);
	trackMatrix.OrthoNormalize();
	// test mat 2
	*/

	// RAYCAST
	/*
	IMesh *mgMesh = mDeformedInstances[(oocurpt/SPLITCOUNT)%mDeformedInstances.size()]->GetMesh();

	float newT;
	tvector3 gnorm;
	if ( mgMesh->PickRay(mobilePoint, -(goodUp*10), gnorm, newT))
	{
	trackMatrix.position = mobilePoint - (goodUp * newT * 10);

	}
	*/
	// RAYCAST

	trackMiddle = goodPointOnTrack;

	float trackWith = LERP( (mAIPoints[oocurpt].mRailRight-mAIPoints[oocurpt].mRailLeft).Length(),
		(mAIPoints[idx2].mRailRight-mAIPoints[idx2].mRailLeft).Length(), lerp1);

	static const float tolerance = 0.03f;
	if (rightLeftFactor>(1.f-tolerance))
	{
		distanceToBorder = trackWith*(tolerance-(1.f-(rightLeftFactor)));
	}
	else if (rightLeftFactor<tolerance)
	{
		distanceToBorder = trackWith*fabsf(rightLeftFactor-tolerance);
	}

	PROFILER_END();


	//LOG("TTak = %5.5f \n", gTimer.ForceGetTime() - tlagStart);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::ComputeAILine()
{
	int nbSplit = int(mBricks.size() * SPLITCOUNT);

	mAIPoints.resize(nbSplit);

	float *mAIRightLength = new float [nbSplit];
	memset( mAIRightLength, 0, sizeof(float)*nbSplit);

	//int loopsc = nbKeys * keyGranul;
	for (int i=0;i<nbSplit;i++)
	{
		float o1len = float(i&(SPLITCOUNT-1))/float(SPLITCOUNT);
		int matn = i>>3;
		int matnp1 = (i>>3)+1;
		int matnm1 = ( (i>>3)-1+mBricks.size())%mBricks.size();
		tmatrix ares;
		if (o1len>0.5f)
		{
			InterpolateMatrix(ares, MATS[matn], MATS[matnp1], o1len-0.5f );
		}
		else
		{
			InterpolateMatrix(ares, MATS[matnm1], MATS[matn], o1len+0.5f );
		}
		InterpolateMatrix2(ares, MATS[matn], MATS[matnp1], o1len );

		ares.m16[15] = 1;
		ares.OrthoNormalize();

		mAIPoints[i].mAIPos = ares.V4.position;
		mAIPoints[i].mAIRight = ares.V4.right;
		mAIPoints[i].mAIUp = ares.V4.up;

		float linval = (i%(SPLITCOUNT-1))*(1.f/float(SPLITCOUNT));
		float loclen = 12;//LERP(32/*mDimStart[(i/SPLITCOUNT)]*/, 32/*mDimEnd[(i/SPLITCOUNT)]*/, linval) * 0.5f;

		tvector3 tmprail;
		tmprail.TransformVector(tvector3(loclen,0,0), ares);
		mAIPoints[i].mRailLeft = mAIPoints[i].mAIPos - tmprail;
		mAIPoints[i].mRailRight = mAIPoints[i].mAIPos + tmprail;

	}
	// iteration
	/*
	for (uint aii = 0;aii<40;aii++)
	{
	for (i=1;i<nbSplit;i++)
	{
	const tvector3 &tv1 = mAIPos[(i-1)%amod];
	const tvector3 &tv2 = mAIPos[(i+1)%amod];

	tvector3 ptCol;
	CollisionClosestPointOnSegment( mAIPos[i], tv1, tv2, ptCol );
	float rghtlen = (ptCol-mAIPos[i]).Length();

	tvector3 tmpv1;
	tmpv1.Cross(mAIPos[i]- tv1, tv2-tv1);
	float rghtsign = (mAIUp[i].Dot(tmpv1)<0)?1.0f:-1.0f;
	if ( (fabs(mAIRightLength[i])+rghtlen) < 80.0f)
	{
	mAIRightLength[i] +=rghtlen;
	mAIPos[i] += mAIRight[i] * rghtlen * rghtsign;//   = ptCol;
	}
	}
	}
	*/
	// line loop creation

	delete [] mAIRightLength;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

const tmatrix ZRushTrack::GetBrickMatrix(unsigned int aIndex) const
{
	tvector3 aipointdir;
	aIndex %= mAIPoints.size();
	aipointdir.Cross(mAIPoints[aIndex].mAIRight, mAIPoints[aIndex].mAIUp);
	aipointdir.Normalize();

	tmatrix mt(mAIPoints[aIndex].mAIRight, mAIPoints[aIndex].mAIUp, aipointdir, mAIPoints[aIndex].mAIPos);

	return mt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::AddPhysicMeshesToPhysicWorld(ZPhysicWorldBullet *aPhysWorld)
{
	std::vector<smartptr<ZPhysicTriMesh> >::iterator iter = mPhysicMeshes.begin();
	for (; iter != mPhysicMeshes.end(); ++iter)
	{
		mPhysicMeshesInstances.push_back(aPhysWorld->AddPhysicModelToWorld( (*iter).ptr(), NULL, 2, 0xFFFFFFFD));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::Tick(float aTimeEllapsed)
{
	std::vector<smartptr<ZTrackBonus> >::iterator iter = mTrackBonuses.begin();
	for (; iter != mTrackBonuses.end(); ++iter)
	{
		(*iter)->Tick(aTimeEllapsed);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::BuidEditingKeyBoxes()
{
	ZAnimation* dt = (ZAnimation*)mAnimation.ptr();
	ZAnimationTrack<tvector3>* zat = (ZAnimationTrack<tvector3>*)dt->m_AnimTrackList[0];
	ZAnimationTrack<tvector4>* zar = (ZAnimationTrack<tvector4>*)dt->m_AnimTrackList[1];

	// --
	if (!mKeyCubeMesh.ptr())
		mKeyCubeMesh = CreateBox(tvector3::one, tcolor::white);

	// free them
	for (unsigned int kko = 0;kko<mKeyTransforms.size();kko++)
	{
		//GDD->GetContextToBuild()->AddDeferredDeletion(mKeyMeshes[kko]);
		mKeyTransforms[kko]->RemoveFromScene();
	}
	mKeyTransforms.clear();

	//mKeyMeshesIndex = 0;
	for (uint kk = 0;kk<zat->m_KeyFrameList.size()-1;kk++)
	{
		tvector4 curVector = zar->m_KeyFrameList[kk].m_Value;
		tquaternion hlpq(curVector.x, curVector.y, curVector.z, curVector.w);// = zar->m_KeyFrameList[kk].m_Value;
		ZMeshInstance *mimi2;
		mimi2 = (ZMeshInstance*)_New(ZMeshInstance);
		//mKeyMeshes[mKeyMeshesIndex] = mimi2;
		mKeyTransforms.push_back(mimi2->GetTransform());

		mimi2->SetMesh(mKeyCubeMesh.ptr());
		mimi2->SetVisible(true);
#ifdef _DEBUG
		mimi2->SetName("tcube");
#endif
		mimi2->SetSelectable(false);

		mimi2->GetMaterial(0)->addTexture(mKeyTexture.ptr());//>setEffect(mKeyMeshMat.ptr());

		tmatrix bigbig, bigtr;//, bigrot;
		bigbig.Scaling(tvector3(10,10,10));
		bigtr.Translation(zat->m_KeyFrameList[kk].m_Value);
		//bigrot.RotationQuaternion(hlpq);
		mimi2->GetTransform()->SetLocalMatrix(bigbig*bigtr);


		mimi2->GetTransform()->AddToScene(GScene);
		mimi2->GetTransform()->Update();



		//mKeyMeshesIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool ZRushTrack::PickAnimationKey(const tvector3 & rayOrigin, const tvector3 & rayDir, ZAnimation ** pAnim, int *pIndex)
{
	ZAnimation* dt = (ZAnimation*)mAnimation.ptr();
	ZAnimationTrack<tvector3>* zat = (ZAnimationTrack<tvector3>*)dt->m_AnimTrackList[0];
	//ZAnimationTrack<tquaternion>* zar = (ZAnimationTrack<tquaternion>*)dt->m_AnimTrackList[1];

	for (uint kk = 0;kk<zat->m_KeyFrameList.size()-1;kk++)
	{

		tvector3 aminB,amaxB;
		aminB = amaxB= zat->m_KeyFrameList[kk].m_Value;//(-50,-50,950);
		aminB -= tvector3(5,5,5);
		amaxB += tvector3(5,5,5);

		if (isRayAABBoxIntersect(aminB, amaxB, rayOrigin, rayDir))
		{
			*pAnim = mAnimation;
			*pIndex = kk;
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::RemoveTrackPiecesFromScene()
{
	std::vector<ZTransform*>::iterator iter = mDeformedMeshesTransforms.begin();
	for (; iter != mDeformedMeshesTransforms.end() ; ++iter)
	{
		(*iter)->RemoveFromScene();
	}
	mDeformedMeshesTransforms.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::ReplaceBrick(int idx, const char * szBrickName)
{
	if (idx < 0)
		return;
	if (idx >= mBricks.size())
		return;
	mBricks[idx].mBrickMesh = NULL;
	mBricks[idx].mBrickName = szBrickName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::DeleteBrick(int idx)
{
	if (idx < 0)
		return;
	if (idx >= mBricks.size())
		return;
	mBricks.erase(mBricks.begin() + idx);
	mTrackBonusEdit.erase( mTrackBonusEdit.begin() + idx );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::InsertBrickBefore(int idx, const char * szBrickName)
{
	if (idx < 0)
		return;
	if (idx >= (mBricks.size()-1))
		return;
	mBricks.insert(mBricks.begin() + idx, SourceBricks_t(szBrickName, NULL));
	mTrackBonusEdit.insert( mTrackBonusEdit.begin() + idx , TrackBonusEdit_t() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::InsertBrickAfter(int idx, const char * szBrickName)
{
	if (idx < 0)
		return;
	if (idx >= mBricks.size())
		return;

	mBricks.insert(mBricks.begin() + idx + 1, SourceBricks_t(szBrickName, NULL));
	mTrackBonusEdit.insert( mTrackBonusEdit.begin() + idx + 1, TrackBonusEdit_t() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int ZRushTrack::GetRoadMeshInstanceIndex(const ZMeshInstance *pInst ) const
{
	int av = 0;

	std::vector<ZMeshInstance*>::const_iterator iter =  mDeformedMeshInstances.begin();
	for ( ; iter != mDeformedMeshInstances.end() ; ++iter)
	{
		if ( pInst == (*iter) )
			return av;
		av++;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::SetBonus(int idx, char aLeft, char aRight, float decal)
{
	if (idx < 0)
		return;
	if (idx >= mBricks.size())
		return;

	TrackBonusEdit_t* tbe = &mTrackBonusEdit[idx];
	tbe->mLeftBonus = aLeft;
	tbe->mRightBonus = aRight;
	tbe->mBonusDecal = decal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::GetBonus(int idx, char &aLeft, char &aRight, float &decal) const
{
	if (idx < 0)
		return;
	if (idx >= mBricks.size())
		return;

	const TrackBonusEdit_t& tbe = mTrackBonusEdit[idx];
	aLeft = tbe.mLeftBonus;
	aRight = tbe.mRightBonus;
	decal = tbe.mBonusDecal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::ComputeTrackBand(AIPoint_t* mPoints, uint nbPoints)
{
	IVertexArray * normvt = GDD->NewVertexArray(); ////
	DPVA(normvt);
	normvt->Init(VAF_XYZ|VAF_TEX0, (nbPoints-1) * 6 );
	ZMesh * nnormGen = (ZMesh*)_New(ZMesh);
	nnormGen->SetVertexBuffer(normvt);
	nnormGen->SetPrimitive(PRIM_TRIANGLELIST);
	ZMeshInstance *mimi = (ZMeshInstance*)_New(ZMeshInstance);
	mimi->SetMesh(nnormGen);
#ifdef _DEBUG
	mimi->SetName("TrackBand");
#endif
	mimi->SetVisible(true);


	uint8 *normnewVA = (uint8*)normvt->Lock(VAL_WRITE);
	nnormGen->AddSubMesh(0, (nbPoints-1)*6 );

	//mimi->GetMaterial(0)->setEffect(mRaceLineFX.ptr());
	mimi->GetMaterial(0)->addTexture(mTrackBandTexture.ptr());


	tvector3 vMin(999999,99999,99999);
	tvector3 vMax(-999999,-99999,-99999);


	static const int RaceLineHeight = 5;
	for (uint i = 0 ; i < (nbPoints-1) ; i ++)
	{
		int ci = i;
		int cip = (i+1)%nbPoints;

		tvector3 pos;

		pos = mPoints[ci].mAIPos;
		vMin.Minimize(pos);
		vMax.Maximize(pos);
		*((tvector3*)normnewVA) = pos;// + mAIUp[i]*10;
		normnewVA+=12;
		*((tvector2*)normnewVA) = tvector2(0,0);
		normnewVA+=8;

		pos = mPoints[ci].mAIPos + mPoints[i].mAIUp*RaceLineHeight;
		vMin.Minimize(pos);
		vMax.Maximize(pos);

		*((tvector3*)normnewVA) = pos;
		normnewVA+=12;
		*((tvector2*)normnewVA) = tvector2(0,1);
		normnewVA+=8;


		pos = mPoints[cip].mAIPos;
		vMin.Minimize(pos);
		vMax.Maximize(pos);
		*((tvector3*)normnewVA) = pos;// + mAIUp[i]*10;
		normnewVA+=12;
		*((tvector2*)normnewVA) = tvector2(1,0);
		normnewVA+=8;




		pos = mPoints[ci].mAIPos + mPoints[i].mAIUp*RaceLineHeight;
		vMin.Minimize(pos);
		vMax.Maximize(pos);
		*((tvector3*)normnewVA) = pos;
		normnewVA+=12;
		*((tvector2*)normnewVA) = tvector2(0,1);
		normnewVA+=8;


		pos = mPoints[cip].mAIPos + mPoints[i].mAIUp*RaceLineHeight;
		vMin.Minimize(pos);
		vMax.Maximize(pos);
		*((tvector3*)normnewVA) = pos;
		normnewVA+=12;
		*((tvector2*)normnewVA) = tvector2(1,1);
		normnewVA+=8;


		pos = mPoints[cip].mAIPos;
		vMin.Minimize(pos);
		vMax.Maximize(pos);
		*((tvector3*)normnewVA) = pos;// + mAIUp[i]*10;
		normnewVA+=12;
		*((tvector2*)normnewVA) = tvector2(1,0);
		normnewVA+=8;
	}
	normvt->Unlock();


	nnormGen->GetBVolume().SetBoxMinMax(vMin, vMax);
	nnormGen->GetBVolume().ComputeBSphereFromBoxMinMax();

	mimi->GetTransform()->AddToScene(GScene);
	mimi->GetTransform()->Update();
	mimi->SetSelectable(false);

	if (mTrandBandTransform)
	{
		mTrandBandTransform->RemoveFromScene();
	}

	mTrandBandTransform = mimi->GetTransform();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::SaveTrack(ZFile& file)
{
	unsigned char format = 1;
	file << format;

	uint i;
	uint32 mNbTrackPieces = (uint32)mBricks.size();
	file << mNbTrackPieces;

	file.Write( &mTrackBonusEdit[0], sizeof(TrackBonusEdit_t) * mNbTrackPieces );

	for (i=0;i<mNbTrackPieces;i++)
	{
		mBricks[i].mBrickName.SaveFile(file);
	}

	// anim
	mAnimation->Save(file);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::LoadTrack(ZFile& file)
{
	uint32 mNbTrackPieces;
	file >> mNbTrackPieces;

	mTrackBonusEdit.resize(mNbTrackPieces);
	file.Read( &mTrackBonusEdit[0], sizeof(TrackBonusEdit_t) * mNbTrackPieces );

	mBricks.resize(mNbTrackPieces);
	for (unsigned int i=0;i<mNbTrackPieces;i++)
	{
		mBricks[i].mBrickName.LoadFile(file);
		mBricks[i].mBrickMesh = NULL;
	}

	// anim
	mAnimation = (ZAnimation*)_New(ZAnimation);
	mAnimation->Load(file);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZRushTrack::ShowEditGizmos(bool bShow)
{
	mbGizmosVisible = bShow;
	if (mTrandBandTransform)
		mTrandBandTransform->SetVisible(bShow);

	std::vector<ZTransform*>::iterator iter = mKeyTransforms.begin();
	for (; iter != mKeyTransforms.end() ; ++iter)
	{
		(*iter)->SetVisible(bShow);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
