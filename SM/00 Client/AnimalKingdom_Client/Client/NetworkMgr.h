#pragma once

#ifndef NetworkMgr_h__
#define NetworkMgr_h__

#include "Singleton.h"

class CScene;
class CPlayer;
class CNetworkMgr
	: public CSingleton<CNetworkMgr>
{
public:
	HRESULT Initialize( HWND hWnd );
	DWORD	Release( void );
private:
	WSADATA			m_wsa;
	SOCKET			m_Socket;
	SOCKADDR_IN		m_tServerAdrr;
	CScene*			m_pScene;
	CPlayer**		m_pPlayerArray;
};


#endif // NetworkMgr_h__
