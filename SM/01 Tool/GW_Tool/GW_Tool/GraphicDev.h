#pragma once

#ifndef GraphicDev_h__
#define GraphicDev_h__

#include "Tool_Header.h"

class CGraphicDev
{
public:
	enum WINMODE { MODE_FULL, MODE_WIN };
private:
	explicit CGraphicDev( void );
	~CGraphicDev( void );
public: // getter
	ID3D11Device* Get_Device( void ) { return m_pDevice; }
	ID3D11DeviceContext* Get_Context( void ) { return m_pDeviceContext; }
public:
	void		BeginScene( void );
	void		EndScene( void );
public:
	static CGraphicDev* Create( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY );
public:
	// Display ���� �Լ�
	HRESULT Ready_GraphicDev( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY );
	void	Clear_DeathBuffer( float fInit );
private:
	HRESULT Create_SwapChain( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY );
	HRESULT Create_RenderTargetView();
	HRESULT Create_StencilView( const WORD& wSizeX, const WORD& wSizeY );
private:
	ID3D11Device*				m_pDevice;			// Device
	IDXGISwapChain*				m_pSwapChain;		// Swap Chain
	ID3D11DeviceContext*		m_pDeviceContext;	// Context

	ID3D11RenderTargetView*		m_pRenderTargetView;// Render Target View
	ID3D11Texture2D*			m_pBackBuffer;		// 

													// Stencil
	ID3D11Texture2D*			m_pStencilBuffer;	// Stencil Buffer
	ID3D11DepthStencilView*		m_pStencilView;		// Depth Stencil View
public:
	void Release( void );
};

#endif