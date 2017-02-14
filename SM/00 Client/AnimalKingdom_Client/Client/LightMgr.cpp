#include "stdafx.h"
#include "LightMgr.h"
#include "Struct.h"
#include "Define.h"
#include "Function.h"

CLightMgr*	CSingleton<CLightMgr>::m_pInstance;

HRESULT CLightMgr::Initialize( ID3D11Device* pDevice, DWORD dwLightingCnt/*= 1 */ )
{
	m_pArrLight = nullptr;
	m_pConstantBufferLight = nullptr;
	m_dwLightingCnt = 0;
	
	// Add( pDevice, LIGHT_POINT, XMFLOAT4( 1.f, 0.f, 0.f, 1.f ), XMFLOAT3( 0.f, 100.f, 0.f ) );
	Add( pDevice, LIGHT_DIRECTIONAL, XMFLOAT4( 1.f, 1.f, 1.f, 1.f ), XMFLOAT3( 0.f, -0.4f, 0.6f ) );

	CreateConstantBuffer( pDevice );

	return S_OK;
}

DWORD CLightMgr::Release( void )
{
	for( DWORD i = 0; i < m_dwLightingCnt; ++i )
		::Safe_Delete( m_pArrLight[ i ] );
	::Safe_Delete_Array( m_pArrLight );
	::Safe_Release( m_pConstantBufferLight );
	m_dwLightingCnt = 0;

	delete this;

	return 0;
}

HRESULT CLightMgr::Add( ID3D11Device* pDevice, LIGHT_TYPE eType, const XMFLOAT4& vDiffuse, const XMFLOAT3& vOption )
{
	LIGHT* pNewLighting = new LIGHT;
	pNewLighting->m_fType = ( float )eType;
	pNewLighting->m_vDiffuse = vDiffuse;

	switch( eType )
	{
	case LIGHT_DIRECTIONAL:
		pNewLighting->m_vDir = vOption;
		break;
	case LIGHT_POINT:
		pNewLighting->m_vPos = vOption;
		break;
	case LIGHT_SPOT:
		// Spot 은 추후 수정 필요
		break;
	default:
		return E_FAIL;
	}

	m_dwLightingCnt++;
	AssembleLightArr( pNewLighting );
	CreateConstantBuffer( pDevice );

	return S_OK;
}

HRESULT CLightMgr::CreateConstantBuffer( ID3D11Device* pDevice )
{
	if( nullptr != m_pConstantBufferLight )
		::Safe_Release( m_pConstantBufferLight );

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( LIGHT ) * m_dwLightingCnt;
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pConstantBufferLight );

	return S_OK;
}

void CLightMgr::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pConstantBufferLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	LIGHT* pConstantBuffer_Light = ( LIGHT* )MappedSubresource.pData;
	for( DWORD i = 0; i < m_dwLightingCnt; ++i )
		pConstantBuffer_Light[ i ] = *m_pArrLight[ i ];

	pContext->Unmap( m_pConstantBufferLight, 0 );
	pContext->PSSetConstantBuffers( SLOT_LIGHT, 1, &m_pConstantBufferLight );
}

void CLightMgr::AssembleLightArr( LIGHT* pNewLighting )
{
	if( nullptr == m_pArrLight )
	{
		m_pArrLight = new LIGHT*;
		m_pArrLight[ 0 ] = pNewLighting;
		return;
	}

	LIGHT**	pArrLight{ new LIGHT*[ m_dwLightingCnt ] };
	memcpy_s( pArrLight, sizeof( LIGHT ) * ( m_dwLightingCnt - 1 ), m_pArrLight, sizeof( LIGHT ) * ( m_dwLightingCnt - 1 ) );
	pArrLight[ m_dwLightingCnt - 1 ] = pNewLighting;
	
	for( DWORD i = 0; i < m_dwLightingCnt - 1; ++i )
		::Safe_Delete( m_pArrLight[ i ] );
	::Safe_Delete_Array( m_pArrLight );

	m_pArrLight = pArrLight;
}

void CLightMgr::Render( ID3D11DeviceContext* pContext )
{
	SetConstantBuffer( pContext );
}
