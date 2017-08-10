#include "stdafx.h"
#include "Player.h"
#include "..\PhysX\PhysXManager.h"
#include <NxActor.h>

Player::Player(Session * s, UINT room, BOOL master)
	: lock_(L"Player"), session_(s), character_(CHARACTER::CHRACTER_NONE), roomNum_(room), isReady_(false), isMaster_(master), cct_(nullptr), actorArray_(nullptr), animator_(nullptr), stateMachine_(nullptr)
	, m_vRotate(0.f, 0.f, 0.f), actorsOriginPose_(nullptr), m_bSweap(false), defend(false), m_fJumpHeight(0.0f), damage_(0), speed_(0.0f), coolTime_(0.0f), skillTime_(0.0f)
{
	stateMachine_ = CStateMachine::Create( this );

	initialize();
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

void Player::initialize()
{
	ResetDir();
	setRotateY(-XM_PI);
	stateMachine_->Set_State(STATE_IDLE);
	beaten_ = true;
	hp_ = MAX_CHAR_HP;
	setStatByCharacter();
	coolTime_ = 0.0f;
	skillTime_ = 0.0f;
}

void Player::update( float fTimeDelta )
{
	// 스킬 쿨타임 체크
	if (coolTime_ > 0.0f)
	{
		coolTime_ -= fTimeDelta;
	}

	// 스킬지속시간체크
	if (skillTime_ > 0.0f)
	{
		skillTime_ -= fTimeDelta;

		if (skillTime_ < 0.0f)
		{
			RoomManager::getInstance().sendUseSkill(roomNum_, session_->getID(), false);
			skillTime_ = 0.0f;
		}
	}

	if( stateMachine_ ) stateMachine_->Update( fTimeDelta );
	checkFalling();

	moveDir_.y += -9.81f * 9.81f * fTimeDelta;
	NxU32	dwCollisionFlag;
	cct_->move( moveDir_, COLLIDABLE_MASK, 0.1f, dwCollisionFlag );
	moveDir_.y = 0.f;
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

	setStatByCharacter();
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
	case CHARACTER_BAT:
		mtxWorld._24 += 3.f;
		fScale = 0.1f; 
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
	PhysXManager::getInstance().ThrowBanana( NxVec3( mtxWorld._14 + vNormalDir.x * 5.f, mtxWorld._24 + vNormalDir.y * 5.f, mtxWorld._34 + vNormalDir.z * 5.f ), NxVec3( vNormalDir.x, vNormalDir.y, vNormalDir.z ), static_cast< COL_GROUP >( cct_->getActor()->getGroup() ), roomNum_, this);
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
	if (state == STATE_SKILL && coolTime_ > 0.0f) {
		SLog(L"left coolTime: %lf",coolTime_);
		return;
	}

	if (state == STATE_DEFEND_END)
	{
		animator_->Play();
	}
	else
	{
		stateMachine_->Change_State(state);
	}
}

bool Player::checkBlocking( NxVec3& vDir )
{
	if( !defend ) return false;
	vDir.y = 0.f;
	vDir.normalize();
	NxVec3 vDefaultDir = NxVec3( -sinf( m_vRotate.y ), 0.f, cosf( m_vRotate.y ) );
	// printf( "b %f, %f, %f\n", vDir.x, vDir.y, vDir.z );
	vDefaultDir.normalize();
	// printf( "C %f, %f, %f\n", vDefaultDir.x, vDefaultDir.y, vDefaultDir.z );
	vDefaultDir = vDefaultDir + vDir;

	if( 1.f/*sqrt( 2.0 )*/ > vDefaultDir.magnitude() )
		return true;

	return false;
}

bool Player::checkBlocking( float fRotateY )
{
	NxVec3 vDefaultDir = NxVec3( -sinf( fRotateY ), 0.f, cosf( fRotateY ) );
	return checkBlocking( vDefaultDir );
}

bool Player::checkFalling( void )
{
	if (RoomManager::getInstance().hasWinner(roomNum_)) return false;

	if( hp_ > 0 && cct_->getPosition().y < 0.f )
	{
		hp_ = 0;
		RoomManager::getInstance().loseCrown(roomNum_, this);
		RoomManager::getInstance().checkWinner( roomNum_, false );
		stateMachine_->Change_State( STATE_DOWN );
		return true;
	}
	return false;
}

void Player::proceedBeaten( int damage )
{
	if (RoomManager::getInstance().hasWinner(roomNum_)) return;

	if ((beaten_ == false) || hp_ <=0) return;

	beaten_ = false;
	hp_ -= damage;
	
	stateMachine_->Change_State( ( STATE_BEATEN1 == stateMachine_->GetPreviousState() )? STATE_BEATEN2 : STATE_BEATEN1);

	RoomManager::getInstance().loseCrown(roomNum_,this);

	if (hp_ <= 0)
	{
		hp_ = 0;
		RoomManager::getInstance().checkWinner(roomNum_, false);
		stateMachine_->Change_State( STATE_DOWN );
	}
	
}

void Player::minimizeController()
{
	NxExtendedVec3 vPos{ 0.f, 100.f, -1000.f };
	cct_->setPosition( vPos );
	( ( NxCapsuleShape* )cct_->getActor()->getShapes()[ 0 ] )->setRadius( 0.f );
	( ( NxCapsuleShape* )cct_->getActor()->getShapes()[ 0 ] )->setHeight( 0.f );
}

void Player::setStatByCharacter()
{
	switch (character_)
	{
	case CHARACTER::CHRACTER_NONE:
		m_fJumpHeight = 0;
		damage_ = 0;
		speed_ = 0;
		break;
	case CHARACTER::CHARACTER_CHM:
		m_fJumpHeight = JUMP_HEIGHT;
		damage_ = CHAMEL_DAMAGE;
		speed_ = CHAMEL_SPEED;
		break;
	case CHARACTER::CHARACTER_MON:
		m_fJumpHeight = JUMP_HEIGHT;
		damage_ = MONKEY_DAMAGE;
		speed_ = MONKEY_SPEED;
		break;
	case CHARACTER::CHARACTER_BAT:
		m_fJumpHeight = JUMP_HEIGHT;
		damage_ = MONKEY_DAMAGE;
		speed_ = MONKEY_SPEED;
		break;
	default:
		SLog(L"! unknown character type set! ");
	}
}

void Player::powerUp()
{
	int defaultDamage;
	float defaultSpeed;

	switch (character_)
	{
	case CHARACTER_CHM:
		defaultDamage = CHAMEL_DAMAGE;
		defaultSpeed = CHAMEL_SPEED;
		break;
	case CHARACTER_MON:
		defaultDamage = MONKEY_DAMAGE;
		defaultSpeed = MONKEY_SPEED;
		break;
	case CHARACTER_BAT:
		defaultDamage = MONKEY_DAMAGE;
		defaultSpeed = MONKEY_SPEED;
		break;
	}
	damage_ = int(defaultDamage * 1.5);
	speed_ = defaultSpeed * 0.5f;
	m_fJumpHeight = JUMP_HEIGHT*0.7f;
}

void Player::powerDown()
{
	setStatByCharacter();
}

void Player::setMoveDir( Vector3 vDir )
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

	else if( character_ == CHARACTER_BAT )
	{
		switch( eState )
		{
		case STATE_ATT1:
			pActor = actorArray_[ 3 ];
			break;
		case STATE_ATT2:
			pActor = actorArray_[ 6 ];
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

void Player::setHp(int hp)
{
	if (hp > MAX_CHAR_HP) hp = MAX_CHAR_HP;
	hp_ = hp;
}

void Player::UseSkill()
{
	coolTime_ = 10.f; // 임시로 쿨타임 10초, 각 스킬별 쿨타임 지정 필요

	switch (character_)
	{
	case CHARACTER::CHARACTER_CHM:
		skillTime_ = 5.0f; //  임시로 지속시간은 5초
		RoomManager::getInstance().sendUseSkill(roomNum_, session_->getID(), true);
		break;
	case CHARACTER::CHARACTER_MON:
		setHp((int)(MAX_CHAR_HP*1.2));
		break;
	case CHARACTER::CHARACTER_BAT:
		skillTime_ = 5.0f; //  임시로 지속시간은 5초
		RoomManager::getInstance().sendUseSkill(roomNum_, session_->getID(), true);
		break;
	default:
		SLog(L" undefined character type!");
		break;
	}
}
