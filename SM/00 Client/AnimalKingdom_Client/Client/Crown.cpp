#include "stdafx.h"
#include "Crown.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"
#include <NxSimpleTypes.h>
#include "NxActor.h"
#include "Animator.h"
#include "Player.h"

CCrown::CCrown()
	: CGameObject()
	, m_pOwner( nullptr )
{
}

CCrown::~CCrown()
{
}

HRESULT CCrown::Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale )
{
	m_pActor = pActor;
	m_pMesh = pMesh;
	m_pTexture = pTexture;
	m_vScale = vScale;

	return S_OK;
}

CCrown* CCrown::Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale )
{
	CCrown* pCrown = new CCrown;

	if( FAILED( pCrown->Initialize( pDevice, pActor, pMesh, pTexture, vScale ) ) )
	{
		pCrown->Release();
		pCrown = nullptr;
	}
	return pCrown;
}

int CCrown::Update( const float& fTimeDelta )
{
	if( nullptr != m_pOwner )
	{
		NxMat34 mtxAnimation = m_pOwner->GetAnimator()->GetCurrentAnimationMatrix( m_pOwner, "Head" );
		NxMat33 mtxRotation{};
		NxVec3	vOffset{};
		
		switch( m_pOwner->GetCharacterType() )
		{
		case CHARACTER_CHM:
			mtxRotation.rotX( 0 );
			vOffset = NxVec3( 0.f, 9.75f, 0.f );
			break;
		case CHARACTER_MON:
			mtxRotation.rotX( XM_PI );
			vOffset = NxVec3( 0.f, -0.7f, -0.05f );
			break;
		default:
			break;
		}
		NxMat34 mtxLocal{ mtxRotation, vOffset };
		m_pActor->setGlobalPose( mtxAnimation * mtxLocal );
	}

	return 0;
}

XMFLOAT4X4* CCrown::GetWorld()
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

void CCrown::SetOwner( CPlayer* pPlayer )
{
	m_pOwner = pPlayer;
	if( pPlayer )
		m_pActor->raiseBodyFlag( NX_BF_KINEMATIC );
	else
		m_pActor->clearBodyFlag( NX_BF_KINEMATIC );
}

void CCrown::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CCrown::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		delete this;
	}

	return 0;
}
