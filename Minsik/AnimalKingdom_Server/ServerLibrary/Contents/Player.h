#pragma once
#include "stdafx.h"

typedef enum {
	NONE
}CHARACTER;

class Player {
	Session*	session_;
	CHARACTER	c_;
	UINT		roomNum_;
	BOOL		isReady_;
	BOOL		isMaster_;
public:
	Player(Session* s, UINT room, BOOL master = false);
	~Player();
	Session* getSession() { return session_; };
};