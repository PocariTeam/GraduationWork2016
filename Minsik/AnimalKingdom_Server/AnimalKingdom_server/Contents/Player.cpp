#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: lock_(L"Player"), session_(s), character_( CHARACTER::CHRACTER_NONE), roomNum_(room), isReady_(false), isMaster_(master), cct_(nullptr), actorArray_( nullptr ), animator_( nullptr ), stateMachine_( nullptr )
{
	stateMachine_ = CStateMachine::Create( this );
	speed_ = 50.0f;
	moveDir_ = NxVec3(0.0f, 0.0f, 0.0f);
}

Player::~Player()
{
	if( animator_ )
	{
		animator_->Release();
		animator_ = nullptr;
	}

	if( actorArray_ )
	{
		delete[] actorArray_;
		actorArray_ = nullptr;
	}

	if( stateMachine_ )
	{
		stateMachine_->Release();
		stateMachine_ = nullptr;
	}
}

void Player::update( float fTimeDelta )
{
	NxVec3	vDir = moveDir_ * speed_ * fTimeDelta;
	vDir.y += -9.81f * 9.81f * fTimeDelta;

	NxU32	dwCollisionFlag;
	cct_->move(vDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag);

	if( stateMachine_ )	stateMachine_->Update( fTimeDelta );
}

PlayerInfo Player::getPlayerInfo()
{
	SAFE_LOCK(lock_);
	PlayerInfo p;
	p.character = character_;
	p.id = session_->getID();
	p.isReady = isReady_;
	p.isMaster = isMaster_;
	return p;
}

void Player::setMaster(BOOL b)
{
	SAFE_LOCK(lock_);
	isMaster_ = b;
}

void Player::setReady(BOOL b)
{
	SAFE_LOCK(lock_);
	isReady_ = b;
}

void Player::setCharacter( CHARACTER c)
{
	SAFE_LOCK(lock_);
	character_ = c;
}

void Player::setMoveDir_State(Vector3 vDir, STATE state)
{
	NxVec3	newDir;
	newDir.x = vDir.x;
	newDir.y = vDir.y;
	newDir.z = vDir.z;

	newDir.normalize();
	if (false == newDir.isZero()) // 방향전환
	{
		NxVec3 oldLook = cct_->getActor()->getGlobalPose().M.getColumn(2);
		NxReal rotAngle = acos(oldLook.dot(newDir));
		NxVec3 cross = oldLook;
		cross = cross.cross(newDir);
		rotAngle *= (cross.y >= 0.0f) ? -1.0f : 1.0f;
		//player->setRotateY(rotAngle);
		//FIX ME!! 캐릭터 회전해줘야 함
	}

	//system_clock::time_point packetTick = system_clock::from_time_t(tick);
	//duration<double> lagTick = system_clock::now() - packetTick;
	//printf("지연시간: %lf 초\n", lagTick.count());

	//NxU32	dwCollisionFlag;
	//// 방향이 바뀌었던 시간차만큼 되돌아간다.
	//NxVec3 pastDir = moveDir_ * speed_ * (FLOAT)lagTick.count();
	////pastDir.y += -GRAVITY * GRAVITY * lagTick.count();
	//cct_->move(-pastDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag);
	//PhysXManager::getInstance().updateCCT(roomNum_);
	//
	//// 시간차만큼 다시 원래 위치로 돌아간다.
	//NxVec3 curDir = newDir * speed_ * (FLOAT)lagTick.count();
	////curDir.y += -GRAVITY * GRAVITY * lagTick.count();
	//cct_->move(curDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag);
	//PhysXManager::getInstance().updateCCT(roomNum_);

	moveDir_ = newDir;
	stateMachine_->Change_State(state);
}
