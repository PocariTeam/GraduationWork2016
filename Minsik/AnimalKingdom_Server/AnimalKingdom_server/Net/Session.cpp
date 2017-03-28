#include "stdafx.h"
#include "Session.h"

//////////////////////////////////////////////////////////////////////

/* IoData */

//////////////////////////////////////////////////////////////////////

IoData::IoData()
{
	ZeroMemory(&overlapped_, sizeof(overlapped_));
	ioType_ = IO_ERROR;

	this->clear();
}

void IoData::clear()
{
	packetBuffer_.fill(0);
	buffer_.fill(0);
	totalBytes_ = 0;
	currentBytes_ = 0;
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
	ioData_[IO_RECV].ioType_ = IO_RECV;
	ioData_[IO_SEND].ioType_ = IO_SEND;
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

void Session::onRecv(size_t recvSize)
{
	// 패킷조립 및 실행
	IoData *ioData = &ioData_[IO_RECV];
	char *recvBuf = ioData->buffer_.data();

	while (0 < recvSize)
	{
		char* packetBuffer = ioData->packetBuffer_.data() + ioData->currentBytes_;

		if (0 == ioData->totalBytes_)
		{
			if (recvSize + ioData->currentBytes_ >= sizeof(HEADER))
			{
				int restHeaderSize = sizeof(HEADER) - ioData->currentBytes_;
				memcpy(packetBuffer, recvBuf, restHeaderSize);
				recvBuf += restHeaderSize;
				ioData->currentBytes_ += restHeaderSize; 
				packetBuffer += ioData->currentBytes_;
				recvSize -= restHeaderSize;
				ioData->totalBytes_ = ((HEADER*)ioData->packetBuffer_.data())->size;
			}
			else
			{
				memcpy(packetBuffer, recvBuf, recvSize);
				ioData->currentBytes_ += recvSize;
				recvSize = 0;
				break;
			}
		}

		int restSize = ioData->totalBytes_ - ioData->currentBytes_;

		if (restSize <= recvSize)
		{
			memcpy(packetBuffer, recvBuf, restSize);

			PacketManager::getInstance().recvProcess(this, ioData->packetBuffer_.data());

			ioData->totalBytes_ = ioData->currentBytes_ = 0;

			recvBuf += restSize;
			recvSize -= restSize;
		}
		else
		{
			memcpy(packetBuffer, recvBuf, recvSize);

			ioData->currentBytes_ += recvSize;
			recvSize = 0;
		}
	}

	// 다시 Recv()호출
	this->recv();

}


void Session::recv()
{
	ioData_[IO_RECV].clear();

	WSABUF wsaBuf;
	wsaBuf.buf = ioData_[IO_RECV].buffer_.data();
	wsaBuf.len = SOCKET_BUF_SIZE;

	DWORD flags = 0;
	DWORD recvBytes;
	DWORD retval = WSARecv(socket_, &wsaBuf, 1, &recvBytes, &flags, &ioData_[IO_RECV].overlapped_, NULL);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			SLog(L"! socket error: %d", WSAGetLastError());
		}
	}
}

void Session::send()
{
	WSABUF wsaBuf;
	wsaBuf.buf = ioData_[IO_SEND].buffer_.data();
	wsaBuf.len = ioData_[IO_SEND].totalBytes_;

	DWORD flags = 0;
	DWORD sendBytes;
	DWORD errorCode = WSASend(socket_, &wsaBuf, 1, &sendBytes, flags, &ioData_[IO_SEND].overlapped_, NULL);
	if (errorCode == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
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
