#pragma once

#define		SOCKET_BUF_SIZE			1024*64
#define		PACKET_BUF_SIZE			2048
#define		SERVER_PORT				9000
#define		THREAD_COUNT			4

#define		UPDATE_TIME_SEC			(1.0f/60.0f)
#define		SYNC_TIME_SEC			(1.0f/60.0f)

#define		MINIMUM_START_COUNT		1
#define		PLAYER_CAPACITY			4
#define		GAMEROOM_CAPACITY		8
#define		DYNAMIC_CAPACITY		32

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
	PAK_REQ_State,
	PAK_ANS_SyncPlayer,
	PAK_ANS_SyncDynamic,
	PAK_ANS_SyncDynamicOne
} PAK_ID;

#pragma pack(push, 1)

/////////////////////////////////////////////////////////////

struct Vector3
{
	float x;
	float y;
	float z;
	Vector3() : x(0), y(0), z(0) {};
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
	Vector4() : x(0), y(0), z(0), w(0) {};
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
};

struct RoomInfo
{
	UINT		playerCount;
	BOOL		playing;
};

#define CHARACTER_MAX     (2)

enum eState_Animation { STATE_IDLE, STATE_ATT1, STATE_ATT2, STATE_RUN, STATE_JUMP, STATE_DEFEND, STATE_BEATEN1, STATE_BEATEN2, /*STATE_SPECIAL, */STATE_DOWN, /*STATE_STANDUP, STATE_DIE, */STATE_END };
using STATE = eState_Animation;

enum eCharacter_Type { CHRACTER_NONE = -1, CHARACTER_CHM, CHARACTER_MON, CHARACTER_END };
using CHARACTER = eCharacter_Type;

struct PlayerInfo
{
	UINT		id;
	CHARACTER	character;
	BOOL		isReady;
	BOOL		isMaster;
};

struct PlayerSyncInfo
{
	UINT		id;
	Vector3		position;
	FLOAT		rotY;
	STATE		state;
};

struct DynamicActor
{
	UINT		index;
	Vector3		position;
	Vector3		linear;
	Vector3		angular;
	Vector4		orient;
};

/////////////////////////////////////////////////////////////

struct HEADER
{
	USHORT		size;
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

struct C_Move
{
	HEADER		header;
	Vector3		vDir;
};

struct C_State
{
	HEADER		header;
	STATE		state;
};

struct S_SyncPlayer
{
	HEADER			header;
	PlayerSyncInfo	playerPositions[PLAYER_CAPACITY];
};

struct S_SyncDynamic
{
	HEADER			header;
	UINT			dynamicActorCount;
	DynamicActor	dynamicActors[DYNAMIC_CAPACITY];
};

struct S_SyncDynamicOne
{
	HEADER			header;
	DynamicActor	dynamicActor;
};

/////////////////////////////////////////////////////////////

#pragma pack(pop)