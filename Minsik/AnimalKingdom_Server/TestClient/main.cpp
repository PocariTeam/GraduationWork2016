
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

int g_ID = -1;
bool g_ready = false;
bool g_master = false;

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
	//char inputBuf[SOCKET_BUF_SIZE - 2];
	//int len;

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

		printf("========================================================== \n");
		printf(" 0 방접속, 1 방나가기 2 레디(게임시작) 3 캐릭터선택\n");
		printf("========================================================== \n");
		scanf("%d", &inputKey);

		switch (inputKey)
		{
		case 0:
		{
			C_RoomEnter *pEnterRoom = (C_RoomEnter*)sendData;
			pEnterRoom->header.size = sizeof(C_RoomEnter);
			pEnterRoom->header.packetID = PAK_ID::PAK_REQ_EnterRoom;
			printf("방번호:");
			scanf("%d", &pEnterRoom->roomNumber);
			break;
		}
		case 1:
		{
			HEADER *pExitRoom = (HEADER*)sendData;
			pExitRoom->size = sizeof(HEADER);
			pExitRoom->packetID = PAK_ID::PAK_REQ_ExitRoom;
			g_ready = g_master = false;
			break;
		}
		case 2:
		{
			if (g_master)
			{
				HEADER *pStartGame = (HEADER*)sendData;
				pStartGame->size = sizeof(HEADER);
				pStartGame->packetID = PAK_ID::PAK_REQ_StartGame;
				printf("게임 시작 요청 \n");
			}
			else
			{
				C_RoomReady *pRoomReady = (C_RoomReady*)sendData;
				pRoomReady->header.size = sizeof(C_RoomReady);
				pRoomReady->header.packetID = PAK_ID::PAK_REQ_Ready;
				g_ready = !g_ready;
				pRoomReady->ready = g_ready;
				printf("게임준비[%d] \n", g_ready);
			}
			break;
		}
		case 3:
		{
			C_RoomCharacter* pRoomCharacter = (C_RoomCharacter*)sendData;
			pRoomCharacter->header.size = sizeof(C_RoomCharacter);
			pRoomCharacter->header.packetID = PAK_ID::PAK_REQ_Character;
			printf("캐릭터번호:");
			scanf("%d", &pRoomCharacter->character);
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

void packetProcess(char* buf)
{
	HEADER* header = (HEADER*)buf;

	switch (header->packetID)
	{
	case PAK_ANS_LOGIN:
	{
		S_Login *packet = (S_Login*)buf;
		g_ID = packet->id;
		printf("로그인:: 아이디는 [%d] 입니다. \n", g_ID);
		break;
	}
	case PAK_ID::PAK_RJT_Request:
	{
		printf("\n !! 요청 실패 !! \n");
		break;
	}
	case PAK_ID::PAK_ANS_RoomList:
	{
		S_RoomList* packet = (S_RoomList*)buf;
		for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
		{
			RoomInfo r = packet->roomInfo[i];
			printf("[%d]번 방 - 인원: %d명, 플레이 상태: %d \n", i, r.playerCount, r.playing);
		}
		break;
	}
	case PAK_ID::PAK_ANS_PlayerList:
	{
		S_PlayerList* packet = (S_PlayerList*)buf;
		printf("========================================= \n");
		printf("\t\t [방 정보] %\t\t \n");
		for (int i = 0; i < packet->playerCount; ++i)
		{
			PlayerInfo r = packet->playerInfo[i];
			printf(" id[%d] 캐릭터[%d] 레디[%d] 방장[%d] \n", r.id, r.character, r.isReady, r.isMaster);
			if (r.id == g_ID)
			{
				if (r.isMaster == true && g_master == false)
				{
					printf(" --> 내가 방장이 되었습니다. <-- \n");
					g_master = true;

				}
			}
		}
		printf("========================================= \n");
		break;
	}
	case PAK_ID::PAK_ANS_StartGame:
	{
		S_StartGame* packet = (S_StartGame*)buf;
		printf("========================================= \n");
		printf("\t\t [게임시작] %\t\t \n");
		printf("시작시간: %d \n", packet->startTick);
		printf("========================================= \n");
		break;
	}
	}
}

void recvThreadFunc(SOCKET* sock) {
	char recvBuf[SOCKET_BUF_SIZE] = { 0 };
	char saveBuf[SOCKET_BUF_SIZE] = { 0 };
	int retval;
	int iCurrPacketSize = 0;
	int iStoredPacketSize = 0;
	while (1) 
	{
		// 데이터 받기
		ZeroMemory(recvBuf, sizeof(recvBuf));
		retval = recv(*sock, recvBuf, sizeof(recvBuf), 0);

		if (retval == SOCKET_ERROR) 
		{
			err_quit("recv()");
			break;
		}

		char *pRecvBuf = recvBuf;

		while (0 < retval) 
		{
			if (0 == iCurrPacketSize) 
			{
				if (retval + iStoredPacketSize >= sizeof(HEADER)) 
				{
					int restHeaderSize = sizeof(HEADER) - iStoredPacketSize;
					memcpy(saveBuf + iStoredPacketSize, pRecvBuf, restHeaderSize);
					pRecvBuf += restHeaderSize;
					iStoredPacketSize += restHeaderSize;
					retval -= restHeaderSize;
					iCurrPacketSize = ((HEADER*)saveBuf)->size;
				}
				else 
				{
					memcpy(saveBuf + iStoredPacketSize, pRecvBuf, retval);
					iStoredPacketSize += retval;
					retval = 0;
					break;
				}
			}

			int restSize = iCurrPacketSize - iStoredPacketSize;

			if (restSize <= retval) 
			{
				memcpy(saveBuf + iStoredPacketSize, pRecvBuf, restSize);

				packetProcess(saveBuf);

				iCurrPacketSize = iStoredPacketSize = 0;

				pRecvBuf += restSize;
				retval -= restSize;
			}
			else 
			{
				memcpy(saveBuf + iStoredPacketSize, pRecvBuf, retval);

				iStoredPacketSize += retval;
				retval = 0;
				//recvBuf += recvSize;
			}
		}
	}
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