#pragma once
#include "stdafx.h"

class Player {
	Session*	session_;
	CHARACTER	character_;
	UINT		roomNum_;
	BOOL		isReady_;
	BOOL		isMaster_;
public:
	Player(Session* s, UINT room, BOOL master = false);
	~Player();
	Session*	getSession() { return session_; };
	PlayerInfo	getPlayerInfo();
	void		setMaster(BOOL b) { isMaster_ = b; };
	void		setReady(BOOL b) { isReady_ = b; };
	void		setCharacter(CHARACTER c) { character_ = c; };
	BOOL		getReady() { return isReady_; }
};