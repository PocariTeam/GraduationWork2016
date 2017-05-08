#include "stdafx.h"
#include "Player.h"
#include "..\PhysX\PhysXManager.h"
#include <NxActor.h>

Player::Player(Session * s, UINT room, BOOL master)
	: lock_( L"Player" ), session_( s ), character_( CHARACTER::CHRACTER_NONE ), roomNum_( room ), isReady_( false ), isMaster_( master ), cct_( nullptr ), actorArray_( nullptr ), animator_( nullptr ), stateMachine_( nullptr )
	, m_vRotate( 0.f, 0.f, 0.f ), actorsOriginPose_( nullptr ), m_bSweap( false )
	, m_fJumpHeight( 130.f ), damage_( 50 ), beaten_( true )
{
	stateMachine_ = CStateMachine::Create( this );
	speed_ = 80.0f;
	moveDir_ = NxVec3(0.0f, 0.0f, 0.0f);
	hp_ = 100;
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
	if( 0 > hp_ ) stateMachine_->Change_State( STATE_DOWN );

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
	XMFLOAT4X4 mtxWorld = CMathematics::ConvertToXMFloat4x4( &cct_->getActor()->getGlobalPose() );
	
	float	   fScale{ 1.f };
	switch( character_ )
	{
	case CHARACTER_CHM:
		mtxWorld._24 -= 1.1f;
		fScale = 1.5f;
		break;
	case CHARACTER_MON:
		mtxWorld._24 -= 2.5f;
		fScale = 21.209435f;
		break;
	default:
		break;
	}

	XMMATRIX   mtxLoadWorld = XMMatrixMultiply( XMLoadFloat4x4( &mtxWorld ), XMMatrixRotationY( m_vRotate.y ) * XMMatrixScaling( fScale, fScale, fScale ) );
	XMStoreFloat4x4( &mtxWorld, mtxLoadWorld );
	return mtxWorld;
}

void Player::ThrowBanana( void )
{
	XMFLOAT4X4	mtxWorld = GetWorld();
	XMVECTOR vDir = XMVector3Normalize( XMLoadFloat3( &XMFLOAT3( mtxWorld._13, mtxWorld._23, mtxWorld._33 ) ) );
	XMFLOAT3 vNormalDir;
	XMStoreFloat3( &vNormalDir, vDir );
	PhysXManager::getInstance().ThrowBanana( NxVec3( mtxWorld._14 + vNormalDir.x * 5.f, mtxWorld._24 + vNormalDir.y * 5.f, mtxWorld._34 + vNormalDir.z * 5.f ), NxVec3( vNormalDir.x, vNormalDir.y, vNormalDir.z ), static_cast< COL_GROUP >( cct_->getActor()->getGroup() ), roomNum_ );
}

void Player::sweapOn( void )
{
	m_bSweap = true;
}

void Player::sweapOff( void )
{
	m_bSweap = false;
}

void Player::setState( STATE state )
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

void Player::Attack( STATE eState )
{
	if( !m_bSweap ) return;

	NxActor*	pActor{};

	if( character_ == CHARACTER_CHM )
	{
		switch( eState )
		{
		case STATE_ATT1:
			pActor = actorArray_[ 2 ];
			break;
		case STATE_ATT2:
			pActor = actorArray_[ 1 ];
			break;
		default:
			return;
		}
	}

	if( pActor )
	{
		NxSweepQueryHit result[ 100 ];
		UINT MyGroup = UINT( pActor->getGroup() );
		LPVOID pUserdata = this;
		pActor->linearSweep( pActor->getShapes()[ 0 ]->getGlobalPosition(), NX_SF_DYNAMICS, pUserdata, 50, result,
			( NxUserEntityReport<NxSweepQueryHit>* )&PhysXManager::getInstance().entityReport_ );
	}
}
