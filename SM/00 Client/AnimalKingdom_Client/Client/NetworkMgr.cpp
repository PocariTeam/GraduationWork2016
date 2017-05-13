#include "stdafx.h"
#include "NetworkMgr.h"
#include "Scene.h"
#include "Jungle.h"
#include <fstream>

CNetworkMgr*	CSingleton<CNetworkMgr>::m_pInstance;

HRESULT CNetworkMgr::Initialize()
{
	ZeroMemory( m_sendBuf, sizeof( m_sendBuf ) );
	ZeroMemory( m_recvBuf, sizeof( m_recvBuf ) );
	ZeroMemory( m_saveBuf, sizeof( m_saveBuf ) );
	ZeroMemory( m_tPlayerInfo, sizeof( m_tPlayerInfo ) );
	m_wsaSendBuf.buf = m_sendBuf;
	m_wsaSendBuf.len = sizeof( m_sendBuf );
	m_wsaRecvBuf.buf = m_recvBuf;
	m_wsaRecvBuf.len = sizeof( m_recvBuf );
	m_iCurrPacketSize = 0;
	m_iStoredPacketSize = 0;
	m_nPlayerID = -1;
	m_nRoomNum = -1;
	m_bReady = false;
	m_bMaster = false;
	m_dwPlayerCnt = 0;

	WSADATA	wsa;


	if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsa ) )
		return E_FAIL;

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	if( m_Socket == INVALID_SOCKET )
	{
		printf( " socket() error! \n" );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNetworkMgr::connectServer( HWND hwnd )
{
	char serverAddr[ 256 ];

	ifstream	In{ "../Executable/Resources/IP.txt" };
	In >> serverAddr;
	In.close();

	ZeroMemory( &m_tServerAdrr, sizeof( m_tServerAdrr ) );
	m_tServerAdrr.sin_family = AF_INET;
	m_tServerAdrr.sin_port = htons( SERVER_PORT );
	m_tServerAdrr.sin_addr.s_addr = inet_addr( serverAddr );

	int retval = connect( m_Socket, ( SOCKADDR * )&m_tServerAdrr, sizeof( m_tServerAdrr ) );
	if (retval == SOCKET_ERROR)
	{
		strcat_s(serverAddr, "\nnetwork connect fail!");
		MessageBox(hwnd, serverAddr, nullptr, MB_OK);
		printf(" connect() error! \n");
		exit(0);
		return E_FAIL;
	}

	retval = WSAAsyncSelect( m_Socket, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT );
	if( retval == SOCKET_ERROR )
	{
		printf( " WSAAsyncSelect() error! \n" );
		return E_FAIL;
	}

	return S_OK;
}

void CNetworkMgr::processSocketMessage( HWND hWnd, LPARAM lParam )
{
	if( WSAGETSELECTERROR( lParam ) )
	{
		printf( " WSAGETSELECTERROR()! \n" );
		return;
	}

	switch( WSAGETSELECTEVENT( lParam ) )
	{
	case FD_READ:
	{
		DWORD iobyte, ioflag = 0;
		int retval = WSARecv( m_Socket, &m_wsaRecvBuf, 1, &iobyte, &ioflag, NULL, NULL );
		if( GetLastError() == WSAEWOULDBLOCK )
		{
			PostMessage( hWnd, WM_SOCKET, m_Socket, FD_READ );
		}
		CNetworkMgr::assemblePacket( iobyte );
		break;
	}
	case FD_WRITE:
		break;
	case FD_CLOSE:
		printf( " 접속 종료 \n" );
		break;
	}
}

void CNetworkMgr::assemblePacket( int recvByte )
{
	char *pRecvBuf = m_recvBuf;

	while( 0 < recvByte )
	{
		if( 0 == m_iCurrPacketSize )
		{
			if( recvByte + m_iStoredPacketSize >= sizeof( HEADER ) )
			{
				int restHeaderSize = sizeof( HEADER ) - m_iStoredPacketSize;
				memcpy( m_saveBuf + m_iStoredPacketSize, pRecvBuf, restHeaderSize );
				pRecvBuf += restHeaderSize;
				m_iStoredPacketSize += restHeaderSize;
				recvByte -= restHeaderSize;
				m_iCurrPacketSize = ( ( HEADER* )m_saveBuf )->size;
			}
			else
			{
				memcpy( m_saveBuf + m_iStoredPacketSize, pRecvBuf, recvByte );
				m_iStoredPacketSize += recvByte;
				recvByte = 0;
				break;
			}
		}

		int restSize = m_iCurrPacketSize - m_iStoredPacketSize;

if (restSize <= recvByte)
{
	memcpy(m_saveBuf + m_iStoredPacketSize, pRecvBuf, restSize);

	CNetworkMgr::processPacket();

	m_iCurrPacketSize = m_iStoredPacketSize = 0;

	pRecvBuf += restSize;
	recvByte -= restSize;
}
else
{
	memcpy(m_saveBuf + m_iStoredPacketSize, pRecvBuf, recvByte);

	m_iStoredPacketSize += recvByte;
	recvByte = 0;
}
	}
}

void CNetworkMgr::processPacket()
{
	HEADER* header = (HEADER*)m_saveBuf;

	switch (header->packetID)
	{
	case PAK_ANS_Login:
	{
		S_Login *packet = (S_Login*)m_saveBuf;
		m_nPlayerID = packet->id;
		printf("로그인:: 아이디는 [%d] 입니다. \n", m_nPlayerID);
		break;
	}
	case PAK_ID::PAK_ANS_RoomList:
	{
		S_RoomList* packet = (S_RoomList*)m_saveBuf;
		for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
		{
			RoomInfo r = packet->roomInfo[i];
			printf("[%d]번 방 - 인원: %d명, 플레이 상태: %d \n", i, r.playerCount, r.playing);
		}
		if (m_nRoomNum >= 0)
		{
			m_nRoomNum = -1;
			m_bMaster = m_bReady = false;
		}
		m_pScene->NotifyRoomInfo(packet);
		break;
	}
	case PAK_ID::PAK_ANS_PlayerList:
	{
		S_PlayerList* packet = (S_PlayerList*)m_saveBuf;
		printf("========================================= \n");
		printf("\t\t [방 정보] \t\t \n");
		m_dwPlayerCnt = packet->playerCount;

		for (UINT i = 0; i < m_dwPlayerCnt; ++i)
		{
			m_tPlayerInfo[i] = packet->playerInfo[i];
			printf(" id[%d] 캐릭터[%d] 레디[%d] 방장[%d] \n", (int)m_tPlayerInfo[i].id, m_tPlayerInfo[i].character, m_tPlayerInfo[i].isReady, m_tPlayerInfo[i].isMaster);
			if (m_tPlayerInfo[i].id == m_nPlayerID)
			{
				if (m_nRoomNum == -1)
				{
					m_nRoomNum = packet->roomNum;
				}

				if (m_tPlayerInfo[i].isMaster == TRUE && m_bMaster == false)
				{
					printf(" --> 내가 방장이 되었습니다. <-- \n");
					m_bMaster = true;
				}
			}
		}
		printf("========================================= \n");
		m_pScene->NotifyPlayerInfo(m_tPlayerInfo, m_dwPlayerCnt);
		break;
	}
	case PAK_ID::PAK_ANS_ReadyGame:
	{
		printf("========================================= \n");
		printf("\t\t [게임 준비] \t\t \n");
		printf("========================================= \n");
		m_pScene->NotifyGameStart();
		break;
	}
	case PAK_ID::PAK_ANS_StartGame:
	{
		printf("========================================= \n");
		printf("\t\t [게임 시작] \t\t \n");
		printf("========================================= \n");
		m_pScene->NotifyGameStart();
		break;
	}
	case PAK_ID::PAK_ANS_Crown:
	{
		S_Crown* packet = (S_Crown*)m_saveBuf;
		if (packet->id == 0)
		{
			printf(" --> 왕관이 떨어졌다! \n");
		}
		else
		{
			printf(" --> [%d] 플레이어가 왕관 획득! \n", packet->id);
		}
		m_pScene->NotifyCrownOwner(packet->id);
		break;
	}
	case PAK_ID::PAK_ANS_Winner:
	{
		S_Winner* packet = (S_Winner*)m_saveBuf;
		if (packet->id == 0)
		{
			printf("========================================= \n");
			printf("\t\t 무승부 \t\t \n");
			printf("========================================= \n");
			m_pScene->NotifyWinner(m_nPlayerID);
		}
		else
		{
			printf("========================================= \n");
			printf("\t\t 승자는 id[%d] \t\t \n", packet->id);
			printf("========================================= \n");
			m_pScene->NotifyWinner(packet->id);
		}
		break;
	}
	case PAK_ID::PAK_ANS_FinishGame:
	{
		printf("========================================= \n");
		printf("\t\t [게임 완료] \t\t \n");
		printf("========================================= \n");
		m_pScene->NotifyGameFinished();
		break;
	}
	case PAK_ID::PAK_RJT_Request:
	{
		printf( "\n !! 요청 실패 !! \n" );
		MessageBox(NULL, "the most recent request failed! ", nullptr, MB_OK);
		break;
	}
	case PAK_ID::PAK_ANS_SyncPlayer:
	{
		S_SyncPlayer* packet = (S_SyncPlayer* )m_saveBuf;
		for( unsigned int i = 0; i < m_dwPlayerCnt; ++i )
		{
			XMFLOAT3 position;
			position.x = packet->playerPositions[i].position.x;
			position.y = packet->playerPositions[i].position.y;
			position.z = packet->playerPositions[i].position.z;
			m_pScene->Sync( packet->playerPositions[ i ].id, packet->playerPositions[i].hp, position, packet->playerPositions[ i ].rotY , packet->playerPositions[i].state );
		}
		m_pScene->SetPlayingTime(packet->playingTime);
		m_pScene->SetWinningTime(packet->winningTime);
		break;
	}
	case PAK_ID::PAK_ANS_SyncDynamic:
	{
		CPhysics::GetInstance()->UpdateDynamicActors((S_SyncDynamic*)m_saveBuf);
		break;
	}
	case PAK_ID::PAK_ANS_SyncDynamicOne:
	{
		CPhysics::GetInstance()->UpdateDynamicOneActor((S_SyncDynamicOne*)m_saveBuf);
		break;
	}
	default:
		printf("Unknown packet ID : %d \n", header->packetID);
	}
}

void CNetworkMgr::sendBufData()
{
	m_wsaSendBuf.len = ( ( HEADER* )m_wsaSendBuf.buf )->size;

	DWORD iobyte;
	if( WSASend( m_Socket, &m_wsaSendBuf, 1, &iobyte, 0, NULL, NULL ) == SOCKET_ERROR )
	{
		printf( " send() Error! \n" );
	}
}

void CNetworkMgr::sendRequestRoomList()
{
	HEADER *pReqRoom = ( HEADER* )m_sendBuf;
	pReqRoom->size = sizeof( HEADER );
	pReqRoom->packetID = PAK_ID::PAK_REQ_RoomList;

	sendBufData();
}

void CNetworkMgr::sendEnterRoom( int roomNum )
{
	C_RoomEnter *pEnterRoom = ( C_RoomEnter* )m_sendBuf;
	pEnterRoom->header.size = sizeof( C_RoomEnter );
	pEnterRoom->header.packetID = PAK_ID::PAK_REQ_EnterRoom;
	pEnterRoom->roomNumber = roomNum;

	sendBufData();
}

void CNetworkMgr::sendExitRoom()
{
	HEADER *pExitRoom = ( HEADER* )m_sendBuf;
	pExitRoom->size = sizeof( HEADER );
	pExitRoom->packetID = PAK_ID::PAK_REQ_ExitRoom;

	sendBufData();
}

void CNetworkMgr::sendReadyRoom()
{
	C_RoomReady *pRoomReady = ( C_RoomReady* )m_sendBuf;
	pRoomReady->header.size = sizeof( C_RoomReady );
	pRoomReady->header.packetID = PAK_ID::PAK_REQ_ReadyRoom;
	m_bReady = !m_bReady;
	pRoomReady->ready = m_bReady;

	sendBufData();
}

void CNetworkMgr::sendStartRoom()
{
	HEADER *pStartGame = ( HEADER* )m_sendBuf;
	pStartGame->size = sizeof( HEADER );
	pStartGame->packetID = PAK_ID::PAK_REQ_StartRoom;

	sendBufData();
}

void CNetworkMgr::sendMoveCharacter( NxVec3 dir)
{
	C_Move *pMove = ( C_Move* )m_sendBuf;
	pMove->header.packetID = PAK_ID::PAK_REQ_Move;
	pMove->header.size = sizeof( C_Move );
	pMove->vDir.x = dir.x;
	pMove->vDir.y = dir.y;
	pMove->vDir.z = dir.z;

	sendBufData();
}


void CNetworkMgr::sendSelectCharacter( CHARACTER ch )
{
	C_RoomCharacter* pRoomCharacter = ( C_RoomCharacter* )m_sendBuf;
	pRoomCharacter->header.size = sizeof( C_RoomCharacter );
	pRoomCharacter->header.packetID = PAK_ID::PAK_REQ_Character;
	pRoomCharacter->character = ch;

	sendBufData();
}

void CNetworkMgr::sendCharacterState( STATE state )
{
	C_State* pState = (C_State*)m_sendBuf;
	pState->header.size = sizeof(C_State);
	pState->header.packetID = PAK_ID::PAK_REQ_State;
	pState->state = state;

	sendBufData();
}

void CNetworkMgr::unreadyAllPlayer( void )
{
	for( UINT i = 0; i < m_dwPlayerCnt; ++i )
		m_tPlayerInfo[ i ].isReady = FALSE;
	m_bReady = false;
}

DWORD CNetworkMgr::Release( void )
{
	closesocket( m_Socket );
	WSACleanup();
	delete this;

	return 0;
}
