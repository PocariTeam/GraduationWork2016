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

}

PhysXManager::~PhysXManager()
{
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

/*
	NxSceneDesc sceneDesc;
	sceneDesc.groundPlane = true;
	sceneDesc.gravity = NxVec3(0, -9.81f, 0);
	sceneDesc.simType = NX_SIMULATION_HW;
	m_pScene = m_pPhysicsSDK->createScene(sceneDesc);

	m_pCCTManager = NxCreateControllerManager(m_pAllocator);
*/

	SLog(L"# Initialize PhysX SDK.");
	return true;
}
