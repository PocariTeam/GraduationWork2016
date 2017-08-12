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
	void			sendPlayerSync(UINT roomNum);
	void			sendDynamicSync(UINT roomNum);
	void			sendDynamicOneSync(UINT roomNum, NxActor* actor);

	BOOL			setPlayerReady(Session* session, BOOL b);
	BOOL			setPlayerCharacter(Session* session, CHARACTER c);
	BOOL			startRoom(Session* session);
	BOOL			setupRoom(UINT32 roomNum);

	BOOL			moveRequestRoom(Session* session, Vector3 vDir);
	BOOL			stateRequestRoom(Session* session, STATE state);

	UINT			getPlayerCountRoom(UINT32 roomNum);

	void			sendPlayerList(UINT32 roomNum);
	void			sendReadyGame(UINT32 roomNum);
	void			sendGetCrown(UINT roomNum, Player* player);
	void			sendUseSkill(UINT roomNum, int id, bool use);
	void			sendDefendEnd(UINT roomNum, int id);

	BOOL			getPlaying(UINT roomNum);
	BOOL			hasWinner(UINT roomNum);

	void			loseCrown(UINT roomNum, Player* player);
	void			checkWinner(UINT roomNum, bool timeOut);
};