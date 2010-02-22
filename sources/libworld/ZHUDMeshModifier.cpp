#include "stdafx.h"

BEGIN_SERIALIZATION(ZHUDMeshModifier)
END_SERIALIZATION()


ZHUDMeshModifier::ZHUDMeshModifier()
{
	mHUDMesh = (ZMeshInstance*)_New( ZMeshInstance );
	ZMesh *nMesh = (ZMesh*)_New( ZMesh );
	nMesh->AddSubMesh( 0, 0 );
	nMesh->SetPrimitive(PRIM_TRIANGLELIST);

	IVertexArray *pva = GDD->NewVertexArray();
	pva->Init(VAF_XYZ|VAF_COLOR|VAF_TEX0, 2048, true);
	nMesh->SetVertexBuffer(pva);
	mHUDMesh->SetMesh(nMesh);

	// attach it to camera

	ZTransformInertia * ptia = (ZTransformInertia *)_New( ZTransformInertia );
	tmatrix mt;
	mt.Translation( 0.f, 0.f, 5.f );
	ptia->SetLocalMatrix( mt );
	
	ptia->SetTimeFactor( 20.f );
	ptia->SetMaximumDistance( 100.0f );
	
	GetActiveCamera()->GetTransform()->AddChild( ptia );
	ptia->AddChild(mHUDMesh->GetTransform());
	mHUDMesh->SetVisible(false);
}

ZHUDMeshModifier::~ZHUDMeshModifier()
{
}

void ZHUDMeshModifier::Tick(float aTimeEllapsed)
{
	PROFILER_START(HUDMeshModifierTick);

	PROFILER_END();
}

void ZHUDMeshModifier::StartQuads()
{
	mNbVt = 0;
	mCurVts = mVts = (guiMeshVT*)mHUDMesh->GetMesh()->GetVertexBuffer()->Lock(VAL_WRITE);
}

void ZHUDMeshModifier::PushQuad(int Qnb, const tvector2& pos, const tvector2& size, const tmatrix& mt, const tcolor& col, float fact)
{
	tulong tulc = col.ConvToRGBA();

	mCurVts[0].pos = tvector3(pos.x, pos.y, 0);
	mCurVts[0].color = tulc;
	mCurVts[0].uv = tvector2(UVSources[Qnb].x, UVSources[Qnb].y)*(1.0f/1024.0f);
	mCurVts[1].pos = tvector3(pos.x+size.x*fact, pos.y, 0);
	mCurVts[1].color = tulc;
	mCurVts[1].uv = tvector2(LERP(UVSources[Qnb].x, UVSources[Qnb].z, fact), UVSources[Qnb].y)*(1.0f/1024.0f);
	mCurVts[2].pos = tvector3(pos.x, pos.y+size.y, 0);
	mCurVts[2].color = tulc;
	mCurVts[2].uv = tvector2(UVSources[Qnb].x, UVSources[Qnb].w)*(1.0f/1024.0f);


	mCurVts[3].pos = tvector3(pos.x+size.x*fact, pos.y, 0);
	mCurVts[3].color = tulc;
	mCurVts[3].uv = tvector2(LERP(UVSources[Qnb].x, UVSources[Qnb].z, fact), UVSources[Qnb].y)*(1.0f/1024.0f);
	mCurVts[4].pos = tvector3(pos.x+size.x*fact, pos.y+size.y, 0);
	mCurVts[4].color = tulc;
	mCurVts[4].uv = tvector2(LERP(UVSources[Qnb].x, UVSources[Qnb].z, fact), UVSources[Qnb].w)*(1.0f/1024.0f);
	mCurVts[5].pos = tvector3(pos.x, pos.y+size.y, 0);
	mCurVts[5].color = tulc;
	mCurVts[5].uv = tvector2(UVSources[Qnb].x, UVSources[Qnb].w)*(1.0f/1024.0f);
	/*
	mCurVts[5].pos = tvector3(pos.x, pos.y, 0);
	mCurVts[5].color = tulc;
	mCurVts[5].uv = tvector2(UVSources[Qnb].x, UVSources[Qnb].y)*(1.0f/1024.0f);
	mCurVts[4].pos = tvector3(pos.x+size.x*fact, pos.y, 0);
	mCurVts[4].color = tulc;
	mCurVts[4].uv = tvector2(LERP(UVSources[Qnb].x, UVSources[Qnb].z, fact), UVSources[Qnb].y)*(1.0f/1024.0f);
	mCurVts[3].pos = tvector3(pos.x, pos.y+size.y, 0);
	mCurVts[3].color = tulc;
	mCurVts[3].uv = tvector2(UVSources[Qnb].x, UVSources[Qnb].w)*(1.0f/1024.0f);


	mCurVts[2].pos = tvector3(pos.x+size.x*fact, pos.y, 0);
	mCurVts[2].color = tulc;
	mCurVts[2].uv = tvector2(LERP(UVSources[Qnb].x, UVSources[Qnb].z, fact), UVSources[Qnb].y)*(1.0f/1024.0f);
	mCurVts[1].pos = tvector3(pos.x+size.x*fact, pos.y+size.y, 0);
	mCurVts[1].color = tulc;
	mCurVts[1].uv = tvector2(LERP(UVSources[Qnb].x, UVSources[Qnb].z, fact), UVSources[Qnb].w)*(1.0f/1024.0f);
	mCurVts[0].pos = tvector3(pos.x, pos.y+size.y, 0);
	mCurVts[0].color = tulc;
	mCurVts[0].uv = tvector2(UVSources[Qnb].x, UVSources[Qnb].w)*(1.0f/1024.0f);
	*/

	for (int i=0;i<6;i++)
	{
		mCurVts[i].pos.TransformPoint(mt);
	}
	mCurVts += 6;
	mNbVt += 6;
}
void ZHUDMeshModifier::EndQuads()
{
	mHUDMesh->GetMesh()->GetVertexBuffer()->Unlock();
	mHUDMesh->GetMesh()->SetSubMesh(0, 0 , mNbVt );
}