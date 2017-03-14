#include "stdafx.h"
#include "NetworkMgr.h"

CNetworkMgr*	CSingleton<CNetworkMgr>::m_pInstance;

HRESULT CNetworkMgr::Initialize( HWND hWnd )
{
	if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &m_wsa ) )
		return E_FAIL;

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
	m_tServerAdrr.sin_family = AF_INET;
	// m_tServerAdrr.sin_port = htons();


	return S_OK;
}

DWORD CNetworkMgr::Release( void )
{
	closesocket( m_Socket );
	WSACleanup();

	delete this;

	return 0;
}
