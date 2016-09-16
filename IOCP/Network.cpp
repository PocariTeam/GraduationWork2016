#include "Network.h"

SOCKETINFO::SOCKETINFO()
{
	ZeroMemory(&overlapped, sizeof(overlapped));
	ZeroMemory(&wsabuf, sizeof(wsabuf));
	ZeroMemory(IOBuf, sizeof(MAX_PACKET_SIZE));
	ZeroMemory(packetBuf, sizeof(MAX_PACKET_SIZE));
	sock = NULL;
	optype = OP_TYPE::OP_RECV;
	iCurrPacketSize = iStoredPacketSize = 0;

}

SOCKETINFO::~SOCKETINFO()
{
}


CNetwork::CNetwork()
{
	m_listenSock = NULL;
	m_hIOCP = NULL;
	m_nID = 0;
}


CNetwork::~CNetwork()
{
}

void CNetwork::initNetwork()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
		err_quit("윈속 초기화 실패");
	// 입출력 완료 포트 생성
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_hIOCP == NULL)
		err_quit("IOCP 생성 실패");

	cout << "CNetwork 초기화 완료" << endl;

}

void CNetwork::startServer()
{
	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	m_vpThreadlist.push_back(new thread{ mem_fun(&CNetwork::acceptThread), this });
	for (DWORD i = 0; i < si.dwNumberOfProcessors * 2; ++i) {
		m_vpThreadlist.push_back(new thread{ mem_fun(&CNetwork::workerThread), this });
	}

	cout << "acceptThread와 " << si.dwNumberOfProcessors * 2 << "개의 workerThread 생성" << endl;
}

void CNetwork::endServer()
{
	for (auto &data : m_vpThreadlist){
		data->join();
	}
	WSACleanup();

	cout << "서버 종료" << endl;
}

bool CNetwork::acceptThread()
{
	m_listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSock == INVALID_SOCKET) CNetwork::err_quit("WSASocket() error!");

	//주소 bind()
	sockaddr_in	listenSockAddr;
	ZeroMemory(&listenSockAddr, sizeof(listenSockAddr));
	listenSockAddr.sin_family = AF_INET;
	listenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	listenSockAddr.sin_port = htons(SERVER_PORT);

	int retval = ::bind(m_listenSock, (sockaddr*)&listenSockAddr, sizeof(listenSockAddr));
	if (retval == SOCKET_ERROR){
		err_quit("Bind() error!");
		return false;
	}

	//listen()
	retval = listen(m_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR){
		err_quit("Listen() error!");
		return false;
	}

	// 데이터 통신에 사용할 변수
	SOCKET			clientSock;
	SOCKADDR_IN		clientAddr;
	DWORD			flags = 0;
	int				addrLen;

	while (1) {

		//accept
		addrLen = sizeof(clientAddr);
		clientSock = accept(m_listenSock, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		cout << "클라이언트 접속: IP주소:" << inet_ntoa(clientAddr.sin_addr) << ", 포트번호:" << ntohs(clientAddr.sin_port) << endl;

		// 소켓 정보 구조체 할당
		SOCKETINFO *pSocketInfo = new SOCKETINFO;
		if (pSocketInfo == NULL) break;
		ZeroMemory(&pSocketInfo->overlapped, sizeof(pSocketInfo->overlapped));
		pSocketInfo->sock = clientSock;
		pSocketInfo->optype = OP_TYPE::OP_RECV;
		pSocketInfo->nID = m_nID++;
		pSocketInfo->wsabuf.buf = pSocketInfo->IOBuf;
		pSocketInfo->wsabuf.len = MAX_PACKET_SIZE;

		m_vpClientInfo.push_back(pSocketInfo);

		// 소켓과 입출력 완료 포트 연결
		CreateIoCompletionPort((HANDLE)clientSock, m_hIOCP, pSocketInfo->nID, 0);	//핸들, 포트, 키값, 최대스레드(의미x)


		// 비동기 입출력 시작
		flags = 0;
		retval = WSARecv(clientSock, &pSocketInfo->wsabuf, 1, NULL, &flags, &pSocketInfo->overlapped, NULL);
		// (소켓, 버퍼, 버퍼개수, 읽은바이트수, 플래그, 오버랩, 컴플리션루틴)
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				err_display("WSARecv()1");
			}
			continue;
		}
	}

	return false;
}

void CNetwork::workerThread()
{
	DWORD IOsize;
	ULONG key;
	SOCKETINFO *sockInfo;

	while (1){

		GetQueuedCompletionStatus(m_hIOCP, &IOsize, &key, (LPOVERLAPPED *)&sockInfo, INFINITE);

		// 접속 종료 처리
		if (0 == IOsize){
			auto sockdata = m_vpClientInfo[key];
			m_vpClientInfo[key] = nullptr;
			delete sockdata;
			cout << "클라이언트 접속종료" << endl;
			Logout(nullptr, key);
			continue;
		}
		
		if (sockInfo->optype == OP_TYPE::OP_RECV){

			// 패킷조립 및 실행
			unsigned recvSize = IOsize;
			char *recvBuf = sockInfo->IOBuf;

			while (0 < recvSize){

				//현재 처리하는 패킷이 없을 경우 recvBuf의 첫번째 바이트를 사이즈로 설정
				if (0 == sockInfo->iCurrPacketSize){
					sockInfo->iCurrPacketSize = recvBuf[0];
				}

				// 패킷을 만들기 위해 필요한 남은 사이즈 = 현재 받아야할 패킷사이즈 - 현재까지 저장한 패킷사이즈
				UINT restSize = sockInfo->iCurrPacketSize - sockInfo->iStoredPacketSize;

				// io로 받은 데이터의 크기가 패킷을 만들기 위해 필요한 사이즈보다 크거나 같은 경우 패킷을 조립한다.
				if (restSize <= recvSize){

					// 패킷버퍼에 패킷 사이즈를 채워 줄 만큼 
					memcpy(sockInfo->packetBuf + sockInfo->iStoredPacketSize, recvBuf, restSize);
					
					// 패킷처리
					if (!packetProcess(sockInfo->packetBuf, key)){
						auto sockdata = m_vpClientInfo[key];
						m_vpClientInfo[key] = nullptr;
						delete sockdata;
						Logout(nullptr, key);
						continue;
					}

					sockInfo->iCurrPacketSize = sockInfo->iStoredPacketSize = 0;

					recvBuf += restSize;
					recvSize -= restSize;
				}
				else{
					// 처리할 만큼의 사이즈가 아닌 경우 패킷버퍼에 저장 해 놓음
					memcpy(sockInfo->packetBuf + sockInfo->iStoredPacketSize, recvBuf, recvSize);

					sockInfo->iStoredPacketSize += recvSize;
					recvSize = 0;
					//recvBuf += recvSize;
				}
			}

			// 다시 Recv()호출
			DWORD flags = 0;
			int retval = WSARecv(sockInfo->sock, &sockInfo->wsabuf, 1, NULL, &flags, (LPOVERLAPPED)sockInfo, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					err_display("WSARecv()2");
				}
			}
		}
		else if (sockInfo->optype == OP_TYPE::OP_SEND){
			// Send()였다면 그냥 무시하기 위해 받은 구조체 삭제
			delete sockInfo;
		}
		else{
			exit(-1);
		}
	}
}

bool CNetwork::packetProcess(CHAR* buf, int id)
{
	//cout << "패킷 처리"<< (int)buf[1] << endl;
	bool issuccess = true;

	switch (buf[1])
	{
	case PAK_SYNC:
		issuccess = syncData(buf, id);
		break;
	case PAK_ID:
		issuccess = Login(buf, id);
		break;
	}
	return issuccess;
}

bool CNetwork::Login(void * buf, int id)
{
	// 헤더작성
	UCHAR sendData[MAX_PACKET_SIZE] = { 0 };
	HEADER *phead = (HEADER*)sendData;
	phead->byPacketID = PAK_ID;
	phead->ucSize = sizeof(HEADER) + 1; // 헤더 + 아이디번호
	// 본문작성
	STOC_SYNC *pData = (STOC_SYNC*)(sendData + sizeof(HEADER));
	pData->ID = id;

	// 아이디번호 부여
	transmitProcess(sendData, id);

	phead->byPacketID = PAK_REG;
	phead->ucSize = sizeof(HEADER) + 1; // 헤더 + 아이디
	// 다른 플레이어에게 접속 사실을 알림
	// 실제코드에서는 위치정보 등 부가정보도 넘겨야 함
	for (auto client : m_vpClientInfo) {
		transmitProcess(sendData, client->nID);
	}

	return true;
}

bool CNetwork::Logout(void * buf, int id)
{
	return true;
}

bool CNetwork::syncData(void * buf, int id)
{
	CTOS_SYNC *recvData = (CTOS_SYNC*)((UCHAR*)buf + sizeof(HEADER));

	printf("[%d]번 클라로부터 받은 데이터: %s \n", id, recvData->data);
	
	// 이 부분에 받은 데이터를 가지고 계산을 해서 그 결과를 클라들에게 주어야 한다.
	// 현재는 받은 데이터를 그대로 모든 클라에게 전달한다.
	
	// 헤더작성
	UCHAR sendData[MAX_PACKET_SIZE] = { 0 };
	HEADER *phead = (HEADER*)sendData;
	phead->byPacketID = PAK_SYNC;
	phead->ucSize = (UCHAR)(sizeof(HEADER) + strlen(recvData->data) + 2); // 헤더 + 버퍼내용 + 2(아이디 + 널)
	// 본문작성
	STOC_SYNC *pData = (STOC_SYNC*)(sendData + sizeof(HEADER));
	pData->ID = id;
	memcpy(pData->data, recvData, strlen(recvData->data)+1);

	for (auto client : m_vpClientInfo) {
		transmitProcess(sendData, client->nID);
	}

	return true;
}

void CNetwork::transmitProcess(void *buf, int id)
{
	SOCKETINFO *psock = new SOCKETINFO;
	UCHAR paksize = ((UCHAR*)buf)[0];

	memcpy(psock->IOBuf, buf, paksize);
	psock->optype = OP_TYPE::OP_SEND;

	ZeroMemory(&psock->overlapped, sizeof(WSAOVERLAPPED));
	psock->wsabuf.buf = psock->IOBuf;
	psock->wsabuf.len = paksize;

	unsigned long IOsize;
	SOCKETINFO* sock = m_vpClientInfo[id];
	if (!sock)
		err_display("Wrong Sock access!!");

	int retval = WSASend(sock->sock, &psock->wsabuf, 1, &IOsize, NULL, &psock->overlapped, NULL);

	if (retval == SOCKET_ERROR){
		int err_code = WSAGetLastError();
		if (WSA_IO_PENDING != err_code){
			err_display("[CNetworkManager::sendPacket()] WSASend");
			printf("%s \n", WSAGetLastError());
			//로그아웃
			//Logout(sockid, NULL);
			return;
		}
	}
	//cout << "패킷 전송 완료" << IOsize << endl;
}

void CNetwork::err_quit(char * msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void CNetwork::err_display(char * msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
