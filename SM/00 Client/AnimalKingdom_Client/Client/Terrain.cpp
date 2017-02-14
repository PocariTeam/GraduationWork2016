#include "stdafx.h"
#include "Terrain.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"
#include "Define.h"

CTerrain::CTerrain()
{
}


CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Initialize( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture )
{
	m_pMesh = pMesh;
	m_pTexture = pTexture;

	CreateConstantBuffer( pDevice );

	return S_OK;
}

void CTerrain::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( XMFLOAT4X4 );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pConstantBufferWorld );
}

CTerrain* CTerrain::Create( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture )
{
	CTerrain* pTerrain = new CTerrain;

	if( FAILED( pTerrain->Initialize( pDevice, pMesh, pTexture ) ) )
	{
		pTerrain->Release();
		pTerrain = nullptr;
	}
	return pTerrain;
}

void CTerrain::Render( ID3D11DeviceContext* pContext )
{
	SetConstantBuffer( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

void CTerrain::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	// if( nullptr == m_pActor ) return;

	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pConstantBufferWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	XMFLOAT4X4* pWorld = ( XMFLOAT4X4* )MappedSubresource.pData;
	XMStoreFloat4x4( pWorld, XMMatrixTranspose( XMMatrixScaling( 0.1f, 0.1f, 0.1f ) ) );
	pContext->Unmap( m_pConstantBufferWorld, 0 );
	pContext->VSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBufferWorld );
}

DWORD CTerrain::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pTexture );
	::Safe_Release( m_pMesh );

	delete this;

	return 0;
}
