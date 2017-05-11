#pragma once

#ifndef Jungle_h__
#define Jungle_h__

#include "Scene.h"

class CNormal_UI;
class CBar_UI;
class CPlayer;
class CNumber_UI;
class CSection;
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
	void	SetPlayingTime( const float& fTime );
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void  Render( ID3D11DeviceContext* pContext );
	void  Sync( UINT id, int hp, XMFLOAT3 vPos, float fRotateY, STATE state );
	void  NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt );
	void  Check_Key( const float& fTimeDelta );
	void  Change_CameraDest( void );
	void  NotifyGameStart( void );
	void  NotifyGameFinished();
	void  NotifyWinner( UINT ID );
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice );
private:
	map<int, CPlayer*>		m_mapPlayer;
	int						m_iPlayerID;
	int						m_iFocus;		// 자신이 죽고, 다른 플레이어 볼 때 사용( 다른 플레이어 ID )
	PlayerInfo*				m_pPlayerInfo;
	UINT					m_dwPlayerCnt;
	CNumber_UI**			m_dpTime_UI;
	CSection**				m_dpSection;
	bool					m_bDebug;
	/* Check Key */
	bool					m_bOverlapped;
	/* Ready / Start / End */
	bool					m_bStart;
	bool					m_bFinished;
	CNormal_UI*				m_pStateNotify;
	/* HP Bar */
	CBar_UI**			m_dpHP_Bar;
};

#endif // Jungle_h__