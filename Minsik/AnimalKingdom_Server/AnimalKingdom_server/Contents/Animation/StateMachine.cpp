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

void CStateMachine::Change_State( CState* pState )
{
	if( m_pCurrentState == pState ) return;

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
	}
}

void CStateMachine::RevertToPreviousState( void )
{
	Change_State( m_pPreviousState );
}