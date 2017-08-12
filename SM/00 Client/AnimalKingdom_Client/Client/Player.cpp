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
#include "Bat.h"
#include "Jungle.h"

CPlayer::CPlayer()
	: CGameObject()
	, m_pStateMachine( nullptr )
	, m_pAnimator( nullptr )
	, m_pCharacterController( nullptr )
	, m_pActorsOriginPose( nullptr )
	, m_iID( 0 )
	, m_dwActorCnt( 0 )
	, m_vRotate( 0.f, -XM_PI, 0.f )
	, m_pInputMgr( CInputMgr::GetInstance() )
	, m_bSweap( false )
	, m_dpSections( nullptr )
	, m_iSectionCnt( 0 )
	, m_fHp( 100 )
	, m_bInCave( false )
	, m_vOverlapped( 0.f, 1.f, 0.f )
	, m_eCharactor( CHARACTER_CHM )
	, m_fDefenceTime( 0.f )
	, m_bSkillOn ( false )
	, m_fCanUseSkill ( 1.0f )
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose, int iID )
{
	m_pCharacterController = pCharacterController;
	m_pCharacterController->getActor()->userData = this;
	m_pActor = ( NxActor* )m_pCharacterController->getUserData();
	m_pStateMachine = CStateMachine::Create( this );
	m_pActorsOriginPose = pActorOriginPose;
	m_iID = iID;

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

	if( m_pAnimator->GetCurrentAnimationFinished() && STATE_JUMP != eState && STATE_DOWN != eState && STATE_DEAD != eState )
	{
		if( m_pInputMgr->Get_KeyboardState( DIK_S ) )
			eState = ( STATE_ATT1 == m_pStateMachine->GetPreviousState() ) ? STATE_ATT2 : STATE_ATT1;
		else if( m_pInputMgr->Get_KeyboardState( DIK_A ) )
			eState = STATE_DEFEND;
		else if( m_pInputMgr->Get_KeyboardState( DIK_D ) && m_fCanUseSkill >= 1.0f)
			eState = STATE_SKILL;
		else if( m_pInputMgr->Get_KeyboardState( DIK_F ) )
			eState = STATE_JUMP;
		else if( !vDir.isZero()
			&& ( STATE_RUN == eState || STATE_IDLE == eState ) )
			eState = STATE_RUN;
		else
			eState = STATE_IDLE;
	}

	// 방어 중일때 방어가 풀리는 조건 체크
	if( eState == STATE_DEFEND )
	{
		m_fDefenceTime += fTimeDelta;
		if( m_fDefenceTime > 2.f || false == m_pInputMgr->Get_KeyboardState( DIK_A ) )
		{
			m_fDefenceTime = 0.f;
			m_pAnimator->Play();
			CNetworkMgr::GetInstance()->sendCharacterState(STATE_DEFEND_END);
		}
	}

	if( eState != m_pStateMachine->GetCurrentState() )
		CNetworkMgr::GetInstance()->sendCharacterState( eState );
	if( ( STATE_RUN == eState || STATE_JUMP == eState || STATE_IDLE == eState ) && false == m_vOverlapped.equals( vDir, 1.f ) )
	{
		m_vOverlapped = vDir;
		CNetworkMgr::GetInstance()->sendMoveCharacter( vDir );
	}
}

void CPlayer::Jump( const float& fTimeDelta, float fAnimatePercent )
{
	/*m_vDir.y += m_fJumpHeight * cos( fAnimatePercent * XM_PI ) * fTimeDelta + GRAVITY * GRAVITY * fTimeDelta;*/
}

int CPlayer::Update( const float& fTimeDelta )
{
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
			m_bInCave = true;
			break;
		}

	if( i == m_iSectionCnt ) m_bInCave = false;

	return 0;
}

void CPlayer::Render( ID3D11DeviceContext* pContext )
{
	if( CJungle::m_bFocusIncave != m_bInCave )
		return;
	m_pAnimator->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

void CPlayer::Render_Dummy( ID3D11DeviceContext* pContext )
{
	m_pAnimator->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

void CPlayer::Render_Stencil( ID3D11DeviceContext* pContext )
{
	m_pAnimator->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CPlayer::Release( void )
{
	DWORD dwRefCnt = CGameObject::Release();
	if( 0 == dwRefCnt )
	{
		::Safe_Release( m_pAnimator );
		::Safe_Release( m_pStateMachine );

		delete[] m_pActorsOriginPose;
		delete[]( ( NxActor** )m_pCharacterController->getUserData() );
	}

	return dwRefCnt;
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

void CPlayer::ResetOverlapped()
{
	m_vOverlapped = NxVec3( 0.f, 1.f, 0.f );
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
	/*if( m_vDir.isZero() ) return;

	m_vDir.normalize();
	NxVec3	vDefault_Dir{ 0.f, 0.f, 1.f };

	NxReal fRotateY = acos( vDefault_Dir.dot( m_vDir ) );
	NxVec3 vCross = vDefault_Dir;
	vCross = vCross.cross( m_vDir );
	fRotateY *= ( vCross.y >= 0.0f ) ? -1.0f : 1.0f;
	m_vRotate.y = fRotateY;

	m_vDir = m_vDir * m_fSpeed * fTimeDelta;*/
}

void CPlayer::Sync( NxVec3& vPos, float hp, float fRotateY, STATE state, FLOAT canUseSkill)
{
	//NxU32	dwCollisionFlag;
	//vPos.subtract( vPos, m_pCharacterController->getActor()->getGlobalPosition() );
	//m_pCharacterController->move( vPos, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
	NxExtendedVec3 setPos{ vPos.x, vPos.y, vPos.z };
	m_pCharacterController->setPosition( setPos );
	m_vRotate.y = fRotateY;
	m_fHp = hp;
	m_fCanUseSkill = canUseSkill;
	m_pStateMachine->Change_State(state);

	// 애니메이션 멈춤을 방지하기 위한 임시 방편 ( 기본상태일때는 무조건 재생)
	// 단, 방어 풀리는 애니메이션을 볼 수 없음
	if (state == STATE_IDLE) m_pAnimator->Play(); 
}

void CPlayer::Attack( STATE eState )
{
}

void CPlayer::UseSkill()
{
	// 필요하면 자식 클래스(카멜레온, 원숭이, 박쥐)에 대한 가상함수를 정의하면 됨.
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

CPlayer* CPlayer::Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPoseArray, CHARACTER eType, int iID )
{
	CPlayer* pPlayer{ nullptr };

	switch( eType )
	{
	case CHARACTER_CHM:
		pPlayer = CChameleon::Create( pDevice, pCharacterController, pActorOriginPoseArray, eType, iID );
		break;
	case CHARACTER_MON:
		pPlayer = CMonkey::Create( pDevice, pCharacterController, pActorOriginPoseArray, eType, iID );
		break;
	case CHARACTER_BAT:
		pPlayer = CBat::Create( pDevice, pCharacterController, pActorOriginPoseArray, eType, iID );
		break;
	}

	return pPlayer;
}

