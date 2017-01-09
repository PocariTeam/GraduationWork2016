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
#include "UserAllocator.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxControllerManager.h"
#include "HitReport.h"

extern UserEntityReport gUserEntityReport;
extern ControllerHitReport gControllerHitReport;

CPhysicsMgr*	CPhysicsMgr::m_pInstance;


//점프를 위한 변수들
/*
a(t) = cte = g
v(t) = g*t + v0
y(t) = g*t^2 + v0*t + y0
*/
bool gJump = false;
NxF32 jumpTime; 
NxF32 GRAVITY = -9.81f;

CPhysicsMgr::CPhysicsMgr(void)
	: m_pPhysicsSDK(nullptr)
	, m_pScene(nullptr)
	, m_pCBmtxWorld(nullptr)
	, m_pCCTManager(nullptr)
	, m_pMyCCT(nullptr)
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
	//actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);

	float	fSKINWIDTH = 0.5f;
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
		desc.callback = &gControllerHitReport;
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
		desc.stepOffset = 0.5f;
		desc.stepOffset = InitialRadius * 0.5f * scale;
		desc.userData = (NxActor*)actor;
		//	desc.stepOffset	= 0.01f;
		//	desc.stepOffset		= 0;	// Fixes some issues
		//	desc.stepOffset		= 10;
		desc.callback = &gControllerHitReport;
		pCtrl = m_pCCTManager->createController(m_pScene, desc);
	}

	NxActor *CCTActor = pCtrl->getActor();
	CCTActor->setName("Character Controller");
	NxBounds3 bound;
	NxVec3 extents;
	CCTActor->getShapes()[0]->getWorldBounds(bound);
	bound.getExtents(extents);
	CCTActor->getShapes()[0]->setLocalPosition(NxVec3(0, extents.y, 0));

	return pCtrl;

}

void CPhysicsMgr::SetShapesCollisionGroup(NxActor* actor, NxCollisionGroup group)
{
	
	NxU32 nbShapes = actor->getNbShapes();
	NxShape*const* shapes = actor->getShapes();

	while (nbShapes--)
	{
		shapes[nbShapes]->setGroup(group);
	}
}


HRESULT CPhysicsMgr::SetupScene()
{
	if (!m_pScene) {
		return E_FAIL;
	}
	
/* 
	PPU를 이용한 물리계산, 해당 Compartment를 액터에 넣어주어야 함

	NxCompartmentDesc cdesc;
	cdesc.type = NX_SCT_RIGIDBODY;
	cdesc.deviceCode = NX_DC_PPU_AUTO_ASSIGN;
	NxCompartment *c = m_pScene->createCompartment(cdesc);
*/

	NxU32 nbActors = m_pScene->getNbActors();
	NxActor** aList = m_pScene->getActors();

	if (m_pScene)
	{
		for (NxU32 i = 0; i < nbActors; i++)
		{
			NxActor *a = aList[i];
			NxVec3 pos = a->getGlobalPosition();

			printf(" - 액터 '%s'의 global: (%f, %f, %f) \n", a->getName(), pos.x, pos.y, pos.z);
			
			// 충돌그루핑
			if (a->isDynamic()) 
			{
				// 캐릭터 설정 (지금은 myPlayer가 나의 캐릭터라고 가정)
				if (strcmp(a->getName(), "myPlayer") == 0) 
				{
					SetShapesCollisionGroup(a, CollGroup::MY_CHARACTER);

					// 임시설정값 (실제는 파일에서 미리 설정해주어야 함)
					a->raiseBodyFlag(NX_BF_KINEMATIC);
					a->setGlobalPosition(NxVec3(0, 0, 0));

					// 컨트롤러 생성
					m_pMyCCT = CreateCharacterController(a, a->getGlobalPosition(), 1.0f);

					// 테스트용 상대방
					CreateCharacterController(a, NxVec3(5, 0, -5), 2.0f);

				}
				else
				{
					SetShapesCollisionGroup(a, CollGroup::DYNAMIC);
				}
			}
			else 
			{
				SetShapesCollisionGroup(a, CollGroup::STATIC);
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
	
	// 테스트용
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			CreateSphere(NxVec3(-30.0f + 3 * j, 0, -30.0f + 3*i), 0.5f, 10.0f);
		}
	}
	CreateCube(NxVec3(7, 0, 7), 10); // 스태틱
	auto a = CreateCube(NxVec3(-7, 0, 7), 10); // 키네마틱
	a->raiseBodyFlag(NX_BF_KINEMATIC);
	SetShapesCollisionGroup(a, CollGroup::OTHER_CHARACTER);

	CreateCube(NxVec3(0, -61, 0), 60);

	return S_OK;
}



HRESULT CPhysicsMgr::CreateScene(ID3D11Device* pDevice)
{
	if (!LoadSceneFromFile("../Executable/Resources/Scene/human_fix.xml", NXU::FT_XML)) {
		printf("LoadScene() is failed! \n");
		return E_FAIL;
	};

	SetupScene();

#ifdef _DEBUG
	CreateContantBuffer(pDevice);
#endif
	return S_OK;
}


void CPhysicsMgr::Update(const float & fTimeDelta)
{
	NxVec3 dir(0, 0, 0);
	NxF32 fSpeed = 10.0f;

	if (GetAsyncKeyState(VK_UP) & 0x8000) 
	{
		dir += NxVec3(0, 0, +1);
	}
	if ((GetAsyncKeyState(VK_DOWN) & 0x8000))
	{
		dir += NxVec3(0, 0, -1);
	}
	if ((GetAsyncKeyState(VK_LEFT) & 0x8000))
	{
		dir += NxVec3(-1, 0, 0);
	}
	if ((GetAsyncKeyState(VK_RIGHT) & 0x8000))
	{
		dir += NxVec3(+1, 0, 0);
	}
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && !gJump)
	{
		jumpTime = 0.0f;
		gJump = true;
	}

	dir.normalize();
	dir *= fTimeDelta*fSpeed;

	if (gJump) {
		jumpTime += fTimeDelta;
		NxF32 h = GRAVITY*jumpTime*jumpTime + fSpeed*jumpTime;
		dir.y += (h - GRAVITY)*fTimeDelta;
	}

	if (!dir.isZero())
	{
		// 이동
		NxU32 collisionFlags;
		m_pMyCCT->move(dir, COLLIDABLE_MASK, 0.001f, collisionFlags);

		if (collisionFlags & NXCC_COLLISION_DOWN)
		{
			if(gJump) { gJump = false; }
			printf("높이: %f \n", m_pMyCCT->getPosition().y);
		}

		m_pCCTManager->updateControllers();
		NxExtendedVec3 pos = m_pMyCCT->getPosition();
		((NxActor*)m_pMyCCT->getUserData())->moveGlobalPosition(NxVec3(pos.x, pos.y, pos.z));

		/*	테스트용: 플레이어 액터만 움직이기
		NxVec3 oldPos = ((NxActor*)m_pMyCCT->getUserData())->getGlobalPosition();
		((NxActor*)m_pMyCCT->getUserData())->moveGlobalPosition(oldPos+dir);
		*/

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

		
		/*
		// Sweep API를 이용한 충돌체크
		NxSweepQueryHit result[100];
		NxCapsule testBody;
		((NxActor*)m_pMyCCT->getUserData())->getShapes()[1]->isCapsule()->getWorldCapsule(testBody);
		m_pScene->linearCapsuleSweep(testBody, pos, (NX_SF_STATICS | NX_SF_DYNAMICS), NULL, 16, result,
		(NxUserEntityReport<NxSweepQueryHit>*)&gUserEntityReport, 1 << OTHER_CHARACTER );
		*/
		
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

NxActor* CPhysicsMgr::CreateCube(const NxVec3& pos, int size, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene is NULL! \n");
		return nullptr;
	}

	NxBoxShapeDesc boxDesc;
	boxDesc.name = "TestCubeShape";
	boxDesc.dimensions = NxVec3((float)size * 0.5f, (float)size * 0.5f, (float)size*0.5f);
	boxDesc.localPose.t = NxVec3((float)0.0f, (float)size*0.5f, (float)size*0.0f);
	

	NxActorDesc actorDesc;
	actorDesc.name = "TestCubeActor";
	actorDesc.globalPose.t = pos;

	if (density) {
		NxBodyDesc bodyDesc;
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
		boxDesc.group = CollGroup::DYNAMIC;
	}
	else {
		actorDesc.body = NULL;
		boxDesc.group = CollGroup::STATIC;
	}

	actorDesc.shapes.pushBack(&boxDesc);

	return m_pScene->createActor(actorDesc);
}

NxActor* CPhysicsMgr::CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene is NULL! \n");
		return nullptr;
	}

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.name = "TestCapsuleShape";
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0, radius + height * 0.5f, 0);

	NxActorDesc actorDesc;
	actorDesc.name = "TestCapsuleActor";
	actorDesc.globalPose.t = pos;

	if (density) {
		NxBodyDesc bodyDesc;
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
		capsuleDesc.group = CollGroup::DYNAMIC;
	}
	else {
		actorDesc.body = NULL;
		capsuleDesc.group = CollGroup::STATIC;
	}

	actorDesc.shapes.pushBack(&capsuleDesc);

	return m_pScene->createActor(actorDesc);
}

NxActor* CPhysicsMgr::CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene is NULL! \n");
		return nullptr;
	}

	NxSphereShapeDesc sphereDesc;
	sphereDesc.name = "TestSphereShape";
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0, radius, 0);

	NxActorDesc actorDesc;
	actorDesc.name = "TestSphereActor";
	actorDesc.globalPose.t = pos;

	if (density) {
		NxBodyDesc bodyDesc;
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
		sphereDesc.group = CollGroup::DYNAMIC;
	}
	else {
		actorDesc.body = NULL;
		sphereDesc.group = CollGroup::STATIC;
	}
	actorDesc.shapes.pushBack(&sphereDesc);

	return m_pScene->createActor(actorDesc);
}