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
	bool			enterRoom(Session* session, UINT roomNum);
	bool			exitRoom(Session* session);
	RoomInfo		*getRoomList();
	void			sendPlayerList(UINT32 roomNum);
	bool			setPlayerReady(Session* session, BOOL b);
	bool			setPlayerCharacter(Session* session, CHARACTER c);
};