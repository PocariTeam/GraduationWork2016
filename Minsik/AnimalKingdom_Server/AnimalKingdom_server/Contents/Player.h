#pragma once
#include "stdafx.h"

class NxController;
class NxActor;
class CAnimator;
class CStateMachine;

class Player {
	int				hp_;
	int				damage_;
	bool			beaten_;
	Session*		session_;
	CHARACTER		character_;

	UINT			roomNum_;
	BOOL			isReady_;
	BOOL			isMaster_;

	FLOAT			speed_;
	NxVec3			moveDir_;

	NxController*	cct_;

	NxActor**		actorArray_;
	NxMat34*		actorsOriginPose_;
	UINT			actorCount_;
	/* 애니메이션 재생기 */
	CAnimator*		animator_;
	/* 상태 머신 */
	CStateMachine*	stateMachine_;

	Lock			lock_;
	/* 플레이어 회전각 */
	XMFLOAT3				m_vRotate;
	float					m_fJumpHeight;
	bool					m_bSweap;
public:
	void			Jump( const float& fTimeDelta, float fAnimatePercent );
	void			Move( const float& fTimeDelta );
	void			Attack( STATE eState );
	void			ResetDir( void ) { moveDir_ = NxVec3{ 0.f, 0.f, 0.f }; }
public:
	void		setRotateY( FLOAT y ) { m_vRotate.y = y; };
	float		getRotateY() { return m_vRotate.y; }
public:
	Player(Session* s, UINT room, BOOL master = false);
	~Player();
	void			update( float fTimeDelta );
	Session*		getSession() { return session_; };
	PlayerInfo		getPlayerInfo();
	BOOL			getReady() { return isReady_; }
	BOOL			getMaster() { return isMaster_; }
	void			setMaster(BOOL b);
	void			setReady(BOOL b);
	void			setCharacter( CHARACTER c);
	void			setCCT( NxController* cct );
	void			setActorArray( NxActor** dpActorArray, UINT actorcnt );
	void			setAnimator( CAnimator* pAnimator ) { animator_ = pAnimator; }
	NxController*	getCCT() { return cct_; }
	NxActor**		getActors() { return actorArray_; }
	CAnimator*		getAnimator() { return animator_; }
	UINT			getActorCount() { return actorCount_; }
	NxMat34*		getActorsOriginPose() {	return actorsOriginPose_; }
	CStateMachine*  getFSM() { return stateMachine_; }
	XMFLOAT4X4		GetWorld();
	void			ThrowBanana( void );
	void			sweapOn( void );
	void			sweapOff( void );
	void			setMoveDir(Vector3 vDir);
	void			setState(STATE state);
	int				getHp() { return hp_; }
	void			setHp( int hp ) { hp_ = hp; }
	bool			getBeaten() { return beaten_; }
	void			setBeaten( bool bBeaten ) { beaten_ = bBeaten; }
	int				getDamage() { return damage_; }
};