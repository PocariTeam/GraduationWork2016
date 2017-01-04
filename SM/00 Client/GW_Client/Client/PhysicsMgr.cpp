#include "stdafx.h"
#include "PhysicsMgr.h"
#include "Utilities.h"
#include "GameObject.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "Define.h"


CPhysicsMgr*	CPhysicsMgr::m_pInstance;


CPhysicsMgr::CPhysicsMgr( void )
	: m_pPhysicsSDK( nullptr )
	, m_pScene( nullptr )
	, m_pArrActors( nullptr )
	, m_dwActorCnt( 0 )
	, m_pCBmtxWorld( nullptr )
{

}

CPhysicsMgr* CPhysicsMgr::GetInstance( void )
{
	if( nullptr == m_pInstance )
		m_pInstance = new CPhysicsMgr;

	return m_pInstance;
}

void CPhysicsMgr::DestroyInstance( void )
{
	if( nullptr != m_pInstance )
	{
		m_pInstance->Release();
		m_pInstance = nullptr;
	}
}

void CPhysicsMgr::Connect_Actors( vector<CGameObject*>* pvecGameObject )
{
	if( nullptr == pvecGameObject ) return;
	// if( pvecGameObject->size() != m_dwActorCnt ) return;

	NxActor** dpActor = m_pArrActors;

	for( DWORD i = 0; i < 1 /*pvecGameObject->size()*/; ++i, ++dpActor )
	{
		NxActor* pActor = *dpActor;
		pActor->userData = ( *pvecGameObject )[i];
		( *pvecGameObject )[i]->SetActor( pActor );
	}
}

HRESULT CPhysicsMgr::CreateContantBuffer( ID3D11Device * pDevice )
{
	/* VS 용 World */
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( XMFLOAT4X4 );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCBmtxWorld );
	return S_OK;
}

void CPhysicsMgr::SetContantBuffer( ID3D11DeviceContext* pContext, NxF32* pMtxWorld )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pCBmtxWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	XMFLOAT4X4* pmtxWorld = ( XMFLOAT4X4* )MappedSubresource.pData;
	memcpy( pmtxWorld, pMtxWorld, sizeof( XMFLOAT4X4 ) );
	pContext->Unmap( m_pCBmtxWorld, 0 );
	pContext->VSSetConstantBuffers( VS_SLOT_WORLD_MATRIX, 1, &m_pCBmtxWorld );
}

HRESULT CPhysicsMgr::Initialize()
{
	// Initialize PhysicsSDK
	NxPhysicsSDKDesc sdkDesc;
	sdkDesc.flags ^= NX_SDKF_NO_HARDWARE;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	m_pPhysicsSDK = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, NULL, NULL, sdkDesc, &errorCode );
	if( m_pPhysicsSDK == NULL )
	{
		printf( "\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError( errorCode ) );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPhysicsMgr::LoadScene(const char *pFilename, NXU::NXU_FileType type)
{
	HRESULT success = false;

	if (m_pPhysicsSDK)
	{

		NXU::NxuPhysicsCollection *c = NXU::loadCollection(pFilename, type);

		if (c)
		{
			if (m_pScene)
			{
				// Release old scene
				m_pPhysicsSDK->releaseScene(*m_pScene);
				m_pScene = nullptr;
			}
			if (m_pPhysicsSDK)
			{
				success = NXU::instantiateCollection(c, *m_pPhysicsSDK, m_pScene, 0, 0);
			}
			NXU::releaseCollection(c);

			NxU32 nbActors = m_pScene->getNbActors();
			NxActor **alist = m_pScene->getActors();

			for (NxU32 i = 0; i<nbActors; i++)
			{
				NxActor *a = alist[i];
				NxVec3 pos = a->getGlobalPosition();
				printf("액터 '%s'의 위치: (%f, %f, %f) \n", a->getName(), pos.x, pos.y, pos.z);

				NxU32 nbShapes = a->getNbShapes();
				if (nbShapes)
				{
					NxShape ** slist = (NxShape **)a->getShapes();
					for (NxU32 j = 0; j<nbShapes; j++)
					{
						NxShape *s = slist[j];
						NxVec3 spos = s->getGlobalPosition();
						printf("충돌메쉬[%d]%s의 위치: (%f, %f, %f) \n", j, s->getName(), spos.x, spos.y, spos.z);

					}
				}
			}

		}
		else
		{
			printf("NxuPhysicsCollection == NULL! \n");
		}
	}


	if (success)
		printf("Scene %d loaded from file %s.\n", m_pScene, pFilename);

	return success;
}

HRESULT CPhysicsMgr::CreateScene( ID3D11Device* pDevice )
{
	m_pPhysicsSDK->setParameter( NX_SKIN_WIDTH, 0.05f );

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = NxVec3( 0.0f, -9.81f, 0.0f );
	if( m_pPhysicsSDK->getHWVersion() != NX_HW_VERSION_NONE ) {
		sceneDesc.simType = NX_SIMULATION_HW;
	}
	m_pScene = m_pPhysicsSDK->createScene( sceneDesc );

	if( m_pScene == NULL )
	{
		printf( "\nError: Unable to create a PhysX scene, exiting the sample.\n\n" );
		return E_FAIL;
	}

	// Set default material
	NxMaterial* defaultMaterial = m_pScene->getMaterialFromIndex( 0 );
	defaultMaterial->setRestitution( 0.5f );
	defaultMaterial->setStaticFriction( 0.5f );
	defaultMaterial->setDynamicFriction( 0.5f );

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &planeDesc );
	m_pScene->createActor( actorDesc );

	CreateCube( NxVec3( 0.f, 5.0f, 0.f ), 2, 10.0f );
	CreateCapsule(NxVec3(-5.0f, 5.0f, 0.f), 1, 0.5f, 10.0f);
	CreateSphere(NxVec3(5.0f, 5.0f, 0.f), 1, 10.0f);

	m_pArrActors = m_pScene->getActors();
	m_dwActorCnt = m_pScene->getNbActors();
#ifdef _DEBUG
	CreateContantBuffer( pDevice );
#endif
	return S_OK;
}

void CPhysicsMgr::Update( const float & fTimeDelta )
{
	if( m_pScene == NULL )
		return;

	NxActor** dpActor = m_pArrActors;

	/*for( DWORD i = 0; i < m_dwActorCnt; ++i, ++dpActor )
	{
		CGameObject* pGameObject = ( CGameObject* )(*dpActor)->userData;
		pGameObject->Update( fTimeDelta );
	}*/

	m_pScene->simulate( fTimeDelta );
	m_pScene->flushStream();
	m_pScene->fetchResults( NX_RIGID_BODY_FINISHED, true );
}

void CPhysicsMgr::Render( ID3D11DeviceContext* pContext )
{
	NxActor** dpActor = m_pArrActors;

	CMesh* pBox = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Box" );
	CMesh* pSphere = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Sphere" );
	CMesh* pCapsule = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Capsule" );
	NxF32* mtxWorld = new NxF32[16];
	NxVec3	vScale{};
	NxF32	fRadius{}, fHeight{};
	NxMat34 mtxRealWorld{}, mtxScale{};
	mtxRealWorld.zero();
	mtxScale.zero();

	for( DWORD i = 0; i < m_dwActorCnt; ++i, ++dpActor )
	{
		DWORD dwShapeCnt = ( *dpActor )->getNbShapes();
		NxShape* const * dpShape = ( *dpActor )->getShapes();
		
		for( DWORD j = 0; j < dwShapeCnt; ++j, ++dpShape )
		{
			switch( ( *dpShape )->getType() )
			{
			case NX_SHAPE_BOX:
			{
				vScale = ( ( NxBoxShape* )( *dpShape ) )->getDimensions() * 2.f;
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( vScale );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				SetContantBuffer( pContext, mtxWorld );
				pBox->Render( pContext );
			}
				break;
			case NX_SHAPE_SPHERE:
				fRadius = ( ( NxSphereShape* )( *dpShape ) )->getRadius() * 2.f;
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( NxVec3( fRadius ) );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				SetContantBuffer( pContext, mtxWorld );
				pSphere->Render( pContext );
				break;
			case NX_SHAPE_CAPSULE:
				fRadius = ( ( NxCapsuleShape* )( *dpShape ) )->getRadius() * 2.f;
				fHeight = ( ( NxCapsuleShape* )( *dpShape ) )->getHeight();
				mtxRealWorld = ( *dpShape )->getGlobalPose();
				mtxScale.M.diagonal( NxVec3( fRadius, ( fHeight + fRadius ) * 0.5f, fRadius ) );
				mtxRealWorld.multiply( mtxRealWorld, mtxScale );
				mtxRealWorld.getRowMajor44( mtxWorld );
				SetContantBuffer( pContext, mtxWorld );
				pCapsule->Render( pContext );
				break;
			}
		}
	}

	pBox->Release();
	pSphere->Release();
	pCapsule->Release();

	delete[] mtxWorld;
}

void CPhysicsMgr::Release_Scene()
{
	if( nullptr != m_pScene )
	{
		m_pPhysicsSDK->releaseScene( *m_pScene );
		m_pScene = nullptr;
	}
}

void CPhysicsMgr::Release()
{
	if( nullptr != m_pScene )
	{
		m_pPhysicsSDK->releaseScene( *m_pScene );
		m_pScene = nullptr;
	}

	if( nullptr != m_pPhysicsSDK )
	{
		NxReleasePhysicsSDK( m_pPhysicsSDK );
		m_pPhysicsSDK = nullptr;
	}

	delete this;
}

void CPhysicsMgr::CreateCube( const NxVec3& pos, int size, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene == NULL! \n");
		return;
	}

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3( ( float )size * 0.5f, ( float )size * 0.5f, ( float )size*0.5f );
	boxDesc.localPose.t = NxVec3( ( float )0.0f, ( float )size*0.5f, ( float )size*0.0f );

	NxBodyDesc bodyDesc;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack( &boxDesc );
	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	
	m_pScene->createActor( actorDesc );
}

void CPhysicsMgr::CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene == NULL! \n");
		return;
	}

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0, radius + height * 0.5f, 0);

	NxBodyDesc bodyDesc;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&capsuleDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	m_pScene->createActor(actorDesc);
}


void CPhysicsMgr::CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	if (m_pScene == NULL) {
		printf("m_pScene == NULL! \n");
		return;
	}

	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0, radius, 0);

	NxBodyDesc bodyDesc;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	m_pScene->createActor(actorDesc);
}