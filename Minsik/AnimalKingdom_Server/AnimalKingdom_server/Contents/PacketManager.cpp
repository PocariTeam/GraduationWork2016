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
		result = RoomManager::getInstance().moveRequestRoom(session, ((C_Move*)buf)->vDir, ((C_Move*)buf)->state);
		break;
	default:
		SLog(L"! Unkown Packet packetID: %d", pHeader->packetID);
	}

	if (!result)
	{
		PacketManager::sendRejectRequest(session);
	}
}

void PacketManager::sendLogin(Session *session)
{
	S_Login packet;
	packet.header.packetID = PAK_ID::PAK_ANS_Login;
	packet.header.size = sizeof(packet);
	packet.id = session->getID();

	session->send((char*)&packet);
}

void PacketManager::sendRejectRequest(Session* session)
{
	HEADER packet;
	packet.packetID = PAK_ID::PAK_RJT_Request;
	packet.size = sizeof(packet);

	session->send((char*)&packet);
}

void PacketManager::sendPlayerList(UINT roomNum)
{
	if (roomNum == NOT_IN_ROOM)
	{
		SLog(L"! bad sendPlayerList request.");
		return;
	}

	RoomManager::getInstance().sendPlayerList(roomNum);

	SLog(L"* the [%d] room's player list was sent. ", roomNum);

}

void PacketManager::sendStartGame(UINT roomNum)
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
	S_RoomList packet;
	packet.header.packetID = PAK_ID::PAK_ANS_RoomList;
	packet.header.size = sizeof(packet);
	RoomInfo *p = RoomManager::getInstance().getRoomList();
	memcpy(packet.roomInfo, p, sizeof(RoomInfo)*GAMEROOM_CAPACITY);
	SAFE_DELETE_ARRAY(p);

	session->send((char*)&packet);

	SLog(L"* room list was sent to id[%d]", session->getID());

}
