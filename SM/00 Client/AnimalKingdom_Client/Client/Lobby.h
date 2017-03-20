#pragma once

#ifndef Lobby_h__
#define Lobby_h__

#include "Scene.h"
#include "Define.h"
#include "Protocol.h"

class CShader;
class CLobby
	: public CScene
{
private:
	explicit CLobby();
	virtual ~CLobby();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	int		Update( const float& fTimeDelta );
	DWORD	Release( void );
	void	Render( ID3D11DeviceContext* pContext );
private:
	int	Check_Key( void );
public:
	static	CScene*	Create( HWND hWnd, ID3D11Device* pDevice );
private:
	RECT			m_rcRoom[ GAMEROOM_CAPACITY ];
	/* Check Key */
	bool			m_bOverlapped;
};


#endif // Lobby_h__
