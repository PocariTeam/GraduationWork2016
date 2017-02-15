#include "stdafx.h"
#include "GameRoom.h"

GameRoom::GameRoom(UINT num)
	: lock_(L"GameRoom"), playing_(false), playerCount_(0), roomNum_(num)
{
}

GameRoom::~GameRoom()
{
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		SAFE_DELETE(*iter);
	}
}

bool GameRoom::enter(Session* session)
{
	SAFE_LOCK(lock_);

	if (playing_)
	{
		SLog(L"! [%d] room is playing now.", roomNum_);
		return false;
	}

	if (playerCount_ < PLAYER_CAPACITY)
	{
		BOOL isMaster;
		isMaster = (playerCount_ == 0) ? true : false;
		playerList_.push_back(new Player(session, roomNum_, isMaster));
		++playerCount_;
		session->setRoomNumber(roomNum_);
		SLog(L"* [%S] enters the [%d] room. ", session->getAddress().c_str(),roomNum_ );
		return true;
	}
	else
	{
		SLog(L"! PlayerCount Overflow, currentCount: %d", playerCount_);
		return false;
	}
}

bool GameRoom::exit(Session* session)
{
	SAFE_LOCK(lock_);
	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			SLog(L"* [%S] exits from the [%d] room. ", session->getAddress().c_str(), roomNum_);
			playerList_.remove(*p);
			--playerCount_;
			session->setRoomNumber(NOT_IN_ROOM);
			return true;
		}
	}
	return false;
}

RoomInfo GameRoom::getRoomInfo()
{
	SAFE_LOCK(lock_);
	RoomInfo r;
	r.playerCount = playerCount_;
	r.playing = playing_;
	return r;
}
