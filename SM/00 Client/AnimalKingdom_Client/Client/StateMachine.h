#pragma once

#ifndef StateMachine_h__
#define StateMachine_h__

#include "Enum.h"

class CState;
class CPlayer;
class CStateMachine
{
private:
	explicit CStateMachine( void );
public:
	void Update( const float& fTimeDelta );
	DWORD Release();
public:
	static CStateMachine* Create( CPlayer* pOwner );
private:
	HRESULT Initialize( CPlayer* pOwner );
private:
	CPlayer*	m_pOwner;

	CState*			m_pCurrentState;
	CState*			m_pPreviousState;
	CState*			m_pGlobalState;
public:
	STATE	GetCurrentState();
	STATE	GetPreviousState();
	void	Change_State( CState* pState );
	void	Change_State( STATE	eState );
	void	RevertToPreviousState( void );
};

#endif // StateMachine_h__