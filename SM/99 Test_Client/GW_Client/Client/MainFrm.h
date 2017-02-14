#pragma once

#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

#include "Define.h"

class CGraphicDev;
class CScene;
/* Client MainFrame */
class CMainFrm
{
private:
	/* MainFrame Initialize */
	HRESULT		Initialize( const HWND hWnd );
public:
	/* MainFrame Update */
	int			Update( const float& fTimeDelta );
	/* MainFrame Render */
	void		Render( void );
	/* MainFrame Release */
	void		Release( void );
public:
	/* MainFrame Create */
	static		CMainFrm*	Create( const HWND hWnd );
private:
	HRESULT	Change_Scene( void );
private:
	CGraphicDev*	m_pGraphicDev;
	HWND			m_hWnd;

	/* Scene */
	CScene*			m_pScene;
	BYTE			m_bySceneNum;

	/* For FPS */
	TCHAR		m_szFPS[ MIN_STR ];
	DWORD		m_dwFrameCnt;
	float		m_fAccTime;
	/* Constructor & Destructor - Private -> new(X) */
private:
	CMainFrm();
	~CMainFrm();
};

#endif