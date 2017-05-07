#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

/* IoData */

//////////////////////////////////////////////////////////////////////


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

	UINT				id_;
	INT				roomNum_;

public:
	UINT  				totalBytes_;
	UINT				storedBytes_;
	overlappedEx		recvOver_;

public:
	Session();
	void			initialize();
	bool			onAccept(SOCKET socket, SOCKADDR_IN addrInfo);
	void			onRecv(UINT recvSize);
	void			recv();
	void			send(char *sendBuf);

	str_t			getAddress();
	UINT			getID()				{ return id_; };
	void			setID(UINT id)		{ id_ = id; };
	SOCKET&			getSocket()			{ return socket_; };
	INT				getRoomNumber()		{ return roomNum_; };
	void			setRoomNumber(INT32 n) { roomNum_ = n; };

};