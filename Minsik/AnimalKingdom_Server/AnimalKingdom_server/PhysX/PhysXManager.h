#pragma once
#include "stdafx.h"

#include "NxPhysics.h"
#include "NXU_helper.h"

class UserAllocator;

class PhysXManager : public Singleton<PhysXManager> {
private:
	UserAllocator*			userAllocator_;
	NxPhysicsSDK*			physicsSDK_;
	NxScene*				scenes_[GAMEROOM_CAPACITY];
public:
	PhysXManager();
	~PhysXManager();
	BOOL LoadSceneFromFile(const char * pFilename, NXU::NXU_FileType type, UINT32 roomNum);
	BOOL initPhysX();
};