#pragma once
#include "stdafx.h"

class GameRoom;
class Session;

class RoomManager : public Singleton<RoomManager> {
private:
	array<GameRoom*, GAMEROOM_CAPACITY>		roomArray_;
	Lock									lock_;
public:
	RoomManager();
	~RoomManager();
	bool enterRoom(Session* session, UINT roomNum);
	bool exitRoom(Session* session);
	RoomInfo *getRoomListInfo();
};