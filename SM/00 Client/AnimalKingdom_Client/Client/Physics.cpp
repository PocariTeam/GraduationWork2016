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

CPhysics*	CSingleton<CPhysics>::m_pInstance;

CEntityReport		CPhysics::m_EntityReport;
CControllerReport	CPhysics::m_ControllerReport;
CCollisionReport	CPhysics::m_CollisionReport;

DWORD CPhysics::Release( void )
{
	Release_Scene();

	NXU::releasePersistentMemory();

	if( nullptr != m_pPhysicsSDK )
	{
		m_pPhysicsSDK->release();
		m_pPhysicsSDK = nullptr;
	}

	if( nullptr != m_pAllocator )
	{
		CloseCooking();
		delete m_pAllocator;
		m_pAllocator = nullptr;
	}

	delete this;

	return 0;
}

int CPhysics::Update( const float& fTimeDelta )
{
	m_pScene->fetchResults( NX_RIGID_BODY_FINISHED, true );
	m_pScene->simulate( fTimeDelta );
	m_pScene->flushStream();

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
				pShader->SetConstantBuffer( pContext, mtxWorld );
				pBox->Render( pContext );
			break;
			case NX_SHAPE_SPHERE:
				fRadius = ( ( NxSphereShape* )( *dpShape ) )->getRadius() * 2.f;
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( NxVec3( fRadius ) );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				pShader->SetConstantBuffer( pContext, mtxWorld );
				pSphere->Render( pContext );
				break;
			case NX_SHAPE_CAPSULE:
				fRadius = ( ( NxCapsuleShape* )( *dpShape ) )->getRadius() * 2.f;
				fHeight = ( ( NxCapsuleShape* )( *dpShape ) )->getHeight();
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( NxVec3( fRadius, ( fHeight + fRadius ) * 0.5f, fRadius ) );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				pShader->SetConstantBuffer( pContext, mtxWorld );
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
		NxReleaseControllerManager( m_pCharacterControllerMgr );
		m_pPhysicsSDK->releaseScene( *m_pScene );
		m_pCharacterControllerMgr = nullptr;
		m_pScene = nullptr;
	}
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

	NxSceneDesc tSceneDesc;
	tSceneDesc.groundPlane = true;
	tSceneDesc.gravity = NxVec3( 0, -9.81f, 0 );
	tSceneDesc.simType = NX_SIMULATION_HW;
	m_pScene = m_pPhysicsSDK->createScene( tSceneDesc );

	m_pCharacterControllerMgr = NxCreateControllerManager( m_pAllocator );

	return S_OK;
}

HRESULT CPhysics::Load_Scene( ID3D11Device* pDevice, list<CShader*>* plistShader, const char* pFileName, NXU::NXU_FileType eType/* = NXU::FT_XML*/ )
{
	if( FAILED( CreateSceneFromFile( pFileName, eType ) ) )
	{
#ifdef _DEBUG
		printf( "Physics Load_Scene Failed\n" );
#endif
		return E_FAIL;
	}

	return SetupScene( pDevice, plistShader );
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

HRESULT CPhysics::SetupScene( ID3D11Device* pDevice, list<CShader*>* plistShader )
{
	m_pPhysicsSDK->setParameter( NX_SKIN_WIDTH, 0.2f );
	m_pScene->setUserContactReport( &m_CollisionReport );
	m_pScene->setActorGroupPairFlags( COL_MINE, COL_DYNAMIC, NX_NOTIFY_ON_START_TOUCH );

	// Create the default material
	NxMaterial* pDefaultMaterial = m_pScene->getMaterialFromIndex( 0 );
	pDefaultMaterial->setRestitution( 0.0 );
	pDefaultMaterial->setStaticFriction( 0.5 );
	pDefaultMaterial->setDynamicFriction( 0.5 );

	m_pScene->simulate( 0 );

#ifdef _DEBUG
	if( m_pScene->getSimType() == NX_SIMULATION_HW )
		printf( "Hardware Simulate\n" );
	else
		printf( "Software Simulate\n" );
#endif

	NxU32		iActorCnt = m_pScene->getNbActors();
	NxActor**	dpActorArray = m_pScene->getActors();

	CShader*	pShader_Mesh, *pShader_Light, *pShader_Blend, *pShader_Debug, *pShader_Animate;

	pShader_Mesh = CShaderMgr::GetInstance()->Clone( "Shader_Mesh" );
	pShader_Light = CShaderMgr::GetInstance()->Clone( "Shader_Light" );
	pShader_Blend = CShaderMgr::GetInstance()->Clone( "Shader_Blend" );
	pShader_Debug = CShaderMgr::GetInstance()->Clone( "Shader_Debug" );
	pShader_Animate = CShaderMgr::GetInstance()->Clone( "Shader_AnimateMesh" );

	CMesh* pLight_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	CGameObject* pLight_Screen = CWallpaper::Create( pDevice, pLight_Mesh );

	CMesh* pBlend_Mesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	CGameObject* pBlend_Screen = CWallpaper::Create( pDevice, pBlend_Mesh );

	pShader_Light->Add_RenderObject( pLight_Screen );
	pShader_Blend->Add_RenderObject( pBlend_Screen );

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
				if( 0 == strcmp( pActor->getName(), "chm" ) )
				{
					pActor->setGroup( COL_MINE );
					SetCollisionGroup( pActor, COL_MINE );
					m_pMyCharacterController = CreateCharacterController( pActor, pActor->getGlobalPosition(), 2.8f );

					NxShape* const * dpShape = pActor->getShapes();

					for( NxU32 i = 0; i < iActorShapeCnt; ++i )
					{
						dpShape[ i ]->userData = new NxMat34;
						*( NxMat34* )dpShape[ i ]->userData = dpShape[ i ]->getLocalPose();
					}

					CGameObject* pPlayer = CPlayer::Create( pDevice, pActor, CAnimationMgr::CHARACTER_CHM );
					pShader_Animate->Add_RenderObject( pPlayer );
				}

				/*else if( strcmp( pActor->getName(), "yourPlayer" ) == 0 )
				{
					pActor->setGroup( CollGroup::OTHER_CHARACTER );
					SetShapesCollisionGroup( pActor, CollGroup::OTHER_CHARACTER );
					CreateCharacterController( pActor, pActor->getGlobalPosition(), 2.8f );
				}*/

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


					CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Map" );
					CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Map" );
					CGameObject* pTerrain = CTerrain::Create( pDevice, pActor, pMesh, pTexture, XMFLOAT3( 1048.53164f, 1048.53164f, 1048.53164f ) );
					pShader_Mesh->Add_RenderObject( pTerrain );
				}

				else
				{
					pActor->setGroup( COL_STATIC );
					SetCollisionGroup( pActor, COL_STATIC );
				}
				
			}

			
			if( iActorShapeCnt )
			{
				NxShape** dpActorShapeArray = ( NxShape** )pActor->getShapes();
				for( NxU32 j = 0; j < iActorShapeCnt; j++ )
				{
					NxShape* pActorShape = dpActorShapeArray[ j ];
					NxVec3 vShapePos = pActorShape->getLocalPosition();
					printf( "[ %d ] %s's Local Position : ( %f, %f, %f )\n", j, pActorShape->getName(), vShapePos.x, vShapePos.y, vShapePos.z );
				}
			}
		}
	}

	plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Mesh );
	plistShader[ RENDER_DEPTHTEST ].push_back( pShader_Animate );
	plistShader[ RENDER_LIGHT ].push_back( pShader_Light );
	plistShader[ RENDER_BLEND ].push_back( pShader_Blend );
	plistShader[ RENDER_DEBUG ].push_back( pShader_Debug );

	CRenderer::GetInstance()->Copy_RenderGroup( plistShader );

	return S_OK;
}

NxController* CPhysics::CreateCharacterController( NxActor* pActor, const NxVec3& vPos, NxReal fScale )
{
	//actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);

	float	fSkinWidth = 0.1f;
	NxController*	pOut{};

	if( !true )
	{
		// Box Controller
		NxVec3	vScale( 0.5f, 1.0f, 0.5f );
		NxBoxControllerDesc tBoxDesc;
		tBoxDesc.extents = vScale * fScale;
		tBoxDesc.position.x = vPos.x;
		// gSpace = tBoxDesc.extents.y;
		// tBoxDesc.position.y = vPos.y + gSpace;
		tBoxDesc.position.z = vPos.z;
		tBoxDesc.upDirection = NX_Y;
		tBoxDesc.slopeLimit = 0;
		tBoxDesc.slopeLimit = cosf( NxMath::degToRad( 45.0f ) );
		tBoxDesc.skinWidth = fSkinWidth;
		tBoxDesc.stepOffset = 0.1f;
		tBoxDesc.userData = pActor;
		tBoxDesc.callback = &m_ControllerReport;
		pOut = m_pCharacterControllerMgr->createController( m_pScene, tBoxDesc );
	}

	else
	{
		// Capsule Controller
		NxF32	fRadius = 2.f;
		NxF32	fHeight = 5.0f;
		NxCapsuleControllerDesc		tCapsuleDesc;
		tCapsuleDesc.radius = fRadius * fScale;
		tCapsuleDesc.height = fHeight * fScale;
		tCapsuleDesc.position.x = vPos.x;
		// gSpace = ( tCapsuleDesc.height * 0.5f + tCapsuleDesc.radius );
		tCapsuleDesc.position.y = vPos.y - 8.f; //+ gSpace;
		tCapsuleDesc.position.z = vPos.z;
		tCapsuleDesc.upDirection = NX_Y;
		// tCapsuleDesc.slopeLimit = cosf(NxMath::degToRad(45.0f));
		tCapsuleDesc.slopeLimit = 0;
		tCapsuleDesc.skinWidth = fSkinWidth;
		tCapsuleDesc.stepOffset = 0.1f;
		tCapsuleDesc.stepOffset = fRadius * 0.5f * fScale;
		tCapsuleDesc.userData = pActor;
		tCapsuleDesc.callback = &m_ControllerReport;
		pOut = m_pCharacterControllerMgr->createController( m_pScene, tCapsuleDesc );
	}

	char szName[ MAX_PATH ] = "Character Controller Actor of ";
	strcat_s( szName, MAX_PATH, pActor->getName() );
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
