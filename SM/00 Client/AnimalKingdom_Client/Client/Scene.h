#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include "Enum.h"
#include "Protocol.h"

class CShader;
class CCamera;
class CScene
{
public:
	enum SCENE_TYPE { SCENE_LOGO, SCENE_LOBBY, SCENE_ROOM, SCENE_JUNGLE, SCENE_END };
protected:
	virtual HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	virtual int Update( const float& fTimeDelta );
	virtual DWORD Release( void );
	virtual void Render( ID3D11DeviceContext* pContext );
	POINT	GetMousePosition( HWND hWnd );
public:
	virtual void NotifyRoomInfo( S_RoomList* pRoomlistArray ) {}
	virtual void NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt ) {}
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice, BYTE byStageNum );
protected:
	CScene();
	virtual ~CScene() = default;
protected:
	HWND				 m_hWnd;
	CCamera*			 m_pCamera;
	using SHADERLIST = list<CShader*>;
	SHADERLIST		m_listShader[ RENDER_END ];

};

#endif