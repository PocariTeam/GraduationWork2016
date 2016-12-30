#pragma once

#include "NxPhysics.h"
#include "ErrorStream.h"

class CPhysicsManager {
public:
	NxPhysicsSDK*	m_PhysicsSDK;
	NxScene*		m_Scene;

public:
	bool InitNx();
	void Update(const float& fTimeDelta);
	void ExitNx();
	void CreateCube(const NxVec3& pos, int size = 2, const NxVec3 * initialVelocity = NULL);
};