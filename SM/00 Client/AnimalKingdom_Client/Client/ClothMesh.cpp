#include "stdafx.h"
#include "ClothMesh.h"

CClothMesh::CClothMesh()
	: CMesh()
{
}


CClothMesh::CClothMesh( const CClothMesh& Instance )
	: CMesh( Instance )
{
}

CClothMesh::~CClothMesh()
{
}

void CClothMesh::UpdateGeometryInformation( ID3D11DeviceContext* pContext, VERTEX_PNT* pVtx_Array, DWORD* pIndex_Array )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	VERTEX_PNT* pVtxBuffer = ( VERTEX_PNT* )MappedSubresource.pData;
	pVtxBuffer = pVtx_Array;
	pContext->Unmap( m_pVB, 0 );

	pContext->Map( m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	DWORD* pIdxBuffer = ( DWORD* )MappedSubresource.pData;
	pIdxBuffer = pIndex_Array;
	pContext->Unmap( m_pIB, 0 );
}

CMesh* CClothMesh::Clone( void )
{
	return new CClothMesh( *this );
}

HRESULT CClothMesh::CreateBuffer( ID3D11Device* pDevice, VERTEX_PNT* pVtx_Array, DWORD dwVtxCnt, DWORD* pIndex_Array, DWORD dwIdxCnt )
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_dwVtxSize = sizeof( VERTEX_PNT );
	m_dwVtxCnt = dwVtxCnt;

	D3D11_BUFFER_DESC			VTXBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		VTXData;

	ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	VTXBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
	VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VTXBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	VTXData.pSysMem = pVtx_Array;

	if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
		return E_FAIL;

	m_dwIdxSize = sizeof( DWORD );
	m_dwIdxCnt = dwIdxCnt;

	D3D11_BUFFER_DESC			IdxBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		IdxData;

	ZeroMemory( &IdxBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	IdxBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	IdxBuffer_Desc.ByteWidth = m_dwIdxSize * m_dwIdxCnt;
	IdxBuffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IdxBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ZeroMemory( &IdxData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	IdxData.pSysMem = pIndex_Array;

	if( FAILED( pDevice->CreateBuffer( &IdxBuffer_Desc, &IdxData, &m_pIB ) ) )
		return E_FAIL;

	return S_OK;
}

CClothMesh* CClothMesh::Create( ID3D11Device* pDevice, VERTEX_PNT* pVtx_Array, DWORD dwVtxCnt, DWORD* pIndex_Array, DWORD dwIdxCnt )
{
	CClothMesh*	pCloth = new CClothMesh;

	if( FAILED( pCloth->CreateBuffer( pDevice, pVtx_Array, dwVtxCnt, pIndex_Array, dwIdxCnt ) ) )
	{
		pCloth->Release();
		pCloth = nullptr;
	}

	return pCloth;
}

DWORD CClothMesh::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}