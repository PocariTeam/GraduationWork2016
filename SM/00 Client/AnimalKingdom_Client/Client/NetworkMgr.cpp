#include "stdafx.h"
#include "NetworkMgr.h"

CNetworkMgr*	CSingleton<CNetworkMgr>::m_pInstance;

HRESULT CNetworkMgr::Initialize()
{
	ZeroMemory( m_sendBuf, sizeof( m_sendBuf ) );
	ZeroMemory( m_recvBuf, sizeof( m_recvBuf ) );
	ZeroMemory( m_saveBuf, sizeof( m_saveBuf ) );
	m_iCurrPacketSize = 0;
	m_iStoredPacketSize = 0;
	m_nPlayerID = -1;
	m_nRoomNum = -1;
	m_bReady = false;
	m_bMaster = false;

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
	strcpy_s( serverAddr, "127.0.0.1" ); // 임시로 루프백주소 사용
	//printf("서버주소 입력(xxx.xxx.xxx.xxx):");
	//scanf("%s", serverAddr);

	ZeroMemory( &m_tServerAdrr, sizeof( m_tServerAdrr ) );
	m_tServerAdrr.sin_family = AF_INET;
	m_tServerAdrr.sin_port = htons( SERVER_PORT );
	m_tServerAdrr.sin_addr.s_addr = inet_addr( serverAddr );

	int retval = connect( m_Socket, ( SOCKADDR * )&m_tServerAdrr, sizeof( m_tServerAdrr ) );
	if( retval == SOCKET_ERROR )
	{
		printf( " connect() error! \n" );
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
		int retval = recv( m_Socket, m_recvBuf, sizeof( m_recvBuf ), 0 );
		if( GetLastError() == WSAEWOULDBLOCK )
		{
			PostMessage( hWnd, WM_SOCKET, m_Socket, FD_READ );
		}
		CNetworkMgr::assemblePacket( retval );
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

		if( restSize <= recvByte )
		{
			memcpy( m_saveBuf + m_iStoredPacketSize, pRecvBuf, restSize );

			CNetworkMgr::processPacket();

			m_iCurrPacketSize = m_iStoredPacketSize = 0;

			pRecvBuf += restSize;
			recvByte -= restSize;
		}
		else
		{
			memcpy( m_saveBuf + m_iStoredPacketSize, pRecvBuf, recvByte );

			m_iStoredPacketSize += recvByte;
			recvByte = 0;
		}
	}
}

void CNetworkMgr::processPacket()
{
	HEADER* header = ( HEADER* )m_saveBuf;

	switch( header->packetID )
	{
	case PAK_ANS_LOGIN:
	{
		S_Login *packet = ( S_Login* )m_saveBuf;
		m_nPlayerID = packet->id;
		printf( "로그인:: 아이디는 [%d] 입니다. \n", m_nPlayerID );
		break;
	}
	case PAK_ID::PAK_ANS_RoomList:
	{
		S_RoomList* packet = ( S_RoomList* )m_saveBuf;
		for( int i = 0; i < GAMEROOM_CAPACITY; ++i )
		{
			RoomInfo r = packet->roomInfo[ i ];
			printf( "[%d]번 방 - 인원: %d명, 플레이 상태: %d \n", i, r.playerCount, r.playing );
		}
		if( m_nRoomNum >= 0 )
		{
			m_nRoomNum = -1;
			m_bMaster = m_bReady = false;
		}
		break;
	}
	case PAK_ID::PAK_ANS_PlayerList:
	{
		S_PlayerList* packet = ( S_PlayerList* )m_saveBuf;
		printf( "========================================= \n" );
		printf( "\t\t [방 정보] \t\t \n" );
		for( UINT i = 0; i < packet->playerCount; ++i )
		{
			PlayerInfo r = packet->playerInfo[ i ];
			printf( " id[%d] 캐릭터[%d] 레디[%d] 방장[%d] \n", ( int )r.id, r.character, r.isReady, r.isMaster );
			if( r.id == m_nPlayerID )
			{
				if( m_nRoomNum == -1 )
				{
					m_nRoomNum = packet->roomNum;
				}

				if( r.isMaster == TRUE && m_bMaster == false )
				{
					printf( " --> 내가 방장이 되었습니다. <-- \n" );
					m_bMaster = true;

				}
			}
		}
		printf( "========================================= \n" );
		break;
	}
	case PAK_ID::PAK_ANS_StartGame:
	{
		S_PlayerList* packet = ( S_PlayerList* )m_saveBuf;
		printf( "========================================= \n" );
		printf( "\t\t [게임시작] \t\t \n" );
		for( UINT i = 0; i < packet->playerCount; ++i )
		{
			PlayerInfo r = packet->playerInfo[ i ];
			printf( " id[%d] 캐릭터[%d] 레디[%d] 방장[%d] \n", ( int )r.id, r.character, r.isReady, r.isMaster );
		}
		printf( "========================================= \n" );
		break;
	}
	case PAK_ID::PAK_RJT_Request:
	{
		printf( "\n !! 요청 실패 !! \n" );
		break;
	}
	}
}

void CNetworkMgr::sendBufData()
{
	if( send( m_Socket, m_sendBuf, ( ( HEADER* )m_sendBuf )->size, 0 ) == SOCKET_ERROR ) {
		printf( " send() Error! \n" );
	}
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
	pRoomReady->header.packetID = PAK_ID::PAK_REQ_Ready;
	m_bReady = !m_bReady;
	pRoomReady->ready = m_bReady;

	sendBufData();
}

void CNetworkMgr::sendStartRoom()
{
	HEADER *pStartGame = ( HEADER* )m_sendBuf;
	pStartGame->size = sizeof( HEADER );
	pStartGame->packetID = PAK_ID::PAK_REQ_StartGame;

	sendBufData();
}

void CNetworkMgr::sendSelectCharacter( S_CHARACTER ch )
{
	C_RoomCharacter* pRoomCharacter = ( C_RoomCharacter* )m_sendBuf;
	pRoomCharacter->header.size = sizeof( C_RoomCharacter );
	pRoomCharacter->header.packetID = PAK_ID::PAK_REQ_Character;
	pRoomCharacter->character = ch;

	sendBufData();
}

DWORD CNetworkMgr::Release( void )
{
	closesocket( m_Socket );
	WSACleanup();
	delete this;

	return 0;
}
