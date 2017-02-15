#pragma once
#include "stdafx.h"

#define PLAYER_CAPACITY 4

class Player;

class GameRoom {
private:
	list<Player*> playerList_;
	UINT	roomNum_;
	UINT	playerCount_;
	BOOL	playing_;
	Lock	lock_;
public:
	GameRoom(UINT roomNum);
	~GameRoom();
	bool enter(Session* session);
	bool exit(Session* session);
	RoomInfo getRoomInfo();
};