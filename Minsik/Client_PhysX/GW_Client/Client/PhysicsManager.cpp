#include "stdafx.h"
#include "PhysicsManager.h"
#include "Utilities.h"

bool CPhysicsManager::InitNx()
{
	// Initialize PhysicsSDK
	NxPhysicsSDKDesc sdkDesc;
	sdkDesc.flags ^= NX_SDKF_NO_HARDWARE;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	m_PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, sdkDesc, &errorCode);
	if (m_PhysicsSDK == NULL)
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}

	m_PhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3(0.0f, -9.81f, 0.0f);
	if (m_PhysicsSDK->getHWVersion() != NX_HW_VERSION_NONE) {
		sceneDesc.simType = NX_SIMULATION_HW;
	}
	m_Scene = m_PhysicsSDK->createScene(sceneDesc);

	if (m_Scene == NULL)
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

	// Set default material
	NxMaterial* defaultMaterial = m_Scene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.5f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	m_Scene->createActor(actorDesc);

	return true;
}

void CPhysicsManager::Update(const float & fTimeDelta)
{
	if (m_Scene == NULL)
		return;

	m_Scene->simulate(fTimeDelta);
	m_Scene->flushStream();
	m_Scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void CPhysicsManager::ExitNx()
{
	if (m_PhysicsSDK != NULL)
	{
		if (m_Scene != NULL) m_PhysicsSDK->releaseScene(*m_Scene);
		m_Scene = NULL;
		NxReleasePhysicsSDK(m_PhysicsSDK);
		m_PhysicsSDK = NULL;
	}

}

void CPhysicsManager::CreateCube(const NxVec3& pos, int size, const NxVec3* initialVelocity)
{
	if (m_Scene == NULL) return;

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping = 0.5f;
	if (initialVelocity) bodyDesc.linearVelocity = *initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3((float)size*0.5f, (float)size*0.5f, (float)size*0.5f);
	boxDesc.localPose.t = NxVec3((float)0.0f, (float)size*0.5f, (float)size*0.0f);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = 10.0f;
	actorDesc.globalPose.t = pos;
	m_Scene->createActor(actorDesc)->userData = (void*)size_t(size);

}