#include "stdafx.h"
#include "PhysicsManager.h"
#include "Utilities.h"

bool CPhysicsManager::InitNx()
{
	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, desc, &errorCode);
	if (gPhysicsSDK == NULL)
	{
		MessageBox(0, "SDK create error!", nullptr, MB_OK);
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(0.0f, -9.81f, 0.0f);
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (gScene == NULL)
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

	// Set default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);

	return true;
}

void CPhysicsManager::Update(const float & fTimeDelta)
{
	if (gScene == NULL)
		return;

	gScene->simulate(fTimeDelta);
}

void CPhysicsManager::ExitNx()
{
	if (gPhysicsSDK != NULL)
	{
		if (gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

}