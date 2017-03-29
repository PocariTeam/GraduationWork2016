#pragma once

#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

class CGraphicDev;
class CScene;
class CRenderer;
class CInputMgr;
class CRenderTargetMgr;
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
	/* Device */
	CGraphicDev*			m_pGraphicDev;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
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
	/* Singleton */
	CInputMgr*			m_pInputMgr;
	CRenderTargetMgr*	m_pRenderTargetMgr;
	CRenderer*			m_pRenderer;
private:
	CMainFrm();
	~CMainFrm();
};

#endif