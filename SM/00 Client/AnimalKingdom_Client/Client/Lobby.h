#pragma once

#ifndef Lobby_h__
#define Lobby_h__

#include "Scene.h"
#include "Define.h"

class CButton_UI;
class CInputMgr;
class CRenderer;
class CLobby
	: public CScene
{
	const WORD ROOM_PRESENT_CNT = 8;
public:
	enum eBtn_Index
	{
		BTN_REFRESH = 8
		, BTN_PREVIOUS
		, BTN_NEXT
		, BTN_EXIT
		, BTN_END
	};
	using BTN_INDEX = eBtn_Index;
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
	void	NotifyRoomInfo( S_RoomList* pRoomlistPacket );
	void	NotifyEnterRoom();
private:
	int		Check_Key( void );
	void	Change_Page( void );
public:
	static	CScene*	Create( HWND hWnd, ID3D11Device* pDevice );
private:
	/* Room */
	RoomInfo*		m_pRoomInfo;
	UINT			m_dwMaxPageCnt;
	UINT			m_dwCurrentPage;
	UINT			m_dwMaxRoomCnt;
	/* Button */
	CButton_UI**	m_dpBtns;
	/* Check Key */
	bool			m_bOverlapped;
	bool			m_bEnter;
	/* Singleton */
	CRenderer*		m_pRenderer;
	CInputMgr*		m_pInputMgr;
};


#endif // Lobby_h__
