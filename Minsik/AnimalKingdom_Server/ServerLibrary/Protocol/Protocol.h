#pragma once

#define		SOCKET_BUF_SIZE			1024*10
#define		GAMEROOM_CAPACITY		10
#define		SERVER_PORT				9000
#define		SERVERIP				"127.0.0.1"

typedef enum {
	PAK_REQ_EnterRoom,
	PAK_REQ_ExitRoom,
	PAK_RJT_Room,
	PAK_ANS_RoomList
} PAK_ID;

struct RoomInfo
{
	UINT	playerCount;
	BOOL	playing;
};

#pragma pack(push, 1)

struct HEADER
{
	UINT16			size;
	PAK_ID			packetID;
};

struct C_EnterRoom
{
	HEADER	header;
	UINT	roomNumber;
};

struct S_RoomListInfo
{
	HEADER		header;
	RoomInfo	roomInfo[GAMEROOM_CAPACITY];
	//array<RoomInfo, GAMEROOM_CAPACITY> roomInfo;
};

#pragma pack(pop)