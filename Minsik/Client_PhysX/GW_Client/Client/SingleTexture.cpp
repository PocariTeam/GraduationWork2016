#include "stdafx.h"
#include "SingleTexture.h"
#include "Function.h"

CSingleTexture::CSingleTexture()
	: CTexture()
	, m_pShaderResourceView( nullptr )
	, m_pSamplerState( nullptr )
{
}

CSingleTexture::CSingleTexture( const CSingleTexture & Instance )
	: CTexture( Instance )
	, m_pShaderResourceView( Instance.m_pShaderResourceView )
	, m_pSamplerState( Instance.m_pSamplerState )
{
}


CSingleTexture::~CSingleTexture()
{
}

HRESULT CSingleTexture::Initialize( ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const TCHAR * pPath )
{
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile( pDevice, pPath, nullptr, nullptr, &m_pShaderResourceView, nullptr );

	D3D11_SAMPLER_DESC tSampler_Desc;
	ZeroMemory( &tSampler_Desc, sizeof( D3D11_SAMPLER_DESC ) );
	tSampler_Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSampler_Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSampler_Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSampler_Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	tSampler_Desc.MaxAnisotropy = 1;
	tSampler_Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	tSampler_Desc.MinLOD = -FLT_MAX;
	tSampler_Desc.MaxLOD = FLT_MAX;
	pDevice->CreateSamplerState( &tSampler_Desc, &m_pSamplerState );

	return hr;
}

CSingleTexture * CSingleTexture::Create( ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const TCHAR * pPath )
{
	CSingleTexture* pTexture = new CSingleTexture;

	if( FAILED( pTexture->Initialize( pDevice, pContext, pPath ) ) ) {
		pTexture->Release();
		pTexture = nullptr;
	}

	return pTexture;
}

void CSingleTexture::Render( ID3D11DeviceContext * pContext )
{
	pContext->PSSetShaderResources( 0, 1, &m_pShaderResourceView );
	pContext->PSSetSamplers( 0, 1, &m_pSamplerState );
}

DWORD CSingleTexture::Release( void )
{
	DWORD dwRefCnt = CTexture::Release();

	if( 0 == dwRefCnt ) {
		::Safe_Release( m_pShaderResourceView );
		::Safe_Release( m_pSamplerState );
	}
	delete this;

	return dwRefCnt;
}
