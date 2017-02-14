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
	// ����
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
	/* ��������ü�� ũ�� ���� */
	m_dwVtxSize = sizeof( XMFLOAT3 );

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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
	VTXData.pSysMem = m_pVertex;

	/* Vertex Buffer ���� */
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