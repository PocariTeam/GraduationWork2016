#pragma once

#ifndef RenderTargetMgr_h__
#define RenderTargetMgr_h__

#include "Singleton.h"

#include "RenderTarget.h"
class CRenderTarget;
class CRenderTargetMgr
	: public CSingleton<CRenderTargetMgr>
{
public:
	enum eRT_Type {	RT_BACK, RT_NORMAL, RT_DEPTH, RT_ALBEDO, RT_LIGHT, RT_SPECULAR, RT_END };
public:
	HRESULT Initialize( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain, const WORD& wSizeX, const WORD& wSizeY );
	DWORD	Release( void );

private:
	void	Add( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain, const WORD& wSizeX, const WORD& wSizeY );
	HRESULT	CreateDepthStencilBuffer( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY );
	HRESULT	CreateConstantBuffer( ID3D11Device* pDevice );
	void	SetConstantBuffer( ID3D11DeviceContext* pContext, eRT_Type eType );
	void	AssembleRenderTargetView( void );
	void	AssembleShaderResourceView( void );

public:
	void				ResizeRenderTarget( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, IDXGISwapChain* pSwapChain, const WORD& wSizeX, const WORD& wSizeY );
	void				ClearRenderTargetView( ID3D11DeviceContext* pContext );
	void				ClearDepthStencilView( ID3D11DeviceContext* pContext );
	void				SetRenderTargetView( ID3D11DeviceContext* pContext, UINT iSelect, UINT iCnt );
	void				SetShaderResourceView( ID3D11DeviceContext* pContext, UINT iStartSlot, UINT iSelect, UINT iCnt );
	ID3D11Texture2D*	GetTexture( eRT_Type eType ) { return m_vecRenderTarget[ eType ]->Get_Texture(); }
	void				Render( ID3D11DeviceContext* pContext );
private:
	vector<CRenderTarget*>		m_vecRenderTarget;
	ID3D11RenderTargetView**	m_pArrRenderTargetView;
	ID3D11ShaderResourceView**	m_pArrShaderResourceView;

	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11Texture2D*			m_pDepthStencilBuffer;

	ID3D11Buffer*				m_pConstantBuffer;
};

#endif // RenderTargetMgr_h__