#include "stdafx.h"
#include "PacketManager.h"

void PacketManager::recvProcess(Session* session, char* buf)
{
	HEADER *pHeader = (HEADER*)buf;
	BOOL result = false;

	switch (pHeader->packetID)
	{
	case PAK_ID::PAK_REQ_RoomList:
		if (session->getRoomNumber() == NOT_IN_ROOM)
		{
			PacketManager::sendRoomList(session);
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case PAK_ID::PAK_REQ_EnterRoom:
		result = RoomManager::getInstance().enterRoom(session, ((C_RoomEnter*)buf)->roomNumber);
		if (result)
		{
			PacketManager::sendPlayerList(session->getRoomNumber());
		}
		break;
	case PAK_ID::PAK_REQ_ExitRoom:
		result = RoomManager::getInstance().exitRoom(session);
		if (result)
		{
			PacketManager::sendRoomList(session);
		}
		break;
	case PAK_ID::PAK_REQ_Ready:
		result = RoomManager::getInstance().setPlayerReady(session, ((C_RoomReady*)buf)->ready);
		if (result)
		{
			PacketManager::sendPlayerList(session->getRoomNumber());
		}
		break;
	case PAK_ID::PAK_REQ_Character:
		result = RoomManager::getInstance().setPlayerCharacter(session, ((C_RoomCharacter*)buf)->character);
		if (result)
		{
			PacketManager::sendPlayerList(session->getRoomNumber());
		}
		break;
	case PAK_ID::PAK_REQ_StartGame:
		result = RoomManager::getInstance().startRoom(session);
		if (result)
		{
			PacketManager::sendStartGame(session->getRoomNumber());
		}
		break;
	case PAK_ID::PAK_REQ_Move:
		result = RoomManager::getInstance().moveRequestRoom(session, ((C_Move*)buf)->tick, ((C_Move*)buf)->vDir);
		break;
	default:
		SLog(L"! Unkown Packet packetID: %d", pHeader->packetID);
	}

	if (!result)
	{
		PacketManager::sendRejectRequest(session);
	}
}

void PacketManager::sendLogin(Session * session)
{
	S_Login* packet = (S_Login*)session->ioData_[IO_SEND].buffer_.data();
	packet->header.packetID = PAK_ID::PAK_ANS_Login;
	packet->header.size = sizeof(S_Login);
	packet->id = session->getID();
	session->ioData_[IO_SEND].totalBytes_ = sizeof(S_Login);

	session->send();
}

void PacketManager::sendRejectRequest(Session* session)
{
	HEADER* packet = (HEADER*)session->ioData_[IO_SEND].buffer_.data();
	packet->packetID = PAK_ID::PAK_RJT_Request;
	packet->size = sizeof(HEADER);
	session->ioData_[IO_SEND].totalBytes_ = sizeof(HEADER);

	session->send();
}

void PacketManager::sendPlayerList(UINT32 roomNum)
{
	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! bad sendPlayerList request.");
		return;
	}

	RoomManager::getInstance().sendPlayerList(roomNum);

	SLog(L"* the [%d] room's player list was sent. ", roomNum);

}

void PacketManager::sendStartGame(UINT32 roomNum)
{
	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! bad sendStartGame request.");
		return;
	}

	RoomManager::getInstance().sendStartGame(roomNum);

	SLog(L"* the [%d] room's start request was sent. ", roomNum);
}

void PacketManager::sendRoomList(Session * session)
{
	S_RoomList* packet = (S_RoomList*)session->ioData_[IO_SEND].buffer_.data();
	packet->header.packetID = PAK_ID::PAK_ANS_RoomList;
	packet->header.size = sizeof(S_RoomList);
	RoomInfo *p = RoomManager::getInstance().getRoomList();
	memcpy(packet->roomInfo, p, sizeof(RoomInfo)*GAMEROOM_CAPACITY);
	SAFE_DELETE_ARRAY(p);
	session->ioData_[IO_SEND].totalBytes_ = sizeof(S_RoomList);

	session->send();

	SLog(L"* room list was sent to [%S]", session->getAddress().c_str());

}
