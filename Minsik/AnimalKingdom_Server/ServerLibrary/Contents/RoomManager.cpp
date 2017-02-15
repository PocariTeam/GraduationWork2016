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

bool RoomManager::enterRoom(Session* session,UINT roomNum)
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
		SLog(L"! Wrong RoomNumber, inputNumber: %d", roomNum);
		return false;
	}
}

bool RoomManager::exitRoom(Session* session)
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

RoomInfo* RoomManager::getRoomListInfo()
{
	SAFE_LOCK(lock_);

	RoomInfo *pRoomList = new RoomInfo[GAMEROOM_CAPACITY];

	for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
	{
		pRoomList[i] = roomArray_[i]->getRoomInfo();
	}
	return pRoomList;
}
