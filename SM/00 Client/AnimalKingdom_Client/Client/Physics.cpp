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
#include "Skybox.h"
#include <set>
#include <xfunctional>
#include "NetworkMgr.h"
#include "NormalShader.h"
#include "Banana.h"
#include "Normal_UI.h"

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
				fRadius = ( ( NxCapsuleShape* )( *dpShape ) )->getRadius() * 2.f;
				fHeight = ( ( NxCapsuleShape* )( *dpShape ) )->getHeight();
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( NxVec3( fRadius, ( fHeight + fRadius ) * 0.5f, fRadius ) );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				pShader->SetConstantBuffer( pContext, XMFLOAT4X4( mtxWorld ) );
				pCapsule->Render( pContext );
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
	if( nullptr != m_pScene )
	{
		m_pCharacterControllerMgr->purgeControllers();
		m_pPhysicsSDK->releaseScene( *m_pScene );
		m_pScene = nullptr;
	}
}

void CPhysics::UpdateCharactercontrollerMgr()
{
	m_pCharacterControllerMgr->updateControllers();
}

void CPhysics::UpdateDynamicActors(S_SyncDynamic *packet)
{
	if (NULL == m_pScene)
		return;

	NxActor** ppActors = m_pScene->getActors();
	
	for (unsigned int i = 0; i < packet->dynamicActorCount; ++i)
	{
		unsigned int j = packet->dynamicActors[i].index;

		// 에러체크용: 후에 삭제
		if (j >= m_pScene->getNbActors())
		{
			printf(" 받은 동적 객체의 인덱스가 총 액터수보다 많습니다!! \n");
			break;
		}

		//printf("[index:%d] %s \n", j, ppActors[j]->getName());

		Vector3 p = packet->dynamicActors[i].position;
		Vector4 o = packet->dynamicActors[i].orient;

		ppActors[j]->setGlobalPosition(NxVec3(p.x,p.y,p.z));
		ppActors[j]->setGlobalOrientationQuat(NxQuat(NxVec3(o.x,o.y,o.z),o.w));

		Vector3 l = packet->dynamicActors[i].linear;
		Vector3 a = packet->dynamicActors[i].angular;
		ppActors[j]->setLinearVelocity(NxVec3(l.x, l.y, l.z));
		ppActors[j]->setAngularVelocity(NxVec3(a.x, a.y, a.z));
	
	}
}

void CPhysics::UpdateDynamicOneActor(S_SyncDynamicOne * packet)
{
	if (NULL == m_pScene)
		return;

	UINT	i = packet->dynamicActor.index;
	Vector3 p = packet->dynamicActor.position;
	Vector4 o = packet->dynamicActor.orient;

	NxActor* pActors = m_pScene->getActors()[i];

	pActors->setGlobalPosition(NxVec3(p.x, p.y, p.z));
	pActors->setGlobalOrientationQuat(NxQuat(NxVec3(o.x, o.y, o.z), o.w));

	Vector3 l = packet->dynamicActor.linear;
	Vector3 a = packet->dynamicActor.angular;
	pActors->setLinearVelocity(NxVec3(l.x, l.y, l.z));
	pActors->setAngularVelocity(NxVec3(a.x, a.y, a.z));

}

HRESULT CPhysics::Initialize( ID3D11Device* pDevice )
{
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

	m_pPhysicsSDK->setParameter( NX_SKIN_WIDTH, 0.2f );
	m_pScene->setUserContactReport( &m_CollisionReport );
	m_pScene->setActorGroupPairFlags( COL_PLAYER1 | COL_PLAYER2 | COL_PLAYER3 | COL_PLAYER4, COL_DYNAMIC, NX_NOTIFY_ON_START_TOUCH );

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

	CShader*	pShader_Mesh, *pShader_Light, *pShader_Blend, *pShader_Debug, *pShader_Animate, *pShader_Skybox;

	pShader_Skybox = CShaderMgr::GetInstance()->Clone( "Shader_Skybox" );
	pShader_Mesh = CShaderMgr::GetInstance()->Clone( "Shader_Mesh" );
	pShader_Light = CShaderMgr::GetInstance()->Clone( "Shader_Light" );
	pShader_Blend = CShaderMgr::GetInstance()->Clone( "Shader_Blend" );
	pShader_Debug = CShaderMgr::GetInstance()->Clone( "Shader_Debug" );
	pShader_Animate = CShaderMgr::GetInstance()->Clone( "Shader_AnimateMesh" );

	CMesh* pDot_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Dot" );
	CTexture* pTexture_Skybox = CTextureMgr::GetInstance()->Clone( "Texture_Skybox" );
	CGameObject* pSkybox = CSkybox::Create( pDot_Mesh, pTexture_Skybox );

	CMesh* pLight_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	CGameObject* pLight_Screen = CWallpaper::Create( pDevice, pLight_Mesh );

	CMesh* pBlend_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	CGameObject* pBlend_Screen = CWallpaper::Create( pDevice, pBlend_Mesh );

	pShader_Skybox->Add_RenderObject( pSkybox );
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
				if( 0 == strcmp( pActor->getName(), "player1" )
					|| 0 == strcmp( pActor->getName(), "player2" ) 
					|| 0 == strcmp( pActor->getName(), "player3" ) 
					|| 0 == strcmp( pActor->getName(), "player4" ) )
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
					pShader_Mesh->Add_RenderObject( pTerrain );

					pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Ground2" );
					pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Ground2" );
					pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pShader_Mesh->Add_RenderObject( pTerrain );

					pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Cave" );
					pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Cave" );
					pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pShader_Mesh->Add_RenderObject( pTerrain );

					pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Water" );
					pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Water" );
					pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1.f, 1.f, 1.f ) );
					pShader_Mesh->Add_RenderObject( pTerrain );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree00" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree00" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree00" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 0.25f, 0.25f, 0.25f ) );
					pShader_Mesh->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree01" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree01" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree01" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 0.3f, 0.3f, 0.3f ) );
					pShader_Mesh->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Tree02" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Tree02" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Tree02" );
					CGameObject* pTree = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 0.5f, 0.5f, 0.5f ) );
					pShader_Mesh->Add_RenderObject( pTree );
				}

				else if( 0 == strcmp( pActor->getName(), "Rock00" ) )
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );

					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Rock00" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Rock00" );
					CGameObject* pRock = CEnvironment::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 0.20f, 0.20f, 0.20f ) );
					pShader_Mesh->Add_RenderObject( pRock );
				}

				else
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );
				}

			}
		}

		auto set_iter_begin = setReleaseActorIndex.begin();
		auto set_iter_end = setReleaseActorIndex.end();

		for( ; set_iter_begin != set_iter_end; ++set_iter_begin )
			m_pScene->releaseActor( *dpActorArray[( *set_iter_begin )] );
	}

	plistShader[ RENDER_BACKGROUND ].push_back( pShader_Skybox );
	plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Mesh );
	plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Animate );
	plistShader[ RENDER_LIGHT ].push_back( pShader_Light );
	plistShader[ RENDER_BLEND ].push_back( pShader_Blend );
	plistShader[ RENDER_DEBUG ].push_back( pShader_Debug );

	CRenderer::GetInstance()->Copy_RenderGroup( plistShader );

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
		tCapsuleDesc.radius = fRadius * 0.5f;
		tCapsuleDesc.height = fHeight * 2.f;
		tCapsuleDesc.position.x = pActor->getGlobalPosition().x;
		// gSpace = ( tCapsuleDesc.height * 0.5f + tCapsuleDesc.radius );
		tCapsuleDesc.position.y = pActor->getGlobalPosition().y;
		tCapsuleDesc.position.z = pActor->getGlobalPosition().z;
		tCapsuleDesc.upDirection = NX_Y;
		// tCapsuleDesc.slopeLimit = cosf(NxMath::degToRad(45.0f));
		tCapsuleDesc.slopeLimit = 0;
		tCapsuleDesc.skinWidth = fSkinWidth;
		tCapsuleDesc.stepOffset = 0.1f;
		//tCapsuleDesc.stepOffset = fRadius * 0.5f * fScale;
		tCapsuleDesc.userData = dpActors;
		tCapsuleDesc.callback = &m_ControllerReport;
		pOut = m_pCharacterControllerMgr->createController( m_pScene, tCapsuleDesc );
	}

	char szName[ MAX_PATH ] = "Character Controller Actor of ";
	strcat_s( szName, MAX_PATH, dpActors[ 0 ]->getName() );
	pOut->getActor()->setName( szName );
	// m_pScene->releaseActor( *pActor );

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
		tBoxShapeDesc.localPose.t = NxVec3( tActor_Info.m_fLength * 0.5f, tActor_Info.m_fHeight * 0.5f, tActor_Info.m_fWidth * 0.5f );

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

void CPhysics::CreateBanana( NxVec3& vPos, NxVec3& vDir, COL_GROUP eColGroup )
{
	if( nullptr == m_pShaderlist ) return;

	ACTOR_INFO	tActor_Info;
	tActor_Info.m_dwType = 2;
	tActor_Info.m_fWidth = 2.5f;
	tActor_Info.m_fHeight = 8.f;
	tActor_Info.m_fLength = 2.5f;
	tActor_Info.m_vGlobalPosition.x = vPos.x;
	tActor_Info.m_vGlobalPosition.y = vPos.y;
	tActor_Info.m_vGlobalPosition.z = vPos.z;

	NxActor* pActor = CreateActor( "Banana", tActor_Info, COL_DYNAMIC/*eColGroup*/ );
	// pActor->raiseBodyFlag( NX_BF_KINEMATIC );

	CBanana*	pBanana = CBanana::Create( pActor, vDir );
	m_pShaderlist[ RENDER_DEPTHTEST ].front()->Add_RenderObject( pBanana );
}
