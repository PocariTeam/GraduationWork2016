#pragma once

#ifndef StateMachine_h__
#define StateMachine_h__

class Player;
class CStateMachine
{
private:
	explicit CStateMachine( void );
public:
	void Update( const float& fTimeDelta );
	DWORD Release();
public:
	static CStateMachine* Create( Player* pOwner );
private:
	HRESULT Initialize( Player* pOwner );
private:
	Player*	m_pOwner;

	CState*			m_pCurrentState;
	CState*			m_pPreviousState;
	CState*			m_pGlobalState;
public:
	STATE	GetCurrentState();
	STATE	GetPreviousState();
	void	Change_State( CState* pState );
	void	Change_State( STATE	eState );
	void	Set_State( STATE eState );
	void	RevertToPreviousState( void );
};

#endif // StateMachine_h__