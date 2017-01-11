#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh()
	:m_pVB( nullptr )
	, m_pIB( nullptr )
	, m_PrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
	, m_dwVtxSize( 0 )
	, m_dwVtxCnt( 0 )
	, m_dwIdxSize( 0 )
	, m_dwIdxCnt( 0 )
{

}

HRESULT CMesh::CreateBuffer( ID3D11Device* pDevice, const LPVOID pVertexData, const LPVOID pIndexData )
{
	/* Buffer 를 생성하기 위한 데이터 생성 */
	D3D11_BUFFER_DESC			VTXBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		VTXData;

	/* ID3D11Buffer 의 용도, 크기 등 설정 */
	/* VertexBuffer */
	ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	VTXBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
	VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	/* ID3D11Buffer 가 가리키는 Data 구조체 작성 */
	/* VertexBuffer */
	ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	VTXData.pSysMem = pVertexData;

	/* Vertex Buffer 생성 */
	if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
		return E_FAIL;

	/**************************************************************************************/

	/* Buffer 를 생성하기 위한 데이터 생성 */
	D3D11_BUFFER_DESC			IDXBuffer_Desc;
	D3D11_SUBRESOURCE_DATA		IDXData;

	/* ID3D11Buffer 의 용도, 크기 등 설정 */
	/* IndexBuffer */
	ZeroMemory( &IDXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	IDXBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	IDXBuffer_Desc.ByteWidth = m_dwIdxSize * m_dwIdxCnt;
	IDXBuffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	/* ID3D11Buffer 가 가리키는 Data 구조체 작성 */
	/* IndexBuffer */
	ZeroMemory( &IDXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	IDXData.pSysMem = pIndexData;

	/* Index Buffer 생성 */
	if( FAILED( pDevice->CreateBuffer( &IDXBuffer_Desc, &IDXData, &m_pIB ) ) )
		return E_FAIL;

	return S_OK;
}

void CMesh::Render( ID3D11DeviceContext* pContext )
{
	UINT dwOffset{ 0 };
	pContext->IASetPrimitiveTopology( m_PrimitiveTopology );

	if( nullptr != m_pVB )
		pContext->IASetVertexBuffers( 0, 1, &m_pVB, ( UINT* )&m_dwVtxSize, &dwOffset );
	if( nullptr != m_pIB )
	{
		pContext->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, 0 );
		pContext->DrawIndexed( m_dwIdxCnt, dwOffset, 0 );
	}

	else
		pContext->Draw( m_dwVtxCnt, dwOffset );
}

void CMesh::Release( void )
{
	if( nullptr != m_pVB ) {
		m_pVB->Release();
		m_pVB = nullptr;
	}

	if( nullptr != m_pIB ) {
		m_pIB->Release();
		m_pIB = nullptr;
	}
}