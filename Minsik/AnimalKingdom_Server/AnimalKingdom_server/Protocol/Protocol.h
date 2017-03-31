#pragma once

#define		SOCKET_BUF_SIZE			1024*10
#define		PACKET_BUF_SIZE			1024
#define		SERVER_PORT				9000
#define		THREAD_COUNT			4

#define		PLAYER_CAPACITY			4
#define		GAMEROOM_CAPACITY		8

typedef enum {
	PAK_REQ_RoomList,
	PAK_REQ_EnterRoom,
	PAK_REQ_Character,
	PAK_REQ_Ready,
	PAK_REQ_StartGame,
	PAK_REQ_ExitRoom,
	PAK_ANS_Login,
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
	CHAMEL,
	MONKEY
}S_CHARACTER;
#define CHARACTER_MAX     (2)

enum eState_Animation { STATE_IDLE, STATE_ATT1, STATE_ATT2, STATE_RUN, STATE_JUMP, STATE_DEFEND, STATE_BEATEN1, STATE_BEATEN2, /*STATE_SPECIAL, */STATE_DOWN, /*STATE_STANDUP, STATE_DIE, */STATE_END };
using STATE = eState_Animation;

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