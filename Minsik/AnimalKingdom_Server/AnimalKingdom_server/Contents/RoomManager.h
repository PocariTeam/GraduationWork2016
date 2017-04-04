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

	void			update( UINT roomNum, float fTimeDelta );
	void			sendSync(UINT roomNum);

	BOOL			setPlayerReady(Session* session, BOOL b);
	BOOL			setPlayerCharacter(Session* session, CHARACTER c);
	BOOL			startRoom(Session* session);
	BOOL			setupRoom(UINT32 roomNum);

	BOOL			moveRequestRoom(Session* session, Vector3 vDir, STATE state);

	UINT			getPlayerCountRoom(UINT32 roomNum);

	void			sendPlayerList(UINT32 roomNum);
	void			sendStartGame(UINT32 roomNum);
};