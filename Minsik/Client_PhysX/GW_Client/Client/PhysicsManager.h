#pragma once

#include "NxPhysics.h"
#include "ErrorStream.h"

class CPhysicsManager {
private:
	NxPhysicsSDK*	gPhysicsSDK = NULL;
	NxScene*			gScene = NULL;

public:
	bool InitNx();
	void Update(const float& fTimeDelta);
	void ExitNx();
};