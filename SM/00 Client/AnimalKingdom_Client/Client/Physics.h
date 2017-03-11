#pragma once

#ifndef Physics_h__
#define Physics_h__

#include "Singleton.h"
#include "NxPhysics.h"
#include "CollisionReport.h"
#include "NXU_helper.h"
#include "Enum.h"
#include "Struct.h"

#define GRAVITY -9.8f

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
public:
	static NxControllerManager*	m_pCharacterControllerMgr;
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
	HRESULT		Load_Kinematic( void );
	HRESULT		Load_Scene( ID3D11Device* pDevice, list<CShader*>* plistShader, const char* pFileName, NXU::NXU_FileType eType = NXU::FT_XML );
private:
	HRESULT		CreateSceneFromFile( const char* pFilePath, NXU::NXU_FileType eType );
	HRESULT		SetupScene( ID3D11Device* pDevice, list<CShader*>* plistShader );
	NxController* CreateCharacterController( NxActor* pActor, NxActor** dpActors, int iArraySize );
	void		  SetCollisionGroup( NxActor* pActor, NxCollisionGroup eGroup );
	NxActor*	CreateActor( const char* pActorName, const ACTOR_INFO& tActor_Info );
private:
	map<string, ACTOR_INFO>		m_mapActorInfo[ CHARACTER_END ];
};

#endif // Physics_h__
