#pragma once
#include "stdafx.h"

class Player {
	Session*	session_;
	CHARACTER	character_;
	UINT		roomNum_;
	BOOL		isReady_;
	BOOL		isMaster_;
	Lock		lock_;

public:
	Player(Session* s, UINT room, BOOL master = false);
	~Player();
	Session*	getSession() { return session_; };
	PlayerInfo	getPlayerInfo();
	BOOL		getReady() { return isReady_; }
	BOOL		getMaster() { return isMaster_; }
	void		setMaster(BOOL b);
	void		setReady(BOOL b);
	void		setCharacter(CHARACTER c);
};