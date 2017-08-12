#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include "Enum.h"

class CShader;
class CCamera;
class CScene
{
public:
	enum SCENE_TYPE { SCENE_LOGO, SCENE_LOBBY, SCENE_ROOM, SCENE_JUNGLE, SCENE_END };
protected:
	virtual HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice);
public:
	virtual int Update(const float& fTimeDelta);
	virtual DWORD Release(void);
	virtual void Render(ID3D11DeviceContext* pContext);
	POINT	GetMousePosition(HWND hWnd);
public:
	virtual void NotifyRoomInfo(S_RoomList* pRoomlistArray) {}
	virtual void NotifyPlayerInfo(PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt) {}
	virtual void NotifyGameStart(void) {}
	virtual void NotifyGameFinished( void ) {}
	virtual void NotifyWinner( UINT ID ) {}
	virtual void Sync( UINT id, float hp, XMFLOAT3 vDir, float fRotateY, STATE state, FLOAT canUseSkill ) {}
	virtual void NotifyCrownOwner(UINT ID) {}
	virtual void NotifyEnterRoom() {};
	virtual void NotifyUseSkill(UINT ID, BOOL use) {}
	virtual void NotifyDefendEnd(UINT ID) {}
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice, BYTE byStageNum );
	virtual void	SetPlayingTime(const float& fTime) {};
	virtual void	SetWinningTime(const float& fTime) {};
protected:
	CScene();
	virtual ~CScene() = default;
protected:
	ID3D11Device*		 m_pDevice;
	HWND				 m_hWnd;
	CCamera*			 m_pCamera;
	float				m_fPlayingTime;
	float				m_fWinningTime;
	using SHADERLIST = list<CShader*>;
	SHADERLIST		m_listShader[ RENDER_END ];
};

#endif