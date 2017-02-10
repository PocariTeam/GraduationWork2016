#include "stdafx.h"
#include "PacketManager.h"

void PacketManager::packetProcess(Session* session, char* buf)
{
	HEADER *pHeader = (HEADER*)buf;

	switch (pHeader->packetID)
	{
	case PAK_EnterRoom:
	{
		C_EnterRoom *pPacket = (C_EnterRoom*)buf;
		RoomManager::getInstance().enterRoom(session, pPacket->roomNumber);
		break;
	}
	case PAK_ExitRoom:
	{
		RoomManager::getInstance().exitRoom(session);
		break;
	}
	default:
		SLog(L"! Unkown Packet ID: %d", pHeader->packetID);
	}
}
