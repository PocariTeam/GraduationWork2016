#pragma once
#include "stdafx.h"

class Session;

class PacketManager : public Singleton<PacketManager> {
public:
	void	recvProcess(Session* session, char* buf);
	void	sendLogin(Session* session);
	void	sendRoomList(Session* session);
	void	sendRejectRequest(Session* session);
	void	sendPlayerList(UINT roomNum);
	void	sendStartGame(UINT roomNum);
};