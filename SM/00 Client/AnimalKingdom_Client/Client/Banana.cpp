#include "stdafx.h"
#include "Banana.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"
#include "Mathematics.h"
#include "NxActor.h"
#include "MeshMgr.h"
#include "TextureMgr.h"
#include "Enum.h"
#include "Physics.h"
#include "Shader.h"
#include "ShaderMgr.h"

CBanana::CBanana()
	: CGameObject()
	, m_pShader( nullptr )
	, m_vScale( 0.06f, 0.06f, 0.06f )
	, m_eMasterGroup( COL_DYNAMIC )
	, m_fLifeTime( 0.f )
{
}

CBanana::~CBanana()
{
}

HRESULT CBanana::Initialize( NxActor* pActor, COL_GROUP eMaster )
{
	m_pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Banana" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Banana" );
	m_eMasterGroup = eMaster;
	m_pActor = pActor;
	m_vOption.w = 1.f;
	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_Banana" );

	return S_OK;
}

bool CBanana::Timer( const float& fTimeDelta )
{
	m_fLifeTime -= fTimeDelta;
	if( m_fLifeTime < 0.f )
		return true;
	return false;
}

CBanana* CBanana::Create( NxActor* pActor, COL_GROUP eMaster )
{
	CBanana* pBanana = new CBanana;

	if( FAILED( pBanana->Initialize( pActor, eMaster ) ) )
	{
		pBanana->Release();
		pBanana = nullptr;
	}
	return pBanana;
}

XMFLOAT4X4* CBanana::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMMATRIX mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
	mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
	XMStoreFloat4x4( &m_mtxWorld, mtxScale );

	return &m_mtxWorld;
}

int CBanana::Update( const float& fTimeDelta )
{
	if( m_vOption.w < 0.f )
		Frozen();

	else if( 0 == strcmp( m_pActor->getName(), "Banana1" ) )
	{
		m_vOption.w -= fTimeDelta * 0.5f;
	}

	else if( COL_DYNAMIC != m_eMasterGroup )
		if( Timer( fTimeDelta ) )
			m_pActor->setName( "Banana1" );

	return 0;
}

void CBanana::Throw( NxVec3& vPos, NxVec3& vDir, COL_GROUP eMaster )
{
	m_pActor->clearBodyFlag( NX_BF_KINEMATIC );
	m_pActor->setLinearDamping( 1 );
	m_pActor->setAngularDamping( 5 );
	m_pActor->setGlobalPosition( vPos );
	m_pActor->setLinearVelocity( vDir * 200.f );
	m_pActor->setAngularVelocity( NxVec3( 90.f, 0.f, 180.f ) );
	m_eMasterGroup = eMaster;
	m_vOption.w = 1.f;
	m_fLifeTime = 3.f;
}

void CBanana::Frozen( void )
{
	m_pActor->setName( "Banana" );
	m_pActor->raiseBodyFlag( NX_BF_KINEMATIC );
	m_pActor->setGlobalPosition( NxVec3( 0.f, 0.f, -1000.f ) );
	m_eMasterGroup = COL_DYNAMIC;
	m_fLifeTime = 3.f;
}

void CBanana::Render( ID3D11DeviceContext* pContext )
{
	m_pShader->SetConstantBuffer( pContext, *GetWorld(), m_vOption );
	m_pShader->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CBanana::Release( void )
{
	// CPhysics::GetInstance()->GetSDK()->releaseCCDSkeleton( *( m_pActor->getShapes()[ 0 ]->getCCDSkeleton() ) );
	if( 0 == CGameObject::Release() )
	{
		::Safe_Release( m_pShader );
		delete this;
	}

	return 0;
}

COL_GROUP CBanana::GetMasterCollisionGroup() const
{
	return m_eMasterGroup;
}