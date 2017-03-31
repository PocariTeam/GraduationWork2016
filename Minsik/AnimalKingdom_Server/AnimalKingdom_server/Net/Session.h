#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

/* IoData */

//////////////////////////////////////////////////////////////////////

typedef enum {
	IO_RECV,
	IO_SEND,
	IO_ERROR
} IO_OPERATION;

struct overlappedEx
{
	OVERLAPPED		overlapped_;
	WSABUF			wsaBuf_;
	IO_OPERATION	ioType_;
	char			buffer_[SOCKET_BUF_SIZE];

	overlappedEx();
};


//////////////////////////////////////////////////////////////////////

/* Session */

//////////////////////////////////////////////////////////////////////

#define NOT_IN_ROOM	-1

class Session
{
	SOCKET				socket_;
	SOCKADDR_IN			addrInfo_;
	char				packetBuffer_[PACKET_BUF_SIZE];

	oid_t				id_;
	INT32				roomNum_;

public:
	size_t  			totalBytes_;
	size_t				storedBytes_;
	overlappedEx		recvOver_;

public:
	Session();
	void			initialize();
	bool			onAccept(SOCKET socket, SOCKADDR_IN addrInfo);
	void			onRecv(size_t recvSize);
	void			recv();
	void			send(char *sendBuf);

	str_t			getAddress();
	oid_t			getID()				{ return id_; };
	void			setID(oid_t id)		{ id_ = id; };
	SOCKET&			getSocket()			{ return socket_; };
	INT32			getRoomNumber()		{ return roomNum_; };
	void			setRoomNumber(INT32 n) { roomNum_ = n; };

};