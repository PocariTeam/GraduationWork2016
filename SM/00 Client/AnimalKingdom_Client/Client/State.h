#pragma once

#ifndef State_h__
#define State_h__

#include "Singleton.h"

class CPlayer;
class CState
{
public:
	virtual void Enter( CPlayer* pOwner, const float& fTImeDelta )PURE;
	virtual void Execute( CPlayer* pOwner, const float& fTImeDelta )PURE;
	virtual void Exit( CPlayer* pOwner, const float& fTimeDelta )PURE;
	virtual STATE GetState( void )PURE;
	virtual DWORD Release( void );
};

class CGlobalState
	: public CState, public CSingleton<CGlobalState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CIdleState
	: public CState, public CSingleton<CIdleState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CDefendState
	: public CState, public CSingleton<CDefendState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CRunState
	: public CState, public CSingleton<CRunState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CJumpState
	: public CState, public CSingleton<CJumpState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CAttackState
	: public CState, public CSingleton<CAttackState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CAttackState2
	: public CState, public CSingleton<CAttackState2>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CBeatenState
	: public CState, public CSingleton<CBeatenState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CBeatenState2
	: public CState, public CSingleton<CBeatenState2>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CDownState
	: public CState, public CSingleton<CDownState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CDeadState
	: public CState, public CSingleton<CDeadState>
{
	void Enter( CPlayer* pOwner, const float& fTImeDelta );
	void Execute( CPlayer* pOwner, const float& fTImeDelta );
	void Exit( CPlayer* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};


#endif // State_h__