#include "stdafx.h"
#include "Dot.h"
#include "Function.h"

CDot::CDot()
	: CMesh()
{
}

CDot::CDot( const CDot& Instance )
	: CMesh( Instance )
{
}

CDot::~CDot()
{
}

CMesh* CDot::Clone( void )
{
	return new CDot( *this );
}

HRESULT CDot::CreateBuffer( ID3D11Device* pDevice )
{
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_dwVtxSize = sizeof( XMFLOAT3 );
	m_dwVtxCnt = 1;
	XMFLOAT3*	pVertex = new XMFLOAT3;
	*pVertex = XMFLOAT3( 0.f, 0.f, 0.f );

	D3D11_BUFFER_DESC			VTXBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		VTXData;

	ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	VTXBuffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
	VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	VTXData.pSysMem = pVertex;

	if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
		return E_FAIL;
	::Safe_Delete( pVertex );

	return S_OK;
}

CDot* CDot::Create( ID3D11Device* pDevice )
{
	CDot*	pDot = new CDot;

	if( FAILED( pDot->CreateBuffer( pDevice ) ) )
	{
		pDot->Release();
		pDot = nullptr;
	}

	return pDot;
}

DWORD CDot::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}
