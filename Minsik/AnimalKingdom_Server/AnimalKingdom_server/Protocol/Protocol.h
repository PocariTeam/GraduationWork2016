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
	PAK_RJT_Request,
	PAK_REQ_Move,
	PAK_ANS_Move
} PAK_ID;

#pragma pack(push, 1)

/////////////////////////////////////////////////////////////

struct Vector3
{
	float x; 
	float y; 
	float z;
};

struct RoomInfo
{
	UINT		playerCount;
	BOOL		playing;
};

typedef enum {
	NONE,
	CHAMEL,
	MONKEY
}S_CHARACTER;
#define CHARACTER_MAX     (3)

struct PlayerInfo
{
	UINT64		id;
	S_CHARACTER	character;
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
	S_CHARACTER	character;
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

struct S_StartGame
{
	HEADER		header;
	time_t		startTick;
};

/////////////////////////////////////////////////////////////

struct C_Move
{
	HEADER		header;
	time_t		tick;
	Vector3		vDir;
};

struct S_Move
{
	HEADER		header;
	UINT		id;
	time_t		tick;
	Vector3		vDir;
};

/////////////////////////////////////////////////////////////

#pragma pack(pop)