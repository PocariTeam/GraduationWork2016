#include "stdafx.h"
#include "GraphicDev.h"
#include "RenderTargetMgr.h"
#include "RenderTarget.h"
#include "Functor.h"
#include "Value.h"
#include "Define.h"

CRenderTargetMgr* CSingleton<CRenderTargetMgr>::m_pInstance;

HRESULT CRenderTargetMgr::Initialize( CGraphicDev* pGraphicDev, const WORD& wSizeX, const WORD& wSizeY )
{
	m_pGraphicDev = pGraphicDev;
	m_pArrRenderTargetView = nullptr;
	m_pArrShaderResourceView = nullptr;
	m_pConstantBuffer = nullptr;

	m_vecRenderTarget.reserve( RT_END );

	ID3D11Device* pDevice{ m_pGraphicDev->Get_Device() };
	IDXGISwapChain* pSwapChain{ m_pGraphicDev->Get_SwapChain() };

	for( int i = 0; i < RT_END; ++i )
		Add( pDevice, pSwapChain, wSizeX, wSizeY );

	AssembleRenderTargetView();
	AssembleShaderResourceView();
	CreateDepthStencilBuffer( pDevice, wSizeX, wSizeY );

	return CreateConstantBuffer( pDevice );
}

DWORD CRenderTargetMgr::Release( void )
{
	Safe_Release( m_pConstantBuffer );
	Safe_Delete_Array( m_pArrRenderTargetView );
	Safe_Delete_Array( m_pArrShaderResourceView );

	for_each( m_vecRenderTarget.begin(), m_vecRenderTarget.end(), ReleaseElement() );
	m_vecRenderTarget.erase( m_vecRenderTarget.begin(), m_vecRenderTarget.end() );
	m_vecRenderTarget.swap( vector<CRenderTarget*>{} );
	m_pGraphicDev = nullptr;

	delete this;

	return 0;
}

void CRenderTargetMgr::Add( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain, const WORD& wSizeX, const WORD& wSizeY )
{
	CRenderTarget* pRenderTarget{ nullptr };

	if( m_vecRenderTarget.empty() )
		pRenderTarget = CRenderTarget::Create( pDevice, pSwapChain );
	else
		pRenderTarget = CRenderTarget::Create( pDevice, wSizeX, wSizeY );

	if( nullptr != pRenderTarget )
		m_vecRenderTarget.push_back( pRenderTarget );
}

HRESULT CRenderTargetMgr::CreateDepthStencilBuffer( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY )
{
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory( &DepthBufferDesc, sizeof( DepthBufferDesc ) );

	DepthBufferDesc.Width = wSizeX;
	DepthBufferDesc.Height = wSizeY;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;

	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	/*UINT dw4xMsaaQuality = 0;
	if( FAILED( pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &dw4xMsaaQuality ) ) )
	{*/
		DepthBufferDesc.SampleDesc.Count = 1;
		DepthBufferDesc.SampleDesc.Quality = 0;
	/*}

	else
	{
		DepthBufferDesc.SampleDesc.Count = 4;
		DepthBufferDesc.SampleDesc.Quality = dw4xMsaaQuality - 1;
	}*/

	if( FAILED( pDevice->CreateTexture2D( &DepthBufferDesc, NULL, &m_pDepthStencilBuffer ) ) )
		return E_FAIL;

	if( FAILED( pDevice->CreateDepthStencilView( m_pDepthStencilBuffer, NULL, &m_pDepthStencilView ) ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTargetMgr::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( XMFLOAT4 );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, nullptr, &m_pConstantBuffer );

	return S_OK;
}

void CRenderTargetMgr::SetConstantBuffer( ID3D11DeviceContext* pContext, eRT_Type eType )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	XMFLOAT4* pWinSize = ( XMFLOAT4* )MappedSubresource.pData;
	switch( eType )
	{
	case RT_NORMAL:
		*pWinSize = XMFLOAT4( -1.f, 1.f, 0.4f, 0.4f );
		break;
	case RT_DEPTH:
		*pWinSize = XMFLOAT4( -1.f, 0.6f, 0.4f, 0.4f );
		break;
	case RT_ALBEDO:
		*pWinSize = XMFLOAT4( -1.f, 0.2f, 0.4f, 0.4f );
		break;
	case RT_LIGHT:
		*pWinSize = XMFLOAT4( -1.f, -0.2f, 0.4f, 0.4f );
		break;
	case RT_SPECULAR:
		*pWinSize = XMFLOAT4( -1.f, -0.6f, 0.4f, 0.4f );
		break;
	}

	pContext->Unmap( m_pConstantBuffer, 0 );
	pContext->VSSetConstantBuffers( SLOT_POS_SIZE, 1, &m_pConstantBuffer );
}

void CRenderTargetMgr::AssembleRenderTargetView( void )
{
	if( m_vecRenderTarget.empty() ) return;

	size_t iElementCnt = m_vecRenderTarget.size();

	Safe_Delete_Array( m_pArrRenderTargetView );
	if( nullptr == m_pArrRenderTargetView )
		m_pArrRenderTargetView = new ID3D11RenderTargetView*[ iElementCnt ];

	for( size_t i = 0; i < iElementCnt; ++i )
		m_pArrRenderTargetView[i] = *m_vecRenderTarget[ i ]->Get_RenderTargetView();
}

void CRenderTargetMgr::AssembleShaderResourceView( void )
{
	if( m_vecRenderTarget.empty() ) return;

	size_t iElementCnt = m_vecRenderTarget.size();
	iElementCnt--;
	
	Safe_Delete_Array( m_pArrShaderResourceView );
	if( nullptr == m_pArrShaderResourceView )
		m_pArrShaderResourceView = new ID3D11ShaderResourceView*[ iElementCnt ];

	for( size_t i = 0; i < iElementCnt; ++i )
		m_pArrShaderResourceView[ i ] = m_vecRenderTarget[ i + 1 ]->Get_ShaderResourceView();
}

void CRenderTargetMgr::ResizeRenderTarget( const WORD& wSizeX, const WORD& wSizeY )
{
	if( m_vecRenderTarget.empty() ) return;

	ID3D11Device*		 pDevice{ m_pGraphicDev->Get_Device() };
	ID3D11DeviceContext* pContext{ m_pGraphicDev->Get_Context() };
	IDXGISwapChain*		 pSwapChain{ m_pGraphicDev->Get_SwapChain() };

	ClearRenderTargetView( pContext );
	ClearDepthStencilView( pContext );

	ID3D11RenderTargetView* pNullRTV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->OMSetRenderTargets( 6, pNullRTV, nullptr );
	
	for_each( m_vecRenderTarget.begin(), m_vecRenderTarget.end(), ReleaseElement() );
	m_vecRenderTarget.erase( m_vecRenderTarget.begin(), m_vecRenderTarget.end() );
	m_vecRenderTarget.swap( vector<CRenderTarget*>{} );

	Safe_Release( m_pDepthStencilView );
	Safe_Release( m_pDepthStencilBuffer );

	pSwapChain->ResizeBuffers( 2, wSizeX, wSizeY, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );

	for( int i = 0; i < RT_END; ++i )
		Add( pDevice, pSwapChain, wSizeX, wSizeY );

	D3D11_VIEWPORT	tViewport;
	ZeroMemory( &tViewport, sizeof( D3D11_VIEWPORT ) );
	tViewport.Width = ( FLOAT )wSizeX;
	tViewport.Height = ( FLOAT )wSizeY;
	tViewport.MaxDepth = 1.f;

	pContext->RSSetViewports( 1, &tViewport );
	CreateDepthStencilBuffer( pDevice, wSizeX, wSizeY );
	AssembleRenderTargetView();
	AssembleShaderResourceView();
}

void CRenderTargetMgr::ClearRenderTargetView( ID3D11DeviceContext* pContext )
{
	FLOAT fColor[ 4 ]{ 0.f, 0.f, 0.f, 0.f };
	
	for( int i = 0; i < RT_END; ++i )
		pContext->ClearRenderTargetView( m_pArrRenderTargetView[ i ], fColor );
}

void CRenderTargetMgr::SetRenderTargetView( ID3D11DeviceContext* pContext, UINT iSelect, UINT iCnt )
{
	pContext->OMSetRenderTargets( iCnt, &m_pArrRenderTargetView[ iSelect ], m_pDepthStencilView );
}

void CRenderTargetMgr::SetShaderResourceView( ID3D11DeviceContext* pContext, UINT iStartSlot, UINT iSelect, UINT iCnt )
{
	for( UINT i = iSelect; i < iSelect + iCnt; ++i )
	{
		pContext->PSSetShaderResources( iStartSlot++, 1, &m_pArrShaderResourceView[ i - 1 ] );
	}
}

void CRenderTargetMgr::Render( ID3D11DeviceContext* pContext )
{
	for( UINT i = ( UINT )RT_NORMAL; i < ( UINT )RT_END; ++i )
	{
		SetShaderResourceView( pContext, 0, i, 1 );
		SetConstantBuffer( pContext, ( eRT_Type )i );
		pContext->Draw( 6, 0 );
	}
}

void CRenderTargetMgr::ClearDepthStencilView( ID3D11DeviceContext* pContext )
{
	pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0 );
}