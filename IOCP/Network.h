#pragma once
#include "stdafx.h"
#include "Protocol.h"

enum class OP_TYPE
{
	OP_SEND = 0,
	OP_RECV
};

// 소켓 정보 저장을 위한 구조체
struct SOCKETINFO
{
	OVERLAPPED overlapped;
	WSABUF wsabuf;
	SOCKET sock;
	OP_TYPE optype = OP_TYPE::OP_RECV;
	int nID;
	char IOBuf[MAX_PACKET_SIZE];
	char packetBuf[MAX_PACKET_SIZE];
	int	 iCurrPacketSize; 
	int	 iStoredPacketSize;

	SOCKETINFO();
	~SOCKETINFO();
};

class CNetwork
{
	vector<thread*>					m_vpThreadlist;
	vector<SOCKETINFO*>				m_vpClientInfo;
	SOCKET							m_listenSock;
	HANDLE							m_hIOCP;
	UINT							m_nID;
public:

	CNetwork();
	~CNetwork();

	//오류 출력 함수
	void err_quit(char *msg);
	void err_display(char *msg);

	void initNetwork();
	void startServer();
	void endServer();

	//스레드 함수
	bool acceptThread();
	void workerThread();

	//패킷 처리 함수
	bool packetProcess(CHAR*, int);
	bool Login(void *buf, int id);
	bool Logout(void *buf, int id);
	bool syncData(void *buf, int id);
	void transmitProcess(void * buf, int id);
};
