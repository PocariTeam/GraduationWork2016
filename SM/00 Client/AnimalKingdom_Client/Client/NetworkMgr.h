#pragma once

#ifndef NetworkMgr_h__
#define NetworkMgr_h__

#include "Singleton.h"
#include "Protocol.h"

#pragma warning( disable : 4996 )
#define		WM_SOCKET				WM_USER + 1

class CScene;
class CPlayer;
class CNetworkMgr
	: public CSingleton<CNetworkMgr>
{
public:
	HRESULT Initialize();
	HRESULT connectServer( HWND hWnd );
	DWORD	Release( void );

	void	processSocketMessage( HWND hwnd, LPARAM lParam );
	void	assemblePacket( int recvByte );
	void	processPacket();

	void	sendBufData();
	void	sendEnterRoom( int roomNum );
	void	sendExitRoom();
	void	sendReadyRoom();
	void	sendStartRoom();
	void	sendSelectCharacter( S_CHARACTER ch );

public:
	bool	isMaster() { return m_bMaster; }
private:
	SOCKET			m_Socket;
	SOCKADDR_IN		m_tServerAdrr;
	char			m_sendBuf[ SOCKET_BUF_SIZE ];
	char			m_recvBuf[ SOCKET_BUF_SIZE ];
	char			m_saveBuf[ SOCKET_BUF_SIZE ];
	int				m_iCurrPacketSize;
	int				m_iStoredPacketSize;

	int				m_nPlayerID;
	int				m_nRoomNum;
	bool			m_bReady;
	bool			m_bMaster;

	CScene*			m_pScene;
	CPlayer**		m_pPlayerArray;
};


#endif // NetworkMgr_h__
