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
	: userAllocator_(nullptr)
	, physicsSDK_(nullptr)
{
	for (int i = 0; i < GAMEROOM_CAPACITY; i++)
	{
		scenes_[i] = nullptr;
	}
}

PhysXManager::~PhysXManager()
{
	for (int i = 0; i < GAMEROOM_CAPACITY; i++)
	{
		if (scenes_[i])
		{
			physicsSDK_->releaseScene(*scenes_[i]);
			scenes_[i] = nullptr;
		}
	}

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

BOOL PhysXManager::LoadSceneFromFile(const char *pFilename, NXU::NXU_FileType type, UINT32 roomNum)
{
	BOOL success = false;

	if (physicsSDK_)
	{

		NXU::NxuPhysicsCollection *c = NXU::loadCollection(pFilename, type);

		if (c)
		{
			if (scenes_[roomNum])
			{
				physicsSDK_->releaseScene(*scenes_[roomNum]);
				scenes_[roomNum] = nullptr;
			}

			success = NXU::instantiateCollection(c, *physicsSDK_, scenes_[roomNum], 0, 0);
			scenes_[roomNum] = physicsSDK_->getScene(roomNum);

			NXU::releaseCollection(c);
		}
		else
		{
			SLog(L"! NxuPhysicsCollection is NULL!");
		}
	}

	if (success)
		SLog(L"* Scene loaded from file '%S'.", pFilename);

	return success;
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
		SLog(L"! SDK create error (%d - %s).", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}

	for (int i = 0; i < GAMEROOM_CAPACITY; i++)
	{
		LoadSceneFromFile("Jungle_fix.xml", NXU::FT_XML, i);
	}


	for (int i = 0; i < GAMEROOM_CAPACITY; i++)
	{
		SLog(L"* [%d]'s NbActors: %d ", i, scenes_[i]->getNbActors());
	}

//	m_pCCTManager = NxCreateControllerManager(m_pAllocator);


	SLog(L"# Initialize PhysX SDK.");
	return true;
}
