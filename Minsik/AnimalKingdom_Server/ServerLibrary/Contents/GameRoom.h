#pragma once
#include "stdafx.h"

class Player;

class GameRoom {
private:
	list<Player*>		playerList_;
	UINT				roomNum_;
	UINT				playerCount_;
	BOOL				isPlaying_;
	Lock				lock_;
public:
	GameRoom(UINT roomNum);
	~GameRoom();
	bool		enter(Session* s);
	bool		exit(Session* s);
	bool		setPlayerReady(Session* s, BOOL b);
	bool		setPlayerCharacter(Session* s, CHARACTER c);
	RoomInfo	getRoomInfo();
	void		sendPlayerList();
};