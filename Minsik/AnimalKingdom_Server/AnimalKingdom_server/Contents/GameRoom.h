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
	BOOL		exit(Session* s);
	BOOL		setPlayerReady(Session* s, BOOL b);
	BOOL		setPlayerCharacter(Session* s, S_CHARACTER c);
	RoomInfo	getRoomInfo();
	void		sendPlayerList();
	void		sendStartGame();

	static void updateTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

	
};