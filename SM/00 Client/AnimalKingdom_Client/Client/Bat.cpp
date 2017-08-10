#include "stdafx.h"
#include "Bat.h"
#include "AnimateMeshMgr.h"
#include "TextureMgr.h"
#include "AnimationMgr.h"
#include "Animator.h"
#include "Player.h"
#include "Mathematics.h"
#include "StateMachine.h"
#include "ParticleMgr.h"
#include "ParticleObject.h"

CBat::CBat()
	: CPlayer()
{
}

CBat::~CBat()
{
}

HRESULT CBat::Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType )
{
	CPlayer::Initialize( pDevice, pCharacterController, pActorOriginPose );

	// Mesh, Texture, AnimationController 생성
	m_pMesh = CAnimateMeshMgr::GetInstance()->Clone( "Mesh_Bat" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Bat" );
	m_pAnimator = CAnimationMgr::GetInstance()->Clone( CHARACTER_BAT );
	m_dwActorCnt = 8;
	m_eCharactor = eType;
	m_vOption.x = 1.f;		// 카툰 한다

	return S_OK;
}

int CBat::Update( const float& fTimeDelta )
{
	CPlayer::Update( fTimeDelta );

	float fFalling = m_pCharacterController->getActor()->getGlobalPosition().y;

	if( fFalling <= 0.f || STATE_DEAD != m_pStateMachine->GetCurrentState() )
	{
		XMMATRIX mtxWorld;
		m_mtxWorld = CMathematics::ConvertToXMFloat4x4( &m_pCharacterController->getActor()->getGlobalPose() );
		m_mtxWorld._24 += 3.f;
		mtxWorld = XMMatrixMultiply( XMLoadFloat4x4( &m_mtxWorld ), XMMatrixRotationY( m_vRotate.y ) * XMMatrixScaling( 0.1f, 0.1f, 0.1f ) );

		XMStoreFloat4x4( &m_mtxWorld, mtxWorld );
	}

	else if( STATE_DOWN == m_pStateMachine->GetCurrentState() )
	{
		XMMATRIX mtxWorld;
		m_mtxWorld = CMathematics::ConvertToXMFloat4x4( &m_pCharacterController->getActor()->getGlobalPose() );
		mtxWorld = XMMatrixMultiply( XMLoadFloat4x4( &m_mtxWorld ), XMMatrixRotationY( m_vRotate.y ) * XMMatrixScaling( 0.1f, 0.1f, 0.1f ) );

		XMStoreFloat4x4( &m_mtxWorld, mtxWorld );
	}

	return 0;
}

void CBat::Render( ID3D11DeviceContext* pContext )
{
	CPlayer::Render( pContext );
}

DWORD CBat::Release( void )
{
	if( 0 == CPlayer::Release() )
		delete this;

	return 0;
}

void CBat::Attack( STATE eState )
{
	if( !m_bSweap ) return;
}

XMFLOAT4X4* CBat::GetWorld()
{
	return &m_mtxWorld;
}

CBat* CBat::Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType )
{
	CBat* pBat = new CBat;

	if( FAILED( pBat->Initialize( pDevice, pCharacterController, pActorOriginPose, eType ) ) )
	{
		pBat->Release();
		pBat = nullptr;
	}

	return pBat;
}
