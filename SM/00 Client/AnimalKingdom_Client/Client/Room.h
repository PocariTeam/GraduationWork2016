#pragma once

#ifndef Room_h__
#define Room_h__

#include "Scene.h"
#include "Define.h"
#include "Protocol.h"

class CShader;
class CRoom
	: public CScene
{
private:
	explicit CRoom();
	virtual ~CRoom();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	int		Update( const float& fTimeDelta );
	DWORD	Release( void );
	void	Render( ID3D11DeviceContext* pContext );
public:
	static	CScene*	Create( HWND hWnd, ID3D11Device* pDevice );
public:
	void	NotifyGameStart( void ) { m_bStart = true; }
	void	NotifyPlayerInfo( PlayerInfo* pPlayerInfo ) { m_pPlayerInfo = pPlayerInfo; }
	void	NotifyPlayerCnt( UINT* pPlayerCnt ) { m_pPlayerCnt = pPlayerCnt; }
private:
	CShader*		m_pShader;
	RECT			m_rcRoom[ GAMEROOM_CAPACITY ];
	bool			m_bStart;
	PlayerInfo*		m_pPlayerInfo;
	UINT*			m_pPlayerCnt;

	/* Check Key */
	bool			m_bOverlapped;
};


#endif // Room_h__
