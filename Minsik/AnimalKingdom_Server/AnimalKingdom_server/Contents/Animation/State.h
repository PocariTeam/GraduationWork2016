#pragma once

#ifndef State_h__
#define State_h__

class Player;
class CState
{
public:
	virtual void Enter( Player* pOwner, const float& fTImeDelta ) = 0;
	virtual void Execute( Player* pOwner, const float& fTImeDelta ) = 0;
	virtual void Exit( Player* pOwner, const float& fTimeDelta ) = 0;
	virtual DWORD Release( void );
	virtual STATE GetState( void ) = 0;
};

class CGlobalState
	: public CState, public Singleton<CGlobalState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CIdleState
	: public CState, public Singleton<CIdleState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CDefendState
	: public CState, public Singleton<CDefendState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CRunState
	: public CState, public Singleton<CRunState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CJumpState
	: public CState, public Singleton<CJumpState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CAttackState
	: public CState, public Singleton<CAttackState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CAttackState2
	: public CState, public Singleton<CAttackState2>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CBeatenState
	: public CState, public Singleton<CBeatenState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CBeatenState2
	: public CState, public Singleton<CBeatenState2>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CDownState
	: public CState, public Singleton<CDownState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

class CDeadState
	: public CState, public Singleton<CDeadState>
{
	void Enter( Player* pOwner, const float& fTImeDelta );
	void Execute( Player* pOwner, const float& fTImeDelta );
	void Exit( Player* pOwner, const float& fTimeDelta );
	STATE GetState( void );
};

#endif // State_h__