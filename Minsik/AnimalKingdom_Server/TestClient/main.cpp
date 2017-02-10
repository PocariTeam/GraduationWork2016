
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

int g_nID;

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
	char inputBuf[SOCKET_BUF_SIZE - 2];
	int len;

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

		printf("========================================= \n");
		printf(" 0 ������, 1 �泪���� \n");
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
			printf("���ȣ:");
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


void recvThreadFunc(SOCKET* sock) {
	char recvData[SOCKET_BUF_SIZE] = { 0 };
	int retval;
	// ������ �ޱ�
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
		//	printf("\n[�ý���] �����߽��ϴ�. �ο����� ���̵�� [%d]�� �Դϴ�.", outputBuf->ID);
		//	g_nID = outputBuf->ID;
		//	break;
		//case PAK_REG:
		//	printf("\n[�ý���] %d�� Ŭ�� �����߽��ϴ�.", outputBuf->ID);
		//	printf("\n[���� ������]:");
		//	break;
		//case PAK_RMV:
		//	printf("\n[�ý���] %d�� Ŭ�� �����߽��ϴ�.", outputBuf->ID);
		//	printf("\n[���� ������]:");
		//	break;
		//case PAK_SYNC:
		//	// ���� ������ ���
		//	printf("\n[%d]�� Ŭ�� �Է��� ����: %s", outputBuf->ID, outputBuf->data);
		//	printf("\n[���� ������]:");
		//	break;
		//}



	}
}