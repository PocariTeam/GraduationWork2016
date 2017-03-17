#pragma once

#ifndef Room_h__
#define Room_h__

#include "Scene.h"
#include "Define.h"

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
private:
	CShader*		m_pShader;
	RECT			m_rcRoom[ MAX_ROOM_CNT ];
};


#endif // Room_h__
