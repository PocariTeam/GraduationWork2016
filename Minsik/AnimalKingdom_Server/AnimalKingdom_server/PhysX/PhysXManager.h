#pragma once
#include "stdafx.h"

#include "NxPhysics.h"
#include "NXU_helper.h"
#include "CollisionReport.h"

class UserAllocator; 
class NxControllerManager;
class NxController;

typedef struct tagActorInfo
{
	XMFLOAT3	m_vGlobalPosition;
	DWORD		m_dwType;			// S : 1, B : 2, C : 3
	float		m_fRadius;
	float		m_fLength;
	float		m_fWidth;
	float		m_fHeight;
}ACTOR_INFO;

class PhysXManager : public Singleton<PhysXManager> {
private:
	const char*					fileName = "Jungle_player4.xml";
	UserAllocator*				userAllocator_;
	NxPhysicsSDK*				physicsSDK_;
	UINT32						sceneSeedNum;
	NxScene*					scenes_[GAMEROOM_CAPACITY];
	NxControllerManager*		CCTManager_[GAMEROOM_CAPACITY];
	// 엔티티와 컨트롤러는 도형충돌그룹, 충돌리포트는 액터그룹
	CEntityReport				entityReport_;		// P v P ( Sweep Collision )
	CControllerReport			controllerReport_;  // Controller Collision
	CCollisionReport			collisionReport_;	// Normal Collision

	

	Lock						lock_;

public:
	PhysXManager();
	~PhysXManager();
	BOOL Load_Kinematic( void );

	/* dsf */
	NxActor** CreateCharacterActors( S_CHARACTER eCharacterType, UINT iSceneNum, UINT& iActorCnt );
	NxActor*	CreateActor( const char* pActorName, const ACTOR_INFO& tActor_Info, UINT iSceneNum );

	BOOL initPhysX();
	BOOL LoadSceneFromFile(UINT32 roomNum);
	BOOL SetupScene(UINT32 roomNum);
	void ReleaseScene(UINT32 roomNum);
	void updateScene(UINT32 roomNum, float fTimeDelta);
	NxControllerManager* getCCTManager(UINT32 roomNum) { return CCTManager_[roomNum]; };

	void SetCollisionGroup(NxActor * pActor, NxCollisionGroup eGroup);
	NxController* CreateCharacterController(NxActor * actor, const NxVec3 & startPos, NxReal scale, UINT32 roomNum);
private:
	map<string, ACTOR_INFO>		m_mapActorInfo[ CHARACTER_MAX ];
};