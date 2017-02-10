#pragma once
#include "stdafx.h"

class Session;

class PacketManager : public Singleton<PacketManager> {
public:
	void packetProcess(Session* session, char* buf);
};