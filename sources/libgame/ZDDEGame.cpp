#include "ZDDEGame.h"
#include "libplatform/ZenFWManager.h"
#include "libplatform/ZenFWVRAMService.h"
#include "libplatform/ZenFWLoaderNode.h"
#include "libplatform/ZenFWGame.h"
#include "libworld/ZMeshBuilder.h"
#include "libworld/ZRigidBodyBullet.h"
#include "libplatform/IInput.h"
BEGIN_SERIALIZATION(ZDDEGame)
END_SERIALIZATION()

class ZDDECharacter
{
public:
	ZDDECharacter(const tmatrix& mt, ZPhysicWorldBullet *pPhysWorld)
	{
		mPhysicWorld = pPhysWorld;
		mCurrentGravity = -mt.V4.up;
		static ZMesh *mKeyCubeMesh = NULL;
		if (!mKeyCubeMesh)
			mKeyCubeMesh = CreateBox(tvector3::one, tcolor::white);

		ZMeshInstance *mimi2;
		mimi2 = (ZMeshInstance*)_New(ZMeshInstance);
		mTransform = mimi2->GetTransform();

		mimi2->SetMesh(mKeyCubeMesh);
		mimi2->SetVisible(true);

		mimi2->GetMaterial(0)->addTexture(DirectLoadTex("./ZenithDatas/fushia.dds"));//>setEffect(mKeyMeshMat.ptr());

		/*
		tmatrix bigbig, bigtr;//, bigrot;
		bigbig.Scaling(tvector3( 0.6f, 1.f, 0.6f ));
		tmatrix res= bigbig*mt;
		*/
		mimi2->GetTransform()->SetLocalMatrix(mt);
		mimi2->GetTransform()->AddToScene(GScene);
		mimi2->GetTransform()->Update();

		goLeft = goRight = goForward = goBackward = false;
		mCharSpeed = 0;

		mCharBody= pPhysWorld->CreateRigidBodySphere(0.5f, 0.1f);
		mCharBody->SetMatrix(mt);
		mCharBody->SetGravity(tvector3(0.f, 0.f, 0.f));
		directionToGo = tvector3::zero;
	}
	void Tick(float aTimeEllapsed)
	{
		tmatrix currentMatrix = mTransform->GetWorldMatrix();

		tmatrix bodyMat;
		mCharBody->GetMatrix(&bodyMat);
		tvector3 newPos = bodyMat.V4.position;//currentMatrix.V4.position;
		//newPos += mCurrentGravity*aTimeEllapsed;
		
		// move character

		tvector3 forceToApply;
		const tmatrix& camwm = GetActiveCamera()->GetTransform()->GetWorldMatrix();
		tvector3 directionToGoDesired(tvector3::zero);

		if (goLeft)
			directionToGoDesired -= camwm.V4.right;
		if (goRight)
			directionToGoDesired += camwm.V4.right;
		if (goForward)
			directionToGoDesired += camwm.V4.dir;
		if (goBackward)
			directionToGoDesired -= camwm.V4.dir;

		if (goLeft || goRight || goForward || goBackward)
		{

			mCharSpeed = LERP(mCharSpeed, 4.f, aTimeEllapsed*4.f);
			float directionToGoDesiredLength = directionToGoDesired.Length();
			if (directionToGoDesiredLength>0)
				directionToGoDesired.Normalize();
			else
				directionToGoDesired = tvector3::zero;
		}
		else
			mCharSpeed = LERP(mCharSpeed, 0.f, aTimeEllapsed*3.f);

		tvector3 tempdtg;
		tempdtg.Lerp(directionToGo, directionToGoDesired, 1.f);
		directionToGo = tempdtg;

		tvector3 planarDirectionToGo;
		planarDirectionToGo = tvector3(currentMatrix.V4.right) * directionToGo.Dot(tvector3(currentMatrix.V4.right)) + 
			tvector3(currentMatrix.V4.dir) * directionToGo.Dot(tvector3(currentMatrix.V4.dir));
		//newPos += planarDirectionToGo*aTimeEllapsed*4.f * mCharSpeed;
		forceToApply = planarDirectionToGo*4.f;// * mCharSpeed;
		// update his matrix
		tvector3 rO(tvector3(currentMatrix.V4.position) - mCurrentGravity);
		tvector3 rD((mCurrentGravity) *10);
		tvector3 hitPosition, hitNormal;
		float hitFactor;
		if (mPhysicWorld->SyncRayCast(rO, rD, &hitPosition, &hitNormal, &hitFactor, 0xFFFFFFFF))
		{
			tvector3 tempgrav;
			tempgrav.Lerp(mCurrentGravity, -hitNormal, aTimeEllapsed * 8.f);
			tempgrav.Normalize();
			// rebuild matrix
			tvector3 newDir;
			newDir.Cross(currentMatrix.V4.right, -tempgrav);
			newDir.Normalize();
			tvector3 newRight;
			newRight.Cross(-tempgrav, newDir);
			newRight.Normalize();
/*
			if (hitFactor<0.04f)
			{
				newPos -= (mCurrentGravity * (0.04f - hitFactor));
			}
			else
			{
				newPos += (mCurrentGravity * aTimeEllapsed);
			}
			*/

			tmatrix newMat(newRight, -tempgrav, newDir, newPos);
			mCurrentGravity = tempgrav;
			mCharBody->SetMatrix(newMat);
			
			
			mTransform->SetLocalMatrix(newMat);
			mTransform->Update();
		}
		mCharBody->ApplyForce((forceToApply+mCurrentGravity) * aTimeEllapsed *3.f);
	}
	bool goLeft, goRight,goForward, goBackward;
	smartptr<ZTransform> mTransform;
	tvector3 mCurrentGravity;
	ZPhysicWorldBullet *mPhysicWorld;
	float mCharSpeed;
	ZRigidBody *mCharBody;

	tvector3 directionToGo;
};

std::list<ZDDECharacter*> mChars;

void UpdateCamera(ZDDECharacter* pChar, float aTimeEllapsed)
{
	const tmatrix& camwm = GetActiveCamera()->GetTransform()->GetWorldMatrix();
	const tmatrix& charwm = pChar->mTransform->GetWorldMatrix();
	const tvector3& charPos = charwm.V4.position;
	float DistanceToChar = Distance(camwm.V4.position, charPos);
	static const float maxDistance = 4.f;
	static const float minDistance = 1.f;

	static const float maxHeight = 5.f;
	static const float minHeight = 1.f;

	tvector3 desiredPos = camwm.V4.position;

	// distance to character
	const tvector3& campos = camwm.V4.position;
	if (DistanceToChar<minDistance)
		desiredPos = tvector3(campos) - (camwm.V4.dir * (minDistance-DistanceToChar) );
	else if (DistanceToChar>maxDistance)
		desiredPos = tvector3(campos) + (camwm.V4.dir * (DistanceToChar-maxDistance) );
	
	// height
	float above = (desiredPos - charPos).Dot(charwm.V4.up);
	if (above<minHeight)
		desiredPos += charwm.V4.up * ( minHeight - above );
	else if (above>maxHeight)
		desiredPos -= charwm.V4.up * ( above - maxHeight );
	// new camera matrix

	tvector3 newCamPos;
	newCamPos.Lerp(camwm.V4.position, desiredPos, aTimeEllapsed *8.f);

	tvector3 newCamUp;
	newCamUp.Lerp(camwm.V4.up, pChar->mTransform->GetWorldMatrix().V4.up, 1);//aTimeEllapsed *4.f); 
	ZCamera *pCam = GetActiveCamera();
	tmatrix tmpmat;
	tvector3 camTarget = pChar->mTransform->GetWorldMatrix().V4.position;
	tmpmat.LookAt(newCamPos, charPos, newCamUp);
	pCam->GetTransform()->SetLocalMatrix(tmpmat);
	pCam->Update();
}

ZDDEGame::ZDDEGame()
{
	mbAllResourceToScene = true;

	mPhysicWorld.Attach( (ZPhysicWorldBullet*)_New(ZPhysicWorldBullet) );
	mPhysicWorld->Init();

	mWorldRes.Attach((ZGameResources*)_New(ZGameResources));
	mGLoaderService->CreateInstance("./DDE/level00.DAE", mWorldRes);

	ZCamera *pCam = GetActiveCamera();
	tmatrix tmpmat;
	tmpmat.LookAt(tvector3(10, 120, -20), tvector3(0, 110, -30)/*::zero*/, tvector3::YAxis);
	pCam->GetTransform()->SetLocalMatrix(tmpmat);
	pCam->Update();



}

ZDDEGame::~ZDDEGame()
{
}

void ZDDEGame::Update(float aTimeEllapsed)
{
	if (LoadingPipeIsEmpty() && mbAllResourceToScene)
	{
		mbAllResourceToScene = false;
		mWorldRes->AddAllToScene();
		mWorldRes->AddAllPhysicToScene(mPhysicWorld);

		GScene->AddCluterizeJob();

		// spawn character
		tmatrix mt;
		mt.Translation(tvector3(0, 115, -30));
		ZDDECharacter *pChar = new ZDDECharacter(mt, mPhysicWorld);
		mChars.push_back(pChar);
	}

	if (GetInputDevice()->KeyReleased(TKEY_ESC))
	{
		ZManager->AskToQuit();
	}

	mPhysicWorld->Tick(aTimeEllapsed);

	std::list<ZDDECharacter*>::iterator iter = mChars.begin();
	for (; iter != mChars.end(); ++iter)
	{
		(*iter)->Tick(aTimeEllapsed);
	}

	if (!mChars.empty())
	{
		ZDDECharacter *pChar = (*mChars.begin());

		UpdateCamera(pChar, aTimeEllapsed);

		pChar->goLeft = pChar->goRight = pChar->goForward = pChar->goBackward = false;
		if (GetInputDevice()->KeyDown(TKEY_a))
		{
			pChar->goLeft = true;
		}
		if (GetInputDevice()->KeyDown(TKEY_d))
		{
			pChar->goRight = true;
		}

		if (GetInputDevice()->KeyDown(TKEY_w))
		{
			pChar->goForward = true;
		}
		if (GetInputDevice()->KeyDown(TKEY_s))
		{
			pChar->goBackward = true;
		}
	}

}
