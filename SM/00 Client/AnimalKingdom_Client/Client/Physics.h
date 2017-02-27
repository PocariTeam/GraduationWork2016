#pragma once

#ifndef Physics_h__
#define Physics_h__

#include "Singleton.h"
#include "NxPhysics.h"
#include "CollisionReport.h"
#include "NXU_helper.h"

class CShader;
class CMesh;
class CTexture;
class UserAllocator;
class NxControllerManager;
class NxController;
class CPhysics
	: public CSingleton<CPhysics>
{
private:
	UserAllocator*			m_pAllocator{ nullptr };
	NxPhysicsSDK*			m_pPhysicsSDK{ nullptr };
	NxScene*				m_pScene{ nullptr };
	NxControllerManager*	m_pCharacterControllerMgr{ nullptr };
	NxController*			m_pMyCharacterController{ nullptr };
public:
	static CEntityReport		m_EntityReport;		// P v P ( Sweep Collision )
	static CControllerReport	m_ControllerReport; // Controller Collision
	static CCollisionReport		m_CollisionReport;	// Normal Collision
public:
	DWORD		Release( void );
	int			Update( const float& fTimeDelta );
	void		Render( ID3D11DeviceContext* pContext );
	void		Release_Scene( void );
public:
	HRESULT		Initialize( ID3D11Device* pDevice );
	HRESULT		Load_Scene( ID3D11Device* pDevice, list<CShader*>* plistShader, const char* pFileName, NXU::NXU_FileType eType = NXU::FT_XML );
private:
	HRESULT		CreateSceneFromFile( const char* pFilePath, NXU::NXU_FileType eType );
	HRESULT		SetupScene( ID3D11Device* pDevice, list<CShader*>* plistShader );
	NxController* CreateCharacterController( NxActor* pActor, const NxVec3& vPos, NxReal fScale );
	void		  SetCollisionGroup( NxActor* pActor, NxCollisionGroup eGroup );
	//HRESULT CreateScene( ID3D11Device* pDevice );
	//NxActor* CreateCube( const NxVec3& pos, int size, const NxReal density = 0.0f );
	//NxActor* CreateCapsule( const NxVec3 & pos, const NxReal height, const NxReal radius, const NxReal density = 0.0f );
	//NxActor* CreateSphere( const NxVec3 & pos, const NxReal radius, const NxReal density = 0.0f );
};

#endif // Physics_h__
