#pragma once

#ifndef Jungle_h__
#define Jungle_h__

#include "Scene.h"

class CPlayer;
class CNumber_UI;
class CJungle
	: public CScene
{
	enum eNum_Type { NUM_TEN, NUM_ONE, NUM_PROCENT, NUM_COLON, NUM_CENTI, NUM_END };
	using NUM_TYPE = eNum_Type;
private:
	CJungle();
	virtual ~CJungle();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
	void	AccumulateTime( const float& fTimeDelta );
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void  Render( ID3D11DeviceContext* pContext );
	void  Sync( UINT id, XMFLOAT3 vPos, float fRotateY, STATE state );
	void  NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt );
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice );
private:
	map<int, CPlayer*>		m_mapPlayer;
	int						m_iPlayerID;
	PlayerInfo*				m_pPlayerInfo;
	UINT					m_dwPlayerCnt;
	CNumber_UI**			m_dpTime_UI;
	float					m_fAccTime;
};

#endif // Jungle_h__