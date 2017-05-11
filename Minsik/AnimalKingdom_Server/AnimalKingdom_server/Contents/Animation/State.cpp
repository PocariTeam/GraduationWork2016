#include "stdafx.h"
#include "State.h"
#include "../Player.h"
#include "StateMachine.h"

DWORD CState::Release( void )
{
	delete this;

	return 0;
}

///////////////////// Global State /////////////////////

void CGlobalState::Enter( Player* pOwner, const float& fTImeDelta )
{

}

void CGlobalState::Execute( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Update( pOwner, fTImeDelta );
}

void CGlobalState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CGlobalState::GetState( void )
{
	return ( STATE )-1;
}

///////////////////// Idle State /////////////////////

void CIdleState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_IDLE );
}

void CIdleState::Execute( Player* pOwner, const float& fTImeDelta )
{
}

void CIdleState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CIdleState::GetState( void )
{
	return STATE_IDLE;
}

///////////////////// Defend State /////////////////////

void CDefendState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_DEFEND );
}

void CDefendState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CDefendState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CDefendState::GetState( void )
{
	return STATE_DEFEND;
}

///////////////////// Run State /////////////////////

void CRunState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_RUN );
}

void CRunState::Execute( Player* pOwner, const float& fTImeDelta )
{
	pOwner->Move( fTImeDelta );
}

void CRunState::Exit( Player* pOwner, const float& fTimeDelta )
{
}

STATE CRunState::GetState( void )
{
	return STATE_RUN;
}

///////////////////// Jump State /////////////////////

void CJumpState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_JUMP );
}

void CJumpState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
	{
		pOwner->getFSM()->Change_State( STATE_IDLE );
		return;
	}

	pOwner->Move( fTImeDelta );
	if( !pOwner->getAnimator()->isPause() )
		pOwner->Jump( fTImeDelta, pOwner->getAnimator()->GetPerFinish() );
}

void CJumpState::Exit( Player* pOwner, const float& fTimeDelta )
{
}

STATE CJumpState::GetState( void )
{
	return STATE_JUMP;
}

///////////////////// Attack State /////////////////////

void CAttackState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_ATT1 );
}

void CAttackState::Execute( Player* pOwner, const float& fTImeDelta )
{
	pOwner->Attack( STATE_ATT1 );
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CAttackState::Exit( Player* pOwner, const float& fTimeDelta )
{
	pOwner->sweapOff();
}

STATE CAttackState::GetState( void )
{
	return STATE_ATT1;
}

///////////////////// Attack State2 /////////////////////

void CAttackState2::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_ATT2 );
}

void CAttackState2::Execute( Player* pOwner, const float& fTImeDelta )
{
	pOwner->Attack( STATE_ATT2 );
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CAttackState2::Exit( Player* pOwner, const float& fTimeDelta )
{
	pOwner->sweapOff();
}

STATE CAttackState2::GetState( void )
{
	return STATE_ATT2;
}

///////////////////// Beaten State /////////////////////

void CBeatenState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_BEATEN1 );
}

void CBeatenState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CBeatenState::Exit( Player* pOwner, const float& fTimeDelta )
{
	pOwner->setBeaten( true );
}

STATE CBeatenState::GetState( void )
{
	return STATE_BEATEN1;
}

///////////////////// Beaten State2 /////////////////////

void CBeatenState2::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_BEATEN2 );
}

void CBeatenState2::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CBeatenState2::Exit( Player* pOwner, const float& fTimeDelta )
{
	pOwner->setBeaten( true );
}

STATE CBeatenState2::GetState( void )
{
	return STATE_BEATEN2;
}


///////////////////// Down State /////////////////////

void CDownState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_DOWN );
}

void CDownState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_DEAD );
}

void CDownState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CDownState::GetState( void )
{
	return STATE_DOWN;
}

///////////////////// Dead State /////////////////////

void CDeadState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->ResetDir();
	pOwner->getAnimator()->Change_Animation( STATE_DEAD );
	pOwner->setBeaten( false );
	pOwner->minimizeController();
}

void CDeadState::Execute( Player* pOwner, const float& fTImeDelta )
{
}

void CDeadState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CDeadState::GetState( void )
{
	return STATE_DEAD;
}