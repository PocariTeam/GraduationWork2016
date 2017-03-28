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

///////////////////// Run State /////////////////////

void CRunState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_RUN );
}

void CRunState::Execute( Player* pOwner, const float& fTImeDelta )
{
}

void CRunState::Exit( Player* pOwner, const float& fTimeDelta )
{

}

///////////////////// Jump State /////////////////////

void CJumpState::Enter( Player* pOwner, const float& fTImeDelta )
{
	pOwner->getAnimator()->Change_Animation( STATE_JUMP );
}

void CJumpState::Execute( Player* pOwner, const float& fTImeDelta )
{
	if( pOwner->getAnimator()->GetCurrentAnimationFinished() )
		pOwner->getFSM()->Change_State( STATE_IDLE );
}

void CJumpState::Exit( Player* pOwner, const float& fTimeDelta )
{

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
