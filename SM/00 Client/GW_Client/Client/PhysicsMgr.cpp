#include "stdafx.h"
#include "PhysicsMgr.h"
#include "Utilities.h"
#include "GameObject.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "Define.h"
#include "Stream.h"
#include "NxCooking.h"
#include "Cooking.h"
#include "NxControllerManager.h"
#include "UserAllocator.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"


CPhysicsMgr*	CPhysicsMgr::m_pInstance;


CPhysicsMgr::CPhysicsMgr(void)
	: m_pPhysicsSDK(nullptr)
	, m_pScene(nullptr)
	, m_pCBmtxWorld(nullptr)
{

}

CPhysicsMgr* CPhysicsMgr::GetInstance(void)
{
	if (nullptr == m_pInstance)
		m_pInstance = new CPhysicsMgr;

	return m_pInstance;
}

void CPhysicsMgr::DestroyInstance(void)
{
	if (nullptr != m_pInstance)
	{
		m_pInstance->Release();
		m_pInstance = nullptr;
	}
}

void CPhysicsMgr::Connect_Actors(vector<CGameObject*>* pvecGameObject)
{
	if (nullptr == pvecGameObject) return;
	// if( pvecGameObject->size() != m_dwActorCnt ) return;

	NxActor** dpActor = m_pScene->getActors();

	for (DWORD i = 0; i < 1 /*pvecGameObject->size()*/; ++i, ++dpActor)
	{
		NxActor* pActor = *dpActor;
		pActor->userData = (*pvecGameObject)[i];
		(*pvecGameObject)[i]->SetActor(pActor);
	}
}

HRESULT CPhysicsMgr::CreateContantBuffer(ID3D11Device * pDevice)
{
	/* VS 용 World */
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(Buffer_Desc));
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof(XMFLOAT4X4);
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer(&Buffer_Desc, NULL, &m_pCBmtxWorld);
	return S_OK;
}

void CPhysicsMgr::SetContantBuffer(ID3D11DeviceContext* pContext, NxF32* pMtxWorld)
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map(m_pCBmtxWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource);

	XMFLOAT4X4* pmtxWorld = (XMFLOAT4X4*)MappedSubresource.pData;
	memcpy(pmtxWorld, pMtxWorld, sizeof(XMFLOAT4X4));
	pContext->Unmap(m_pCBmtxWorld, 0);
	pContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pCBmtxWorld);
}

HRESULT CPhysicsMgr::Initialize()
{
	m_pAllocator = new UserAllocator;

	bool status = InitCooking(m_pAllocator);
	if (!status) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.");
		return E_FAIL;
	}

	NxPhysicsSDKDesc sdkDesc;
	sdkDesc.flags ^= NX_SDKF_NO_HARDWARE;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, m_pAllocator, NULL, sdkDesc, &errorCode);
	if (m_pPhysicsSDK == NULL)
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return E_FAIL;
	}

	m_pCCTManager = NxCreateControllerManager(m_pAllocator);

	return S_OK;
}

HRESULT CPhysicsMgr::LoadSceneFromFile(const char *pFilename, NXU::NXU_FileType type)
{
	HRESULT success = E_FAIL;

	if (m_pPhysicsSDK)
	{

		NXU::NxuPhysicsCollection *c = NXU::loadCollection(pFilename, type);

		if (c)
		{
			if (m_pScene)
			{
				m_pPhysicsSDK->releaseScene(*m_pScene);
				m_pScene = nullptr;
			}

			success = NXU::instantiateCollection(c, *m_pPhysicsSDK, 0, 0, 0);

			m_pScene = m_pPhysicsSDK->getScene(0);

			NXU::releaseCollection(c);
		}
		else
		{
			printf("NxuPhysicsCollection is NULL! \n");
		}
	}

	if (success)
		printf("Scene loaded from file %s.\n", pFilename);

	return success;
}

NxController* CPhysicsMgr::CreateCharacterController(NxActor* actor, const NxVec3& startPos, NxReal scale)
{
	float	fSKINWIDTH = 0.1f;
	NxController *pCtrl;

	if (true) {

		// 박스 컨트롤러
		NxVec3	InitialExtents(0.5f, 1.0f, 0.5f);
		NxBoxControllerDesc desc;
		desc.extents = InitialExtents * scale;
		desc.position.x = startPos.x;
		desc.position.y = startPos.y;
		desc.position.z = startPos.z;
		desc.upDirection = NX_Y;
		desc.slopeLimit = 0;
		desc.slopeLimit = cosf(NxMath::degToRad(45.0f));
		desc.skinWidth = fSKINWIDTH;
		desc.stepOffset = 0.5;
		desc.userData = (NxActor*)actor;
		//	desc.stepOffset	= 0.01f;
		//		desc.stepOffset		= 0;
		//		desc.stepOffset		= 10;
		//desc.callback = &gControllerHitReport;
		pCtrl = m_pCCTManager->createController(m_pScene, desc);
	}
	else {

		// 캡슐 컨트롤러
		NxF32	InitialRadius = 0.5f;
		NxF32	InitialHeight = 2.0f;
		NxCapsuleControllerDesc desc;
		desc.position.x = startPos.x;
		desc.position.y = startPos.y;
		desc.position.z = startPos.z;
		desc.radius = InitialRadius * scale;
		desc.height = InitialHeight * scale;
		desc.upDirection = NX_Y;
		//		desc.slopeLimit		= cosf(NxMath::degToRad(45.0f));
		desc.slopeLimit = 0;
		desc.skinWidth = fSKINWIDTH;
		desc.stepOffset = 0.5;
		desc.stepOffset = InitialRadius * 0.5 * scale;
		desc.userData = (NxActor*)actor;
		//	desc.stepOffset	= 0.01f;
		//	desc.stepOffset		= 0;	// Fixes some issues
		//	desc.stepOffset		= 10;
		//desc.callback = &gControllerHitReport;
		pCtrl = m_pCCTManager->createController(m_pScene, desc);
	}
	pCtrl->setCollision(false);
	NxBounds3 bound;
	pCtrl->getActor()->getShapes()[0]->getWorldBounds(bound);
	NxVec3 extents;
	bound.getExtents(extents);
	pCtrl->getActor()->getShapes()[0]->setLocalPosition(NxVec3(0, extents.y, 0));

	return pCtrl;

}

HRESULT CPhysicsMgr::SetScene()
{
	if (!m_pScene) {
		return E_FAIL;
	}

	 NxU32 nbActors = m_pScene->getNbActors();
	 NxActor** aList = m_pScene->getActors();

	if (m_pScene)
	{
		for (NxU32 i = 0; i < nbActors; i++)
		{
			NxActor *a = aList[i];
			NxVec3 pos = a->getGlobalPosition();

			printf(" - 액터 '%s'의 global: (%f, %f, %f) \n", a->getName(), pos.x, pos.y, pos.z);

			// 캐릭터 설정 (지금은 myPlayer가 나의 캐릭터라고 가정)
			if (strcmp(a->getName(), "myPlayer")==0) {

				// 임시설정값 (실제는 파일에서 미리 설정해주어야 함)
				a->raiseBodyFlag(NX_BF_KINEMATIC);
				a->setGlobalPosition(NxVec3(0, 0, 0));


				// 컨트롤러 생성
				m_pMyCCT = CreateCharacterController(a, a->getGlobalPosition(),5.0f);
			}


			NxU32 nbShapes = a->getNbShapes();
			if (nbShapes)
			{
				NxShape ** slist = (NxShape **)a->getShapes();
				for (NxU32 j = 0; j < nbShapes; j++)
				{
					NxShape *s = slist[j];
					NxVec3 spos = s->getLocalPosition();
					printf("   [%d] %s의 local: (%f, %f, %f) \n", j, s->getName(), spos.x, spos.y, spos.z);
				}
			}
		}
	}

	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < 20; ++j) {
			CreateSphere(NxVec3(-30 + 3 * j, 0, -30 + 3*i), 0.5, 10.0f);
		}
	}


	return S_OK;
}



HRESULT CPhysicsMgr::CreateScene(ID3D11Device* pDevice)
{
	if (!LoadSceneFromFile("../Executable/Resources/Scene/human_fix.xml", NXU::FT_XML)) {
		printf("LoadScene() is failed! \n");
		return E_FAIL;
	};

	SetScene();

#ifdef _DEBUG
	CreateContantBuffer(pDevice);
#endif
	return S_OK;
}

void CPhysicsMgr::Update(const float & fTimeDelta)
{

	NxVec3 dir(0, 0, 0);
	NxF32 fSpeed = 5.0f;
	NxU32 collisionFlags;

	if (GetAsyncKeyState(VK_UP) & 0x8000) 
	{
		dir += NxVec3(0, 0, 1)*fTimeDelta*fSpeed;
	}
	if ((GetAsyncKeyState(VK_DOWN) & 0x8000))
	{
		dir += NxVec3(0, 0, -1)*fTimeDelta*fSpeed;
	}
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000))
	{
		dir += NxVec3(-1, 0, 0)*fTimeDelta*fSpeed;
	}
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000))
	{
		dir += NxVec3(+1, 0, 0)*fTimeDelta*fSpeed;
	}
	if (!dir.isZero())
	{
		// 이동
		m_pMyCCT->move(dir, COLLIDABLE_MASK, 0.000001f, collisionFlags);
		NxVec3 pos = m_pMyCCT->getActor()->getGlobalPosition();
		((NxActor*)m_pMyCCT->getUserData())->moveGlobalPosition(pos);

		// 회전
		dir.normalize();
		//NxVec3 oldLook = m_pMyCCT->getActor()->getGlobalPose().M.getRow(3);
		NxVec3 oldLook(0, 0, -1.0f);
		NxVec3 cross = oldLook.cross(dir);
		NxReal rotAngle = acos(oldLook.dot(dir));
		rotAngle *= (cross.y > 0.0f) ? 1.0f : -1.0f;
		NxMat33 rot;
		rot.rotY(rotAngle);
		((NxActor*)m_pMyCCT->getUserData())->moveGlobalOrientation(rot);
	}

	NxActor** dpActor = m_pScene->getActors();
	NxU32 nbActors = m_pScene->getNbActors();

	for( DWORD i = 0; i < nbActors; ++i, ++dpActor )
	{
	CGameObject* pGameObject = ( CGameObject* )(*dpActor)->userData;
	if (pGameObject) {
		pGameObject->Update(fTimeDelta);
	}
	}

	m_pScene->simulate(fTimeDelta);
	m_pScene->flushStream();
	m_pScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void CPhysicsMgr::Render(ID3D11DeviceContext* pContext)
{
	NxActor** dpActor = m_pScene->getActors();

	CMesh* pBox = CResourceMgr::GetInstance()->CloneMesh("Mesh_Box");
	CMesh* pSphere = CResourceMgr::GetInstance()->CloneMesh("Mesh_Sphere");
	CMesh* pCapsule = CResourceMgr::GetInstance()->CloneMesh("Mesh_Capsule");
	NxF32* mtxWorld = new NxF32[16];
	NxVec3	vScale{};
	NxF32	fRadius{}, fHeight{};
	NxMat34 mtxRealWorld{}, mtxScale{};
	mtxRealWorld.zero();
	mtxScale.zero();

	DWORD nbActors = m_pScene->getNbActors();
	for (DWORD i = 0; i < nbActors; ++i, ++dpActor)
	{
		DWORD dwShapeCnt = (*dpActor)->getNbShapes();
		NxShape* const * dpShape = (*dpActor)->getShapes();

		for (DWORD j = 0; j < dwShapeCnt; ++j, ++dpShape)
		{
			switch ((*dpShape)->getType())
			{
			case NX_SHAPE_BOX:
			{
				vScale = ((NxBoxShape*)(*dpShape))->getDimensions() * 2.f;
				mtxRealWorld = (*dpShape)->getGlobalPose();
				mtxScale.M.diagonal(vScale);
				mtxRealWorld.multiply(mtxRealWorld, mtxScale);
				mtxRealWorld.getRowMajor44(mtxWorld);
				SetContantBuffer(pContext, mtxWorld);
				pBox->Render(pContext);
			}
			break;
			case NX_SHAPE_SPHERE:
				fRadius = ((NxSphereShape*)(*dpShape))->getRadius() * 2.f;
				mtxRealWorld = (*dpShape)->getGlobalPose();
				mtxScale.M.diagonal(NxVec3(fRadius));
				mtxRealWorld.multiply(mtxRealWorld, mtxScale);
				mtxRealWorld.getRowMajor44(mtxWorld);
				SetContantBuffer(pContext, mtxWorld);
				pSphere->Render(pContext);
				break;
			case NX_SHAPE_CAPSULE:
				fRadius = ((NxCapsuleShape*)(*dpShape))->getRadius() * 2.f;
				fHeight = ((NxCapsuleShape*)(*dpShape))->getHeight();
				mtxRealWorld = (*dpShape)->getGlobalPose();
				mtxScale.M.diagonal(NxVec3(fRadius, (fHeight + fRadius) * 0.5f, fRadius));
				mtxRealWorld.multiply(mtxRealWorld, mtxScale);
				mtxRealWorld.getRowMajor44(mtxWorld);
				SetContantBuffer(pContext, mtxWorld);
				pCapsule->Render(pContext);
				break;
			}
		}
	}

	pBox->Release();
	pSphere->Release();
	pCapsule->Release();

	delete[] mtxWorld;
}

void CPhysicsMgr::Release_Scene()
{
	if (m_pScene)
	{
		m_pCCTManager->purgeControllers();
		NxReleaseControllerManager(m_pCCTManager);
		m_pPhysicsSDK->releaseScene(*m_pScene);
		m_pCCTManager = nullptr;
		m_pScene = nullptr;
	}
}

void CPhysicsMgr::Release()
{

	if (m_pPhysicsSDK)
	{
		NxReleasePhysicsSDK(m_pPhysicsSDK);
		m_pPhysicsSDK = nullptr;
	}

	if (m_pAllocator)
	{
		delete m_pAllocator;
		m_pAllocator = nullptr;
	}

	delete this;
}

void CPhysicsMgr::CreateCube(const NxVec3& pos, int size, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene == NULL! \n");
		return;
	}

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3((float)size * 0.5f, (float)size * 0.5f, (float)size*0.5f);
	boxDesc.localPose.t = NxVec3((float)0.0f, (float)size*0.5f, (float)size*0.0f);

	NxBodyDesc bodyDesc;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;

	m_pScene->createActor(actorDesc);
}

void CPhysicsMgr::CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene == NULL! \n");
		return;
	}

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0, radius + height * 0.5f, 0);

	NxBodyDesc bodyDesc;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&capsuleDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	m_pScene->createActor(actorDesc);
}


void CPhysicsMgr::CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene == NULL! \n");
		return;
	}

	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0, radius, 0);

	NxBodyDesc bodyDesc;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	m_pScene->createActor(actorDesc);
}