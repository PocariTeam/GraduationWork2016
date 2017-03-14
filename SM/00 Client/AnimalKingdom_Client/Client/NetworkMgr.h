#pragma once

#ifndef NetworkMgr_h__
#define NetworkMgr_h__

#include "Singleton.h"
#include "Protocol.h"

#define		WM_SOCKET				WM_USER +1

class CScene;
class CPlayer;
class CNetworkMgr
	: public CSingleton<CNetworkMgr>
{
public:
	HRESULT Initialize();
	HRESULT connectServer(HWND hWnd);
	void	processSocketMessage(HWND hwnd, LPARAM lParam);
	void	assemblePacket(int recvByte);
	void	processPacket();
	DWORD	Release( void );
private:
	SOCKET			m_Socket;
	SOCKADDR_IN		m_tServerAdrr;
	char			m_recvBuf[SOCKET_BUF_SIZE];
	char			m_saveBuf[SOCKET_BUF_SIZE];
	int				m_iCurrPacketSize;
	int				m_iStoredPacketSize;
	int				m_nPlayerID;
	bool			m_bMaster;

	CScene*			m_pScene;
	CPlayer**		m_pPlayerArray;
};


#endif // NetworkMgr_h__
