#include "stdafx.h"
#include "ActorShape.h"
#include "GraphicDev.h"

CActorShape::CActorShape( const CActorShape& Instance )
	: CMesh( Instance )
{
}

CActorShape::~CActorShape( void )
{

}

CMesh* CActorShape::Clone()
{
	return new CActorShape( *this );
}

HRESULT CActorShape::LoadMeshFromJsm( ID3D11Device* pDevice, const TCHAR* pJsmPath )
{
	ifstream	Input( pJsmPath, ios::binary );
	char pToken[MAX_PATH]{ 0 };

	Input >> pToken;
	// 정점
	if( !strcmp( pToken, "b" ) ) {
		Input >> m_dwVtxCnt;

		m_pVertex = new XMFLOAT3[m_dwVtxCnt];

		for( DWORD i = 0; i < m_dwVtxCnt; ++i )
			Input >> m_pVertex[i].x >> m_pVertex[i].y >> m_pVertex[i].z;
	}

	Input.close();

	return S_OK;
}

HRESULT CActorShape::CreateBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pJsmPath )
{
	LoadMeshFromJsm( pDevice, pJsmPath );
	/* 정점구조체의 크기 저장 */
	m_dwVtxSize = sizeof( XMFLOAT3 );

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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
	VTXData.pSysMem = m_pVertex;

	/* Vertex Buffer 생성 */
	if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
		return E_FAIL;

	delete[] m_pVertex;
	m_pVertex = nullptr;

	return S_OK;
}

CActorShape* CActorShape::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pJsmPath )
{
	CActorShape*		pActorShape = new CActorShape;

	if( FAILED( pActorShape->CreateBuffer( pDevice, pContext, pJsmPath ) ) ) {
		pActorShape->Release();
		pActorShape = nullptr;
	}

	return pActorShape;
}

DWORD CActorShape::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();

	delete this;

	return dwRefCnt;
}