#pragma once

#ifndef Jungle_h__
#define Jungle_h__

#include "Scene.h"

class CPlayer;
class CJungle
	: public CScene
{
private:
	CJungle();
	virtual ~CJungle();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void  Render( ID3D11DeviceContext* pContext );
	void  Move(UINT32 id, XMFLOAT3 vDir, STATE state);
	void	NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt );
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice );
private:
	map<int, CPlayer*>		m_mapPlayer;
	int						m_iPlayerID;
	PlayerInfo*				m_pPlayerInfo;
	UINT					m_dwPlayerCnt;
};

#endif // Jungle_h__