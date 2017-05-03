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
	const char*					fileName = "Jungle.xml";
	UserAllocator*				userAllocator_;
	NxPhysicsSDK*				physicsSDK_;
	UINT						sceneSeedNum;
	NxScene*					scenes_[GAMEROOM_CAPACITY];
	NxControllerManager*		CCTManager_[GAMEROOM_CAPACITY];
	// 엔티티와 컨트롤러는 도형충돌그룹, 충돌리포트는 액터그룹
public:
	CEntityReport				entityReport_;		// P v P ( Sweep Collision )
	CControllerReport			controllerReport_;  // Controller Collision
	CCollisionReport			collisionReport_;	// Normal Collision
private:
	map<string, ACTOR_INFO>		m_mapActorInfo[CHARACTER_MAX];

	Lock						lock_;

public:
	PhysXManager();
	~PhysXManager();

	BOOL					Load_Kinematic( void );

	/* dsf */
	NxActor**				CreateCharacterActors( COL_GROUP eColgroup, CHARACTER eCharacterType, UINT iSceneNum, UINT& iActorCnt );
	NxActor*				CreateActor( COL_GROUP eColgroup, const char* pActorName, const ACTOR_INFO& tActor_Info, UINT iSceneNum );

	BOOL					initPhysX();
	BOOL					LoadSceneFromFile(UINT roomNum);
	BOOL					SetupScene(UINT roomNum);
	void					ReleaseScene(UINT roomNum);
	void					updateScene(UINT roomNum, float fTimeDelta);
	NxControllerManager*	getCCTManager(UINT roomNum) { return CCTManager_[roomNum]; };
	void					updateCCT(UINT roomNum);

	void					SetCollisionGroup(NxActor * pActor, NxCollisionGroup eGroup);
	NxController*			CreateCharacterController(NxActor * actor, const NxVec3 & startPos, NxReal scale, UINT roomNum);

	S_SyncDynamic*			getDynamicInfo(UINT roomNum);
	S_SyncDynamicOne*		getDynamicOneInfo(NxActor *actor);

};