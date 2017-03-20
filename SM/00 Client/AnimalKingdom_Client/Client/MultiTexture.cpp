#include "stdafx.h"
#include "MultiTexture.h"
#include "Function.h"

CMultiTexture::CMultiTexture()
	: CTexture()
	, m_dpShaderResourceView( nullptr )
	, m_dpSamplerState( nullptr )
	, m_dwTextureCnt( 0 )
{
}


CMultiTexture::CMultiTexture( const CMultiTexture& Instance )
	: CTexture( Instance )
	, m_dpShaderResourceView( Instance.m_dpShaderResourceView )
	, m_dpSamplerState( Instance.m_dpSamplerState )
	, m_dwTextureCnt( Instance.m_dwTextureCnt )
{
}

CMultiTexture::~CMultiTexture()
{
}

CTexture* CMultiTexture::Clone( void )
{
	return new CMultiTexture( *this );
}

void CMultiTexture::Render( ID3D11DeviceContext* pContext )
{
	for( UINT i = 0; i < ( UINT ) m_dwTextureCnt; ++i )
	{
		pContext->DSSetShaderResources( i, 1, &m_dpShaderResourceView[ i ] );
		pContext->DSSetSamplers( i, 1, &m_dpSamplerState[ i ] );

		pContext->GSSetShaderResources( i, 1, &m_dpShaderResourceView[ i ] );
		pContext->GSSetSamplers( i, 1, &m_dpSamplerState[ i ] );

		pContext->PSSetShaderResources( i, 1, &m_dpShaderResourceView[ i ] );
		pContext->PSSetSamplers( i, 1, &m_dpSamplerState[ i ] );
	}
}

void CMultiTexture::Render( ID3D11DeviceContext* pContext, int iIndex )
{
	pContext->DSSetShaderResources( 0, 1, &m_dpShaderResourceView[ iIndex ] );
	pContext->DSSetSamplers( 0, 1, &m_dpSamplerState[ iIndex ] );

	pContext->GSSetShaderResources( 0, 1, &m_dpShaderResourceView[ iIndex ] );
	pContext->GSSetSamplers( 0, 1, &m_dpSamplerState[ iIndex ] );

	pContext->PSSetShaderResources( 0, 1, &m_dpShaderResourceView[ iIndex ] );
	pContext->PSSetSamplers( 0, 1, &m_dpSamplerState[ iIndex ] );
}

DWORD CMultiTexture::Release()
{
	DWORD dwRefCnt = CTexture::Release();

	if( 0 == dwRefCnt )
	{
		for( DWORD i = 0; i < m_dwTextureCnt; ++i )
		{
			::Safe_Release( m_dpShaderResourceView[ i ] );
			::Safe_Release( m_dpSamplerState[ i ] );
		}

		::Safe_Delete_Array( m_dpShaderResourceView );
		::Safe_Delete_Array( m_dpSamplerState );
	}

	delete this;

	return dwRefCnt;
}

CMultiTexture* CMultiTexture::Create( ID3D11Device* pDevice, const char* pPath, DWORD dwTextureCnt )
{
	CMultiTexture* pTexture = new CMultiTexture;

	if( FAILED( pTexture->Initialize( pDevice, pPath, dwTextureCnt ) ) ) {
		pTexture->Release();
		pTexture = nullptr;
	}

	return pTexture;
}

HRESULT CMultiTexture::Initialize( ID3D11Device* pDevice, const char* pPath, DWORD dwTextureCnt )
{
	HRESULT hr{};

	m_dwTextureCnt = dwTextureCnt;

	char* pTemp = new char[ MAX_PATH ];
	m_dpShaderResourceView = new ID3D11ShaderResourceView*[ m_dwTextureCnt ];
	m_dpSamplerState = new ID3D11SamplerState*[ m_dwTextureCnt ];

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

	for( DWORD i = 0; i < m_dwTextureCnt; ++i ) 
	{
		wsprintf( pTemp, pPath, i );

		hr = D3DX11CreateShaderResourceViewFromFile( pDevice, pTemp, nullptr, nullptr, &m_dpShaderResourceView[i], nullptr );
		if( FAILED( hr ) )	return E_FAIL;

		hr = pDevice->CreateSamplerState( &tSampler_Desc, &m_dpSamplerState[i] );
		if( FAILED( hr ) )	return E_FAIL;
	}

	delete[] pTemp;

	return hr;
}
