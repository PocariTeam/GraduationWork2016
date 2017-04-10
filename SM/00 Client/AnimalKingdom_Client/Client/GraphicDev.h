#pragma once

#ifndef GraphicDev_h__
#define GraphicDev_h__

class CGraphicDev
{
public:
	enum WINMODE { MODE_WIN, MODE_FULL };
private:
	explicit CGraphicDev( void );
	~CGraphicDev( void );
public: // getter
	ID3D11Device* Get_Device( void ) { return m_pDevice; }
	ID3D11DeviceContext* Get_Context( void ) { return m_pDeviceContext; }
	IDXGISwapChain* Get_SwapChain( void ) { return m_pSwapChain; }
public:
	void		EndScene( void );
public:
	static CGraphicDev* Create( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY );
public:
	// Display ���� �Լ�
	HRESULT Initialize( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY );
private:
	HRESULT Create_SwapChain( WINMODE Mode, HWND hWnd, IDXGIFactory* pdxgiFactory, const WORD& wSizeX, const WORD& wSizeY );
public:
	DWORD	ChangeDisplayMode();
	void	SetViewport( const WORD& wSizeX, const WORD& wSizeY );
private:
	ID3D11Device*				m_pDevice;				// Device
	IDXGISwapChain*				m_pSwapChain;			// Swap Chain
	ID3D11DeviceContext*		m_pDeviceContext;		// Context
	DXGI_MODE_DESC*				m_pDisplayMode;
	UINT						m_dwDisplayModeCnt;
public:
	DWORD Release( void );
};

#endif