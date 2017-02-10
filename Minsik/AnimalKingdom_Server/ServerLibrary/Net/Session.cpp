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
	char *recvBuf = ioData_[IO_RECV].buffer_.data();
	IoData *ioData = &ioData_[IO_RECV];
	
	while (0 < recvSize) 
	{
		//현재 처리하는 패킷이 없을 경우 recvBuf의 첫번째 바이트를 사이즈로 설정
		if (0 == ioData->totalBytes_) {
			// FIX: 항상 2바이트 이상 읽는다는 가정하에 짜여진 코드
			HEADER* pHeader = (HEADER*)recvBuf;
			ioData->totalBytes_ = pHeader->size;
		}

		// 패킷을 만들기 위해 필요한 남은 사이즈 = 
		//	현재 받아야할 패킷사이즈 - 현재까지 저장한 패킷사이즈
		UINT restSize = ioData->totalBytes_ - ioData->currentBytes_;

		char *packetBuffer = ioData->packetBuffer_.data() + ioData->currentBytes_;

		if (restSize <= recvSize)			// 패킷 조립
		{
			memcpy(packetBuffer, recvBuf, restSize);

			PacketManager::getInstance().packetProcess(this, ioData->packetBuffer_.data());
			
			recvBuf += restSize;
			recvSize -= restSize;
		}
		else								// 패킷 저장
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

str_t Session::getAddress()
{
	array<char, SIZE_64> ip;
	inet_ntop(AF_INET, &(addrInfo_.sin_addr), ip.data(), ip.size());
	return ip.data();
}
