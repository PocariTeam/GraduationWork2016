#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: lock_( L"Player" ), session_( s ), character_( CHARACTER::CHRACTER_NONE ), roomNum_( room ), isReady_( false ), isMaster_( master ), cct_( nullptr ), actorArray_( nullptr ), animator_( nullptr ), stateMachine_( nullptr )
	, m_vRotate( 0.f, 0.f, 0.f )
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

XMFLOAT4X4 Player::GetWorld()
{
	XMMATRIX mtxWorld = CMathematics::ConvertToXMMatrix( &cct_->getActor()->getGlobalPose() );
	mtxWorld = XMMatrixMultiply( mtxWorld, XMMatrixRotationY( m_vRotate.y ) );
	XMFLOAT4X4 Out;

	XMStoreFloat4x4( &Out, mtxWorld );

	return Out;
}

void Player::setMoveDir_State( Vector3 vDir, STATE state )
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
		m_vRotate.y = rotAngle;
		//FIX ME!! 캐릭터 회전해줘야 함
	}

	moveDir_ = newDir;
	stateMachine_->Change_State(state);
}
