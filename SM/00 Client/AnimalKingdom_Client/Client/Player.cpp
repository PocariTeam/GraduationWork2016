#include "stdafx.h"
#include "Player.h"
#include "Texture.h"
#include "AnimationMgr.h"
#include "Mesh.h"
#include "Function.h"
#include "Chameleon.h"
#include "Animator.h"
#include "StateMachine.h"
#include "InputMgr.h"
#include "Mathematics.h"
#include <NxControllerManager.h>
#include <NxQuat.h>
#include "NetworkMgr.h"
#include "Monkey.h"
#include "Section.h"

CPlayer::CPlayer()
	: CGameObject()
	, m_pStateMachine(nullptr)
	, m_pAnimator(nullptr)
	, m_pCharacterController(nullptr)
	, m_pActorsOriginPose(nullptr)
	, m_dwActorCnt(0)
	, m_vRotate(0.f, 0.f, 0.f)
	, m_pInputMgr(CInputMgr::GetInstance())
	, m_fSpeed(80.f)
	, m_bSweap(false)
	, m_fJumpHeight(130.f)
	, m_dpSections( nullptr )
	, m_iSectionCnt( 0 )
	, m_iHp(100)
	, m_bAlpha( false )
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose )
{
	m_pCharacterController = pCharacterController;
	m_pCharacterController->getActor()->userData = this;
	m_pActor = ( NxActor* )m_pCharacterController->getUserData();
	m_pStateMachine = CStateMachine::Create( this );
	m_pActorsOriginPose = pActorOriginPose;

	return S_OK;
}

void CPlayer::Check_Key( const float& fTimeDelta )
{
	STATE eState = m_pStateMachine->GetCurrentState();
	NxVec3 vDir{ 0.f, 0.f, 0.f };

	if( STATE_DEAD == eState ) return;

	if( m_pInputMgr->Get_KeyboardState( DIK_UP ) )
		vDir += NxVec3{ 0.f, 0.f, 1.f };
	if( m_pInputMgr->Get_KeyboardState( DIK_DOWN ) )
		vDir += NxVec3{ 0.f, 0.f, -1.f };
	if( m_pInputMgr->Get_KeyboardState( DIK_RIGHT ) )
		vDir += NxVec3{ 1.f, 0.f, 0.f };
	if( m_pInputMgr->Get_KeyboardState( DIK_LEFT ) )
		vDir += NxVec3{ -1.f, 0.f, 0.f };

	if( m_pAnimator->GetCurrentAnimationFinished() && STATE_JUMP != eState )
	{
		if( m_pInputMgr->Get_KeyboardState( DIK_S ) )
			eState = ( STATE_ATT1 == m_pStateMachine->GetPreviousState() )? STATE_ATT2 : STATE_ATT1;
		// else if( m_pInputMgr->Get_KeyboardState( DIK_D ) )
		//	eState = ( STATE_BEATEN1 == m_pStateMachine->GetPreviousState() ) ? STATE_BEATEN2 : STATE_BEATEN1;
		else if( m_pInputMgr->Get_KeyboardState( DIK_A ) )
			eState = STATE_DEFEND;
		else if( m_pInputMgr->Get_KeyboardState( DIK_F ) )
			eState = STATE_JUMP;
		else if( !vDir.isZero()
			&& ( STATE_RUN == eState || STATE_IDLE == eState ) )
			eState = STATE_RUN;
		else
			eState = STATE_IDLE;
	}
	
	if( eState != m_pStateMachine->GetCurrentState() )
		CNetworkMgr::GetInstance()->sendCharacterState( eState );
	if( false == m_vDir.equals( vDir, 1.f ) && ( STATE_RUN == eState || STATE_JUMP == eState || STATE_IDLE == eState ) )
		CNetworkMgr::GetInstance()->sendMoveCharacter( vDir );
}

void CPlayer::Jump( const float& fTimeDelta, float fAnimatePercent )
{
	m_vDir.y += m_fJumpHeight * cos( fAnimatePercent * XM_PI ) * fTimeDelta + GRAVITY * GRAVITY * fTimeDelta;
}

int CPlayer::Update( const float& fTimeDelta )
{
	/*printf( "컨트롤러 : %f, %f, %f\n", m_pCharacterController->getPosition().x, m_pCharacterController->getPosition().y, m_pCharacterController->getPosition().z );
	printf( "컨트롤러의 액터 : %f, %f, %f\n", m_pCharacterController->getActor()->getGlobalPosition().x, m_pCharacterController->getActor()->getGlobalPosition().y, m_pCharacterController->getActor()->getGlobalPosition().z );
	printf( "행렬 : %f, %f, %f\n", m_mtxWorld._14, m_mtxWorld._24, m_mtxWorld._34 );*/

	m_pStateMachine->Update( fTimeDelta );

	m_vDir.y += -GRAVITY * GRAVITY * fTimeDelta;

	NxU32	dwCollisionFlag;
	m_pCharacterController->move( m_vDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
	m_vDir = NxVec3{ 0.f, 0.f, 0.f };

	if( nullptr == m_dpSections ) return 0;

	UINT i = 0;
	NxVec3 vPos = m_pCharacterController->getActor()->getGlobalPosition();
	for( ; i < m_iSectionCnt; ++i )
		if( m_dpSections[ i ]->Check_InPlane( XMFLOAT3( vPos.x, vPos.y, vPos.z ) ) )
		{
			m_bAlpha = true;
			break;
		}

	if( i == m_iSectionCnt ) m_bAlpha = false;

	return 0;
}

void CPlayer::Render( ID3D11DeviceContext* pContext )
{
	m_pAnimator->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CPlayer::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pAnimator );
	::Safe_Release( m_pStateMachine );

	delete[] m_pActorsOriginPose;
	delete[]( ( NxActor** )m_pCharacterController->getUserData() );

	return 0;
}

void CPlayer::MinimizeController( void )
{
	( ( NxCapsuleShape* )m_pCharacterController->getActor()->getShapes()[ 0 ] )->setRadius( 0.f );
	( ( NxCapsuleShape* )m_pCharacterController->getActor()->getShapes()[ 0 ] )->setHeight( 0.f );
}

void CPlayer::SetSection( CSection** dpSections, UINT iSectionCnt )
{
	m_dpSections = dpSections;
	m_iSectionCnt = iSectionCnt;
}

XMFLOAT4X4* CPlayer::GetWorld()
{
	return &m_mtxWorld;
}

STATE CPlayer::GetCurrentState()
{
	return m_pStateMachine->GetCurrentState();
}

void CPlayer::Change_State( STATE eState )
{
	m_pStateMachine->Change_State( eState );
}

void CPlayer::Move( const float& fTimeDelta )
{
	if( m_vDir.isZero() ) return;

	m_vDir.normalize();
	NxVec3	vDefault_Dir{ 0.f, 0.f, 1.f };

	NxReal fRotateY = acos( vDefault_Dir.dot( m_vDir ) );
	NxVec3 vCross = vDefault_Dir;
	vCross = vCross.cross( m_vDir );
	fRotateY *= ( vCross.y >= 0.0f ) ? -1.0f : 1.0f;
	m_vRotate.y = fRotateY;

	m_vDir = m_vDir * m_fSpeed * fTimeDelta;
}

void CPlayer::Sync( NxVec3& vPos, int hp, float fRotateY, STATE state)
{
	//NxU32	dwCollisionFlag;
	//vPos.subtract( vPos, m_pCharacterController->getActor()->getGlobalPosition() );
	//m_pCharacterController->move( vPos, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
	NxExtendedVec3 setPos{ vPos.x, vPos.y, vPos.z };
	m_pCharacterController->setPosition( setPos );
	m_vRotate.y = fRotateY;
	m_iHp = hp;
	m_pStateMachine->Change_State(state);
}

void CPlayer::Attack( STATE eState )
{
}

void CPlayer::ThrowBanana( void )
{
}

void CPlayer::SweapOn( void )
{
	m_bSweap = true;
}

void CPlayer::SweapOff( void )
{
	m_bSweap = false;
}

CPlayer* CPlayer::Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPoseArray, CHARACTER eType )
{
	CPlayer* pPlayer{ nullptr };

	switch( eType )
	{
	case CHARACTER_CHM:
		pPlayer = CChameleon::Create( pDevice, pCharacterController, pActorOriginPoseArray );
		break;
	case CHARACTER_MON:
		pPlayer = CMonkey::Create( pDevice, pCharacterController, pActorOriginPoseArray );
		break;
	}

	return pPlayer;
}

