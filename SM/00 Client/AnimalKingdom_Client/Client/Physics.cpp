#include "stdafx.h"
#include "Physics.h"
#include "NxControllerManager.h"
#include "UserAllocator.h"
#include "Cooking.h"
#include "Utilities.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "Enum.h"
#include "ShaderMgr.h"
#include "MeshMgr.h"
#include "TextureMgr.h"
#include "Terrain.h"
#include "AnimationMgr.h"
#include "Player.h"
#include "Wallpaper.h"
#include "Renderer.h"
#include "Mesh.h"
#include "RenderState.h"
#include "Environment.h"
#include "StateMachine.h"
#include <NxController.h>
#include <NxScene.h>
#include <fstream>
#include "Skydome.h"
#include <set>
#include <xfunctional>
#include "NetworkMgr.h"
#include "NormalShader.h"
#include "Banana.h"
#include "Normal_UI.h"
#include "LightMgr.h"

CPhysics*	CSingleton<CPhysics>::m_pInstance;

CEntityReport			CPhysics::m_EntityReport;
CControllerReport		CPhysics::m_ControllerReport;
CCollisionReport		CPhysics::m_CollisionReport;
NxControllerManager*	CPhysics::m_pCharacterControllerMgr;

DWORD CPhysics::Release( void )
{
	Release_Scene();

	NXU::releasePersistentMemory();

	if( nullptr != m_pPhysicsSDK )
	{
		m_pPhysicsSDK->release();
		m_pPhysicsSDK = nullptr;
	}

	NxReleaseControllerManager( m_pCharacterControllerMgr );
	m_pCharacterControllerMgr = nullptr;

	if( nullptr != m_pAllocator )
	{
		CloseCooking();
		delete m_pAllocator;
		m_pAllocator = nullptr;
	}

	for( int i = 0; i < ( int )CHARACTER_END; ++i )
		m_mapActorInfo[ i ].erase( m_mapActorInfo[ i ].begin(), m_mapActorInfo[ i ].end() );

	delete this;

	return 0;
}

int CPhysics::Update( const float& fTimeDelta )
{
	m_pCharacterControllerMgr->updateControllers();
	m_pScene->simulate( fTimeDelta );
	m_pScene->flushStream();
	m_pScene->fetchResults( NX_RIGID_BODY_FINISHED, true );

	return 0;
}

void CPhysics::Render( ID3D11DeviceContext* pContext )
{
	// 천 그리기
	if( m_pCloth != nullptr )
	{
		m_pCloth->draw( false );
	}
	///////////////

	NxActor** dpActor = m_pScene->getActors();

	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	CMesh* pBox = CMeshMgr::GetInstance()->Clone( "Mesh_Box" );
	CMesh* pSphere = CMeshMgr::GetInstance()->Clone( "Mesh_Sphere" );
	CMesh* pCapsule = CMeshMgr::GetInstance()->Clone( "Mesh_Capsule" );
	NxF32* mtxWorld = new NxF32[ 16 ];
	NxVec3	vScale{};
	NxF32	fRadius{}, fHeight{};
	NxMat34 mtxRealWorld{}, mtxScale{};
	mtxRealWorld.zero();
	mtxScale.zero();
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_ActorShape" );
	pShader->Render( pContext );
	DWORD nbActors = m_pScene->getNbActors();
	for( DWORD i = 0; i < nbActors; ++i, ++dpActor )
	{
		DWORD dwShapeCnt = ( *dpActor )->getNbShapes();
		NxShape* const * dpShape = ( *dpActor )->getShapes();

		for( DWORD j = 0; j < dwShapeCnt; ++j, ++dpShape )
		{
			switch( ( *dpShape )->getType() )
			{
			case NX_SHAPE_BOX:
				vScale = ( ( NxBoxShape* )( *dpShape ) )->getDimensions() * 2.f;
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( vScale );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				pShader->SetConstantBuffer( pContext, XMFLOAT4X4( mtxWorld ) );
				pBox->Render( pContext );
				break;
			case NX_SHAPE_SPHERE:
				fRadius = ( ( NxSphereShape* )( *dpShape ) )->getRadius() * 2.f;
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( NxVec3( fRadius ) );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				pShader->SetConstantBuffer( pContext, XMFLOAT4X4( mtxWorld ) );
				pSphere->Render( pContext );
				break;
			case NX_SHAPE_CAPSULE:
				//fRadius = ( ( NxCapsuleShape* )( *dpShape ) )->getRadius() * 2.f;
				//fHeight = ( ( NxCapsuleShape* )( *dpShape ) )->getHeight();
				//mtxRealWorld = ( *dpShape )->getGlobalPose();
				//mtxScale.M.diagonal( NxVec3( fRadius, ( fHeight + fRadius ) * 0.5f, fRadius ) );
				//mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				//mtxRealWorld.getRowMajor44( mtxWorld );
				//// mtxWorld[ 7 ] -= ( fRadius + fHeight ) * 0.5f;
				//pShader->SetConstantBuffer( pContext, XMFLOAT4X4( mtxWorld ) );
				//pCapsule->Render( pContext );
				break;
			}
		}
	}

	pBox->Release();
	pSphere->Release();
	pCapsule->Release();
	pShader->Release();

	delete[] mtxWorld;
}

void CPhysics::Release_Scene( void )
{
	if( m_pCloth != nullptr )
	{
		delete m_pCloth;
		m_pCloth = nullptr;
	}

	while( !m_BananaQueue.empty() )
		m_BananaQueue.pop();

	if( nullptr != m_pScene )
	{
		m_pCharacterControllerMgr->purgeControllers();
		m_pScene->shutdownWorkerThreads();
		m_pPhysicsSDK->releaseScene( *m_pScene );
		m_pScene = nullptr;
	}
}

void CPhysics::UpdateCharactercontrollerMgr()
{
	m_pCharacterControllerMgr->updateControllers();
}

void CPhysics::UpdateDynamicActors( S_SyncDynamic *packet )
{
	if( NULL == m_pScene )
		return;

	NxActor** ppActors = m_pScene->getActors();
	unsigned int nbActors = m_pScene->getNbActors();

	// printf("총 액터수: %d \n", nbActors);

	for( unsigned int i = 0; i < packet->dynamicActorCount; ++i )
	{
		unsigned int j = packet->dynamicActors[ i ].index;

		// 에러체크용: 후에 삭제
		if( j >= nbActors )
		{
			printf( " 받은 동적 객체의 인덱스가 총 액터수보다 많습니다!! \n" );
			break;
		}

		// printf("[index:%d] %s \n", j, ppActors[j]->getName());

		Vector3 p = packet->dynamicActors[ i ].position;
		Vector4 o = packet->dynamicActors[ i ].orient;

		ppActors[ j ]->setGlobalPosition( NxVec3( p.x, p.y, p.z ) );
		ppActors[ j ]->setGlobalOrientationQuat( NxQuat( NxVec3( o.x, o.y, o.z ), o.w ) );

		Vector3 l = packet->dynamicActors[ i ].linear;
		Vector3 a = packet->dynamicActors[ i ].angular;
		ppActors[ j ]->setLinearVelocity( NxVec3( l.x, l.y, l.z ) );
		ppActors[ j ]->setAngularVelocity( NxVec3( a.x, a.y, a.z ) );

	}
}

void CPhysics::UpdateDynamicOneActor( S_SyncDynamicOne * packet )
{
	if( NULL == m_pScene )
		return;

	UINT	i = packet->dynamicActor.index;
	Vector3 p = packet->dynamicActor.position;
	Vector4 o = packet->dynamicActor.orient;

	NxActor* pActors = m_pScene->getActors()[ i ];

	pActors->setGlobalPosition( NxVec3( p.x, p.y, p.z ) );
	pActors->setGlobalOrientationQuat( NxQuat( NxVec3( o.x, o.y, o.z ), o.w ) );

	Vector3 l = packet->dynamicActor.linear;
	Vector3 a = packet->dynamicActor.angular;
	pActors->setLinearVelocity( NxVec3( l.x, l.y, l.z ) );
	pActors->setAngularVelocity( NxVec3( a.x, a.y, a.z ) );

}

HRESULT CPhysics::Initialize( ID3D11Device* pDevice )
{
	m_pCloth = nullptr;

	m_pAllocator = new UserAllocator;
	bool bResult = InitCooking( m_pAllocator );

	if( !bResult )
	{
#ifdef _DEBUG
		printf( "Physics InitCooking Failed\n" );
#endif
		return E_FAIL;
	}

	NxPhysicsSDKDesc	tSDKDesc;
	tSDKDesc.flags ^= NX_SDKF_NO_HARDWARE;

	NxSDKCreateError	eErrorCode = NXCE_NO_ERROR;
	m_pPhysicsSDK = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, m_pAllocator, NULL, tSDKDesc, &eErrorCode );

	if( nullptr == m_pPhysicsSDK )
	{
#ifdef _DEBUG
		printf( "\nPhysics SDK Create Failed (%d - %s)\n", eErrorCode, getNxSDKCreateError( eErrorCode ) );
#endif
		return E_FAIL;
	}

	m_pCharacterControllerMgr = NxCreateControllerManager( m_pAllocator );

	Load_Kinematic();

	return S_OK;
}

HRESULT CPhysics::Load_Kinematic( void )
{
	ifstream	In{ "../Executable/Resources/Actor/KinematicActor.txt" };
	int			iCharacterType{}, iActorCnt{};
	char		szJointName[ MAX_PATH ]{ "" };
	ACTOR_INFO	tActorInfo;

	while( !In.eof() )
	{
		In >> iCharacterType;
		In >> iActorCnt;

		for( int i = 0; i < iActorCnt; ++i )
		{
			In >> szJointName;
			In >> tActorInfo.m_dwType;
			In >> tActorInfo.m_vGlobalPosition.x; In >> tActorInfo.m_vGlobalPosition.y; In >> tActorInfo.m_vGlobalPosition.z;
			In >> tActorInfo.m_fRadius; In >> tActorInfo.m_fLength; In >> tActorInfo.m_fWidth; In >> tActorInfo.m_fHeight;

			m_mapActorInfo[ iCharacterType ].insert( make_pair( szJointName, tActorInfo ) );
		}
	}

	In.close();

	return S_OK;
}

HRESULT CPhysics::Load_Scene( ID3D11Device* pDevice, list<CShader*>* plistShader, map<int, CPlayer*>* pmapPlayer, const char* pFileName, NXU::NXU_FileType eType/* = NXU::FT_XML*/ )
{
	if( FAILED( CreateSceneFromFile( pFileName, eType ) ) )
	{
#ifdef _DEBUG
		printf( "Physics Load_Scene Failed\n" );
#endif
		return E_FAIL;
	}

	return SetupScene( pDevice, plistShader, pmapPlayer );
}

HRESULT CPhysics::CreateSceneFromFile( const char* pFilePath, NXU::NXU_FileType eType )
{
	if( m_pPhysicsSDK )
	{
		NXU::NxuPhysicsCollection* pCollection = NXU::loadCollection( pFilePath, eType );

		if( nullptr != pCollection )
		{
			if( m_pScene )
			{
				m_pPhysicsSDK->releaseScene( *m_pScene );
				m_pScene = nullptr;
			}

			NXU::instantiateCollection( pCollection, *m_pPhysicsSDK, 0, 0, 0 );
			NXU::releaseCollection( pCollection );

			m_pScene = m_pPhysicsSDK->getScene( 0 );
			return S_OK;
		}

		else
			return E_FAIL;
	}

	return E_FAIL;
}

HRESULT CPhysics::SetupScene( ID3D11Device* pDevice, list<CShader*>* plistShader, map<int, CPlayer*>* pmapPlayer )
{
	m_pShaderlist = plistShader;

	m_pPhysicsSDK->setParameter( NX_SKIN_WIDTH, 2.5f );
	m_pPhysicsSDK->setParameter( NX_CONTINUOUS_CD, true );
	m_pPhysicsSDK->setParameter( NX_CCD_EPSILON, 0.1f );
	m_pPhysicsSDK->setParameter( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 15 * 15 );
	m_pPhysicsSDK->setParameter( NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 14 * 14 );
	m_pPhysicsSDK->setParameter( NX_BOUNCE_THRESHOLD, -5 );
	m_pPhysicsSDK->setParameter( NX_DYN_FRICT_SCALING, 10 );
	m_pPhysicsSDK->setParameter( NX_STA_FRICT_SCALING, 10 );

	m_pScene->setGravity( NxVec3( 0.0f, -9.81f * 3, 0.0f ) );
	m_pScene->setUserContactReport( &m_CollisionReport );
	m_pScene->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER1, NX_NOTIFY_ON_START_TOUCH );
	m_pScene->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER2, NX_NOTIFY_ON_START_TOUCH );
	m_pScene->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER3, NX_NOTIFY_ON_START_TOUCH );
	m_pScene->setActorGroupPairFlags( COL_DYNAMIC, COL_PLAYER4, NX_NOTIFY_ON_START_TOUCH );
	m_pScene->setActorGroupPairFlags( COL_DYNAMIC, COL_DYNAMIC, NX_NOTIFY_ON_START_TOUCH );
	m_pScene->setActorGroupPairFlags( COL_DYNAMIC, COL_STATIC, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH );

	m_pScene->setGroupCollisionFlag( COL_PLAYER1, COL_DYNAMIC, false );
	m_pScene->setGroupCollisionFlag( COL_PLAYER2, COL_DYNAMIC, false );
	m_pScene->setGroupCollisionFlag( COL_PLAYER3, COL_DYNAMIC, false );
	m_pScene->setGroupCollisionFlag( COL_PLAYER4, COL_DYNAMIC, false );

	// Create the default material
	NxMaterial* pDefaultMaterial = m_pScene->getMaterialFromIndex( 0 );
	pDefaultMaterial->setRestitution( 0.0 );
	pDefaultMaterial->setStaticFriction( 0.5 );
	pDefaultMaterial->setDynamicFriction( 0.5 );

	m_pScene->simulate( 0.003f );
	m_pScene->flushStream();
	m_pScene->fetchResults( NX_RIGID_BODY_FINISHED, true );

#ifdef _DEBUG
	if( m_pScene->getSimType() == NX_SIMULATION_HW )
		printf( "Hardware Simulate\n" );
	else
		printf( "Software Simulate\n" );
#endif

	NxU32		iActorCnt = m_pScene->getNbActors();
	NxActor**	dpActorArray = m_pScene->getActors();

	PlayerInfo	pPlayerInfo[ PLAYER_CAPACITY ];
	UINT		iPlayerCnt{};
	UINT		iCreatePlayerCnt{};

	CNetworkMgr::GetInstance()->getPlayerInfo( pPlayerInfo, iPlayerCnt );

	CShader*	pShader_Mesh, *pShader_Light, *pShader_Blend, *pShader_Debug, *pShader_Animate, *pShader_Skydome, *pShader_Cave, *pShader_Mesh_Alpha;

	pShader_Skydome = CShaderMgr::GetInstance()->Clone( "Shader_Skydome" );
	pShader_Mesh = CShaderMgr::GetInstance()->Clone( "Shader_Mesh" );
	pShader_Cave = CShaderMgr::GetInstance()->Clone( "Shader_Mesh" );
	pShader_Light = CShaderMgr::GetInstance()->Clone( "Shader_Light" );
	pShader_Blend = CShaderMgr::GetInstance()->Clone( "Shader_Blend" );
	pShader_Debug = CShaderMgr::GetInstance()->Clone( "Shader_Debug" );
	pShader_Animate = CShaderMgr::GetInstance()->Clone( "Shader_AnimateMesh" );
	pShader_Mesh_Alpha = CShaderMgr::GetInstance()->Clone( "Shader_Mesh_Alpha" );

	CMesh* pSkydome_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Skydome" );
	CTexture* pTexture_Skydome = CTextureMgr::GetInstance()->Clone( "Texture_Skydome" );

	CGameObject* pSkydome = CSkydome::Create( pSkydome_Mesh, pTexture_Skydome
		, CLightMgr::GetInstance()->Add( pDevice, CLightMgr::LIGHT_DIRECTIONAL, XMFLOAT4( 0.4f, 0.4f, 0.4f, 1.f ), XMFLOAT4( 0.f, -0.3f, 0.7f, 0.f ) ) );

	CMesh* pLight_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	CGameObject* pLight_Screen = CWallpaper::Create( pDevice, pLight_Mesh );

	CMesh* pBlend_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	CGameObject* pBlend_Screen = CWallpaper::Create( pDevice, pBlend_Mesh );

	pShader_Skydome->Add_RenderObject( pSkydome );
	pShader_Light->Add_RenderObject( pLight_Screen );
	pShader_Blend->Add_RenderObject( pBlend_Screen );

	set<int, less<int> >	setReleaseActorIndex;

	if( nullptr != m_pScene )
	{
		for( NxU32 i = 0; i < iActorCnt; i++ )
		{
			NxActor* pActor = dpActorArray[ i ];
			NxVec3  vPos = pActor->getGlobalPosition();
			NxU32   iActorShapeCnt = pActor->getNbShapes();
#ifdef _DEBUG
			printf( "Actor %s's Global Position : ( %f, %f, %f )\n", pActor->getName(), vPos.x, vPos.y, vPos.z );
#endif
			// Collision Grouping
			if( pActor->isDynamic() )
			{
				if( 0 == strcmp( pActor->getName(), "Player1" )
					|| 0 == strcmp( pActor->getName(), "Player2" )
					|| 0 == strcmp( pActor->getName(), "Player3" )
					|| 0 == strcmp( pActor->getName(), "Player4" ) )
				{
					if( iPlayerCnt == iCreatePlayerCnt )
					{
						setReleaseActorIndex.insert( i );
						continue;
					}

					CHARACTER eType = pPlayerInfo[ iCreatePlayerCnt ].character;
					DWORD dwCharacterActorCnt = ( DWORD )m_mapActorInfo[ eType ].size();
					NxActor** dpCharacterActors = new NxActor*[ dwCharacterActorCnt ];
					NxMat34*  pActorOriginPose = new NxMat34[ dwCharacterActorCnt ];

					auto iter_begin = m_mapActorInfo[ eType ].begin();
					auto iter_end = m_mapActorInfo[ eType ].end();

					setReleaseActorIndex.insert( i );

					int j = 0;
					for( ; iter_begin != iter_end; ++j, ++iter_begin )
					{
						dpCharacterActors[ j ] = CreateActor( ( *iter_begin ).first.c_str(), ( *iter_begin ).second
							, COL_GROUP( COL_PLAYER1 << iCreatePlayerCnt ) );
						dpCharacterActors[ j ]->raiseBodyFlag( NX_BF_KINEMATIC );
						pActorOriginPose[ j ] = dpCharacterActors[ j ]->getGlobalPose();
						// Collision Grouping 은 CreateActor 함수 안에서 현재 하고 있음 ( 추후 변경도 가능 )
					}
					dpActorArray = m_pScene->getActors();

					NxController* pController = CreateCharacterController( pActor, dpCharacterActors, j );
					pController->getActor()->setGroup( COL_GROUP( COL_PLAYER1 << iCreatePlayerCnt ) );
					CGameObject* pPlayer = CPlayer::Create( pDevice, pController, pActorOriginPose, eType );
					for( int k = 0; k < j; ++k ) dpCharacterActors[ k ]->userData = pPlayer;
					pShader_Animate->Add_RenderObject( pPlayer );
					pmapPlayer->insert( make_pair( ( int )pPlayerInfo[ iCreatePlayerCnt ].id, ( CPlayer* )pPlayer ) );
					iCreatePlayerCnt++;
				}

				else if( 0 == strcmp( pActor->getName(), "Crown" ) )
				{
					m_pCrown = CCrown::Create( pDevice, pActor, CMeshMgr::GetInstance()->Clone( "Mesh_Crown" ), CTextureMgr::GetInstance()->Clone( "Texture_Crown" ), XMFLOAT3( 0.17f, 0.17f, 0.17f ) );
					pShader_Cave->Add_RenderObject( m_pCrown );
					pActor->setGroup( COL_DYNAMIC );
					pActor->userData = m_pCrown;
					SetCollisionGroup( pActor, COL_DYNAMIC );
				}

				else if( 0 == strcmp( pActor->getName(), "Branch00" ) )
				{
					CGameObject* pBranch = CEnvironment::Create( pDevice, pActor, CMeshMgr::GetInstance()->Clone( "Mesh_Branch00" ), CTextureMgr::GetInstance()->Clone( "Texture_Branch00" ), XMFLOAT3( 0.79901f, 0.79901f, 0.79901f ) );
					pActor->setGroup( COL_DYNAMIC );
					pBranch->Add_Ref();
					pBranch->Add_Ref();
					pShader_Mesh->Add_RenderObject( pBranch );
					pShader_Mesh_Alpha->Add_RenderObject( pBranch );
					SetCollisionGroup( pActor, COL_DYNAMIC );
				}

				else
				{
					pActor->setGroup( COL_DYNAMIC );
					SetCollisionGroup( pActor, COL_DYNAMIC );
				}
			}

			else
			{
				if( 0 == strcmp( pActor->getName(), "map" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Ground1" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Ground1" );
					CGameObject* pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTerrain->Add_Ref();
					pTerrain->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTerrain );
					pShader_Mesh_Alpha->Add_RenderObject( pTerrain );

					pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Ground2" );
					pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Ground2" );
					pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTerrain->Add_Ref();
					pTerrain->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTerrain );
					pShader_Mesh_Alpha->Add_RenderObject( pTerrain );

					pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Cave" );
					pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Cave" );
					pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pShader_Cave->Add_RenderObject( pTerrain );

					pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Water" );
					pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Water" );
					pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTerrain->Add_Ref();
					pTerrain->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTerrain );
					pShader_Mesh_Alpha->Add_RenderObject( pTerrain );
				}

				else if( 0 == strcmp( pActor->getName(), "SideGround0" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_SideGround0" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_SideGround0" );
					CGameObject* pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 0.97493f, 1.f, 2.0422f ) );
					pTerrain->Add_Ref();
					pTerrain->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTerrain );
					pShader_Mesh_Alpha->Add_RenderObject( pTerrain );
				}

				else if( 0 == strcmp( pActor->getName(), "SideGround1" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_SideGround1" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_SideGround1" );
					CGameObject* pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.00757f ) );
					pTerrain->Add_Ref();
					pTerrain->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTerrain );
					pShader_Mesh_Alpha->Add_RenderObject( pTerrain );
				}

				else if( 0 == strcmp( pActor->getName(), "ground3" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Ground3" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Ground3" );
					CGameObject* pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 0.32454f, 1.f ) );
					pTerrain->Add_Ref();
					pTerrain->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTerrain );
					pShader_Mesh_Alpha->Add_RenderObject( pTerrain );
				}

				else if( 0 == strcmp( pActor->getName(), "Bridge" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Bridge" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Bridge" );
					CGameObject* pBridge = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pBridge->Add_Ref();
					pBridge->Add_Ref();
					pShader_Mesh->Add_RenderObject( pBridge );
					pShader_Mesh_Alpha->Add_RenderObject( pBridge );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree00" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree00" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree00" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTree->Add_Ref();
					pTree->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTree );
					pShader_Mesh_Alpha->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree01" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree01" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree01" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTree->Add_Ref();
					pTree->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTree );
					pShader_Mesh_Alpha->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree02" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree02" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree02" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTree->Add_Ref();
					pTree->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTree );
					pShader_Mesh_Alpha->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree03" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree03" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree03" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTree->Add_Ref();
					pTree->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTree );
					pShader_Mesh_Alpha->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree04" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree04" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree04" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pTree->Add_Ref();
					pTree->Add_Ref();
					pShader_Mesh->Add_RenderObject( pTree );
					pShader_Mesh_Alpha->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Mushroom" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Mushroom" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Mushroom" );
					CGameObject* pMushroom = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.95783f, 1.95783f, 1.95783f ) );
					pMushroom->Add_Ref();
					pMushroom->Add_Ref();
					pShader_Mesh->Add_RenderObject( pMushroom );
					pShader_Mesh_Alpha->Add_RenderObject( pMushroom );
				}

				else if( 0 == strcmp( pActor->getName(), "Palmtree" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Palmtree" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Palmtree" );
					CGameObject* pPalmtree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pPalmtree->Add_Ref();
					pPalmtree->Add_Ref();
					pShader_Mesh->Add_RenderObject( pPalmtree );
					pShader_Mesh_Alpha->Add_RenderObject( pPalmtree );
				}

				else if( 0 == strcmp( pActor->getName(), "Flower00" ) )
				{
					pActor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Flower00" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Flower00" );
					CGameObject* pFlower = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pFlower->Add_Ref();
					pFlower->Add_Ref();
					pShader_Mesh->Add_RenderObject( pFlower );
					pShader_Mesh_Alpha->Add_RenderObject( pFlower );
				}

				else if( 0 == strcmp( pActor->getName(), "Rock00" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Rock00" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Rock00" );
					CGameObject* pRock = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pRock->Add_Ref();
					pRock->Add_Ref();
					pShader_Mesh->Add_RenderObject( pRock );
					pShader_Mesh_Alpha->Add_RenderObject( pRock );
				}

				else if( 0 == strcmp( pActor->getName(), "Rock01" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Rock01" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Rock01" );
					CGameObject* pRock = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pRock->Add_Ref();
					pRock->Add_Ref();
					pShader_Mesh->Add_RenderObject( pRock );
					pShader_Mesh_Alpha->Add_RenderObject( pRock );
				}

				else if( 0 == strcmp( pActor->getName(), "Rock02" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Rock02" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Rock02" );
					CGameObject* pRock = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pRock->Add_Ref();
					pRock->Add_Ref();
					pShader_Mesh->Add_RenderObject( pRock );
					pShader_Mesh_Alpha->Add_RenderObject( pRock );
				}

				else if( 0 == strcmp( pActor->getName(), "Rock03" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Rock03" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Rock03" );
					CGameObject* pRock = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.54092f, 1.54092f, 1.54092f ) );
					pRock->Add_Ref();
					pRock->Add_Ref();
					pShader_Mesh->Add_RenderObject( pRock );
					pShader_Mesh_Alpha->Add_RenderObject( pRock );
				}

				else if( 0 == strcmp( pActor->getName(), "Bush00" ) )
				{
					pActor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Bush00" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Bush00" );
					CGameObject* pBush = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 0.0372f, 1.f ) );
					pBush->Add_Ref();
					pBush->Add_Ref();
					pShader_Mesh->Add_RenderObject( pBush );
					pShader_Mesh_Alpha->Add_RenderObject( pBush );
				}

				else if( 0 == strcmp( pActor->getName(), "Bush01" ) )
				{
					pActor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Bush01" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Bush01" );
					CGameObject* pBush = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 0.0372f, 1.f ) );
					pBush->Add_Ref();
					pBush->Add_Ref();
					pShader_Mesh->Add_RenderObject( pBush );
					pShader_Mesh_Alpha->Add_RenderObject( pBush );
				}

				else
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );
				}
			}
		}

		plistShader[ RENDER_BACKGROUND ].push_back( pShader_Skydome );
		plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Mesh );
		plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Cave );
		plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Animate );
		plistShader[ RENDER_LIGHT ].push_back( pShader_Light );
		plistShader[ RENDER_BLEND ].push_back( pShader_Blend );
		plistShader[ RENDER_ALPHA ].push_back( CShaderMgr::GetInstance()->Clone( "Shader_Mesh_Alpha" ) );
		plistShader[ RENDER_ALPHA ].push_back( pShader_Mesh_Alpha );
		plistShader[ RENDER_DEBUG ].push_back( pShader_Debug );

		auto set_iter_begin = setReleaseActorIndex.begin();
		auto set_iter_end = setReleaseActorIndex.end();

		for( ; set_iter_begin != set_iter_end; ++set_iter_begin )
			m_pScene->releaseActor( *dpActorArray[ ( *set_iter_begin ) ] );

		for( int i = 0; i < BANANA_COUNT; ++i )
			CreateBanana();

		CreateCloth( pDevice );
	}

	return S_OK;
}

NxController* CPhysics::CreateCharacterController( NxActor* pActor, NxActor** dpActors, int iArraySize )
{
	//actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);

	float	fSkinWidth = 0.1f;
	NxController*	pOut{};

	if( !true )
	{
		// Box Controller
		NxVec3	vScale( 0.5f, 1.0f, 0.5f );
		NxBoxControllerDesc tBoxDesc;
		tBoxDesc.extents = vScale;
		tBoxDesc.position.x = pActor->getGlobalPosition().x;
		// gSpace = tBoxDesc.extents.y;
		tBoxDesc.position.y = pActor->getGlobalPosition().y;
		tBoxDesc.position.z = pActor->getGlobalPosition().z;
		tBoxDesc.upDirection = NX_Y;
		tBoxDesc.slopeLimit = 0;
		tBoxDesc.slopeLimit = cosf( NxMath::degToRad( 45.0f ) );
		tBoxDesc.skinWidth = fSkinWidth;
		tBoxDesc.stepOffset = 0.1f;
		tBoxDesc.userData = dpActors;
		tBoxDesc.callback = &m_ControllerReport;
		pOut = m_pCharacterControllerMgr->createController( m_pScene, tBoxDesc );
	}

	else
	{
		// Capsule Controller
		NxShape* pShape = pActor->getShapes()[ 0 ];
		NxF32	fRadius = ( ( NxCapsuleShape* )( pShape ) )->getRadius();
		NxF32	fHeight = ( ( NxCapsuleShape* )( pShape ) )->getHeight();
		NxCapsuleControllerDesc		tCapsuleDesc;
		tCapsuleDesc.radius = fRadius;
		tCapsuleDesc.height = fHeight * 1.f;
		tCapsuleDesc.position.x = pActor->getGlobalPosition().x;
		// gSpace = ( tCapsuleDesc.height * 0.5f + tCapsuleDesc.radius );
		tCapsuleDesc.position.y = pActor->getGlobalPosition().y;
		tCapsuleDesc.position.z = pActor->getGlobalPosition().z;
		tCapsuleDesc.upDirection = NX_Y;
		// tCapsuleDesc.slopeLimit = cosf(NxMath::degToRad(45.0f));
		tCapsuleDesc.slopeLimit = 0.f/*cosf( NxMath::degToRad( 45.0f ) )*/;
		tCapsuleDesc.skinWidth = fSkinWidth;
		tCapsuleDesc.stepOffset = 0.5f;
		//tCapsuleDesc.stepOffset = fRadius * 0.5f * fScale;
		tCapsuleDesc.userData = dpActors;
		tCapsuleDesc.callback = &m_ControllerReport;
		pOut = m_pCharacterControllerMgr->createController( m_pScene, tCapsuleDesc );
	}

	char szName[ MAX_PATH ] = "Character Controller Actor of ";
	strcat_s( szName, MAX_PATH, dpActors[ 0 ]->getName() );
	pOut->getActor()->setName( szName );

	return pOut;
}

void CPhysics::SetCollisionGroup( NxActor* pActor, NxCollisionGroup eGroup )
{
	NxU32			iActorShapeCnt = pActor->getNbShapes();
	NxShape* const* dpActorShapeArray = pActor->getShapes();

	while( iActorShapeCnt-- )
		dpActorShapeArray[ iActorShapeCnt ]->setGroup( eGroup );
}

NxActor* CPhysics::CreateActor( const char* pActorName, const ACTOR_INFO& tActor_Info, COL_GROUP eColGroup )
{
	switch( tActor_Info.m_dwType )
	{
	case NX_SHAPE_SPHERE:
	{
		NxSphereShapeDesc	tSphereShapeDesc;
		tSphereShapeDesc.radius = tActor_Info.m_fRadius;
		tSphereShapeDesc.name = pActorName;
		tSphereShapeDesc.localPose.t = NxVec3( 0.f, 0.f, 0.f );

		NxActorDesc tActorDesc;
		tActorDesc.name = pActorName;
		tActorDesc.globalPose.t.set( tActor_Info.m_vGlobalPosition.x, tActor_Info.m_vGlobalPosition.y, tActor_Info.m_vGlobalPosition.z );

		NxBodyDesc bodyDesc;
		tActorDesc.body = &bodyDesc;

		tActorDesc.density = 1;
		tActorDesc.group = eColGroup;
		tSphereShapeDesc.group = eColGroup;

		tActorDesc.shapes.pushBack( &tSphereShapeDesc );

		return m_pScene->createActor( tActorDesc );
	}
	case NX_SHAPE_BOX:
	{
		NxBoxShapeDesc		tBoxShapeDesc;
		tBoxShapeDesc.dimensions = NxVec3( tActor_Info.m_fLength * 0.5f, tActor_Info.m_fHeight * 0.5f, tActor_Info.m_fWidth * 0.5f );
		tBoxShapeDesc.name = pActorName;
		tBoxShapeDesc.localPose.t = NxVec3( 0.f, 0.f, 0.f );

		NxActorDesc tActorDesc;
		tActorDesc.name = pActorName;
		tActorDesc.globalPose.t.set( tActor_Info.m_vGlobalPosition.x, tActor_Info.m_vGlobalPosition.y, tActor_Info.m_vGlobalPosition.z );

		NxBodyDesc bodyDesc;
		tActorDesc.body = &bodyDesc;
		tActorDesc.density = 1;
		tActorDesc.group = eColGroup;
		tBoxShapeDesc.group = eColGroup;

		tActorDesc.shapes.pushBack( &tBoxShapeDesc );

		return m_pScene->createActor( tActorDesc );
	}
	case NX_SHAPE_CAPSULE:
	{
		NxCapsuleShapeDesc		tCapsuleShapeDesc;
		tCapsuleShapeDesc.radius = tActor_Info.m_fRadius;
		tCapsuleShapeDesc.height = tActor_Info.m_fHeight;
		tCapsuleShapeDesc.name = pActorName;
		tCapsuleShapeDesc.localPose.t = NxVec3( 0.f, 0.f, 0.f );

		NxActorDesc tActorDesc;
		tActorDesc.name = pActorName;
		tActorDesc.globalPose.t.set( tActor_Info.m_vGlobalPosition.x, tActor_Info.m_vGlobalPosition.y, tActor_Info.m_vGlobalPosition.z );

		NxBodyDesc bodyDesc;
		tActorDesc.body = &bodyDesc;
		tActorDesc.density = 1;
		tActorDesc.group = eColGroup;
		tCapsuleShapeDesc.group = eColGroup;

		tActorDesc.shapes.pushBack( &tCapsuleShapeDesc );

		return m_pScene->createActor( tActorDesc );
	}
	default:
		return nullptr;
	}
}

void CPhysics::CreateMeshFromShape( NxSimpleTriangleMesh &triMesh, NxShape *shape )
{
	NxBoxShape *boxShape = shape->isBox();
	if( boxShape != NULL )
	{
		NxBox obb = NxBox( NxVec3( 0.0f, 0.0f, 0.0f ), boxShape->getDimensions(), NxMat33( NX_IDENTITY_MATRIX ) );
		triMesh.points = new NxVec3[ 8 ];
		triMesh.numVertices = 8;
		triMesh.pointStrideBytes = sizeof( NxVec3 );
		triMesh.numTriangles = 2 * 6;
		triMesh.triangles = new NxU32[ 2 * 6 * 3 ];
		triMesh.triangleStrideBytes = sizeof( NxU32 ) * 3;
		triMesh.flags = 0;
		NxComputeBoxPoints( obb, ( NxVec3 * )triMesh.points );
		memcpy( ( NxU32 * )triMesh.triangles, NxGetBoxTriangles(), sizeof( NxU32 ) * 2 * 6 * 3 );
	}
	else
	{
		NX_ASSERT( !"Invalid shape type" );
	}

	NX_ASSERT( triMesh.isValid() );
}

void CPhysics::CreateCloth( ID3D11Device* pDevice )
{
	// 테스트용 액터
	ACTOR_INFO	testActorDesc;
	testActorDesc.m_dwType = NX_SHAPE_BOX;
	testActorDesc.m_fRadius = 2.5f;
	testActorDesc.m_fWidth = 20.f;
	testActorDesc.m_fHeight = 20.f;
	testActorDesc.m_fLength = 20.f;
	testActorDesc.m_vGlobalPosition.x = -100.f;
	testActorDesc.m_vGlobalPosition.y = 200.f;
	testActorDesc.m_vGlobalPosition.z = 100.f;

	NxActor* pActor = CreateActor( "testBoxForCloth", testActorDesc, COL_DYNAMIC );

	// 옷감
	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3( -100, 200, 100 );
	clothDesc.thickness = 0.2f;
	//clothDesc.density = 1.0f;
	clothDesc.bendingStiffness = 1.0f;
	clothDesc.stretchingStiffness = 1.0f;
	//clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	//clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;

	if( m_pScene->getSimType() == NX_SIMULATION_HW )
		clothDesc.flags |= NX_CLF_HARDWARE;

	m_pCloth = new MyCloth( m_pScene, pDevice, clothDesc, 8.0f, 7.0f, 0.15f, NULL/*텍스쳐파일*/ );

	if( !m_pCloth->getNxCloth() )
	{
		printf( "Error: Unable to create the cloth for the current scene.\n" );
		delete m_pCloth;
	}
	else
	{
		m_pCloth->getNxCloth()->attachToShape( *pActor->getShapes(), NX_CLOTH_ATTACHMENT_TWOWAY );
	}


}

void CPhysics::CreateBanana( void )
{
	if( nullptr == m_pShaderlist ) return;

	ACTOR_INFO	tActor_Info;
	tActor_Info.m_dwType = NX_SHAPE_CAPSULE;
	tActor_Info.m_fRadius = 2.5f;
	tActor_Info.m_fWidth = 2.5f;
	tActor_Info.m_fHeight = 7.0f;
	tActor_Info.m_fLength = 2.5f;
	tActor_Info.m_vGlobalPosition.x = 0.f;
	tActor_Info.m_vGlobalPosition.y = 0.f;
	tActor_Info.m_vGlobalPosition.z = -1000.f;

	NxActor* pActor = CreateActor( "Banana", tActor_Info, COL_DYNAMIC );
	pActor->raiseBodyFlag( NX_BF_KINEMATIC );

	CBanana*	pBanana = CBanana::Create( pActor, COL_DYNAMIC );
	pActor->userData = pBanana;
	m_pShaderlist[ RENDER_ALPHA ].front()->Add_RenderObject( pBanana );
	m_BananaQueue.push( pBanana );
}

void CPhysics::ThrowBanana( NxVec3& vPos, NxVec3& vDir, COL_GROUP eColGroup )
{
	CBanana* pBanana = m_BananaQueue.front();
	if( pBanana->GetMasterCollisionGroup() > COL_DYNAMIC ) return;
	pBanana->Throw( vPos, vDir, eColGroup );
	m_BananaQueue.pop();
	m_BananaQueue.push( pBanana );
}

void CPhysics::SetCrownOwner( CPlayer * p )
{
	m_pCrown->SetOwner( p );
}

queue<CBanana*>* CPhysics::GetBananaQueue( void )
{
	return &m_BananaQueue;
}
