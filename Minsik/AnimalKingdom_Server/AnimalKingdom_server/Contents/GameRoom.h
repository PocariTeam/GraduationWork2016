#pragma once
#include "stdafx.h"

#define		GAME_START_DELAY		5000

#define		UPDATE_TIME_SEC			(1.0f/60.0f)
#define		SYNC_TIME_SEC			(1.0f/60.0f)

class Player;

class GameRoom {
private:
	map<UINT, Player*>	players_;
	UINT				roomNum_;
	UINT				playerCount_;
	BOOL				isPlaying_;
	Lock				lock_;
	UINT				updateTimerID_;
	UINT				syncTimerID_;

public:
	GameRoom(UINT roomNum);
	~GameRoom();
	BOOL			enter(Session* s);
	BOOL			startRoom(Session* s);
	BOOL			setupGame();
	BOOL			exit(Session* s);
	BOOL			setPlayerReady(Session* s, BOOL b);
	BOOL			setPlayerCharacter(Session* s, CHARACTER c);
	RoomInfo		getRoomInfo();
	BOOL			moveRequest(Session * session, Vector3 vDir);
	BOOL			stateRequest(Session * session, STATE state);
	void			sendPlayerList();
	void			sendReadyGame();
	void			sendStartGame();
	UINT			getPlayerCount() { return playerCount_; }

	static void		updateTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
	static void		syncTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

	void			update( float fTimeDelta );
	void			sendPlayerSync();
	void			sendDynamicSync();
	void			sendDynamicOneSync(NxActor* actor);
	BOOL			getPlaying();
};