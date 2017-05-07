#pragma once
#include "stdafx.h"

#define MAX_IOCP_THREAD		SIZE_64

typedef enum {
	IO_RECV,
	IO_SEND,
	EVENT_BANANA,
	IO_ERROR
} IO_OPERATION;

struct event_obj
{
	unsigned int sceneNum;
	void*		obj_ptr;

};

struct event_node
{
	event_obj				*eventObj;
	unsigned int			startTime;
	IO_OPERATION			type;
};

class cmp_event {
public:
	bool operator() (const event_node& lhs, const event_node& rhs) const
	{
		return (lhs.startTime > rhs.startTime);
	}
};

class IOCPServer : public Singleton<IOCPServer> {

	int								port_;
	int								workerThreadCount_;
	SOCKET							listenSocket_;
	HANDLE							iocp_;
	Thread							*acceptThread_;
	Thread							*timerThread_;
	array<Thread *, SIZE_64>		workerThread_;

	priority_queue<event_node, vector<event_node>, cmp_event>  timer_priority_Queue;
	mutex							timerLock_;

private:
	static DWORD WINAPI			acceptThread(LPVOID serverPtr);
	static DWORD WINAPI			workerThread(LPVOID serverPtr);
	bool						timerThread();
	bool						createListenSocket();
	void						printHostInfo();
	void						initialize();
	void						onAccept(SOCKET accepter, SOCKADDR_IN addrInfo);
	void						command(wstr_t cmd);
public:
	bool						run();
	void						pushEvent(event_obj *obj, unsigned int startTime, IO_OPERATION type);
	IOCPServer();
	~IOCPServer();
};