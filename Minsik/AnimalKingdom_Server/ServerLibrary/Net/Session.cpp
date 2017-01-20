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
	buffer_.fill(0);
	totalBytes_ = 0;
	currentBytes_ = 0;
}

bool IoData::needMoreIO(size_t transferSize)
{
	currentBytes_ += transferSize;
	if (currentBytes_ < totalBytes_) {
		return true;
	}
	return false;
}

int32_t IoData::setupTotalBytes()
{
	packet_size_t offset = 0;
	packet_size_t packetLen[1] = { 0, };
	if (totalBytes_ == 0) {
		memcpy_s((void *)packetLen, sizeof(packetLen), (void *)buffer_.data(), sizeof(packetLen));
		//PacketObfuscation::getInstance().decodingHeader((Byte*)&packetLen, sizeof(packetLen));

		totalBytes_ = (size_t)packetLen[0];
	}
	offset += sizeof(packetLen);

	return offset;
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
	ioData_[IO_READ].ioType_ = IO_READ;
	ioData_[IO_WRITE].ioType_ = IO_WRITE;
}

bool Session::onAccept(SOCKET socket, SOCKADDR_IN addrInfo)
{
	socket_ = socket;
	int addrLen;
	getpeername(socket_, (struct sockaddr *)&addrInfo_, &addrLen);
	addrInfo_ = addrInfo;

	return true;
}

str_t Session::clientAddress()
{
	array<char, SIZE_64> ip;
	inet_ntop(AF_INET, &(addrInfo_.sin_addr), ip.data(), ip.size());
	return ip.data();
}
