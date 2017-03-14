#include "stdafx.h"
#include "State.h"
#include "Player.h"
#include "Animator.h"
#include "StateMachine.h"
#include "InputMgr.h"

CGlobalState*	CSingleton<CGlobalState>::m_pInstance;
CIdleState*		CSingleton<CIdleState>::m_pInstance;
CAttackState*	CSingleton<CAttackState>::m_pInstance;
CRunState*		CSingleton<CRunState>::m_pInstance;
CJumpState*		CSingleton<CJumpState>::m_pInstance;
CDefendState*	CSingleton<CDefendState>::m_pInstance;
CBeatenState*	CSingleton<CBeatenState>::m_pInstance;
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
	pOwner->Check_Key( fTImeDelta );
}

void CGlobalState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

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

///////////////////// Run State /////////////////////

void CRunState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_RUN );
}

void CRunState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
}

void CRunState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}

///////////////////// Jump State /////////////////////

void CJumpState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_JUMP );
}

void CJumpState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{
	if( pOwner->GetAnimator()->GetCurrentAnimationFinished() )
		pOwner->GetFSM()->Change_State( STATE_IDLE );
}

void CJumpState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

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

///////////////////// Down State /////////////////////

void CDownState::Enter( CPlayer* pOwner, const float& fTImeDelta )
{
	pOwner->GetAnimator()->Change_Animation( STATE_DOWN );
}

void CDownState::Execute( CPlayer* pOwner, const float& fTImeDelta )
{

}

void CDownState::Exit( CPlayer* pOwner, const float& fTimeDelta )
{

}
