#pragma once
#include "stdafx.h"

#include "NxPhysics.h"
#include "NXU_helper.h"
#include "CollisionReport.h"

class UserAllocator; 
class NxControllerManager;
class NxController;

class PhysXManager : public Singleton<PhysXManager> {
private:
	const char*					fileName = "Jungle_player4.xml";
	UserAllocator*				userAllocator_;
	NxPhysicsSDK*				physicsSDK_;
	UINT32						sceneSeedNum;
	NxScene*					scenes_[GAMEROOM_CAPACITY];
	NxControllerManager*		CCTManager_[GAMEROOM_CAPACITY];
	// ��ƼƼ�� ��Ʈ�ѷ��� �����浹�׷�, �浹����Ʈ�� ���ͱ׷�
	CEntityReport				entityReport_;		// P v P ( Sweep Collision )
	CControllerReport			controllerReport_;  // Controller Collision
	CCollisionReport			collisionReport_;	// Normal Collision

	Lock						lock_;

public:
	PhysXManager();
	~PhysXManager();
	BOOL initPhysX();
	BOOL LoadSceneFromFile(UINT32 roomNum);
	BOOL SetupScene(UINT32 roomNum);
	void ReleaseScene(UINT32 roomNum);
	void updateScene(UINT32 roomNum, float fTimeDelta);
	NxControllerManager* getCCTManager(UINT32 roomNum) { return CCTManager_[roomNum]; };

	void SetCollisionGroup(NxActor * pActor, NxCollisionGroup eGroup);
	NxController* CreateCharacterController(NxActor * actor, const NxVec3 & startPos, NxReal scale, UINT32 roomNum);
};