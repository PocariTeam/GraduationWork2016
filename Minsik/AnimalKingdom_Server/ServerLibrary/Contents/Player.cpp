#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: session_(s), character_(CHARACTER::NONE), roomNum_(room), isReady_(false), isMaster_(master)
{
}

Player::~Player()
{
}

PlayerInfo Player::getPlayerInfo()
{
	PlayerInfo p;
	p.character = character_;
	p.id = session_->getID();
	p.isReady = isReady_;
	p.isMaster = isMaster_;
	return p;
}
