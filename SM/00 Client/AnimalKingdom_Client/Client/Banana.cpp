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

CBanana::CBanana()
	: CGameObject()
	, m_vScale( 0.06f, 0.06f, 0.06f )
{
}

CBanana::~CBanana()
{
}

HRESULT CBanana::Initialize( NxActor* pActor, NxVec3& vDir )
{
	/*ACTOR_INFO	tActor_Info;
	tActor_Info.m_dwType = 2;
	tActor_Info.m_fWidth = m_vScale.x;
	tActor_Info.m_fHeight = m_vScale.y;
	tActor_Info.m_fLength = m_vScale.z;
	m_pActor = CPhysics::GetInstance()->CreateActor( "Banana", tActor_Info, COL_DYNAMIC );*/
	m_pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Banana" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Banana" );
	m_pActor = pActor;
	m_vDir = vDir;
	m_pActor->setLinearVelocity( m_vDir * 200.f );
	m_pActor->setAngularVelocity( NxVec3( 90.f, 0.f, 180.f ) );

	return S_OK;
}

CBanana* CBanana::Create( NxActor* pActor, NxVec3& vDir )
{
	CBanana* pBanana = new CBanana;

	if( FAILED( pBanana->Initialize( pActor, vDir ) ) )
	{
		pBanana->Release();
		pBanana = nullptr;
	}
	return pBanana;
}

XMFLOAT4X4 CBanana::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMMATRIX mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
	mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
	XMStoreFloat4x4( &Out, mtxScale );

	return Out;
}

int CBanana::Update( const float& fTimeDelta )
{
	return 0;
}

void CBanana::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CBanana::Release( void )
{
	CGameObject::Release();

	delete this;

	return 0;
}
