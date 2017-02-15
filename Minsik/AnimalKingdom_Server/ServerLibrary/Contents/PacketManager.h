#pragma once
#include "stdafx.h"

class Session;

class PacketManager : public Singleton<PacketManager> {
public:
	void recvProcess(Session* session, char* buf);
	void sendRejectRoom(Session* session);
	void sendRoomList(Session* session);
};