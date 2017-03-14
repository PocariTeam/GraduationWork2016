#include "stdafx.h"
#include "NetworkMgr.h"

CNetworkMgr*	CSingleton<CNetworkMgr>::m_pInstance;

HRESULT CNetworkMgr::Initialize()
{
	ZeroMemory(m_recvBuf, sizeof(m_recvBuf));
	ZeroMemory(m_saveBuf, sizeof(m_saveBuf));
	m_iCurrPacketSize = 0;
	m_iStoredPacketSize = 0;
	m_nPlayerID = -1;
	m_bMaster = false;

	WSADATA	wsa;

	if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsa) )
		return E_FAIL;

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	if (m_Socket == INVALID_SOCKET)
	{
		printf(" socket() error! \n");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNetworkMgr::connectServer(HWND hwnd)
{
	char serverAddr[256];
	strcpy(serverAddr, "127.0.0.1"); // �ӽ÷� �������ּ� ���
	//printf("�����ּ� �Է�(xxx.xxx.xxx.xxx):");
	//scanf("%s", serverAddr);

	ZeroMemory(&m_tServerAdrr, sizeof(m_tServerAdrr));
	m_tServerAdrr.sin_family = AF_INET;
	m_tServerAdrr.sin_port = htons(SERVER_PORT);
	m_tServerAdrr.sin_addr.s_addr = inet_addr(serverAddr);

	int retval = connect(m_Socket, (SOCKADDR *)&m_tServerAdrr, sizeof(m_tServerAdrr));
	if (retval == SOCKET_ERROR)
	{
		printf(" connect() error! \n");
		return E_FAIL;
	}

	retval = WSAAsyncSelect(m_Socket, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);
	if (retval == SOCKET_ERROR)
	{
		printf(" WSAAsyncSelect() error! \n");
		return E_FAIL;
	}

	return S_OK;
}

void CNetworkMgr::processSocketMessage(HWND hWnd, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		printf(" WSAGETSELECTERROR()! \n");
		return;
	}
	
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		int retval = recv(m_Socket, m_recvBuf, sizeof(m_recvBuf), 0);
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			PostMessage(hWnd, WM_SOCKET, m_Socket, FD_READ);
		}
		CNetworkMgr::assemblePacket(retval);
		break;
	}
	case FD_WRITE:
		break;
	case FD_CLOSE:
		printf(" ���� ���� \n");
		CNetworkMgr::Release();
		break;
	}
}

void CNetworkMgr::assemblePacket(int recvByte)
{
	char *pRecvBuf = m_recvBuf;

	while (0 < recvByte)
	{
		if (0 == m_iCurrPacketSize)
		{
			if (recvByte + m_iStoredPacketSize >= sizeof(HEADER))
			{
				int restHeaderSize = sizeof(HEADER) - m_iStoredPacketSize;
				memcpy(m_saveBuf + m_iStoredPacketSize, pRecvBuf, restHeaderSize);
				pRecvBuf += restHeaderSize;
				m_iStoredPacketSize += restHeaderSize;
				recvByte -= restHeaderSize;
				m_iCurrPacketSize = ((HEADER*)m_saveBuf)->size;
			}
			else
			{
				memcpy(m_saveBuf + m_iStoredPacketSize, pRecvBuf, recvByte);
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
	case PAK_ANS_LOGIN:
	{
		S_Login *packet = (S_Login*)m_saveBuf;
		m_nPlayerID = packet->id;
		printf("�α���:: ���̵�� [%d] �Դϴ�. \n", m_nPlayerID);
		break;
	}
	case PAK_ID::PAK_RJT_Request:
	{
		printf("\n !! ��û ���� !! \n");
		break;
	}
	case PAK_ID::PAK_ANS_RoomList:
	{
		S_RoomList* packet = (S_RoomList*)m_saveBuf;
		for (int i = 0; i < GAMEROOM_CAPACITY; ++i)
		{
			RoomInfo r = packet->roomInfo[i];
			printf("[%d]�� �� - �ο�: %d��, �÷��� ����: %d \n", i, r.playerCount, r.playing);
		}
		break;
	}
	case PAK_ID::PAK_ANS_PlayerList:
	{
		S_PlayerList* packet = (S_PlayerList*)m_saveBuf;
		printf("========================================= \n");
		printf("\t\t [�� ����] %\t\t \n");
		for (int i = 0; i < packet->playerCount; ++i)
		{
			PlayerInfo r = packet->playerInfo[i];
			printf(" id[%d] ĳ����[%d] ����[%d] ����[%d] \n", r.id, r.character, r.isReady, r.isMaster);
			if (r.id == m_nPlayerID)
			{
				if (r.isMaster == true && m_bMaster == false)
				{
					printf(" --> ���� ������ �Ǿ����ϴ�. <-- \n");
					m_bMaster = true;

				}
			}
		}
		printf("========================================= \n");
		break;
	}
	case PAK_ID::PAK_ANS_StartGame:
	{
		S_PlayerList* packet = (S_PlayerList*)m_saveBuf;
		printf("========================================= \n");
		printf("\t\t [���ӽ���] %\t\t \n");
		for (int i = 0; i < packet->playerCount; ++i)
		{
			PlayerInfo r = packet->playerInfo[i];
			printf(" id[%d] ĳ����[%d] ����[%d] ����[%d] \n", r.id, r.character, r.isReady, r.isMaster);
		}
		printf("========================================= \n");
		break;
	}
	}
}

DWORD CNetworkMgr::Release( void )
{
	closesocket( m_Socket );
	WSACleanup();
	delete this;

	return 0;
}
