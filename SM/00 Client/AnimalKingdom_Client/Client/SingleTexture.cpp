#include "stdafx.h"
#include "SingleTexture.h"
#include "Function.h"

CSingleTexture::CSingleTexture()
	: CTexture()
	, m_pShaderResourceView( nullptr )
	, m_pSamplerState( nullptr )
{
}


CSingleTexture::CSingleTexture( const CSingleTexture& Instance )
	: CTexture( Instance )
	, m_pShaderResourceView( Instance.m_pShaderResourceView )
	, m_pSamplerState( Instance.m_pSamplerState )
{

}

CSingleTexture::~CSingleTexture()
{
}

CTexture* CSingleTexture::Clone( void )
{
	return new CSingleTexture( *this );
}

void CSingleTexture::Render( ID3D11DeviceContext* pContext )
{
	pContext->PSSetShaderResources( 0, 1, &m_pShaderResourceView );
	pContext->PSSetSamplers( 0, 1, &m_pSamplerState );
}

CSingleTexture* CSingleTexture::Create( ID3D11Device * pDevice, const char * pPath )
{
	CSingleTexture* pTexture = new CSingleTexture;

	if( FAILED( pTexture->Initialize( pDevice, pPath ) ) ) {
		pTexture->Release();
		pTexture = nullptr;
	}

	return pTexture;
}

HRESULT CSingleTexture::Initialize( ID3D11Device* pDevice, const char* pPath )
{
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile( pDevice, pPath, nullptr, nullptr, &m_pShaderResourceView, nullptr );

	if( FAILED( hr ) )	return E_FAIL;

	D3D11_SAMPLER_DESC	 tSampler_Desc;
	ZeroMemory( &tSampler_Desc, sizeof( D3D11_SAMPLER_DESC ) );
	tSampler_Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	tSampler_Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tSampler_Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tSampler_Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	tSampler_Desc.MaxAnisotropy = 1;
	tSampler_Desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	tSampler_Desc.MinLOD = -FLT_MAX;
	tSampler_Desc.MaxLOD = FLT_MAX;
	hr = pDevice->CreateSamplerState( &tSampler_Desc, &m_pSamplerState );

	return hr;
}

DWORD CSingleTexture::Release( void )
{
	DWORD dwRefCnt = CResources::Release();

	if( 0 == dwRefCnt )
	{
		::Safe_Release( m_pShaderResourceView );
		::Safe_Release( m_pSamplerState );
	}

	delete this;

	return dwRefCnt;
}
