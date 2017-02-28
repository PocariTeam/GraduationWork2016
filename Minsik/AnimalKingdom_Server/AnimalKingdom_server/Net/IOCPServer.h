#pragma once
#include "stdafx.h"

#define MAX_IOCP_THREAD		SIZE_64

class IOCPServer : public Singleton<IOCPServer> {

	int								port_;
	int								workerThreadCount_;
	SOCKET							listenSocket_;
	HANDLE							iocp_;
	Thread							*acceptThread_;
	array<Thread *, SIZE_64>		workerThread_;

private:
	static DWORD WINAPI			acceptThread(LPVOID serverPtr);
	static DWORD WINAPI			workerThread(LPVOID serverPtr);
	bool						createListenSocket();
	void						printHostInfo();
	void						initialize();
	void						onAccept(SOCKET accepter, SOCKADDR_IN addrInfo);
	void						command(wstr_t cmd);
public:
	bool						run();
	IOCPServer();
	~IOCPServer();
};