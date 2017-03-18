#include "stdafx.h"
#include "RoomManager.h"

RoomManager::RoomManager()
	: lock_(L"RoomManager")
{
	for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
	{
		roomArray_[i] = new GameRoom(i);
	}
}

RoomManager::~RoomManager()
{
	for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
	{
		SAFE_DELETE(roomArray_[i]);
	}
}

BOOL RoomManager::enterRoom(Session* session,UINT roomNum)
{
	SAFE_LOCK(lock_);

	if (session->getRoomNumber() != NOT_IN_ROOM)
	{
		SLog(L"! [%S] is in already in the [%d] room.", session->getAddress().c_str(), session->getRoomNumber());
		return false;
	}

	if (roomNum < GAMEROOM_CAPACITY) 
	{
		return roomArray_[roomNum]->enter(session);
	}
	else
	{
		SLog(L"! wrong room number, inputNumber: %d", roomNum);
		return false;
	}
}

BOOL RoomManager::exitRoom(Session* session)
{
	SAFE_LOCK(lock_);

	INT32 roomNum = session->getRoomNumber();

	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! [%S] is NOT in room.", session->getAddress().c_str());
		return false;
	}

	if (roomNum < GAMEROOM_CAPACITY)
	{
		return roomArray_[roomNum]->exit(session);
	}
	else
	{
		SLog(L"! [%S]'s roomNum_ is over GAMEROOM_CAPACITY. ", session->getAddress().c_str());
		return false;
	}

}

RoomInfo* RoomManager::getRoomList()
{
	SAFE_LOCK(lock_);

	RoomInfo *pRoomList = new RoomInfo[GAMEROOM_CAPACITY];

	for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
	{
		pRoomList[i] = roomArray_[i]->getRoomInfo();
	}
	return pRoomList;
}

void RoomManager::sendPlayerList(UINT32 roomNum)
{
	SAFE_LOCK(lock_);

	roomArray_[roomNum]->sendPlayerList();
}

void RoomManager::sendStartGame(UINT32 roomNum)
{
	SAFE_LOCK(lock_);

	roomArray_[roomNum]->sendStartGame();
}

BOOL RoomManager::setPlayerReady(Session* session, BOOL b)
{
	SAFE_LOCK(lock_);

	UINT roomNum = session->getRoomNumber();
	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! [%S] is NOT in room.", session->getAddress().c_str());
		return false;
	}
	return roomArray_[roomNum]->setPlayerReady(session, b);
}

BOOL RoomManager::setPlayerCharacter(Session * session, S_CHARACTER c)
{
	SAFE_LOCK(lock_);

	UINT roomNum = session->getRoomNumber();
	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! [%S] is NOT in room.", session->getAddress().c_str());
		return false;
	}

	return roomArray_[roomNum]->setPlayerCharacter(session, c);
}

BOOL RoomManager::startRoom(Session * session)
{
	SAFE_LOCK(lock_);

	UINT roomNum = session->getRoomNumber();
	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! [%S] is NOT in room.", session->getAddress().c_str());
		return false;
	}

	return roomArray_[roomNum]->startGame(session);
}

BOOL RoomManager::setupRoom(UINT32 roomNum)
{
	SAFE_LOCK(lock_);

	if (roomNum < GAMEROOM_CAPACITY)
	{
		return roomArray_[roomNum]->setupGame();
	}
	else
	{
		SLog(L"! wrong room number, inputNumber: %d", roomNum);
		return false;
	}
}

BOOL RoomManager::moveRequestRoom(Session* session, time_t tick, Vector3 vDir)
{
	SAFE_LOCK(lock_);
	UINT roomNum = session->getRoomNumber();
	if (roomNum < GAMEROOM_CAPACITY)
	{
		return roomArray_[roomNum]->moveRequest(session, tick, vDir);
	}
	else
	{
		SLog(L"! wrong room number, inputNumber: %d", roomNum);
		return false;
	}
}

UINT RoomManager::getPlayerCountRoom(UINT32 roomNum)
{
	SAFE_LOCK(lock_);

	if (roomNum < GAMEROOM_CAPACITY)
	{
		return roomArray_[roomNum]->getPlayerCount();
	}
	else
	{
		SLog(L"! wrong room number, inputNumber: %d", roomNum);
		return false;
	}

	return 0;
}
