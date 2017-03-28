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
	for (int i = 0; i < GAMEROOM_CAPACITY; i++)
	{
		ReleaseScene(i);
	}


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

BOOL PhysXManager::LoadSceneFromFile(UINT32 roomNum)
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
		return SetupScene(roomNum);
	}

	return success;
}

NxController* PhysXManager::CreateCharacterController(NxActor* actor, const NxVec3& startPos, NxReal scale, UINT32 roomNum)
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
		NxF32	InitialRadius = 0.5f;
		NxF32	InitialHeight = 2.0f;
		NxCapsuleControllerDesc desc;
		desc.radius = InitialRadius * scale;
		desc.height = InitialHeight * scale;
		desc.position.x = startPos.x;
		desc.position.y = startPos.y; //+ gSpace;
		desc.position.z = startPos.z;
		desc.upDirection = NX_Y;
		//		desc.slopeLimit		= cosf(NxMath::degToRad(45.0f));
		desc.slopeLimit = 0;
		desc.skinWidth = fSKINWIDTH;
		desc.stepOffset = 0.1f;
		desc.stepOffset = InitialRadius * 0.5f * scale;
		desc.userData = (NxActor*)actor;
		desc.callback = &controllerReport_;
		pCtrl = CCTManager_[roomNum]->createController(scenes_[roomNum], desc);
		
#endif

	char CCTName[256] = "CCTActor of ";
	strcat_s(CCTName, actor->getName());
	pCtrl->getActor()->setName(CCTName);
	return pCtrl;

}


BOOL PhysXManager::SetupScene(UINT32 roomNum)
{
	SAFE_LOCK(lock_);

	if (scenes_[roomNum] == nullptr)
	{
		SLog(L"! scenes_[%d] is nullptr.", roomNum);
		return false;
	}

	CCTManager_[roomNum] = NxCreateControllerManager(userAllocator_);

	physicsSDK_->setParameter(NX_SKIN_WIDTH, 0.2f);
	scenes_[roomNum]->setUserContactReport(&collisionReport_);
	scenes_[roomNum]->setActorGroupPairFlags(COL_PLAYER, COL_DYNAMIC, NX_NOTIFY_ON_START_TOUCH);

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

	UINT currentPlayer = 0;
	UINT playerCount = RoomManager::getInstance().getPlayerCountRoom(roomNum);

	for (NxU32 i = 0; i < nbActors; i++)
	{
		NxActor *a = aList[i];

		NxVec3 pos = a->getGlobalPosition();

		SLog(L"* actor '%S' global: (%f, %f, %f)", a->getName(), pos.x, pos.y, pos.z);

		// 충돌그루핑
		if (a->isDynamic())
		{
			if (  0 == strcmp(a->getName(), "player00")
				|| 0 == strcmp(a->getName(), "player01")
				|| 0 == strcmp(a->getName(), "player02")
				|| 0 == strcmp(a->getName(), "player03")
				)
			{
				if (currentPlayer >= playerCount)
					continue;

				a->setGroup(CollGroup::COL_PLAYER);
				SetCollisionGroup(a, CollGroup::COL_PLAYER);
				CreateCharacterController(a, a->getGlobalPosition(), 2.8f,roomNum);
				currentPlayer++;
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

	return true;
}

void PhysXManager::ReleaseScene(UINT32 roomNum)
{
	SAFE_LOCK(lock_);

	if (CCTManager_[roomNum])
	{
		CCTManager_[roomNum]->purgeControllers();
		NxReleaseControllerManager(CCTManager_[roomNum]);
		CCTManager_[roomNum] = nullptr;
	}
	if (scenes_[roomNum])
	{
		physicsSDK_->releaseScene(*scenes_[roomNum]);
		scenes_[roomNum] = nullptr;
		sceneSeedNum--;
	}
}

void PhysXManager::updateScene(UINT32 roomNum, float fTimeDelta)
{
	CCTManager_[roomNum]->updateControllers();
	scenes_[roomNum]->simulate(fTimeDelta);
	scenes_[roomNum]->flushStream();
	scenes_[roomNum]->fetchResults(NX_RIGID_BODY_FINISHED, true);
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
	return true;
}
