#include "stdafx.h"
#include "State.h"
#include "Player.h"
#include "Animator.h"
#include "StateMachine.h"
#include "InputMgr.h"

CGlobalState*	CSingleton<CGlobalState>::m_pInstance;
CIdleState*		CSingleton<CIdleState>::m_pInstance;
CAttackState*	CSingleton<CAttackState>::m_pInstance;
CAttackState2*	CSingleton<CAttackState2>::m_pInstance;
CSKillState*	CSingleton<CSKillState>::m_pInstance;
CRunState*		CSingleton<CRunState>::m_pInstance;
CJumpState*		CSingleton<CJumpState>::m_pInstance;
CDefendState*	CSingleton<CDefendState>::m_pInstance;
CBeatenState*	CSingleton<CBeatenState>::m_pInstance;
CBeatenState2*	CSingleton<CBeatenState2>::m_pInstance;
CDownState*		CSingleton<CDownState>::m_pInstance;
CDeadState*		CSingleton<CDeadState>::m_pInstance;

DWORD CState::Release( void )
{
	delete this;

	return 0;
}

///////////////////// Global State /////////////////////
void CGlobalState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{

}

void CGlobalState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Update( pOwner, fTImeDelta );
}

void CGlobalState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CGlobalState::GetState( void )
{
	/* 잘못된 요청 */
	printf( "request wrong state! \n" );
	return ( STATE )-1;
}

///////////////////// Idle State /////////////////////
void CIdleState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_IDLE );
}

void CIdleState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
}

void CIdleState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CIdleState::GetState( void )
{
	return STATE_IDLE;
}

///////////////////// Defend State /////////////////////
void CDefendState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_DEFEND );
}

void CDefendState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CDefendState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CDefendState::GetState( void )
{
	return STATE_DEFEND;
}

///////////////////// Run State /////////////////////
void CRunState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_RUN );
}

void CRunState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->Move( fTImeDelta );
}

void CRunState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{
	pOwner->SetDir( NxVec3( 0.f, 0.f, 0.f ) );
}

STATE CRunState::GetState( void )
{
	return STATE_RUN;
}

///////////////////// Jump State /////////////////////
void CJumpState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_JUMP );
}

void CJumpState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
	{
		pOwner->GetFSM()->Change_State( STATE_IDLE );
		return;
	}

	pOwner->Move( fTImeDelta );
	if( !pOwner->GetAnimator()->isPause() )
		pOwner->Jump( fTImeDelta, pOwner->GetAnimator()->GetPerFinish() );
}

void CJumpState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{
	pOwner->GetAnimator()->Play();
}

STATE CJumpState::GetState( void )
{
	return STATE_JUMP;
}

///////////////////// Attack State /////////////////////
void CAttackState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_ATT1 );
}

void CAttackState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->Attack( STATE_ATT1 );
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CAttackState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{
	pOwner->SweapOff();
}

STATE CAttackState::GetState( void )
{
	return STATE_ATT1;
}

///////////////////// Attack State2 /////////////////////
void CAttackState2::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_ATT2 );
}

void CAttackState2::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->Attack( STATE_ATT2 );
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CAttackState2::Exit( CPlayer* pOwner, const float& fTimeDelta )
{
	pOwner->SweapOff();
}

STATE CAttackState2::GetState( void )
{
	return STATE_ATT2;
}

///////////////////// Skill State /////////////////////
void CSKillState::Enter( CPlayer * pOwner, const float & fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_SKILL );
}

void CSKillState::Execute( CPlayer * pOwner, const float & fTImeDelta )
{
	pOwner->UseSkill();
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CSKillState::Exit( CPlayer * pOwner, const float & fTimeDelta )
{
}

STATE CSKillState::GetState( void )
{
	return STATE_SKILL;
}

///////////////////// Beaten State /////////////////////
void CBeatenState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_BEATEN1 );
}

void CBeatenState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CBeatenState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CBeatenState::GetState( void )
{
	return STATE_BEATEN1;
}

///////////////////// Beaten State2 /////////////////////
void CBeatenState2::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_BEATEN2 );
}

void CBeatenState2::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CBeatenState2::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CBeatenState2::GetState( void )
{
	return STATE_BEATEN2;
}

///////////////////// Down State /////////////////////
void CDownState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_DOWN );
}

void CDownState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_DEAD );
}

void CDownState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CDownState::GetState( void )
{
	return STATE_DOWN;
}

///////////////////// Down State /////////////////////
void CDeadState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->ResetOverlapped();
	pOwner->GetAnimator()->Change_Animation( STATE_DEAD );
	pOwner->MinimizeController();
}

void CDeadState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
}

void CDeadState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CDeadState::GetState( void )
{
	return STATE_DEAD;
}