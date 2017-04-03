#include "stdafx.h"
#include "Session.h"

//////////////////////////////////////////////////////////////////////

/* IoData */

//////////////////////////////////////////////////////////////////////

overlappedEx::overlappedEx()
{
	ZeroMemory(&overlapped_, sizeof(overlapped_));
	ioType_ = IO_ERROR;
}

//////////////////////////////////////////////////////////////////////

/* Session */

//////////////////////////////////////////////////////////////////////

Session::Session()
{
	this->initialize();
}

void Session::initialize()
{
	ZeroMemory(&socket_, sizeof(socket_));
	ZeroMemory(&addrInfo_, sizeof(addrInfo_));
	ZeroMemory(packetBuffer_, sizeof(packetBuffer_));
	recvOver_.wsaBuf_.buf = recvOver_.buffer_;
	recvOver_.wsaBuf_.len = sizeof(recvOver_.buffer_);
	recvOver_.ioType_ = IO_OPERATION::IO_RECV;
	totalBytes_ = storedBytes_ = 0;
	id_ = -1;
	roomNum_ = NOT_IN_ROOM;
}

bool Session::onAccept(SOCKET socket, SOCKADDR_IN addrInfo)
{
	socket_ = socket;
	int addrLen;
	getpeername(socket_, (struct sockaddr *)&addrInfo_, &addrLen);
	addrInfo_ = addrInfo;

	return true;
}

void Session::onRecv(UINT recvSize)
{
	// 패킷조립 및 실행
	char *recvBuf = recvOver_.buffer_;

	while (0 < recvSize)
	{
		if (0 == totalBytes_)
		{
			if (recvSize + storedBytes_ >= sizeof(HEADER))
			{
				UINT restHeaderSize = sizeof(HEADER) - storedBytes_;
				memcpy(packetBuffer_ + storedBytes_, recvBuf, restHeaderSize);
				recvBuf += restHeaderSize;
				storedBytes_ += restHeaderSize; 
				recvSize -= restHeaderSize;
				totalBytes_ = ((HEADER*)packetBuffer_)->size;

				if (totalBytes_ <= 0)
				{
					SErrLog(L" totalBytes is under 0 !");
					break;
				}
			}
			else
			{
				memcpy(packetBuffer_ + storedBytes_, recvBuf, recvSize);
				storedBytes_ += recvSize;
				recvSize = 0;
				break;
			}
		}

		UINT restSize = totalBytes_ - storedBytes_;

		if (restSize <= recvSize)
		{
			memcpy(packetBuffer_ + storedBytes_, recvBuf, restSize);

			PacketManager::getInstance().recvProcess(this, packetBuffer_);

			totalBytes_ = storedBytes_ = 0;

			recvBuf += restSize;
			recvSize -= restSize;
		}
		else
		{
			memcpy(packetBuffer_ + storedBytes_, recvBuf, recvSize);

			storedBytes_ += recvSize;
			recvSize = 0;
		}
	}

	// 다시 Recv()호출
	this->recv();

}


void Session::recv()
{
	DWORD flags = 0;
	DWORD recvBytes;
	DWORD retval = WSARecv(socket_, &recvOver_.wsaBuf_, 1, &recvBytes, &flags, &recvOver_.overlapped_, NULL);
	if (retval == SOCKET_ERROR) 
	{
		if (WSAGetLastError() != ERROR_IO_PENDING) 
		{
			SLog(L"! socket error: %d", WSAGetLastError());
		}
	}
}

void Session::send(char *sendBuf)
{
	overlappedEx *sendOver = new overlappedEx;
	sendOver->ioType_ = IO_OPERATION::IO_SEND;
	sendOver->wsaBuf_.buf = sendOver->buffer_;
	sendOver->wsaBuf_.len = ((HEADER*)sendBuf)->size;
	memcpy(sendOver->buffer_, sendBuf, sendOver->wsaBuf_.len);
	DWORD errorCode = WSASend(socket_, &sendOver->wsaBuf_, 1, NULL, 0, &sendOver->overlapped_, NULL);
	if (errorCode == SOCKET_ERROR) 
	{
		if (WSAGetLastError() != WSA_IO_PENDING) 
		{
			SLog(L"! socket error: %d", WSAGetLastError());
			return;
		}
	}

}

str_t Session::getAddress()
{
	array<char, SIZE_64> ip;
	inet_ntop(AF_INET, &(addrInfo_.sin_addr), ip.data(), ip.size());
	return ip.data();
}
