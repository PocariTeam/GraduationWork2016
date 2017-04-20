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
CRunState*		CSingleton<CRunState>::m_pInstance;
CJumpState*		CSingleton<CJumpState>::m_pInstance;
CDefendState*	CSingleton<CDefendState>::m_pInstance;
CBeatenState*	CSingleton<CBeatenState>::m_pInstance;
CBeatenState2*	CSingleton<CBeatenState2>::m_pInstance;
CDownState*		CSingleton<CDownState>::m_pInstance;

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
	pOwner->SetDir( NxVec3( 0.f ) );
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
}

STATE CJumpState::GetState( void )
{
	return STATE_JUMP;
}

///////////////////// Attack State /////////////////////

void CAttackState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_ATT1 );
}

void CAttackState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CAttackState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CAttackState::GetState( void )
{
	return STATE_ATT1;
}

///////////////////// Attack State2 /////////////////////

void CAttackState2::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_ATT2 );
}

void CAttackState2::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CAttackState2::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CAttackState2::GetState( void )
{
	return STATE_ATT2;
}

///////////////////// Beaten State /////////////////////

void CBeatenState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
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
	pOwner->GetAnimator()->Change_Animation( STATE_DOWN );
}

void CDownState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CDownState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

STATE CDownState::GetState( void )
{
	return STATE_DOWN;
}