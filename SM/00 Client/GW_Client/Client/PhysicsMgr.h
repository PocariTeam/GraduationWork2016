#pragma once

#ifndef _PHYSICSMGR_H_
#define _PHYSICSMGR_H_

#include "NxPhysics.h"
#include "NXU_helper.h"

class CGameObject;

class CPhysicsMgr {
	explicit CPhysicsMgr( void );
	~CPhysicsMgr( void ) = default;
private:
	static CPhysicsMgr*	m_pInstance;
public:
	static CPhysicsMgr*	GetInstance( void );
	static void			DestroyInstance( void );
public:
	DWORD  Get_ActorCnt() { return m_dwActorCnt; }
	void   Connect_Actors( vector<CGameObject*>* pvecGameObject );
private:
	NxPhysicsSDK*	m_pPhysicsSDK;
	NxScene*		m_pScene;
	NxActor**		m_pArrActors;
	DWORD			m_dwActorCnt;
	ID3D11Buffer*	m_pCBmtxWorld;
private:
	HRESULT			CreateContantBuffer( ID3D11Device * pDevice );
	void			SetContantBuffer( ID3D11DeviceContext* pContext, NxF32* pMtxWorld );
public:
	HRESULT Initialize();
	HRESULT LoadScene(const char * pFilename, NXU::NXU_FileType type);
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