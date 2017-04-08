#pragma once
#include "stdafx.h"

class NxController;
class NxActor;
class CAnimator;
class CStateMachine;

class Player {
	Session*		session_;
	CHARACTER		character_;

	UINT			roomNum_;
	BOOL			isReady_;
	BOOL			isMaster_;

	FLOAT			speed_;
	NxVec3			moveDir_;

	NxController*	cct_;

	NxActor**		actorArray_;
	UINT			actorCount_;
	/* 애니메이션 재생기 */
	CAnimator*		animator_;
	/* 상태 머신 */
	CStateMachine*	stateMachine_;

	Lock			lock_;
	/* 플레이어 회전각 */
	XMFLOAT3				m_vRotate;
	float					m_fJumpTime;
public:
	void			Jump( const float& fTimeDelta );
	void			Move( const float& fTimeDelta );
	void			ResetJumpTime( void ) { m_fJumpTime = 0.f; }
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
	void			setCCT(NxController* cct) { cct_ = cct; }
	void			setActorArray( NxActor** dpActorArray, UINT actorcnt ) { actorArray_ = dpActorArray; actorCount_ = actorcnt; }
	void			setAnimator( CAnimator* pAnimator ) { animator_ = pAnimator; }
	NxController*	getCCT() { return cct_; }
	NxActor**		getActors() { return actorArray_; }
	CAnimator*		getAnimator() { return animator_; }
	UINT			getActorCount() { return actorCount_; }
	CStateMachine*  getFSM() { return stateMachine_; }
	XMFLOAT4X4		GetWorld();

	void			setMoveDir(Vector3 vDir);
	void			setState(STATE state);
};