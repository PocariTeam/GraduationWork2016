#include "stdafx.h"
#include "Texture.h"

HRESULT CTexture::Initialize( ID3D11Device* pDevice, vector<const TCHAR*>& vecPath )
{
	HRESULT hr{ E_FAIL };
	size_t iTextureCnt = vecPath.size();
	if( 0 == iTextureCnt ) return hr;

	m_mapShaderResourceView.reserve( iTextureCnt );

	for( size_t i = 0; i < iTextureCnt; ++i )
	{
		pair<const TCHAR*, ID3D11ShaderResourceView*> pairShaderResourceView;
		pairShaderResourceView.first = vecPath[i];
		hr = D3DX11CreateShaderResourceViewFromFile( pDevice, vecPath[i], nullptr, nullptr, &pairShaderResourceView.second, nullptr );

		if( FAILED( hr ) ) { 
			AfxMessageBox( L"Load Texture Failed" );
			return hr;
		}

		m_mapShaderResourceView.push_back( pairShaderResourceView );
	}

	m_iTextureCnt = iTextureCnt;
	
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

	CreateConstantBuffer( pDevice );

	return hr;
}

void CTexture::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( XMFLOAT4 );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCBTexture );
}

void CTexture::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pCBTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	XMFLOAT4* pTextureInfo = ( XMFLOAT4 * )MappedSubresource.pData;
	pTextureInfo->x = ( float )m_iTextureCnt;

	pContext->Unmap( m_pCBTexture, 0 );
	pContext->PSSetConstantBuffers( SLOT_TEXTURE, 1, &m_pCBTexture );
}

CTexture* CTexture::Create( ID3D11Device* pDevice, vector<const TCHAR*>& vecPath )
{
	CTexture* pTexture = new CTexture;

	if( FAILED( pTexture->Initialize( pDevice, vecPath ) ) ) {
		pTexture->Release();
		pTexture = nullptr;
	}

	return pTexture;
}

void CTexture::Render( ID3D11DeviceContext * pContext )
{
	UINT iTextureCnt = ( UINT )m_mapShaderResourceView.size();
	SetConstantBuffer( pContext );
	for( UINT i = 0; i < iTextureCnt; ++i )
	{
		pContext->PSSetShaderResources( i, 1, &m_mapShaderResourceView[i].second );
		pContext->PSSetSamplers( i, 1, &m_pSamplerState );
	}
}

DWORD CTexture::Release( void )
{
	if( nullptr != m_pSamplerState )
	{
		m_pSamplerState->Release();
		m_pSamplerState = nullptr;
	}

	if( m_pCBTexture )
	{
		m_pCBTexture->Release();
		m_pCBTexture = nullptr;
	}

	if( !m_mapShaderResourceView.empty() )
	{
		size_t iShaderResourceViewCnt = m_mapShaderResourceView.size();

		for( size_t i = 0; i < iShaderResourceViewCnt; ++i )
		{
			if( nullptr != m_mapShaderResourceView[i].second )
			{
				delete [] m_mapShaderResourceView[i].first;
				m_mapShaderResourceView[i].second->Release();
				m_mapShaderResourceView[i].second = nullptr;
			}
		}
		m_mapShaderResourceView.clear();
		m_mapShaderResourceView.erase( m_mapShaderResourceView.begin(), m_mapShaderResourceView.end() );
		m_mapShaderResourceView.swap( MAP_TEXTURE() );
	}

	delete this;

	return 0;
}
