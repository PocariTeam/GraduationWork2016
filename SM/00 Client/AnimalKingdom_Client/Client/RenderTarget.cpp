#include "stdafx.h"
#include "RenderTarget.h"
#include "Function.h"

CRenderTarget::CRenderTarget()
	: m_pRenderTargetView( nullptr )
	, m_pTexture( nullptr )
	, m_pShaderResourceView( nullptr )
{
}

HRESULT CRenderTarget::Initialize( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain )
{
	return CreateRenderTarget( pDevice, pSwapChain );
}

HRESULT CRenderTarget::Initialize( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY )
{
	return CreateRenderTarget( pDevice, wSizeX, wSizeY );
}

HRESULT CRenderTarget::CreateRenderTarget( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain )
{
	ID3D11Texture2D*	pBackBuffer = NULL;

	if( FAILED( pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer ) ) )
	{
		pBackBuffer->Release();
		return E_FAIL;
	}

	if( FAILED( pDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pRenderTargetView ) ) )
		return E_FAIL;

	pBackBuffer->Release();

	return S_OK;
}

HRESULT CRenderTarget::CreateRenderTarget( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY )
{
	D3D11_TEXTURE2D_DESC tTexture2D_Desc;
	ZeroMemory( &tTexture2D_Desc, sizeof( D3D11_TEXTURE2D_DESC ) );

	tTexture2D_Desc.Width = wSizeX;
	tTexture2D_Desc.Height = wSizeY;
	tTexture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	tTexture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	/*UINT dw4xMsaaQuality = 0;
	if( FAILED( pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R32G32B32A32_FLOAT, 4, &dw4xMsaaQuality ) ) )
	{*/
	tTexture2D_Desc.SampleDesc.Count = 1;
	tTexture2D_Desc.SampleDesc.Quality = 0;
	/*}

	else
	{
		tTexture2D_Desc.SampleDesc.Count = 4;
		tTexture2D_Desc.SampleDesc.Quality = dw4xMsaaQuality - 1;
	}*/

	tTexture2D_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	tTexture2D_Desc.ArraySize = 1;
	tTexture2D_Desc.MipLevels = 1;

	D3D11_RENDER_TARGET_VIEW_DESC	tRTV_Desc;
	ZeroMemory( &tRTV_Desc, sizeof( D3D11_RENDER_TARGET_VIEW_DESC ) );
	tRTV_Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D/*MS*/;
	tRTV_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	D3D11_SHADER_RESOURCE_VIEW_DESC	tSRV_Desc;
	ZeroMemory( &tSRV_Desc, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );
	tSRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D/*MS*/;
	tSRV_Desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	tSRV_Desc.Texture2D.MostDetailedMip = 0;
	tSRV_Desc.Texture2D.MipLevels = 1;
		
	if( FAILED( pDevice->CreateTexture2D( &tTexture2D_Desc, nullptr, &m_pTexture ) ) )
		return E_FAIL;

	if( FAILED( pDevice->CreateRenderTargetView( m_pTexture, &tRTV_Desc, &m_pRenderTargetView ) ) )
		return E_FAIL;

	if( FAILED( pDevice->CreateShaderResourceView( m_pTexture, &tSRV_Desc, &m_pShaderResourceView ) ) )
		return E_FAIL;

	return S_OK;
}

DWORD CRenderTarget::Release( void )
{
	Safe_Release( m_pRenderTargetView );
	Safe_Release( m_pTexture );
	Safe_Release( m_pShaderResourceView );

	delete this;

	return 0;
}

CRenderTarget* CRenderTarget::Create( ID3D11Device* pDevice, IDXGISwapChain* pSwapChain )
{
	CRenderTarget* pRenderTarget = new CRenderTarget;

	if( FAILED( pRenderTarget->Initialize( pDevice, pSwapChain ) ) )
	{
		pRenderTarget->Release();
		pRenderTarget = nullptr;
	}
	return pRenderTarget;
}

CRenderTarget* CRenderTarget::Create( ID3D11Device* pDevice, const WORD& wSizeX, const WORD& wSizeY )
{
	CRenderTarget* pRenderTarget = new CRenderTarget;

	if( FAILED( pRenderTarget->Initialize( pDevice, wSizeX, wSizeY ) ) )
	{
		pRenderTarget->Release();
		pRenderTarget = nullptr;
	}
	return pRenderTarget;
}
