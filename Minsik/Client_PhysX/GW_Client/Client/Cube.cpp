#include "stdafx.h"
#include "Cube.h"
#include "GraphicDev.h"

CCube::CCube( const CCube& Instance )
	: CMesh( Instance )
{
}

CCube::~CCube( void )
{

}

CMesh* CCube::Clone()
{
	return new CCube( *this );
}

HRESULT CCube::CreateBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	/* 정점구조체의 크기 저장 */
	m_dwVtxSize = sizeof( VERTEX_TANGENT );
	/* 정점의 개수 정의 */
	m_dwVtxCnt = 24;

	/* 정점 배열 생성 */
	VERTEX_TANGENT*			pVertex = new VERTEX_TANGENT[m_dwVtxCnt];
	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 앞면
	pVertex[0].vPos = XMFLOAT3( -0.5f, 0.5f, -0.5f );	pVertex[0].vUV = XMFLOAT2( 0.f, 0.f );
	pVertex[1].vPos = XMFLOAT3( 0.5f, 0.5f, -0.5f );	pVertex[1].vUV = XMFLOAT2( 1.f, 0.f );
	pVertex[2].vPos = XMFLOAT3( -0.5f, -0.5f, -0.5f );	pVertex[2].vUV = XMFLOAT2( 0.f, 1.f );
	pVertex[3].vPos = XMFLOAT3( 0.5f, -0.5f, -0.5f );	pVertex[3].vUV = XMFLOAT2( 1.f, 1.f );
	// 왼면
	pVertex[4].vPos = XMFLOAT3( -0.5f, 0.5f, 0.5f );	pVertex[4].vUV = XMFLOAT2( 0.f, 0.f );
	pVertex[5].vPos = pVertex[0].vPos;	pVertex[5].vUV = XMFLOAT2( 1.f, 0.f );
	pVertex[6].vPos = XMFLOAT3( -0.5f, -0.5f, 0.5f );	pVertex[6].vUV = XMFLOAT2( 0.f, 1.f );
	pVertex[7].vPos = pVertex[2].vPos;	pVertex[7].vUV = XMFLOAT2( 1.f, 1.f );
	// 윗면
	pVertex[8].vPos = pVertex[4].vPos;	pVertex[8].vUV = XMFLOAT2( 0.f, 0.f );
	pVertex[9].vPos = XMFLOAT3( 0.5f, 0.5f, 0.5f );	pVertex[9].vUV = XMFLOAT2( 1.f, 0.f );
	pVertex[10].vPos = pVertex[0].vPos;	pVertex[10].vUV = XMFLOAT2( 0.f, 1.f );
	pVertex[11].vPos = pVertex[1].vPos;	pVertex[11].vUV = XMFLOAT2( 1.f, 1.f );
	// 뒷면
	pVertex[12].vPos = pVertex[9].vPos;	pVertex[12].vUV = XMFLOAT2( 0.f, 0.f );
	pVertex[13].vPos = pVertex[4].vPos;	pVertex[13].vUV = XMFLOAT2( 1.f, 0.f );
	pVertex[14].vPos = XMFLOAT3( 0.5f, -0.5f, 0.5f );	pVertex[14].vUV = XMFLOAT2( 0.f, 1.f );
	pVertex[15].vPos = pVertex[6].vPos;	pVertex[15].vUV = XMFLOAT2( 1.f, 1.f );
	// 오른면
	pVertex[16].vPos = pVertex[1].vPos;	pVertex[16].vUV = XMFLOAT2( 0.f, 0.f );
	pVertex[17].vPos = pVertex[9].vPos;	pVertex[17].vUV = XMFLOAT2( 1.f, 0.f );
	pVertex[18].vPos = pVertex[3].vPos;	pVertex[18].vUV = XMFLOAT2( 0.f, 1.f );
	pVertex[19].vPos = pVertex[14].vPos;	pVertex[19].vUV = XMFLOAT2( 1.f, 1.f );
	// 아랫면
	pVertex[20].vPos = pVertex[14].vPos;	pVertex[20].vUV = XMFLOAT2( 0.f, 0.f );
	pVertex[21].vPos = pVertex[6].vPos;	pVertex[21].vUV = XMFLOAT2( 1.f, 0.f );
	pVertex[22].vPos = pVertex[3].vPos;	pVertex[22].vUV = XMFLOAT2( 0.f, 1.f );
	pVertex[23].vPos = pVertex[2].vPos;	pVertex[23].vUV = XMFLOAT2( 1.f, 1.f );

	Calculate_Normal( pVertex[0], pVertex[1], pVertex[2], pVertex[3] );
	Calculate_Normal( pVertex[4], pVertex[5], pVertex[6], pVertex[7] );
	Calculate_Normal( pVertex[8], pVertex[9], pVertex[10], pVertex[11] );
	Calculate_Normal( pVertex[12], pVertex[13], pVertex[14], pVertex[15] );
	Calculate_Normal( pVertex[16], pVertex[17], pVertex[18], pVertex[19] );
	Calculate_Normal( pVertex[20], pVertex[21], pVertex[22], pVertex[23] );

	Calculate_Tangent( pVertex[0], pVertex[1], pVertex[2], pVertex[3] );
	Calculate_Tangent( pVertex[4], pVertex[5], pVertex[6], pVertex[7] );
	Calculate_Tangent( pVertex[8], pVertex[9], pVertex[10], pVertex[11] );
	Calculate_Tangent( pVertex[12], pVertex[13], pVertex[14], pVertex[15] );
	Calculate_Tangent( pVertex[16], pVertex[17], pVertex[18], pVertex[19] );
	Calculate_Tangent( pVertex[20], pVertex[21], pVertex[22], pVertex[23] );

	XMVECTOR vNormal;
	for( DWORD i = 0; i < m_dwVtxCnt; ++i )
	{
		vNormal = XMLoadFloat3( &pVertex[i].vNormal );
		vNormal = XMVector3Normalize( vNormal );
		XMStoreFloat3( &pVertex[i].vNormal, vNormal );
	}

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

	delete[] pVertex;

	return S_OK;
}

void CCube::Calculate_Normal( VERTEX_TANGENT& tVertex_1, VERTEX_TANGENT& tVertex_2, VERTEX_TANGENT& tVertex_3, VERTEX_TANGENT& tVertex_4 )
{
	XMVECTOR vDest, vSour, vTemp, vNormal;

	vDest = XMLoadFloat3( &tVertex_1.vPos );
	vSour = XMLoadFloat3( &tVertex_2.vPos );
	vTemp = XMLoadFloat3( &tVertex_4.vPos );
	vNormal = XMVector3Cross( XMVectorSubtract( vSour, vDest ), XMVectorSubtract( vTemp, vDest ) );
	XMStoreFloat3( &tVertex_1.vNormal, XMVectorAdd( XMLoadFloat3( &tVertex_1.vNormal ), vNormal ) );
	XMStoreFloat3( &tVertex_2.vNormal, XMVectorAdd( XMLoadFloat3( &tVertex_2.vNormal ), vNormal ) );
	XMStoreFloat3( &tVertex_3.vNormal, XMVectorAdd( XMLoadFloat3( &tVertex_3.vNormal ), vNormal ) );
	XMStoreFloat3( &tVertex_4.vNormal, XMVectorAdd( XMLoadFloat3( &tVertex_4.vNormal ), vNormal ) );
}

void CCube::Calculate_Tangent( VERTEX_TANGENT& tVertex_1, VERTEX_TANGENT& tVertex_2, VERTEX_TANGENT& tVertex_3, VERTEX_TANGENT& tVertex_4 )
{
	XMVECTOR vDest, vSour, vTangent;

	vDest = XMLoadFloat3( &tVertex_1.vPos );
	vSour = XMLoadFloat3( &tVertex_2.vPos );
	vTangent = XMVectorSubtract( vSour, vDest );
	XMStoreFloat3( &tVertex_1.vTangent, vTangent );
	XMStoreFloat3( &tVertex_2.vTangent, vTangent );
	XMStoreFloat3( &tVertex_3.vTangent, vTangent );
	XMStoreFloat3( &tVertex_4.vTangent, vTangent );
}

CCube* CCube::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	CCube*		pCube = new CCube;

	if( FAILED( pCube->CreateBuffer( pDevice, pContext ) ) ) {
		pCube->Release();
		pCube = nullptr;
	}

	return pCube;
}

DWORD CCube::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}