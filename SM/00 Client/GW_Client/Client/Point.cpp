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

	/* 정점구조체의 크기 저장 */
	m_dwVtxSize = sizeof( XMFLOAT4 );
	/* 정점의 개수 정의 */
	m_dwVtxCnt = 1;
	/* 정점 배열 생성 */
	XMFLOAT4*	pVertex = new XMFLOAT4;

	*pVertex = XMFLOAT4( 0.f, 0.f, 0.f, 0.f );

	/* Buffer 를 생성하기 위한 데이터 생성 */
	D3D11_BUFFER_DESC			VTXBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		VTXData;

	/* ID3D11Buffer 의 용도, 크기 등 설정 */
	/* VertexBuffer */
	ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	VTXBuffer_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
	VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	/* ID3D11Buffer 가 가리키는 Data 구조체 작성 */
	/* VertexBuffer */
	ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	VTXData.pSysMem = pVertex;

	/* Vertex Buffer 생성 */
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