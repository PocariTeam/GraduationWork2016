#include "stdafx.h"
#include "Wallpaper.h"
#include "Function.h"
#include "Mesh.h"
#include "Define.h"
#include "Value.h"

CWallpaper::CWallpaper()
	: m_pMesh( nullptr )
{
}


CWallpaper::~CWallpaper()
{
}

HRESULT CWallpaper::Initialize( ID3D11Device* pDevice, CMesh* pMesh )
{
	m_pMesh = pMesh;

	CreateConstantBuffer( pDevice );

	return S_OK;
}

void CWallpaper::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( XMFLOAT4 );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, nullptr, &m_pConstantBufferWorld );
}

void CWallpaper::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pConstantBufferWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	XMFLOAT4* pWinSize = ( XMFLOAT4* )MappedSubresource.pData;
	*pWinSize = XMFLOAT4( ( float )g_wWinsizeX, ( float )g_wWinsizeY, 0.f, 0.f );

	pContext->Unmap( m_pConstantBufferWorld, 0 );
	pContext->VSSetConstantBuffers( SLOT_WINSIZE, 1, &m_pConstantBufferWorld );
	pContext->PSSetConstantBuffers( SLOT_WINSIZE, 1, &m_pConstantBufferWorld );
}

CWallpaper* CWallpaper::Create( ID3D11Device* pDevice, CMesh* pMesh )
{
	CWallpaper* pWallpater = new CWallpaper;

	if( FAILED( pWallpater->Initialize( pDevice, pMesh ) ) )
	{
		pWallpater->Release();
		pWallpater = nullptr;
	}
	return pWallpater;
}

void CWallpaper::Render( ID3D11DeviceContext* pContext )
{
	SetConstantBuffer( pContext );
	m_pMesh->Render( pContext );
}

DWORD CWallpaper::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pMesh );

	delete this;

	return 0;
}
