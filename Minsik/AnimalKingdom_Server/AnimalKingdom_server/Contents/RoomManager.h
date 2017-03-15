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
	BOOL			enterRoom(Session* session, UINT roomNum);
	BOOL			exitRoom(Session* session);
	RoomInfo		*getRoomList();

	BOOL			setPlayerReady(Session* session, BOOL b);
	BOOL			setPlayerCharacter(Session* session, S_CHARACTER c);
	BOOL			startRoom(Session* session);

	void			sendPlayerList(UINT32 roomNum);
	void			sendStartGame(UINT32 roomNum);
};