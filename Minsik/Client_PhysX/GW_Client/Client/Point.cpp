#include "stdafx.h"
#include "Point.h"
#include "GraphicDev.h"

CPoint::CPoint( const CPoint& Instance )
	: CMesh( Instance )
{
}

CPoint::~CPoint( void )
{
}

CMesh* CPoint::Clone()
{
	return new CPoint( *this );
}

HRESULT CPoint::CreateBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	/* ��������ü�� ũ�� ���� */
	m_dwVtxSize = sizeof( XMFLOAT4 );
	/* ������ ���� ���� */
	m_dwVtxCnt = 1;
	/* ���� �迭 ���� */
	XMFLOAT4*	pVertex = new XMFLOAT4;

	*pVertex = XMFLOAT4( 0.f, 0.f, 0.f, 0.f );

	/* Buffer �� �����ϱ� ���� ������ ���� */
	D3D11_BUFFER_DESC			VTXBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		VTXData;

	/* ID3D11Buffer �� �뵵, ũ�� �� ���� */
	/* VertexBuffer */
	ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	VTXBuffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
	VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	/* ID3D11Buffer �� ����Ű�� Data ����ü �ۼ� */
	/* VertexBuffer */
	ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	VTXData.pSysMem = pVertex;

	/* Vertex Buffer ���� */
	if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
		return E_FAIL;

	delete pVertex;

	return S_OK;
}


CPoint* CPoint::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	CPoint*		pRectangle = new CPoint;

	if( FAILED( pRectangle->CreateBuffer( pDevice, pContext ) ) ) {
		pRectangle->Release();
		pRectangle = nullptr;
	}

	return pRectangle;
}

DWORD CPoint::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}