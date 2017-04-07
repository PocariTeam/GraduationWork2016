#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: lock_( L"Player" ), session_( s ), character_( CHARACTER::CHRACTER_NONE ), roomNum_( room ), isReady_( false ), isMaster_( master ), cct_( nullptr ), actorArray_( nullptr ), animator_( nullptr ), stateMachine_( nullptr )
	, m_vRotate( 0.f, 0.f, 0.f )
	, m_fJumpTime( 0.f )
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
	if( stateMachine_ ) stateMachine_->Update( fTimeDelta );

	moveDir_.y += -9.81f * 9.81f * fTimeDelta;

	NxU32	dwCollisionFlag;
	cct_->move( moveDir_, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
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
	moveDir_.x = vDir.x;
	moveDir_.y = vDir.y;
	moveDir_.z = vDir.z;

	stateMachine_->Change_State(state);
}

void Player::Jump( const float& fTimeDelta )
{
	m_fJumpTime += fTimeDelta;
	moveDir_.y += 5.f * m_fJumpTime;
}


void Player::Move( const float& fTimeDelta )
{
	if( moveDir_.isZero() ) return;

	NxVec3	vDefault_Dir{ 0.f, 0.f, 1.f };

	NxReal fRotateY = acos( vDefault_Dir.dot( moveDir_ ) );
	NxVec3 vCross = vDefault_Dir;
	vCross = vCross.cross( moveDir_ );
	fRotateY *= ( vCross.y >= 0.0f ) ? -1.0f : 1.0f;
	m_vRotate.y = fRotateY;

	moveDir_ = moveDir_ * speed_ * fTimeDelta;
}