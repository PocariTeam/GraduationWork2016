#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

/* IoData */

//////////////////////////////////////////////////////////////////////

typedef enum {
	IO_RECV,
	IO_SEND,
	IO_ERROR,
} IO_OPERATION;
#define IO_DATA_MAX     (2)

struct IoData
{
	OVERLAPPED		overlapped_;
	IO_OPERATION	ioType_;
	size_t  		totalBytes_;
	size_t			currentBytes_;
	array<char, SOCKET_BUF_SIZE> buffer_;
	array<char, SOCKET_BUF_SIZE> packetBuffer_;

	IoData();
	void clear();
};


//////////////////////////////////////////////////////////////////////

/* Session */

//////////////////////////////////////////////////////////////////////



class Session
{
	SOCKET				socket_;
	SOCKADDR_IN			addrInfo_;
	oid_t				id_;

public:
	array<IoData, IO_DATA_MAX> ioData_;

	Session();
	void			initialize();
	bool			onAccept(SOCKET socket, SOCKADDR_IN addrInfo);
	void			onRecv(size_t recvSize);
	void			recv();

	str_t			getAddress();
	oid_t			getID()				{ return id_; };
	void			setID(oid_t id)		{ id_ = id; };
	SOCKET&			getSocket()			{ return socket_; };

};