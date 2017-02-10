
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <string>
#include "Protocol.h"

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg);
// 소켓 함수 오류 출력
void err_display(char *msg);
// 리시브스레드
void recvThreadFunc(SOCKET* sock);

int g_nID;

int main(int argc, char *argv[])
{
	char serverAddr[256];
	printf("서버주소 입력(xxx.xxx.xxx.xxx):");
	scanf("%s", serverAddr);
	//strcpy(serverAddr, "127.0.0.1");
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(serverAddr);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char sendData[SOCKET_BUF_SIZE] = { 0 };
	char inputBuf[SOCKET_BUF_SIZE - 2];
	int len;

	// 리시브스레드
	std::thread recvThread { recvThreadFunc , &sock};

	int inputKey;
	// 서버와 데이터 통신
	while (1) {
		//printf("보낼 데이터:");
		//if (fgets(inputBuf, SOCKET_BUF_SIZE - 2, stdin) == NULL)
		//	break;
		//
		//// '\n' 문자 제거
		//len = strlen(inputBuf);
		//if (inputBuf[len - 1] == '\n')
		//	inputBuf[len - 1] = '\0';
		//if (strlen(inputBuf) == 0)
		//	continue;

		//// 헤더와 데이터 붙이기
		//HEADER *phead = (HEADER*)sendData;
		//phead->packetID = PAK_SYNC;
		//phead->size = strlen(inputBuf)+1 + sizeof(HEADER);

		//CTOS_SYNC *pData = (CTOS_SYNC*)(sendData + sizeof(HEADER));
		//memcpy(pData->data, inputBuf, strlen(inputBuf) + 1);

		//// 데이터 보내기
		//retval = send(sock, sendData, phead->size, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	break;
		//}

INPUT:

		printf("========================================= \n");
		printf(" 0 방접속, 1 방나가기 \n");
		printf("========================================= \n");
		printf("InputKey:");
		scanf("%d", &inputKey);

		switch (inputKey)
		{
		case 0:
		{
			C_EnterRoom *pEnterRoom = (C_EnterRoom*)sendData;
			pEnterRoom->header.size = sizeof(C_EnterRoom);
			pEnterRoom->header.packetID = PAK_EnterRoom;
			printf("방번호:");
			scanf("%d", &pEnterRoom->roomNumber);
			break;
		}
		case 1:
		{
			HEADER *pExitRoom = (HEADER*)sendData;
			pExitRoom->size = sizeof(HEADER);
			pExitRoom->packetID = PAK_ExitRoom;
			break;
		}
		default:
			printf("정의되지 않은 명령키 \n");
			goto INPUT;
		}
		// 데이터 보내기
		retval = send(sock, sendData, ((HEADER*)sendData)->size, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
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

// 소켓 함수 오류 출력
void err_display(char *msg)
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


void recvThreadFunc(SOCKET* sock) {
	char recvData[SOCKET_BUF_SIZE] = { 0 };
	int retval;
	// 데이터 받기
	while (1) {
		ZeroMemory(recvData, sizeof(recvData));

		//retval = recv(*sock, recvData, sizeof(STOC_SYNC), 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("recv()");
		//	break;
		//}
		//else if (retval == 0)
		//	break;
		//HEADER* pHeader = (HEADER*)recvData;
		//STOC_SYNC *outputBuf = (STOC_SYNC*)((UCHAR*)recvData + sizeof(HEADER));
		//switch (pHeader->packetID) {
		//case PAK_ID:
		//	printf("\n[시스템] 접속했습니다. 부여받은 아이디는 [%d]번 입니다.", outputBuf->ID);
		//	g_nID = outputBuf->ID;
		//	break;
		//case PAK_REG:
		//	printf("\n[시스템] %d번 클라가 접속했습니다.", outputBuf->ID);
		//	printf("\n[보낼 데이터]:");
		//	break;
		//case PAK_RMV:
		//	printf("\n[시스템] %d번 클라가 종료했습니다.", outputBuf->ID);
		//	printf("\n[보낼 데이터]:");
		//	break;
		//case PAK_SYNC:
		//	// 받은 데이터 출력
		//	printf("\n[%d]번 클라가 입력한 정보: %s", outputBuf->ID, outputBuf->data);
		//	printf("\n[보낼 데이터]:");
		//	break;
		//}



	}
}