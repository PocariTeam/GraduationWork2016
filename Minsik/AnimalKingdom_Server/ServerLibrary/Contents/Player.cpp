#include "stdafx.h"
#include "Player.h"

Player::Player(Session * s, UINT room, BOOL master)
	: session_(s), c_(CHARACTER::NONE), roomNum_(room), isReady_(false), isMaster_(master)
{
}

Player::~Player()
{
}
