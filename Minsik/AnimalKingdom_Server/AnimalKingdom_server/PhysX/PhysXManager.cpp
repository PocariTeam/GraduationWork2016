#include <stdafx.h>
#include "PhysXManager.h"
#include "Utilities.h"
#include "Stream.h"
#include "NxCooking.h"
#include "Cooking.h"
#include "UserAllocator.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "NxControllerManager.h"
#include <NxCapsuleShape.h>
#include "..\Banana.h"


PhysXManager::PhysXManager(void)
	: sceneSeedNum(0),
	userAllocator_(nullptr),
	physicsSDK_(nullptr),
	lock_(L"PhysXManager")
{
	for (int i = 0; i < GAMEROOM_CAPACITY; i++)
	{
		scenes_[i] = nullptr;
		CCTManager_[i] = nullptr;
	}
}

PhysXManager::~PhysXManager()
{
	NXU::releasePersistentMemory();

	if (physicsSDK_)
	{
		NxReleasePhysicsSDK(physicsSDK_);
		physicsSDK_ = nullptr;
	}

	if (userAllocator_)
	{
		CloseCooking();
		delete userAllocator_;
		userAllocator_ = nullptr;
	}

}

void PhysXManager::SetCollisionGroup(NxActor* pActor, NxCollisionGroup eGroup)
{
	NxU32			iActorShapeCnt = pActor->getNbShapes();
	NxShape* const* dpActorShapeArray = pActor->getShapes();

	while (iActorShapeCnt--)
		dpActorShapeArray[iActorShapeCnt]->setGroup(eGroup);
}

BOOL PhysXManager::LoadSceneFromFile(UINT roomNum, map<UINT, Player*>* pmapPlayers )
{
	SAFE_LOCK(lock_);

	BOOL success = false;

	if (physicsSDK_)
	{

		NXU::NxuPhysicsCollection *c = NXU::loadCollection(fileName, NXU::FT_XML);

		if (c)
		{
			ReleaseScene(roomNum);
			
			success = NXU::instantiateCollection(c, *physicsSDK_, 0, 0, 0);
			
			scenes_[roomNum] = physicsSDK_->getScene(sceneSeedNum++);
			scenes_[roomNum]->userData = (void*)roomNum;

			NXU::releaseCollection(c);
		}
		else
		{
			SLog(L"! NxuPhysicsCollection is NULL!");
		}
	}

	if (success)
	{
		SLog(L"* Room [%d]'s Scene loaded from file '%S'.", roomNum, fileName);
		return SetupScene(roomNum, pmapPlayers );
	}

	return success;
}

NxController* PhysXManager::CreateCharacterController(NxActor* actor, const NxVec3& startPos, NxReal scale, UINT roomNum)
{
	//actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);

	float	fSKINWIDTH = 0.1f;
	NxController *pCtrl;

#if 0
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
		desc.stepOffset = 0.1f;
		desc.userData = (NxActor*)actor;
		desc.callback = &controllerReport_;
		pCtrl = CCTManager_[roomNum]->createController(scenes_[roomNum], desc);
#else
		// 캡슐 컨트롤러
		// NxF32	InitialRadius = 0.5f;
		// NxF32	InitialHeight = 2.0f;
		NxCapsuleControllerDesc desc;
		desc.radius = ( ( NxCapsuleShape* )actor->getShapes()[ 0 ] )->getRadius();
		desc.height = ( ( NxCapsuleShape* )actor->getShapes()[ 0 ] )->getHeight() * 1.f;
		desc.position.x = startPos.x;
		desc.position.y = startPos.y; //+ gSpace;
		desc.position.z = startPos.z;
		desc.upDirection = NX_Y;
		//		desc.slopeLimit		= cosf(NxMath::degToRad(45.0f));
		desc.slopeLimit = 0;
		desc.skinWidth = fSKINWIDTH;
		desc.stepOffset = 0.1f;
		// desc.stepOffset = InitialRadius * 0.5f * scale;
		// desc.userData = (NxActor*)actor;
		desc.callback = &controllerReport_;
		pCtrl = CCTManager_[roomNum]->createController(scenes_[roomNum], desc);
		
#endif

	char CCTName[256] = "CCTActor of ";
	strcat_s(CCTName, actor->getName());
	pCtrl->getActor()->setName(CCTName);
	return pCtrl;

}

void PhysXManager::CreateMeshFromShape(NxSimpleTriangleMesh &triMesh, NxShape *shape)
{
	NxBoxShape *boxShape = shape->isBox();
	if (boxShape != NULL)
	{
		NxBox obb = NxBox(NxVec3(0.0f, 0.0f, 0.0f), boxShape->getDimensions(), NxMat33(NX_IDENTITY_MATRIX));
		triMesh.points = new NxVec3[8];
		triMesh.numVertices = 8;
		triMesh.pointStrideBytes = sizeof(NxVec3);
		triMesh.numTriangles = 2 * 6;
		triMesh.triangles = new NxU32[2 * 6 * 3];
		triMesh.triangleStrideBytes = sizeof(NxU32) * 3;
		triMesh.flags = 0;
		NxComputeBoxPoints(obb, (NxVec3 *)triMesh.points);
		memcpy((NxU32 *)triMesh.triangles, NxGetBoxTriangles(), sizeof(NxU32) * 2 * 6 * 3);
	}
	else
	{
		NX_ASSERT(!"Invalid shape type");
	}

	NX_ASSERT(triMesh.isValid());
}

void PhysXManager::CreateBanana( UINT iSceneNum )
{
	ACTOR_INFO	tActor_Info;
	tActor_Info.m_dwType = 2;
	tActor_Info.m_fWidth = 2.5f;
	tActor_Info.m_fHeight = 8.f;
	tActor_Info.m_fLength = 2.5f;
	tActor_Info.m_vGlobalPosition.x = 0.f;
	tActor_Info.m_vGlobalPosition.y = 0.f;
	tActor_Info.m_vGlobalPosition.z = -1000.f;

	NxActor* pActor = CreateActor( COL_DYNAMIC, "Banana", tActor_Info, iSceneNum );
	pActor->raiseBodyFlag( NX_BF_KINEMATIC );

	// CCD 충돌체크
	NxShape *shape = pActor->getShapes()[ 0 ];
	NxSimpleTriangleMesh triMesh;
	CreateMeshFromShape( triMesh, shape );
	NxCCDSkeleton *newSkeleton = physicsSDK_->createCCDSkeleton( triMesh );
	shape->setCCDSkeleton( newSkeleton );
	delete[] triMesh.points;
	delete[] triMesh.triangles;

	CBanana*	pBanana = CBanana::Create( pActor, COL_DYNAMIC, iSceneNum );
	pActor->userData = pBanana;
	m_BananaQueue[ iSceneNum ].push( pBanana );
}

void PhysXManager::ThrowBanana( NxVec3& vPos, NxVec3& vDir, COL_GROUP eColGroup, UINT iSceneNum )
{
	CBanana* pBanana = m_BananaQueue[ iSceneNum ].front();
	if( pBanana->GetMasterCollisionGroup() > COL_DYNAMIC ) return;
	pBanana->Throw( vPos, vDir, eColGroup );
	m_BananaQueue[ iSceneNum ].pop();
	m_BananaQueue[ iSceneNum ].push( pBanana );
}

BOOL PhysXManager::SetupScene( UINT roomNum, map<UINT, Player*>* pmapPlayers )
{
	SAFE_LOCK(lock_);

	if (scenes_[roomNum] == nullptr)
	{
		SLog(L"! scenes_[%d] is nullptr.", roomNum);
		return false;
	}

	CCTManager_[roomNum] = NxCreateControllerManager(userAllocator_);

	physicsSDK_->setParameter(NX_SKIN_WIDTH, 2.5f); // 피부두께 (민감도)
	physicsSDK_->setParameter(NX_CONTINUOUS_CD, true); // 고속충돌
	physicsSDK_->setParameter(NX_CCD_EPSILON,0.1f); // 고속충돌 정밀도
	physicsSDK_->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 15 * 15); // 객체가 잠드는 최소한 선속도
	physicsSDK_->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 14 * 14); // 객체가 잠드는 최소한 각속도
	physicsSDK_->setParameter(NX_BOUNCE_THRESHOLD, -5); //  통통 튀는 최소 속도
	physicsSDK_->setParameter(NX_DYN_FRICT_SCALING, 10); // 동적객체 마찰
	physicsSDK_->setParameter(NX_STA_FRICT_SCALING, 10); // 정적객체 마찰

	scenes_[roomNum]->setGravity(NxVec3(0.0f, -9.81f * 3, 0.0f));
	scenes_[roomNum]->setUserContactReport(&collisionReport_);
	scenes_[roomNum]->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER1, NX_NOTIFY_ON_START_TOUCH/* | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH*/ );
	scenes_[ roomNum ]->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER2, NX_NOTIFY_ON_START_TOUCH/* | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH*/ );
	scenes_[ roomNum ]->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER3, NX_NOTIFY_ON_START_TOUCH/* | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH*/ );
	scenes_[ roomNum ]->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER4, NX_NOTIFY_ON_START_TOUCH/* | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH*/ );
	

	scenes_[roomNum]->setActorGroupPairFlags(COL_DYNAMIC, COL_DYNAMIC, NX_NOTIFY_ON_START_TOUCH/* | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH*/);
	scenes_[roomNum]->setActorGroupPairFlags(COL_DYNAMIC, COL_STATIC, NX_NOTIFY_ON_START_TOUCH/* | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH*/);
	
	// Create the default material
	NxMaterial* pDefaultMaterial = scenes_[roomNum]->getMaterialFromIndex(0);
	pDefaultMaterial->setRestitution(0.0);
	pDefaultMaterial->setStaticFriction(0.5);
	pDefaultMaterial->setDynamicFriction(0.5);

	scenes_[roomNum]->simulate(0.003f);
	scenes_[roomNum]->flushStream();
	scenes_[roomNum]->fetchResults(NX_RIGID_BODY_FINISHED, true);

	NxU32 nbActors = scenes_[roomNum]->getNbActors();
	NxActor** aList = scenes_[roomNum]->getActors();
	set<int, less<int> >	setReleaseActorIndex;

	UINT currentPlayer = 0;
	UINT playerCount = RoomManager::getInstance().getPlayerCountRoom(roomNum);

	auto player_iter = pmapPlayers->begin();

	for (NxU32 i = 0; i < nbActors; i++)
	{
		NxActor *a = aList[i];

		NxVec3 pos = a->getGlobalPosition();

		SLog(L"* actor '%S' global: (%f, %f, %f)", a->getName(), pos.x, pos.y, pos.z);

		// 충돌그루핑
		if (a->isDynamic())
		{
			if (  0 == strcmp(a->getName(), "Player1")
				|| 0 == strcmp(a->getName(), "Player2")
				|| 0 == strcmp(a->getName(), "Player3")
				|| 0 == strcmp(a->getName(), "Player4")
				)
			{
				if( playerCount == currentPlayer )
				{
					setReleaseActorIndex.insert( i );
					continue;
				}

				a->setGroup( COL_GROUP( COL_PLAYER1 << currentPlayer ) );
				SetCollisionGroup(a, COL_GROUP( COL_PLAYER1 << currentPlayer ) );
				CreateCharacterController(a, a->getGlobalPosition(), 2.8f,roomNum);
				UINT iActorCnt{};
				NxActor** dpActors = CreateCharacterActors( COL_GROUP( COL_PLAYER1 << currentPlayer ), player_iter->second->getPlayerInfo().character, roomNum, iActorCnt );
				player_iter->second->setActorArray( dpActors, iActorCnt );
				aList = scenes_[ roomNum ]->getActors();
				setReleaseActorIndex.insert( i );
				currentPlayer++;
				player_iter++;
			}

			else if( 0 == strcmp( a->getName(), "Crown" ) )
			{
				a->setGroup( COL_DYNAMIC );
				m_pCrownActor[roomNum] = a;
				SetCollisionGroup( a, COL_DYNAMIC );
				m_pCrownActor[roomNum]->setGlobalPosition(getRandomCrownPosition());
			}

			else
			{
				a->setGroup(CollGroup::COL_DYNAMIC);
				SetCollisionGroup(a, CollGroup::COL_DYNAMIC);
			}
		}
		else
		{
			a->setGroup(CollGroup::COL_STATIC);
			SetCollisionGroup(a, CollGroup::COL_STATIC);
		}

		//NxU32 nbShapes = a->getNbShapes();
		//if (nbShapes)
		//{
		//	NxShape ** slist = (NxShape **)a->getShapes();
		//	for (NxU32 j = 0; j < nbShapes; j++)
		//	{
		//		NxShape *s = slist[j];
		//		NxVec3 spos = s->getLocalPosition();
		//		SLog(L" -> [%d] %S local: (%f, %f, %f)", j, s->getName(), spos.x, spos.y, spos.z);
		//	}
		//}
	}

	auto set_iter_begin = setReleaseActorIndex.begin();
	auto set_iter_end = setReleaseActorIndex.end();

	aList = scenes_[ roomNum ]->getActors();

	for( ; set_iter_begin != set_iter_end; ++set_iter_begin )
		scenes_[ roomNum ]->releaseActor( *aList[ ( *set_iter_begin ) ] );

	for( int i = 0; i < BANANA_COUNT; ++i )
		CreateBanana( roomNum );
	
	return true;
}

void PhysXManager::ReleaseScene(UINT roomNum)
{
	SAFE_LOCK(lock_);

	while( false == m_BananaQueue[ roomNum ].empty() )
	{
		m_BananaQueue[ roomNum ].front()->Release();
		m_BananaQueue[ roomNum ].pop();
	}

	if (CCTManager_[roomNum])
	{
		CCTManager_[roomNum]->purgeControllers();
		NxReleaseControllerManager(CCTManager_[roomNum]);
		CCTManager_[roomNum] = nullptr;
	}
	if (scenes_[roomNum])
	{
		scenes_[roomNum]->shutdownWorkerThreads();
		physicsSDK_->releaseScene(*scenes_[roomNum]);
		scenes_[roomNum] = nullptr;
		sceneSeedNum--;
	}
}

void PhysXManager::updateScene(UINT roomNum, float fTimeDelta)
{
	SAFE_LOCK(lock_);

	checkCrownFalling(roomNum);

	CCTManager_[roomNum]->updateControllers();
	scenes_[roomNum]->simulate(fTimeDelta);
	scenes_[roomNum]->flushStream();
	scenes_[roomNum]->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void PhysXManager::updateCCT(UINT roomNum)
{
	CCTManager_[roomNum]->updateControllers();
}

BOOL PhysXManager::initPhysX()
{

	userAllocator_ = new UserAllocator;

	bool status = InitCooking(userAllocator_);

	if (!status)
	{
		SLog(L"! unable to initialize the cooking library.");
		return false;
	}

	NxPhysicsSDKDesc sdkDesc;
	sdkDesc.flags ^= NX_SDKF_NO_HARDWARE;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	physicsSDK_ = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, userAllocator_, NULL, sdkDesc, &errorCode);
	if (physicsSDK_ == NULL)
	{
		SLog(L"! SDK create error (%d - %S).", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}

	SLog(L"# Initialize PhysX SDK.");

	Load_Kinematic();

	return true;
}

BOOL PhysXManager::Load_Kinematic( void )
{
	ifstream	In{ "../Executable/KinematicActor.txt" };
	int			iCharacterType{}, iActorCnt{};
	char		szJointName[ MAX_PATH ]{ "" };
	ACTOR_INFO	tActorInfo;

	while( !In.eof() )
	{
		In >> iCharacterType;
		In >> iActorCnt;

		for( int i = 0; i < iActorCnt; ++i )
		{
			In >> szJointName;
			In >> tActorInfo.m_dwType;
			In >> tActorInfo.m_vGlobalPosition.x; In >> tActorInfo.m_vGlobalPosition.y; In >> tActorInfo.m_vGlobalPosition.z;
			In >> tActorInfo.m_fRadius; In >> tActorInfo.m_fLength; In >> tActorInfo.m_fWidth; In >> tActorInfo.m_fHeight;

			m_mapActorInfo[ iCharacterType ].insert( make_pair( szJointName, tActorInfo ) );
		}
	}

	In.close();

	return TRUE;
}

NxActor** PhysXManager::CreateCharacterActors( COL_GROUP eColgroup, CHARACTER eCharacterType, UINT iSceneNum, UINT& iActorCnt )
{
	DWORD dwCameleonActorCnt = ( DWORD )m_mapActorInfo[ eCharacterType ].size();
	NxActor** dpCameleonActors = new NxActor*[ dwCameleonActorCnt ];

	auto iter_begin = m_mapActorInfo[ eCharacterType ].begin();
	auto iter_end = m_mapActorInfo[ eCharacterType ].end();

	UINT j = 0;
	for( ; iter_begin != iter_end; ++j, ++iter_begin )
	{
		dpCameleonActors[ j ] = CreateActor( eColgroup, ( *iter_begin ).first.c_str(), ( *iter_begin ).second, iSceneNum );
		dpCameleonActors[ j ]->raiseBodyFlag( NX_BF_KINEMATIC );
		dpCameleonActors[ j ]->userData = new NxMat34;
		*( NxMat34* )dpCameleonActors[ j ]->userData = dpCameleonActors[ j ]->getGlobalPose();
	}

	iActorCnt = j;

	return dpCameleonActors;
}

NxActor* PhysXManager::CreateActor( COL_GROUP eColgroup, const char* pActorName, const ACTOR_INFO& tActor_Info, UINT iSceneNum )
{
	switch( tActor_Info.m_dwType )
	{
	case NX_SHAPE_SPHERE:
	{
		NxSphereShapeDesc	tSphereShapeDesc;
		tSphereShapeDesc.radius = tActor_Info.m_fRadius;
		tSphereShapeDesc.name = pActorName;
		tSphereShapeDesc.localPose.t = NxVec3( 0.f, 0.f, 0.f );

		NxActorDesc tActorDesc;
		tActorDesc.name = pActorName;
		tActorDesc.globalPose.t.set( tActor_Info.m_vGlobalPosition.x, tActor_Info.m_vGlobalPosition.y, tActor_Info.m_vGlobalPosition.z );

		NxBodyDesc bodyDesc;
		tActorDesc.body = &bodyDesc;

		tActorDesc.density = 1;
		tActorDesc.group = eColgroup;
		tSphereShapeDesc.group = eColgroup;

		tActorDesc.shapes.pushBack( &tSphereShapeDesc );

		return scenes_[ iSceneNum ]->createActor( tActorDesc );
	}
	case NX_SHAPE_BOX:
	{
		NxBoxShapeDesc		tBoxShapeDesc;
		tBoxShapeDesc.dimensions = NxVec3( tActor_Info.m_fLength * 0.5f, tActor_Info.m_fHeight * 0.5f, tActor_Info.m_fWidth * 0.5f );
		tBoxShapeDesc.name = pActorName;
		tBoxShapeDesc.localPose.t = NxVec3( 0.f, 0.f, 0.f );

		NxActorDesc tActorDesc;
		tActorDesc.name = pActorName;
		tActorDesc.globalPose.t.set( tActor_Info.m_vGlobalPosition.x, tActor_Info.m_vGlobalPosition.y, tActor_Info.m_vGlobalPosition.z );

		NxBodyDesc bodyDesc;
		tActorDesc.body = &bodyDesc;
		tActorDesc.density = 1;
		tActorDesc.group = eColgroup;
		tBoxShapeDesc.group = eColgroup;

		tActorDesc.shapes.pushBack( &tBoxShapeDesc );

		return scenes_[ iSceneNum ]->createActor( tActorDesc );
	}
	case NX_SHAPE_CAPSULE:
	{
		NxCapsuleShapeDesc		tCapsuleShapeDesc;
		tCapsuleShapeDesc.radius = tActor_Info.m_fRadius;
		tCapsuleShapeDesc.height = tActor_Info.m_fHeight;
		tCapsuleShapeDesc.name = pActorName;
		tCapsuleShapeDesc.localPose.t = NxVec3( 0.f, 0.f, 0.f );

		NxActorDesc tActorDesc;
		tActorDesc.name = pActorName;
		tActorDesc.globalPose.t.set( tActor_Info.m_vGlobalPosition.x, tActor_Info.m_vGlobalPosition.y, tActor_Info.m_vGlobalPosition.z );

		NxBodyDesc bodyDesc;
		tActorDesc.body = &bodyDesc;
		tActorDesc.density = 1;
		tActorDesc.group = eColgroup;
		tCapsuleShapeDesc.group = eColgroup;

		tActorDesc.shapes.pushBack( &tCapsuleShapeDesc );

		return scenes_[ iSceneNum ]->createActor( tActorDesc );
	}
	default:
		return nullptr;
	}
}

S_SyncDynamic* PhysXManager::getDynamicInfo(UINT roomNum)
{
	if (scenes_[roomNum] == NULL) return NULL;

	NxU32 nbActors = scenes_[roomNum]->getNbActors();
	NxActor** aList = scenes_[roomNum]->getActors();

	S_SyncDynamic *packet = new S_SyncDynamic;
	packet->header.packetID = PAK_ID::PAK_ANS_SyncDynamic;
	packet->header.size = sizeof(S_SyncDynamic);


	// printf("총 액터수: %d \n", nbActors);
	unsigned int count = 0;
	for (NxU32 i = 0; i < nbActors; i++)
	{
		if (true == aList[i] ->isDynamic() && 
			false == aList[i]->readBodyFlag(NX_BF_KINEMATIC))
		{
			if (count >= DYNAMIC_CAPACITY)
			{
				SLog(L"! DYNAMIC_CAPACITY is overflow! index: %d",count);
				break;
			}

			// printf("[index:%d] %s \n", i, aList[i]->getName());

			packet->dynamicActors[count].index = i;

			NxVec3 p = aList[i]->getGlobalPosition();
			NxQuat o = aList[i]->getGlobalOrientationQuat();
			packet->dynamicActors[count].position = Vector3(p.x, p.y, p.z);
			packet->dynamicActors[count].orient = Vector4(o.x, o.y, o.z, o.w);

			NxVec3 l = aList[i]->getLinearVelocity();
			NxVec3 a = aList[i]->getAngularVelocity();
			packet->dynamicActors[count].linear = Vector3(l.x, l.y, l.z);
			packet->dynamicActors[count].angular = Vector3(a.x, a.y, a.z);

			++count;
		}
	}
	packet->dynamicActorCount = count;

	return packet;
}

S_SyncDynamicOne* PhysXManager::getDynamicOneInfo(NxActor *actor)
{
	if (false == actor->isDynamic() || true == actor->readBodyFlag(NX_BF_KINEMATIC))
	{
		return NULL;
	}

	NxScene *scene = &actor->getScene();

	NxU32 nbActors = scene->getNbActors();
	NxActor** aList = scene->getActors();

	S_SyncDynamicOne *packet = new S_SyncDynamicOne;
	packet->header.packetID = PAK_ID::PAK_ANS_SyncDynamicOne;
	packet->header.size = sizeof(S_SyncDynamicOne);

	for (NxU32 i = 0; i < nbActors; i++)
	{
		if (actor == aList[i])
		{
			packet->dynamicActor.index = i;

			NxVec3 p = aList[i]->getGlobalPosition();
			NxQuat o = aList[i]->getGlobalOrientationQuat();
			packet->dynamicActor.position = Vector3(p.x, p.y, p.z);
			packet->dynamicActor.orient = Vector4(o.x, o.y, o.z, o.w);

			NxVec3 l = aList[i]->getLinearVelocity();
			NxVec3 a = aList[i]->getAngularVelocity();
			packet->dynamicActor.linear = Vector3(l.x, l.y, l.z);
			packet->dynamicActor.angular = Vector3(a.x, a.y, a.z);
			return packet;
		}
	}

	SLog(L"! not found sync actor.");
	return NULL;
}

NxScene* PhysXManager::getScene( UINT roomNum )
{
	return scenes_[ roomNum ];
}

NxPhysicsSDK* PhysXManager::getSDK()
{
	return physicsSDK_;
}

void PhysXManager::setCrownPosition(UINT roomNum, NxMat34 posMat)
{
	m_pCrownActor[roomNum]->clearBodyFlag(NX_BF_KINEMATIC);
	m_pCrownActor[roomNum]->setGlobalPose(posMat);
	m_pCrownActor[roomNum]->setLinearVelocity(NxVec3(0.0f,1.0f,0.0f) * 50.f);
	m_pCrownActor[roomNum]->setAngularVelocity(NxVec3(90.f, 0.f, 180.f));
}

void PhysXManager::checkCrownFalling(UINT roomNum)
{
	if (RoomManager::getInstance().hasWinner(roomNum)) return;
	if (m_pCrownActor[roomNum]->getGlobalPosition().y > 20.0f) return;
	
	RoomManager::getInstance().sendGetCrown(roomNum, nullptr);

	m_pCrownActor[roomNum]->clearBodyFlag(NX_BF_KINEMATIC);
	m_pCrownActor[roomNum]->setLinearVelocity(NxVec3(0.f,0.f,0.f));
	m_pCrownActor[roomNum]->setGlobalPosition(getRandomCrownPosition());
}

NxVec3 PhysXManager::getRandomCrownPosition()
{
	return NxVec3((float)(rand() % 160 - 80), 230.0f, (float)(rand() % 160 + 70));
}

