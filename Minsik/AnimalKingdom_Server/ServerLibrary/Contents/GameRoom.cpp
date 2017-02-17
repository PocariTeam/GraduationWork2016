#include "stdafx.h"
#include "GameRoom.h"

GameRoom::GameRoom(UINT num)
	: lock_(L"GameRoom"), isPlaying_(false), playerCount_(0), roomNum_(num)
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

	if (isPlaying_)
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

			if (playerCount_ > 0)
			{
				(*playerList_.begin())->setMaster(true);
			}

			PacketManager::getInstance().sendPlayerList(session->getRoomNumber());
			session->setRoomNumber(NOT_IN_ROOM);

			return true;
		}
	}
	return false;
}

bool GameRoom::setPlayerReady(Session* session, BOOL b)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now. ", roomNum_);
		return false;
	}

	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			(*p)->setReady(b);
			SLog(L"* [%S] got ready[%d] in the [%d] room. ", session->getAddress().c_str(), b, roomNum_);
			return true;
		}
	}
	SLog(L"! [%S] is not in the [%d] room. ", session->getAddress().c_str(), roomNum_);

	return false;
}

bool GameRoom::setPlayerCharacter(Session * session, CHARACTER ch)
{
	SAFE_LOCK(lock_);

	if (isPlaying_)
	{
		SLog(L"! the [%d] room is playing now. ", roomNum_);
		return false;
	}

	for (auto p = playerList_.begin(); p != playerList_.end(); p++)
	{
		if ((*p)->getSession() == session)
		{
			if ((*p)->getReady())
			{
				SLog(L"! [%S] is ready now. so it cannot be changed.", session->getAddress().c_str());
				return false;
			}
			if (ch < 0 || ch > CHARACTER_MAX)
			{
				SLog(L"! wrong character. inputCharacter: %d", ch);
				return false;
			}
			(*p)->setCharacter(ch);
			SLog(L"* [%S] chose character[%d] in the [%d] room. ", session->getAddress().c_str(), ch, roomNum_);
			return true;
		}
	}
	SLog(L"! [%S] is not in the [%d] room. ", session->getAddress().c_str(), roomNum_);

	return false;
}

RoomInfo GameRoom::getRoomInfo()
{
	SAFE_LOCK(lock_);
	RoomInfo r;
	r.playerCount = playerCount_;
	r.playing = isPlaying_;
	return r;
}

void GameRoom::sendPlayerList()
{
	SAFE_LOCK(lock_);
	PlayerInfo *pList = new PlayerInfo[PLAYER_CAPACITY];

	int i = 0;
	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		pList[i] = (*iter)->getPlayerInfo();
		i++;
	}

	PlayerInfo p;
	ZeroMemory(&p, sizeof(PlayerInfo));
	while (i < PLAYER_CAPACITY)
	{
		pList[i] = p;
		i++;
	}

	for (auto iter = playerList_.begin(); iter != playerList_.end(); iter++)
	{
		Session* session = (*iter)->getSession();
		S_PlayerList* packet = (S_PlayerList*)session->ioData_[IO_SEND].buffer_.data();
		packet->header.packetID = PAK_ID::PAK_ANS_PlayerList;
		packet->header.size = sizeof(S_PlayerList);
		packet->playerCount = playerCount_;
		memcpy(packet->playerInfo, pList, sizeof(PlayerInfo)*PLAYER_CAPACITY);
		session->ioData_[IO_SEND].totalBytes_ = sizeof(S_PlayerList);
		session->send();
	}

	SAFE_DELETE_ARRAY(pList);
	
}
