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

STATE CGlobalState::GetCurrentState( void )
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

STATE CIdleState::GetCurrentState( void )
{
	return STATE_IDLE;
}

///////////////////// Defend State /////////////////////

void CDefendState::Enter( Player* pOwner, const float& fTImeDelta )
{
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

STATE CDefendState::GetCurrentState( void )
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
	pOwner->ResetDir();
}

STATE CRunState::GetCurrentState( void )
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
	pOwner->Jump( fTImeDelta, pOwner->getAnimator()->GetPerFinish() );
}

void CJumpState::Exit( Player* pOwner, const float& fTimeDelta )
{
}

STATE CJumpState::GetCurrentState( void )
{
	return STATE_JUMP;
}

///////////////////// Attack State /////////////////////

void CAttackState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_ATT1 );
}

void CAttackState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CAttackState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CAttackState::GetCurrentState( void )
{
	return STATE_ATT1;
}

///////////////////// Beaten State /////////////////////

void CBeatenState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_BEATEN1 );
}

void CBeatenState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CBeatenState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CBeatenState::GetCurrentState( void )
{
	return STATE_BEATEN1;
}

///////////////////// Down State /////////////////////

void CDownState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_DOWN );
}

void CDownState::Execute( Player* pOwner, const float& fTImeDelta )
{

}

void CDownState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

STATE CDownState::GetCurrentState( void )
{
	return STATE_DOWN;
}
