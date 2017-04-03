#pragma once
#include "stdafx.h"

#define SESSION_CAPACITY		(5000)

class Session;

class SessionManager : public Singleton<SessionManager>{
	list<Session*>				sessionList_;
	UINT						idSeed_;
	int						    sessionCount_;
	Lock                        lock_;

public:
	SessionManager();
	~SessionManager();
	bool				addSession(Session *session);
	bool				closeSession(Session *session);
	void				forceCloseSession(Session *session);
};