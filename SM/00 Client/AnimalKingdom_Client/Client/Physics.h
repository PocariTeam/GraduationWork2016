#pragma once

#ifndef Physics_h__
#define Physics_h__

#include "Singleton.h"
#include "NxPhysics.h"
#include "CollisionReport.h"
#include "NXU_helper.h"
#include "Enum.h"
#include "Struct.h"
#include "Banana.h"

#define GRAVITY -9.81f
#define BANANA_COUNT 20

class CShader;
class CMesh;
class CTexture;
class UserAllocator;
class NxControllerManager;
class NxController;
class CPlayer;
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

	void		UpdateCharactercontrollerMgr();
	void		UpdateDynamicActors(S_SyncDynamic *packet);
	void		UpdateDynamicOneActor(S_SyncDynamicOne *packet);
public:
	HRESULT		Initialize( ID3D11Device* pDevice );
	HRESULT		Load_Kinematic( void );
	HRESULT		Load_Scene( ID3D11Device* pDevice, list<CShader*>* plistShader, map<int, CPlayer*>* pmapPlayer, const char* pFileName, NXU::NXU_FileType eType = NXU::FT_XML );
	NxScene*	GetScene() { return m_pScene; }
	NxPhysicsSDK*	GetSDK() { return m_pPhysicsSDK; }
private:
	HRESULT		CreateSceneFromFile( const char* pFilePath, NXU::NXU_FileType eType );
	HRESULT		SetupScene( ID3D11Device* pDevice, list<CShader*>* plistShader, map<int, CPlayer*>* pmapPlayer );
	NxController* CreateCharacterController( NxActor* pActor, NxActor** dpActors, int iArraySize );
	void		SetCollisionGroup( NxActor* pActor, NxCollisionGroup eGroup );
	void		CreateMeshFromShape(NxSimpleTriangleMesh &triMesh, NxShape *shape);
public:
	NxActor*	CreateActor( const char* pActorName, const ACTOR_INFO& tActor_Info, COL_GROUP eColGroup );
	void		CreateBanana( void );
	void		ThrowBanana( NxVec3& vPos, NxVec3& vDir, COL_GROUP eColGroup );
private:
	map<string, ACTOR_INFO>		m_mapActorInfo[ CHARACTER_END ];
	list<CShader*>*				m_pShaderlist{ nullptr };
	queue<CBanana*>				m_BananaQueue;
};

#endif // Physics_h__
