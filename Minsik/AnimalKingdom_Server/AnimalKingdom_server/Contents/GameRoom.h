#pragma once
#include "stdafx.h"

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
	BOOL			startGame(Session* s);
	BOOL			setupGame();
	BOOL			exit(Session* s);
	BOOL			setPlayerReady(Session* s, BOOL b);
	BOOL			setPlayerCharacter(Session* s, CHARACTER c);
	RoomInfo		getRoomInfo();
	BOOL			moveRequest(Session * session, Vector3 vDir);
	BOOL			stateRequest(Session * session, STATE state);
	void			sendPlayerList();
	void			sendStartGame();
	UINT			getPlayerCount() { return playerCount_; }

	static void		updateTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
	static void		syncTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

	void			update( float fTimeDelta );
	void			sendSync();
};