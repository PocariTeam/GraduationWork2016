#pragma once

#ifndef _PHYSICSMGR_H_
#define _PHYSICSMGR_H_

#include "NxPhysics.h"
#include "NXU_helper.h"

class CGameObject;
class UserAllocator;
class NxControllerManager;
class NxController;

#define MAX_SCENE 5

class CPhysicsMgr {
	explicit CPhysicsMgr( void );
	~CPhysicsMgr( void ) = default;
private:
	static CPhysicsMgr*	m_pInstance;
public:
	static CPhysicsMgr*	GetInstance( void );
	static void			DestroyInstance( void );
public:
	DWORD  Get_ActorCnt() { return m_pScene[m_nCurrentScene]->getNbActors(); }
	void   Connect_Actors( vector<CGameObject*>* pvecGameObject );
private:
	UserAllocator*			m_pAllocator;
	NxPhysicsSDK*			m_pPhysicsSDK;
	int						m_nCurrentScene;
	NxScene*				m_pScene[MAX_SCENE];
	ID3D11Buffer*			m_pCBmtxWorld;
	NxControllerManager*	m_pCCTManager;
	NxController*			m_pMyCCT[MAX_SCENE];
private:
	HRESULT			CreateContantBuffer( ID3D11Device * pDevice );
	void			SetContantBuffer( ID3D11DeviceContext* pContext, NxF32* pMtxWorld );
public:
	HRESULT Initialize();
	HRESULT LoadSceneFromFile(const char * pFilename, NXU::NXU_FileType type, UINT SceneNum);
	NxController* CreateCharacterController(NxActor* actor, const NxVec3& startPos, NxReal scale);
	void SetShapesCollisionGroup(NxActor * actor, NxCollisionGroup group);
	HRESULT SetupScene(UINT SceneNum);
	HRESULT CreateScene( ID3D11Device* pDevice );
	void Update( const float& fTimeDelta );
	void Render( ID3D11DeviceContext* pContext );
	void Release_Scene();
	void Release();
	NxActor* CreateCube(UINT SceneNum, const NxVec3& pos, int size, const NxReal density=0.0f);
	NxActor* CreateCapsule(UINT SceneNum, const NxVec3 & pos, const NxReal height, const NxReal radius, const NxReal density = 0.0f);
	NxActor* CreateSphere(UINT SceneNum, const NxVec3 & pos, const NxReal radius, const NxReal density = 0.0f);
	NxScene* getCurrentScene() { return m_pScene[m_nCurrentScene]; };
};

#endif