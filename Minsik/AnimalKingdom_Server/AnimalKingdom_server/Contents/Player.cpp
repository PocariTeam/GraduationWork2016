#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: lock_( L"Player" ), session_( s ), character_( CHARACTER::CHRACTER_NONE ), roomNum_( room ), isReady_( false ), isMaster_( master ), cct_( nullptr ), actorArray_( nullptr ), animator_( nullptr ), stateMachine_( nullptr )
	, m_vRotate( 0.f, 0.f, 0.f ), actorsOriginPose_( nullptr )
	, m_fJumpHeight( 100.f )
{
	stateMachine_ = CStateMachine::Create( this );
	speed_ = 50.0f;
	moveDir_ = NxVec3(0.0f, 0.0f, 0.0f);
}

Player::~Player()
{
	if( actorsOriginPose_ )
	{
		delete[] actorsOriginPose_;
		actorsOriginPose_ = nullptr;
	}

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
	moveDir_ = NxVec3{ 0.f, 0.f, 0.f };
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

void Player::setCCT( NxController* cct )
{
	cct_ = cct;
	cct_->getActor()->userData = this;
}

void Player::setActorArray( NxActor** dpActorArray, UINT actorcnt )
{
	actorArray_ = dpActorArray;
	actorCount_ = actorcnt;

	if( actorsOriginPose_ )
	{
		delete[] actorsOriginPose_;
		actorsOriginPose_ = nullptr;
	}

	actorsOriginPose_ = new NxMat34[ actorCount_ ];
	for( UINT i = 0; i < actorCount_; ++i )
	{
		actorsOriginPose_[ i ] = *( NxMat34* )actorArray_[ i ]->userData;
		delete actorArray_[ i ]->userData;
		actorArray_[ i ]->userData = this;
	}
}

XMFLOAT4X4 Player::GetWorld()
{
	XMMATRIX mtxWorld = CMathematics::ConvertToXMMatrix( &cct_->getActor()->getGlobalPose() );
	mtxWorld = XMMatrixMultiply( mtxWorld, XMMatrixRotationY( m_vRotate.y ) );
	XMFLOAT4X4 Out;

	XMStoreFloat4x4( &Out, mtxWorld );
	return Out;
}

void Player::setState(STATE state)
{
	stateMachine_->Change_State(state);
}

void Player::setMoveDir( Vector3 vDir)
{
	moveDir_.x = vDir.x;
	moveDir_.y = vDir.y;
	moveDir_.z = vDir.z;
}

void Player::Jump( const float& fTimeDelta, float fAnimatePercent )
{
	moveDir_.y += m_fJumpHeight * cos( fAnimatePercent * XM_PI ) * fTimeDelta + 9.81f * 9.81f * fTimeDelta;
}

void Player::Move( const float& fTimeDelta )
{
	if( moveDir_.isZero() ) return;

	NxVec3	vDefault_Dir{ 0.f, 0.f, 1.f };
	moveDir_.normalize();

	NxReal fRotateY = acos( vDefault_Dir.dot( moveDir_ ) );
	NxVec3 vCross = vDefault_Dir;
	vCross = vCross.cross( moveDir_ );
	fRotateY *= ( vCross.y >= 0.0f ) ? -1.0f : 1.0f;
	m_vRotate.y = fRotateY;

	moveDir_ = moveDir_ * speed_ * fTimeDelta;
}