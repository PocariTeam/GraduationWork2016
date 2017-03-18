#pragma once
#include "stdafx.h"

class NxController;

class Player {
	Session*		session_;
	S_CHARACTER		character_;
	UINT			roomNum_;
	BOOL			isReady_;
	BOOL			isMaster_;

	NxController*	cct_;

	Lock			lock_;

public:
	Player(Session* s, UINT room, BOOL master = false);
	~Player();
	Session*		getSession() { return session_; };
	PlayerInfo		getPlayerInfo();
	BOOL			getReady() { return isReady_; }
	BOOL			getMaster() { return isMaster_; }
	void			setMaster(BOOL b);
	void			setReady(BOOL b);
	void			setCharacter(S_CHARACTER c);
	void			setCCT(NxController* cct) { cct_ = cct; };
	NxController*	getCCT() { return cct_; }
};