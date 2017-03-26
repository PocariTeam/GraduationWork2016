#pragma once

#ifndef Room_h__
#define Room_h__

#include "Scene.h"
#include "Define.h"
#include "Protocol.h"

class CShader;
class CButton_UI;
class CNormal_UI;
class CRoom
	: public CScene
{
private:
	enum eBtn_Index { BTN_READY, BTN_BACK, BTN_MAP_PREVIOUS, BTN_MAP_NEXT, BTN_CHAMELEON, BTN_MONKEY, BTN_END };
	using BTN_INDEX = eBtn_Index;
private:
	explicit CRoom();
	virtual ~CRoom();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	int		Update( const float& fTimeDelta );
	DWORD	Release( void );
	void	Render( ID3D11DeviceContext* pContext );
private:
	int		Check_Key( void );
public:
	static	CScene*	Create( HWND hWnd, ID3D11Device* pDevice );
public:
	void	NotifyGameStart( void ) { m_bStart = true; }
	void	NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt );
private:
	CButton_UI**	m_dpBtns;
	CNormal_UI**	m_dpReady;
	bool			m_bStart;
	PlayerInfo*		m_pPlayerInfo;
	UINT			m_dwPlayerCnt;

	/* Check Key */
	bool			m_bOverlapped;
};


#endif // Room_h__
