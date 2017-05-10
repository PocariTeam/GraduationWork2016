#include "stdafx.h"
#include "StateMachine.h"

CStateMachine::CStateMachine()
	: m_pOwner( nullptr )
	, m_pCurrentState( &CIdleState::getInstance() )
	, m_pGlobalState( &CGlobalState::getInstance() )
	, m_pPreviousState( nullptr )
{
}

void CStateMachine::Update( const float& fTimeDelta )
{
	if( m_pGlobalState ) m_pGlobalState->Execute( m_pOwner, fTimeDelta );
	if( m_pCurrentState ) m_pCurrentState->Execute( m_pOwner, fTimeDelta );
}

DWORD CStateMachine::Release()
{
	delete this;

	return 0;
}

CStateMachine* CStateMachine::Create( Player* pOwner )
{
	CStateMachine*	pStateMachine = new CStateMachine;

	if( FAILED( pStateMachine->Initialize( pOwner ) ) )
	{
		pStateMachine->Release();
		pStateMachine = nullptr;
	}

	return pStateMachine;
}

HRESULT CStateMachine::Initialize( Player* pOwner )
{
	m_pOwner = pOwner;

	return S_OK;
}

STATE CStateMachine::GetCurrentState()
{
	return m_pCurrentState->GetState();
}

STATE CStateMachine::GetPreviousState()
{
	return m_pPreviousState->GetState();
}

void CStateMachine::Change_State( CState* pState )
{
	if( m_pCurrentState == pState ) return;
	if( STATE_DEAD == m_pCurrentState->GetState() ) return;

	m_pPreviousState = m_pCurrentState;
	m_pCurrentState->Exit( m_pOwner, 0.f );
	m_pCurrentState = pState;
	m_pCurrentState->Enter( m_pOwner, 0.f );
}

void CStateMachine::Change_State( STATE eState )
{
	switch (eState)
	{
	case STATE_IDLE:
		Change_State(&CIdleState::getInstance());
		break;
	case STATE_ATT1:
		Change_State( &CAttackState::getInstance() );
		break;
	case STATE_ATT2:
		Change_State(&CAttackState2::getInstance());
		break;
	case STATE_RUN:
		Change_State(&CRunState::getInstance());
		break;
	case STATE_JUMP:
		Change_State(&CJumpState::getInstance());
		break;
	case STATE_DEFEND:
		Change_State(&CDefendState::getInstance());
		break;
	case STATE_BEATEN1:
		Change_State( &CBeatenState::getInstance() );
		break;
	case STATE_BEATEN2:
		Change_State(&CBeatenState2::getInstance());
		break;
	case STATE_DOWN:
		Change_State(&CDownState::getInstance());
		break;
	case STATE_DEAD:
		Change_State( &CDeadState::getInstance() );
		break;
	}
}

void CStateMachine::Set_State( STATE eState )
{
	switch( eState )
	{
	case STATE_IDLE:
		m_pPreviousState = &CIdleState::getInstance();
		m_pCurrentState = &CIdleState::getInstance();
		break;
	case STATE_ATT1:
		m_pPreviousState = &CAttackState::getInstance();
		m_pCurrentState = &CAttackState::getInstance();
		break;
	case STATE_ATT2:
		m_pPreviousState = &CAttackState2::getInstance();
		m_pCurrentState = &CAttackState2::getInstance();
		break;
	case STATE_RUN:
		m_pPreviousState = &CRunState::getInstance();
		m_pCurrentState = &CRunState::getInstance();
		break;
	case STATE_JUMP:
		m_pPreviousState = &CJumpState::getInstance();
		m_pCurrentState = &CJumpState::getInstance();
		break;
	case STATE_DEFEND:
		m_pPreviousState = &CDefendState::getInstance();
		m_pCurrentState = &CDefendState::getInstance();
		break;
	case STATE_BEATEN1:
		m_pPreviousState = &CBeatenState::getInstance();
		m_pCurrentState = &CBeatenState::getInstance();
		break;
	case STATE_BEATEN2:
		m_pPreviousState = &CBeatenState2::getInstance();
		m_pCurrentState = &CBeatenState2::getInstance();
		break;
	case STATE_DOWN:
		m_pPreviousState = &CDownState::getInstance();
		m_pCurrentState = &CDownState::getInstance();
		break;
	case STATE_DEAD:
		m_pPreviousState = &CDeadState::getInstance();
		m_pCurrentState = &CDeadState::getInstance();
		break;
	}
}

void CStateMachine::RevertToPreviousState( void )
{
	Change_State( m_pPreviousState );
}