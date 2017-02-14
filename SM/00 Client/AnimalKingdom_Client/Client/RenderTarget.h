#pragma once

#ifndef RenderTarget_h__
#define RenderTarget_h__

class CRenderTarget
{
private:
	explicit CRenderTarget();
	~CRenderTarget() = default;
private:
	HRESULT Initialize( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain );
	HRESULT Initialize( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY );
	HRESULT CreateRenderTarget( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain );
	HRESULT CreateRenderTarget( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY );
public:
	DWORD	Release( void );
public:
	static CRenderTarget* Create( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain );
	static CRenderTarget* Create( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY );
private:
	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11ShaderResourceView*	m_pShaderResourceView;
	ID3D11Texture2D*			m_pTexture;
public:
	ID3D11RenderTargetView**	Get_RenderTargetView() { return &m_pRenderTargetView; }
	ID3D11ShaderResourceView*	Get_ShaderResourceView() { return m_pShaderResourceView; }
	ID3D11Texture2D*			Get_Texture( void ) { return m_pTexture; }
};

#endif // RenderTarget_h__