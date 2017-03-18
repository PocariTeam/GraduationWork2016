#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: lock_(L"Player"), session_(s), character_(S_CHARACTER::NONE), roomNum_(room), isReady_(false), isMaster_(master), cct_(nullptr)
{
}

Player::~Player()
{
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

void Player::setCharacter(S_CHARACTER c)
{
	SAFE_LOCK(lock_);
	character_ = c;
}
