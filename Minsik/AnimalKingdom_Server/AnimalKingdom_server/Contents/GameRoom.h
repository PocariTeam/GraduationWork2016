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
	UINT				timerID_;
public:
	GameRoom(UINT roomNum);
	~GameRoom();
	BOOL		enter(Session* s);
	BOOL		startGame(Session* s);
	BOOL		setupGame();
	BOOL		exit(Session* s);
	BOOL		setPlayerReady(Session* s, BOOL b);
	BOOL		setPlayerCharacter(Session* s, S_CHARACTER c);
	RoomInfo	getRoomInfo();
	BOOL		moveRequest(Session * session, time_t tick, Vector3 vDir);
	void		sendPlayerList();
	void		sendStartGame();
	void		sendMovePacket(UINT32 id, time_t tick, Vector3 vDir);
	UINT		getPlayerCount() { return playerCount_; }

	static void updateTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

	
};