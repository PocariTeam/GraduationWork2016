#pragma once

#define		SOCKET_BUF_SIZE			1024*10
#define		SERVER_PORT				9000
#define		THREAD_COUNT			4

#define		PLAYER_CAPACITY			4
#define		GAMEROOM_CAPACITY		10

typedef enum {
	PAK_REQ_EnterRoom,
	PAK_REQ_Character,
	PAK_REQ_Ready,
	PAK_REQ_StartGame,
	PAK_REQ_ExitRoom,
	PAK_ANS_LOGIN,
	PAK_ANS_RoomList,
	PAK_ANS_PlayerList,
	PAK_ANS_StartGame,
	PAK_RJT_Request
} PAK_ID;

#pragma pack(push, 1)

/////////////////////////////////////////////////////////////

struct RoomInfo
{
	UINT		playerCount;
	BOOL		playing;
};

typedef enum {
	NONE,
	CHAMEL,
	MONKEY
}CHARACTER;
#define CHARACTER_MAX     (3)

struct PlayerInfo
{
	UINT64		id;
	CHARACTER	character;
	BOOL		isReady;
	BOOL		isMaster;
};

/////////////////////////////////////////////////////////////

struct HEADER
{
	UINT16		size;
	PAK_ID		packetID;
};

struct C_RoomEnter
{
	HEADER		header;
	UINT		roomNumber;
};

struct C_RoomReady
{
	HEADER		header;
	BOOL		ready;
};

struct C_RoomCharacter
{
	HEADER		header;
	CHARACTER	character;
};

struct S_Login
{
	HEADER		header;
	UINT		id;
};

struct S_RoomList
{
	HEADER		header;
	RoomInfo	roomInfo[GAMEROOM_CAPACITY];
};

struct S_PlayerList
{
	HEADER		header;
	UINT		roomNum;
	UINT		playerCount;
	PlayerInfo	playerInfo[PLAYER_CAPACITY];
};

/////////////////////////////////////////////////////////////

#pragma pack(pop)