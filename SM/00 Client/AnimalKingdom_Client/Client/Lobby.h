#pragma once

#ifndef Lobby_h__
#define Lobby_h__

#include "Scene.h"
#include "Define.h"

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
public:
	static	CScene*	Create( HWND hWnd, ID3D11Device* pDevice );
private:
	CShader*		m_pShader;
	RECT			m_rcRoom[ MAX_ROOM_CNT ];
};


#endif // Lobby_h__
