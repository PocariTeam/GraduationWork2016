#pragma once

#define		SOCKET_BUF_SIZE			1024*10
#define		SERVER_PORT				9000
#define		SERVERIP				"127.0.0.1"

#define		PAK_SYNC				0
#define		PAK_ID					1
#define		PAK_REG					2
#define		PAK_RMV					3


#pragma pack(push, 1)

struct HEADER
{
	UCHAR			ucSize;
	BYTE			byPacketID;
};

struct CTOS_SYNC
{
	char data[SOCKET_BUF_SIZE - 2];
};

struct STOC_SYNC
{
	BYTE	ID;
	char data[SOCKET_BUF_SIZE - 3];
};

#pragma pack(pop)