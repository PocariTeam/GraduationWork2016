#pragma once

#ifndef RenderTargetMgr_h__
#define RenderTargetMgr_h__

#include "Singleton.h"

#include "RenderTarget.h"
class CRenderTarget;
class CGraphicDev;
class CRenderTargetMgr
	: public CSingleton<CRenderTargetMgr>
{
public:
	enum eRT_Type {	RT_BACK, RT_NORMAL, RT_DEPTH, RT_ALBEDO, RT_LIGHT, RT_SPECULAR, RT_END };
public:
	HRESULT Initialize( CGraphicDev* pGraphicDev, const WORD& wSizeX, const WORD& wSizeY );
	DWORD	Release( void );

private:
	void	Add( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain, const WORD& wSizeX, const WORD& wSizeY, int iType );
	HRESULT	CreateDepthStencilBuffer( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY );
	HRESULT	CreateShadowMap( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY );
	HRESULT	CreateConstantBuffer( ID3D11Device* pDevice );
	void	SetConstantBuffer( ID3D11DeviceContext* pContext, eRT_Type eType );
	void	AssembleRenderTargetView( void );
	void	AssembleShaderResourceView( void );

public:
	void				ResizeRenderTarget( const WORD& wSizeX, const WORD& wSizeY );
	void				ClearRenderTargetView( ID3D11DeviceContext* pContext );
	void				ClearDepthStencilView( ID3D11DeviceContext* pContext );
	void				ClearShadowDepthStencilView( ID3D11DeviceContext* pContext );
	void				ResetRenderTargetView( ID3D11DeviceContext* pContext );
	void				SetRenderTargetView( ID3D11DeviceContext* pContext, UINT iSelect, UINT iCnt );
	void				SetShaderResourceView( ID3D11DeviceContext* pContext, UINT iStartSlot, UINT iSelect, UINT iCnt );
	ID3D11Texture2D*	GetTexture( eRT_Type eType ) { return m_vecRenderTarget[ eType ]->Get_Texture(); }
	void				Render( ID3D11DeviceContext* pContext );
	void				Shadow_Begin( void ) { m_pActiveDepthStencilView = m_pShadowDepthStencilView; }
	void				Shadow_End( void ) { m_pActiveDepthStencilView = m_pDepthStencilView; }
private:
	CGraphicDev*				m_pGraphicDev;
	vector<CRenderTarget*>		m_vecRenderTarget;
	ID3D11RenderTargetView**	m_pArrRenderTargetView;
	ID3D11ShaderResourceView**	m_pArrShaderResourceView;

	ID3D11DepthStencilView*		m_pDepthStencilView;

	ID3D11DepthStencilView*		m_pShadowDepthStencilView;
	ID3D11ShaderResourceView*	m_pShadowShaderResourceView;

	ID3D11DepthStencilView*		m_pActiveDepthStencilView;

	ID3D11Buffer*				m_pConstantBuffer;
};

#endif // RenderTargetMgr_h__