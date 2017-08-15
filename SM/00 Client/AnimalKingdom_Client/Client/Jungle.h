#pragma once

#ifndef Jungle_h__
#define Jungle_h__

#include "Scene.h"

class CNormal_UI;
class CBar_UI;
class CPlayer;
class CNumber_UI;
class CSection;
class CSkill_UI;
class CJungle
	: public CScene
{
	enum eNum_Type { NUM_TEN, NUM_ONE, NUM_PROCENT, NUM_COLON, NUM_CENTI, NUM_END };
	using NUM_TYPE = eNum_Type;
private:
	CJungle();
	virtual ~CJungle();
private:
	HRESULT			Initialize( HWND hWnd, ID3D11Device* pDevice );
	virtual void	SetPlayingTime( const float& fTime );
	virtual void	SetWinningTime(const float& fTime);
	void			Change_CrownUI_Position( void );
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void  Render( ID3D11DeviceContext* pContext );
	void  Sync( UINT id, float hp, XMFLOAT3 vPos, float fRotateY, STATE state, FLOAT canUseSkill);
	void  NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt );
	void  Check_Key( const float& fTimeDelta );
	void  Check_PlayerSkill( void );
	void  Change_CameraDest( void );
	virtual void  NotifyGameStart( void );
	virtual void  NotifyGameFinished();
	virtual void  NotifyWinner( UINT ID );
	virtual void NotifyCrownOwner(UINT ID);
	virtual void NotifyUseSkill(UINT ID, BOOL use);
	virtual void NotifyDefendEnd(UINT ID);
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice );
private:
	map<int, CPlayer*>		m_mapPlayer;
	int						m_iPlayerID;
	int						m_iCrownIndex;
	PlayerInfo*				m_pPlayerInfo;
	UINT					m_dwPlayerCnt;
	CNumber_UI**			m_dpTime_UI;
	CNumber_UI**			m_dpCrownTime_UI;
	CNormal_UI*				m_pCrownmark_UI;
	CSection**				m_dpSection;
	bool					m_bDebug;
	/* Check Key */
	bool					m_bOverlapped;
	/* Ready / Start / End */
public:
	static	bool			m_bStart;
	static  bool			m_bFocusIncave;
	static  int				m_iFocus;		// 자신이 죽고, 다른 플레이어 볼 때 사용( 다른 플레이어 ID )
private:
	bool					m_bFinished;
	CNormal_UI*				m_pStateNotify;
	/* HP Bar */
	CBar_UI**				m_dpHP_Bar;
	/* Skill Time UI */
	CSkill_UI**				m_dpSkill_UI;
};

#endif // Jungle_h__