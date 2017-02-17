#include "stdafx.h"
#include "IOCPServer.h"

IOCPServer::IOCPServer()
{
	xml_t config;
	if (!loadConfig(&config)) {
		return;
	}
	this->initialize(&config);

	SLog(L"# Initialize network base");
}

IOCPServer::~IOCPServer()
{
	::closesocket(listenSocket_);
	WSACleanup();
	SLog(L"# ~IOCPServer()");
}

void IOCPServer::initialize(xml_t *config)
{
	// wsa 설정
	WSADATA	wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("! wsa startup fail\n");
		exit(0);
	}

	SLog(L"### WSA 2.2 set complet.");

	//테스크 설정
	//TaskManager::getInstance();

	//서버 설정
	xmlNode_t *root = config->FirstChildElement("App")->FirstChildElement("Server");
	if (!root) {
		SLog(L"@ not exist server setting");
		return;
	}
	xmlNode_t *elem = root->FirstChildElement("IP");
//	strcpy_s(ip_, elem->GetText());

	elem = root->FirstChildElement("Port");
	sscanf_s(elem->GetText(), "%d", &port_);

	elem = root->FirstChildElement("ThreadCount");
	sscanf_s(elem->GetText(), "%d", &workerThreadCount_);
	SLog(L"* IO worker thread count : %d", workerThreadCount_);

	root = config->FirstChildElement("App");
	elem = root->FirstChildElement("Name");

	SLog(L"### %S start!!! ###", elem->GetText());
}

bool IOCPServer::run()
{
	if (MAX_IOCP_THREAD < workerThreadCount_) {
		SErrLog(L"! workerThread limit[%d], but config setting [%d]", MAX_IOCP_THREAD, workerThreadCount_);
		return false;
	}

	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, workerThreadCount_);
	if (iocp_ == nullptr) {
		SErrLog(L"! ioCompletionPort create fail");
		return false;
	}
	this->createListenSocket();

	acceptThread_ = MAKE_THREAD(IOCPServer, acceptThread);
	for (int i = 0; i < workerThreadCount_; ++i) {
		workerThread_[i] = MAKE_THREAD(IOCPServer, workerThread);
	}

	while (!_shutdown) {
		wstring cmdLine;
		std::getline(std::wcin, cmdLine);
		SLog(L"Input was: %s", cmdLine.c_str());
		command(cmdLine);
	}
	return true;
}

void IOCPServer::onAccept(SOCKET accepter, SOCKADDR_IN addrInfo)
{
	Session *session = new Session();
	if (session == nullptr) {
		SLog(L"! accept session create fail");
		return;
	}
	if (!session->onAccept(accepter, addrInfo)) {
		SAFE_DELETE(session);
		return;
	}
	if (!SessionManager::getInstance().addSession(session)) {
		SAFE_DELETE(session);
		return;
	}

	HANDLE handle = CreateIoCompletionPort((HANDLE)accepter, this->iocp_, (ULONG_PTR)&(*session), NULL);
	if (!handle) {
		SAFE_DELETE(session);
		return;
	}

	SLog(L"* client id[%d] accecpt from [%S]", session->getID(), session->getAddress().c_str());
	
	PacketManager::getInstance().sendRoomList(session);
	PacketManager::getInstance().sendLogin(session);
	session->recv();
}

void IOCPServer::printHostInfo() {
	char szLocalHostName[512]; // 이 컴퓨터가 네트워크에서 식별되는 이름이 저장될 문자열 버퍼입니다.
	struct hostent * pLocalHostInformation; // 로컬 호스트의 정보가 담길 구조체의 포인터

	gethostname(szLocalHostName, sizeof(szLocalHostName));
	SLog(L"* Host name is %S.", szLocalHostName);
	pLocalHostInformation = gethostbyname(szLocalHostName);

	/* 한 컴퓨터에서 여러 IP를 할당 받을 수 있습니다. 이를 모두 출력합니다. */
	for (int i = 0; pLocalHostInformation->h_addr_list[i] != NULL; i++) {
		SLog(L"* Hostent.h_addr_list[%d] = \"%S\", port: %d", i, inet_ntoa(*(struct in_addr*)pLocalHostInformation->h_addr_list[i]),port_);
	}
	
}


bool IOCPServer::createListenSocket()
{
	listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listenSocket_ == INVALID_SOCKET) {
		SErrLog(L"! listenSocket fail");
		return false;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)port_);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_pton(AF_INET, ip_, &(serverAddr.sin_addr));
	IOCPServer::printHostInfo();

	// 소켓재사용옵션
	//int reUseAddr = 1;
	//setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, (char *)&reUseAddr, (int)sizeof(reUseAddr));
	
	// 네이글알고리즘
	BOOL optval = TRUE;
	setsockopt(listenSocket_, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));


	int retval = ::bind(listenSocket_, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		SErrLog(L"! bind fail");
		return false;
	}

	retval = ::listen(listenSocket_, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		SErrLog(L"! listen fail");
		return false;
	}

	//array<char, SIZE_64> ip;
	//inet_ntop(AF_INET, &(serverAddr.sin_addr), ip.data(), ip.size());
	//SLog(L"* server listen socket created, ip: %S, port: %d", ip.data(), port_);
	return true;
}

DWORD IOCPServer::acceptThread(LPVOID serverPtr)
{
	IOCPServer	*server = (IOCPServer *)serverPtr;

	while (!_shutdown) 
	{
		SOCKET		acceptSocket = INVALID_SOCKET;
		SOCKADDR_IN recvAddr;
		static int addrLen = sizeof(recvAddr);
		acceptSocket = WSAAccept(server->listenSocket_, (struct sockaddr *)&recvAddr, &addrLen, NULL, 0);
		if (acceptSocket == SOCKET_ERROR) {
			SLog(L"! Accept fail");
			break;
		}
		server->onAccept(acceptSocket, recvAddr);
	}
	return 0;
}

DWORD IOCPServer::workerThread(LPVOID serverPtr)
{
	IOCPServer *server = (IOCPServer *)serverPtr;

	while (!_shutdown) {

		IoData			*ioData = nullptr;
		Session			*session = nullptr;
		DWORD			transferSize;

		BOOL retval = GetQueuedCompletionStatus(server->iocp_, &transferSize, (PULONG_PTR)&session, (LPOVERLAPPED *)&ioData, INFINITE);
		/*if (!retval) 
		{
			continue;
		}*/
		if (session == nullptr) {
			SLog(L"! socket data broken");
			return 0;
		}
		if (transferSize == 0) {
			if (session->getRoomNumber() != NOT_IN_ROOM)
			{
				RoomManager::getInstance().exitRoom(session);
			}
			SessionManager::getInstance().closeSession(session);
			continue;
		}

		switch (ioData->ioType_)
		{
		case IO_SEND:
		{
			// send 처리
			continue;
		}
		case IO_RECV:
		{
			session->onRecv((size_t)transferSize);
			continue;
		}
		case IO_ERROR:
		{
			SLog(L"! IO_ERROR! ");
			SessionManager::getInstance().closeSession(session);
			continue;
		}
		}

	}
	return 0;
}

void IOCPServer::command(wstr_t cmd)
{
	shutdownServer();
	/*if (strcmp((const char*)cmd.c_str(), "shutdown") == 0) {
		shutdownServer();
	}*/
}