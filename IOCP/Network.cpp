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

	for (int i = 0; i < MAX_ID_CNT; ++i){
		m_vpClientInfo.push_back(nullptr);
	}

	cout << "initNetwork()" << endl;

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
	cout << "startServer()" << endl;

	printHostInfo();
}

void CNetwork::endServer()
{
	for (auto &data : m_vpThreadlist){
		data->join();
	}
	WSACleanup();

	cout << "endServer()" << endl;
}

void CNetwork::printHostInfo() {
	char szLocalHostName[512]; // 이 컴퓨터가 네트워크에서 식별되는 이름이 저장될 문자열 버퍼입니다.
	struct hostent * pLocalHostInformation; // 로컬 호스트의 정보가 담길 구조체의 포인터

	gethostname(szLocalHostName, sizeof(szLocalHostName));
	printf("로컬 호스트의 이름은 \"%s\"입니다.\n", szLocalHostName);
	pLocalHostInformation = gethostbyname(szLocalHostName);

	/* 한 컴퓨터에서 여러 IP를 할당 받을 수 있습니다. 이를 모두 출력합니다. */
	for (int i = 0; pLocalHostInformation->h_addr_list[i] != NULL; i++){
		printf("hostent.h_addr_list[%d] = %s\n", i, inet_ntoa(*(struct in_addr*)pLocalHostInformation->h_addr_list[i]));
	}


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

		// accept
		addrLen = sizeof(clientAddr);
		clientSock = accept(m_listenSock, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속차단
		// !! (추가필요) 해당 플레이어에게 그 사실을 알려야 함
		if (m_nID >= MAX_ID_CNT) {
			closesocket(clientSock);
			cout << "[시스템] 최대 수용 " << MAX_ID_CNT << "명을 초과하여 새로운 접속차단" << endl;
			continue;
		}

		// 소켓 정보 구조체 할당
		SOCKETINFO *pSocketInfo = new SOCKETINFO;
		if (pSocketInfo == NULL) break;
		ZeroMemory(&pSocketInfo->overlapped, sizeof(pSocketInfo->overlapped));
		pSocketInfo->sock = clientSock;
		pSocketInfo->optype = OP_TYPE::OP_RECV;
		pSocketInfo->wsabuf.buf = pSocketInfo->IOBuf;
		pSocketInfo->wsabuf.len = MAX_PACKET_SIZE;

		// 빈자리 찾아서 소켓정보벡터에 넣기
		bool bisFull = true;
		for (int i = 0; i < MAX_ID_CNT; ++i) {
			if (!m_vpClientInfo[i]) {
				pSocketInfo->nID = i;
				m_vpClientInfo[i] = pSocketInfo;
				++m_nID;
				bisFull = false;
				cout << "[시스템] 총 접속:" << m_nID << "명 [" << i << "]번 클라 접속! IP주소:" << inet_ntoa(clientAddr.sin_addr) << ", 포트번호:" << ntohs(clientAddr.sin_port) << endl;
				break;
			}
		}
		// 혹시 몰라서 넣은 에러체크. 실제 로직대로라면 무조건 빈자리가 있어야 함.
		if (bisFull) {
			cout << "클라이언트벡터에서 빈자리를 찾지 못했습니다. \n" << endl;
			continue;
		}

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
			// Overlapped 구조체는 입출력이 완료되기 전까지 메모리에서 사라지면 안되므로 동적할당을 했었다.
			// 그러므로 Send()가 완료되었다면 반드시 메모리 해제를 해주어야 한다.
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
		if (client) {
			transmitProcess(sendData, client->nID);
		}
	}

	return true;
}

bool CNetwork::Logout(void * buf, int id)
{
	// 소켓을 닫는다.
	closesocket(m_vpClientInfo[id]->sock);
	delete  m_vpClientInfo[id];
	m_vpClientInfo[id] = nullptr;
	--m_nID;

	// 플레이어들에게 접속종료 사실을 알린다.
	UCHAR sendData[MAX_PACKET_SIZE] = { 0 };
	HEADER *phead = (HEADER*)sendData;
	phead->byPacketID = PAK_RMV;
	phead->ucSize = sizeof(HEADER) + 1;
	STOC_SYNC *pdata1 = (STOC_SYNC*)(sendData + sizeof(HEADER));
	pdata1->ID = id;

	for (auto &data : m_vpClientInfo){
		if (data){
			transmitProcess(sendData, data->nID);
		}
	}

	cout << "[시스템] " << id << "번 클라이언트 접속 종료!" << endl;

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
		if (client) {
			transmitProcess(sendData, client->nID);
		}
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
			return;
		}
	}
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
