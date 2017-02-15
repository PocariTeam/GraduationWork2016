#include "stdafx.h"
#include "PacketManager.h"

void PacketManager::recvProcess(Session* session, char* buf)
{
	HEADER *pHeader = (HEADER*)buf;
	BOOL result = false;

	switch (pHeader->packetID)
	{
	case PAK_ID::PAK_REQ_EnterRoom:
		result = RoomManager::getInstance().enterRoom(session, ((C_EnterRoom*)buf)->roomNumber);
		if (!result)
		{
			PacketManager::sendRoomList(session);
		}
		break;
	case PAK_ID::PAK_REQ_ExitRoom:
		result = RoomManager::getInstance().exitRoom(session);
		if (result)
		{
			PacketManager::sendRoomList(session);
		}
		break;
	default:
		SLog(L"! Unkown Packet ID: %d", pHeader->packetID);
	}
	
	if (!result)
	{
		PacketManager::sendRejectRoom(session);
	}
}

void PacketManager::sendRejectRoom(Session* session)
{
	HEADER* packet = (HEADER*)session->ioData_[IO_SEND].buffer_.data();
	packet->packetID = PAK_ID::PAK_RJT_Room;
	packet->size = sizeof(HEADER);
	session->ioData_[IO_SEND].totalBytes_ = sizeof(HEADER);

	session->send();
}

void PacketManager::sendRoomList(Session * session)
{
	S_RoomListInfo* packet = (S_RoomListInfo*)session->ioData_[IO_SEND].buffer_.data();
	packet->header.packetID = PAK_ID::PAK_ANS_RoomList;
	packet->header.size = sizeof(S_RoomListInfo);
	RoomInfo *p = RoomManager::getInstance().getRoomListInfo();
	memcpy(packet->roomInfo, p, sizeof(RoomInfo)*GAMEROOM_CAPACITY);
	SAFE_DELETE_ARRAY(p);
	session->ioData_[IO_SEND].totalBytes_ = sizeof(S_RoomListInfo);

	session->send();
}
