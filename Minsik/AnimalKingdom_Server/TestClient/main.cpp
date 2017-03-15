
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <string>
#include "Protocol.h"

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg);
// ���� �Լ� ���� ���
void err_display(char *msg);
// ���ú꽺����
void recvThreadFunc(SOCKET* sock);

int g_ID = -1;
bool g_ready = false;
bool g_master = false;

int main(int argc, char *argv[])
{
	char serverAddr[256];
	printf("�����ּ� �Է�(xxx.xxx.xxx.xxx):");
	scanf("%s", serverAddr);
	//strcpy(serverAddr, "127.0.0.1");
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	char sendData[SOCKET_BUF_SIZE] = { 0 };
	//char inputBuf[SOCKET_BUF_SIZE - 2];
	//int len;

	// ���ú꽺����
	std::thread recvThread { recvThreadFunc , &sock};

	int inputKey;
	// ������ ������ ���
	while (1) {
		//printf("���� ������:");
		//if (fgets(inputBuf, SOCKET_BUF_SIZE - 2, stdin) == NULL)
		//	break;
		//
		//// '\n' ���� ����
		//len = strlen(inputBuf);
		//if (inputBuf[len - 1] == '\n')
		//	inputBuf[len - 1] = '\0';
		//if (strlen(inputBuf) == 0)
		//	continue;

		//// ����� ������ ���̱�
		//HEADER *phead = (HEADER*)sendData;
		//phead->packetID = PAK_SYNC;
		//phead->size = strlen(inputBuf)+1 + sizeof(HEADER);

		//CTOS_SYNC *pData = (CTOS_SYNC*)(sendData + sizeof(HEADER));
		//memcpy(pData->data, inputBuf, strlen(inputBuf) + 1);

		//// ������ ������
		//retval = send(sock, sendData, phead->size, 0);
		//if (retval == SOCKET_ERROR) {
		//	err_display("send()");
		//	break;
		//}

INPUT:

		printf("========================================================== \n");
		printf(" 0 ������, 1 �泪���� 2 ����(���ӽ���) 3 ĳ���ͼ���\n");
		printf("========================================================== \n");
		scanf("%d", &inputKey);

		switch (inputKey)
		{
		case 0:
		{
			C_RoomEnter *pEnterRoom = (C_RoomEnter*)sendData;
			pEnterRoom->header.size = sizeof(C_RoomEnter);
			pEnterRoom->header.packetID = PAK_ID::PAK_REQ_EnterRoom;
			printf("���ȣ:");
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
				printf("���� ���� ��û \n");
			}
			else
			{
				C_RoomReady *pRoomReady = (C_RoomReady*)sendData;
				pRoomReady->header.size = sizeof(C_RoomReady);
				pRoomReady->header.packetID = PAK_ID::PAK_REQ_Ready;
				g_ready = !g_ready;
				pRoomReady->ready = g_ready;
				printf("�����غ�[%d] \n", g_ready);
			}
			break;
		}
		case 3:
		{
			C_RoomCharacter* pRoomCharacter = (C_RoomCharacter*)sendData;
			pRoomCharacter->header.size = sizeof(C_RoomCharacter);
			pRoomCharacter->header.packetID = PAK_ID::PAK_REQ_Character;
			printf("ĳ���͹�ȣ:");
			scanf("%d", &pRoomCharacter->character);
			break;
		}
		default:
			printf("���ǵ��� ���� ���Ű \n");
			goto INPUT;
		}
		// ������ ������
		retval = send(sock, sendData, ((HEADER*)sendData)->size, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
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
		printf("�α���:: ���̵�� [%d] �Դϴ�. \n", g_ID);
		break;
	}
	case PAK_ID::PAK_RJT_Request:
	{
		printf("\n !! ��û ���� !! \n");
		break;
	}
	case PAK_ID::PAK_ANS_RoomList:
	{
		S_RoomList* packet = (S_RoomList*)buf;
		for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
		{
			RoomInfo r = packet->roomInfo[i];
			printf("[%d]�� �� - �ο�: %d��, �÷��� ����: %d \n", i, r.playerCount, r.playing);
		}
		break;
	}
	case PAK_ID::PAK_ANS_PlayerList:
	{
		S_PlayerList* packet = (S_PlayerList*)buf;
		printf("========================================= \n");
		printf("\t\t [�� ����] %\t\t \n");
		for (int i = 0; i < packet->playerCount; ++i)
		{
			PlayerInfo r = packet->playerInfo[i];
			printf(" id[%d] ĳ����[%d] ����[%d] ����[%d] \n", r.id, r.character, r.isReady, r.isMaster);
			if (r.id == g_ID)
			{
				if (r.isMaster == true && g_master == false)
				{
					printf(" --> ���� ������ �Ǿ����ϴ�. <-- \n");
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
		printf("\t\t [���ӽ���] %\t\t \n");
		printf("���۽ð�: %d \n", packet->startTick);
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
		// ������ �ޱ�
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

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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