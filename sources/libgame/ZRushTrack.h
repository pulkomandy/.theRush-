#ifndef ZRUSHTRACK_H__
#define ZRUSHTRACK_H__

#include "libworld/ZAnimation.h"
#include "libworld/ZMesh.h"
#include "libworld/ZPhysicTriMeshInstance.h"
#include "ZTrackBonus.h"

class ZPhysicWorldBullet;
class ZMeshInstance;

DECLAREZCLASS(ZRushTrack);

static const int SPLITCOUNT = 8;

class ZRushTrack : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE
public:

	typedef struct AIPoint_t
	{
		tvector3 mAIPos, mAIRight, mAIUp, mRailLeft, mRailRight;
	} AIPoint_t;

	static bool UseEditingGizmo;


	ZRushTrack();
	virtual ~ZRushTrack();

	void Load(ZFile &mFile);
	void Compute();
	void CreateDefaultTrack();
	void BuildBonus();
	void AddPhysicMeshesToPhysicWorld(ZPhysicWorldBullet *aPhysWorld);

	virtual tulong GetMemoryUsed() const 
	{ 
		tulong mSize = sizeof(ZRushTrack);
		return mSize;
	}

	// bricks
	void PushBrick(const char *szBrickName)
	{
		mBricks.push_back(SourceBricks_t(szBrickName, NULL));
	}

	const std::vector<tmatrix>& GetSpawnMatrices() const { return mSpawnMatrices; }

	bool GetClosestSampledPoint(const tvector3& mobilePoint,
											tmatrix& trackMatrix,
											tvector3& trackMiddle,
											int &aaCurIdx,
											float &rightLeftFactor,
											const tvector3& lastKnownGravity,
											float &upDownFactor,
											float& distanceToBorder);

	unsigned int GetNbSamples() { return (mBricks.size()*SPLITCOUNT); }
	const tmatrix GetBrickMatrix(unsigned int aIndex) const;

	void Tick(float aTimeEllapsed);
	int GetNumberOfStartBricks() const { return (mSpawnMatrices.size()>>1); }
	int GetNumberOfStartBricksSamples() const { return ((mSpawnMatrices.size()>>1) * SPLITCOUNT); }
	const std::vector<AIPoint_t>& GetAIPoints() const { return mAIPoints; }

	ZAnimation * GetAnimation() const { return mAnimation.ptr(); }
	void SetAnimation(ZAnimation *pAnim) { mAnimation.Attach(pAnim); }


	ZMeshInstance* GetKeyMesh(int idx) const { return (ZMeshInstance*)mKeyTransforms[idx]->GetSpatialEntity()->QueryInstance(); }
	void ShowEditGizmos(bool bShow);
	bool IsDebugEditGizmoVisible() const { return mbGizmosVisible; }

	bool PickAnimationKey(const tvector3 & rayOrigin, const tvector3 & rayDir, ZAnimation ** pAnim, int *pIndex);

	int GetRoadMeshInstanceIndex(const ZMeshInstance *pInst ) const;

	// bricks management

	void ReplaceBrick(int idx, const char * szBrickName);
	void DeleteBrick(int idx);
	void InsertBrickBefore(int idx, const char * szBrickName);
	void InsertBrickAfter(int idx, const char * szBrickName);

	// Bonus Edit
	void SetBonus(int idx, char aLeft, char aRight, float decal);
	void GetBonus(int idx, char &aLeft, char &aRight, float &decal) const;

	// only call it from editor. no need in game
	void RemoveTrackPiecesFromScene(); 

	// Load/save
	void SaveTrack(ZFile& file);
	void LoadTrack(ZFile& file);
protected:
	bool mbGizmosVisible;
	std::vector<ZMeshInstance*> mDeformedMeshInstances;
	std::vector<ZTransform*> mDeformedMeshesTransforms;
	std::vector<tmatrix> mSpawnMatrices;
	float mStartupHalfLength;
	void BuildSpawnMatrices();

	smartptr<ZAnimation> mAnimation;

	typedef struct SourceBricks_t
	{
		SourceBricks_t()
		{
		}
		SourceBricks_t(const char *brickName, ZMeshInstance *pInst)
		{
			mBrickName = brickName;
			mBrickMesh = pInst;
		}
		tstring mBrickName;
		ZMeshInstance *mBrickMesh;
	} SourceBricks_t;

	std::vector<SourceBricks_t> mBricks;

	std::vector<tvector3> samplesPos;
	std::vector<tvector4> samplesRot;

	std::vector<tmatrix> MATS;

	void DeformMesh(ZMesh *meshSrc, const tmatrix & srcMeshMat, ZMesh *meshDst, int i, 	
							const std::vector<float>& meshLengths, const std::vector<float>& meshMin);
	void DeformMeshForPhysic(ZMesh *meshSrc, const tmatrix & srcMeshMat, tvector3 *mvtDst, int i, 	
							const std::vector<float>& meshLengths, const std::vector<float>& meshMin);


	inline void InterpolateMatrix2(tmatrix& res, const tmatrix& mat1, const tmatrix& mat2, float ratio)
	{
		float a = 1-ratio;
		float b = ratio;
		tmatrix mattmp;

		tvector3 pp1 = mat1.V4.position;
		tvector3 pp2 = mat2.V4.position;
		tvector3 dir = mat2.V4.dir;

		tvector3 NA = -mat2.V4.dir;
		tvector3 NB = mat1.V4.dir;
		tvector3 A = pp1;
		tvector3 B = pp2;
		mattmp.Translation(pp2 - (dir * (pp2-pp1).Length()*0.5f));
		// Calcul de 2 points de contrôle à partir de 2 points et de 2 normales et de PushStrength
		// P1 = 1/3(2A + B + (NA.(A-B)).NA )
		// P2 = 1/3(2B + A + (NB.(B-A)).NB )
		/*
		tvector3 p1 = pp1*2 + pp2 + dir1.Dot(pp1-pp2)*dir1;
		tvector3 p2 = pp2*2 + pp1 + dir2.Dot(pp2-pp1)*dir2;
		tvector3 pm = (p1+p2)*0.5f;
		*/
		float PushStrength=1.5f;
		tvector3 P1,P2;
		float ps;
		ps = PushStrength * ( NA.x*(A.x-B.x) + NA.y*(A.y-B.y) + NA.z*(A.z-B.z) );
		P1.x = 0.66666F * A.x  +  0.33333F * ( B.x + ps*NA.x ) ;
		P1.y = 0.66666F * A.y  +  0.33333F * ( B.y + ps*NA.y ) ;
		P1.z = 0.66666F * A.z  +  0.33333F * ( B.z + ps*NA.z ) ;
		ps = PushStrength * ( NB.x*(B.x-A.x) + NB.y*(B.y-A.y) + NB.z*(B.z-A.z) );
		P1.x += 0.66666F * B.x  +  0.33333F * ( A.x + ps*NB.x ) ;
		P1.y += 0.66666F * B.y  +  0.33333F * ( A.y + ps*NB.y ) ;
		P1.z += 0.66666F * B.z  +  0.33333F * ( A.z + ps*NB.z ) ;

		P1.x *= 0.5f;
		P1.y *= 0.5f;
		P1.z *= 0.5f;

		for (int i=12;i<16;i++)
		{
			//res[i] = mat1.m16[i]*a*a + mattmp.m16[i]*2*a*b + mat2.m16[i]*b*b;
			res[i] = mat1.m16[i]*a*a + P1[i-12]*2*a*b + mat2.m16[i]*b*b;
		}

		//X(a) = Ax·a² + Bx·2·a·b + Cx·b²
		/*
		Y(a) = Ay·a² + By·2·a·b + Cy·b²
		Z(a) = Az·a² + Bz·2·a·b + Cz·b²
		*/
		//res.Lerp(mat1, mat2, ratio);
	}



	inline void InterpolateMatrix(tmatrix& res, const tmatrix& mat1, const tmatrix& mat2, float ratio)
	{
		res.Lerp(mat1, mat2, ratio);
	}

// for collision
	
	std::vector<AIPoint_t> mAIPoints;
	void ComputeAILine();

	tvector3 mProjX, mProjY;

	inline tvector3 projectWithGrav(const tvector3& vt)
	{
		return tvector3(vt.Dot(mProjX), 0, vt.Dot(mProjY));
	}

	// Physic meshes merging
	
	typedef struct PhysicMeshToMerge_t
	{
		PhysicMeshToMerge_t(unsigned short *_indices, tvector3* _vertices, int _vertexSize, int _VASize, int _IASize)
		{
			mBigPhysIndices = _indices;
			mBigPhysVertices = _vertices;
			mBigPhysVAVTSize = _vertexSize;
			mBigPhysVASize = _VASize;
			mBigPhysIASize = _IASize;
		}

		unsigned short * mBigPhysIndices;
		tvector3* mBigPhysVertices;
		int  mBigPhysVAVTSize;
		int  mBigPhysVASize;
		int  mBigPhysIASize;
	} PhysicMeshToMerge_t;
	ZPhysicTriMesh * MergePhysicMeshes(std::vector<PhysicMeshToMerge_t>& mPhysicMeshesToMerge, int mBigPhysNbIndices, int mBigPhysNbVertices);

	void RemoveCollisionMesh();
	std::vector<smartptr<ZPhysicTriMesh> > mPhysicMeshes;
	std::vector<smartptr<ZPhysicTriMeshInstance> > mPhysicMeshesInstances;

	std::vector<smartptr<ZTrackBonus> > mTrackBonuses;

	// keys/edition gizmos
	void BuidEditingKeyBoxes();
	//ZMeshInstance* mKeyMeshes[256];
	//int mKeyMeshesIndex;

	std::vector<ZTransform*> mKeyTransforms;
	smartptr<ZMesh> mKeyCubeMesh;
	//ZMeshInstance *mTrackBand;
	ZTransform* mTrandBandTransform;
	void ComputeTrackBand(AIPoint_t* mPoints, unsigned int nbPoints);
	smartptr<ZTexture> mTrackBandTexture;
	smartptr<ZTexture> mKeyTexture;
	// Bonus
	typedef struct TrackBonusEdit_t
	{
		TrackBonusEdit_t()
		{
			mRightBonus = 0;
			mLeftBonus = 0;
			mBonusDecal = 7.f;
		}

		TrackBonusEdit_t(char left, char right, float decal)
		{
			mRightBonus = right;
			mLeftBonus = left;
			mBonusDecal = decal;
		}
		char mRightBonus;
		char mLeftBonus;
		float mBonusDecal;
	} TrackBonusEdit_t;
	std::vector<TrackBonusEdit_t> mTrackBonusEdit;
	
};

extern ZRushTrack* GTrack;
#endif
