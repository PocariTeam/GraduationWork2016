#pragma once

#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

// #include "Define.h"

class CGraphicDev;
class CScene;
/* Client MainFrame */
class CMainFrm
{
private:
	/* MainFrame Initialize */
	HRESULT		Initialize( const HINSTANCE hInst, const HWND hWnd );
	/* Ready Logo Scene */
	HRESULT		Ready_Logo( ID3D11Device* pDevice );
public:
	/* MainFrame Update */
	int			Update( const float& fTimeDelta );
	/* MainFrame Render */
	void		Render( void );
	/* MainFrame Release */
	DWORD		Release( void );
	/* GraphicDev Resize RenderTarget */
	void		ResizeRenderTarget( const WORD& wSizeX, const WORD& wSizeY );
public:
	/* MainFrame Create */
	static		CMainFrm*	Create( const HINSTANCE hInst, const HWND hWnd );
private:
	HRESULT	Change_Scene( void );
	void	Check_Key( void );
private:
	CGraphicDev*	m_pGraphicDev;
	HWND			m_hWnd;

	/* Scene */
	CScene*				m_pScene;
	BYTE				m_bySceneNum;

	/* For FPS */
	DWORD		m_dwCnt;
	char		m_szFPS[ MAX_PATH ];
	DWORD		m_dwFrameCnt;
	float		m_fAccTime;

	/* Check Key */
	bool		m_bOverlapped;

private:
	CMainFrm();
	~CMainFrm();
};

#endif