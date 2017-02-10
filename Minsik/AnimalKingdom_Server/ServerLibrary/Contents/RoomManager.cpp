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

void RoomManager::enterRoom(Session* session,UINT roomNum)
{
	if (roomNum < GAMEROOM_CAPACITY) 
	{
		roomArray_[roomNum]->enter(session);
	}
	else
	{
		SLog(L"! RoomNumber Overflow, inputNumber: %d", roomNum);
	}
}

void RoomManager::exitRoom(Session* session)
{
	for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
	{
		if (roomArray_[i]->exit(session))
		{
			return;
		}
	}

	SLog(L"! There is no [%S] in every room. ", session->getAddress().c_str());

}
