#include "stdafx.h"
#include "Chameleon.h"
#include "AnimateMeshMgr.h"
#include "TextureMgr.h"
#include "AnimationMgr.h"
#include "Animator.h"
#include "Player.h"
#include "Mathematics.h"
#include "StateMachine.h"
#include "ParticleMgr.h"
#include "ParticleObject.h"

CChameleon::CChameleon()
	: CPlayer()
{
}

CChameleon::~CChameleon()
{
}

HRESULT CChameleon::Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType )
{
	CPlayer::Initialize( pDevice, pCharacterController, pActorOriginPose );

	// Mesh, Texture, AnimationController 생성
	m_pMesh = CAnimateMeshMgr::GetInstance()->Clone( "Mesh_Chameleon" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Chameleon" );
	m_pAnimator = CAnimationMgr::GetInstance()->Clone( CHARACTER_CHM );
	m_dwActorCnt = 7;
	m_eCharactor = eType;
	m_vOption.x = 1.f;		// 카툰 한다
	m_pTrail[ 0 ] = CParticleMgr::GetInstance()->Add( CParticleMgr::PARTICLE_TRAIL, CMathematics::ConvertToXMFloat4x4( &( m_pAnimator->GetCurrentAnimationMatrix( this, "LHand", true ) ) ), XMFLOAT2( 4, 4 ), 1.f );
	m_pTrail[ 1 ] = CParticleMgr::GetInstance()->Add( CParticleMgr::PARTICLE_TRAIL, CMathematics::ConvertToXMFloat4x4( &( m_pAnimator->GetCurrentAnimationMatrix( this, "RHand", true ) ) ), XMFLOAT2( 4, 4 ), 1.f );

	return S_OK;
}

int CChameleon::Update( const float& fTimeDelta )
{
	CPlayer::Update( fTimeDelta );

	float fFalling = m_pCharacterController->getActor()->getGlobalPosition().y;

	if( fFalling <= 0.f || STATE_DEAD != m_pStateMachine->GetCurrentState() )
	{
		XMMATRIX mtxWorld;
		m_mtxWorld = CMathematics::ConvertToXMFloat4x4( &m_pCharacterController->getActor()->getGlobalPose() );
		m_mtxWorld._24 -= 1.1f;
		mtxWorld = XMMatrixMultiply( XMLoadFloat4x4( &m_mtxWorld ), XMMatrixRotationY( m_vRotate.y ) * XMMatrixScaling( 1.5f, 1.5f, 1.5f ) );

		XMStoreFloat4x4( &m_mtxWorld, mtxWorld );
	}

	m_pTrail[ 0 ]->UpdateParticleValue( CMathematics::ConvertToXMFloat4x4( &( m_pAnimator->GetCurrentAnimationMatrix( this, "LHand", true ) ) ) );
	m_pTrail[ 1 ]->UpdateParticleValue( CMathematics::ConvertToXMFloat4x4( &( m_pAnimator->GetCurrentAnimationMatrix( this, "RHand", true ) ) ) );

	return 0;
}

void CChameleon::Render( ID3D11DeviceContext* pContext )
{
	CPlayer::Render( pContext );
}

DWORD CChameleon::Release( void )
{
	if( 0 == CPlayer::Release() )
		delete this;

	return 0;
}

void CChameleon::Attack( STATE eState )
{
	if( !m_bSweap ) return;

	//NxActor*	pActor{};
	//switch( eState )
	//{
	//case STATE_ATT1: // RHand
	//	// pActor = ( ( NxActor** )m_pCharacterController->getUserData() )[ 2 ];
	//	m_pTrail[ 1 ]->UpdateParticleValue( CMathematics::ConvertToXMFloat4x4( &( m_pAnimator->GetCurrentAnimationMatrix( this, "RHand", true ) ) ) );
	//	break;
	//case STATE_ATT2: // LHand
	//	// pActor = ( ( NxActor** )m_pCharacterController->getUserData() )[ 1 ];
	//	m_pTrail[ 0 ]->UpdateParticleValue( CMathematics::ConvertToXMFloat4x4( &( m_pAnimator->GetCurrentAnimationMatrix( this, "LHand", true ) ) ) );
	//	break;
	//case STATE_SKILL:
	//	printf("카멜레온 스킬 사용 \n");
	//	break;
	//default:
	//	return ;
	//}

	/*NxSweepQueryHit result[ 100 ];
	UINT MyGroup = UINT( pActor->getGroup() );
	LPVOID pUserdata = LPVOID( LONGLONG( MyGroup ) );*/

	/*pActor->linearSweep( pActor->getShapes()[ 0 ]->getGlobalPosition(), NX_SF_DYNAMICS, pUserdata, 50, result,
		( NxUserEntityReport<NxSweepQueryHit>* )&CPhysics::m_EntityReport );*/
}

XMFLOAT4X4* CChameleon::GetWorld()
{
	/*XMMATRIX mtxWorld;
	m_mtxWorld = CMathematics::ConvertToXMFloat4x4( &m_pCharacterController->getActor()->getGlobalPose() );
	m_mtxWorld._24 -= 1.f;
	mtxWorld = XMMatrixMultiply( XMLoadFloat4x4( &m_mtxWorld ), XMMatrixRotationY( m_vRotate.y ) * XMMatrixScaling( 1.5f, 1.5f, 1.5f ) );

	XMStoreFloat4x4( &m_mtxWorld, mtxWorld );*/

	return &m_mtxWorld;
}

CChameleon* CChameleon::Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, CHARACTER eType )
{
	CChameleon* pChameleon = new CChameleon;

	if ( FAILED( pChameleon->Initialize( pDevice, pCharacterController, pActorOriginPose, eType ) ) )
	{
		pChameleon->Release();
		pChameleon = nullptr;
	}

	return pChameleon;
}
