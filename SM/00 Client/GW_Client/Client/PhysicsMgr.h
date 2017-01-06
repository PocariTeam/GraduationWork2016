#pragma once

#ifndef _PHYSICSMGR_H_
#define _PHYSICSMGR_H_

#include "NxPhysics.h"
#include "NXU_helper.h"

enum GameGroup
{
	GROUP_NON_COLLIDABLE,
	GROUP_COLLIDABLE_NON_PUSHABLE,
	GROUP_COLLIDABLE_PUSHABLE,
};

#define COLLIDABLE_MASK	(1<<GROUP_COLLIDABLE_NON_PUSHABLE) | (1<<GROUP_COLLIDABLE_PUSHABLE)

class CGameObject;
class UserAllocator;
class NxControllerManager;
class NxController;

class CPhysicsMgr {
	explicit CPhysicsMgr( void );
	~CPhysicsMgr( void ) = default;
private:
	static CPhysicsMgr*	m_pInstance;
public:
	static CPhysicsMgr*	GetInstance( void );
	static void			DestroyInstance( void );
public:
	DWORD  Get_ActorCnt() { return m_pScene->getNbActors(); }
	void   Connect_Actors( vector<CGameObject*>* pvecGameObject );
private:
	UserAllocator*			m_pAllocator;
	NxPhysicsSDK*			m_pPhysicsSDK;
	NxScene*				m_pScene;
	ID3D11Buffer*			m_pCBmtxWorld;
	NxControllerManager*	m_pCCTManager;
	NxController*			m_pMyCCT;
private:
	HRESULT			CreateContantBuffer( ID3D11Device * pDevice );
	void			SetContantBuffer( ID3D11DeviceContext* pContext, NxF32* pMtxWorld );
public:
	HRESULT Initialize();
	HRESULT LoadSceneFromFile(const char * pFilename, NXU::NXU_FileType type);
	NxController* CreateCharacterController(NxActor* actor, const NxVec3& startPos, NxReal scale);
	HRESULT SetScene();
	HRESULT CreateScene( ID3D11Device* pDevice );
	void Update( const float& fTimeDelta );
	void Render( ID3D11DeviceContext* pContext );
	void Release_Scene();
	void Release();
	void CreateCube(const NxVec3& pos, int size, const NxReal density);
	void CreateCapsule(const NxVec3 & pos, const NxReal height, const NxReal radius, const NxReal density);
	void CreateSphere(const NxVec3 & pos, const NxReal radius, const NxReal density);
};

#endif