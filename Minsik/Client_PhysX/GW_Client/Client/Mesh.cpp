#include "stdafx.h"
#include "Mesh.h"
#include "Function.h"

CMesh::CMesh()
	: CBase()
	, m_dwVtxSize( 0 )
	, m_dwVtxCnt( 0 )
	, m_dwIdxSize( 0 )
	, m_dwIdxCnt( 0 )
	, m_pVB( nullptr )
	, m_pIB( nullptr )
	, m_PrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
	, m_iBufferCnt( 0 )
{
}

CMesh::CMesh( const CMesh & Instance )
	: CBase( Instance )
	, m_dwVtxSize( Instance.m_dwVtxSize )
	, m_dwVtxCnt( Instance.m_dwVtxCnt )
	, m_dwIdxSize( Instance.m_dwIdxSize )
	, m_dwIdxCnt( Instance.m_dwIdxCnt )
	, m_pVB( Instance.m_pVB )
	, m_pIB( Instance.m_pIB )
	, m_PrimitiveTopology( Instance.m_PrimitiveTopology )
	, m_iBufferCnt( Instance.m_iBufferCnt )
{
}


CMesh::~CMesh()
{
}

void CMesh::Render( ID3D11DeviceContext * pContext, DWORD dwInstanceCnt/* = 1*/ )
{
	UINT dwOffset{ 0 };
	pContext->IASetPrimitiveTopology( m_PrimitiveTopology );

	if( nullptr != m_pVB )
		pContext->IASetVertexBuffers( 0, 1, &m_pVB, ( UINT* )&m_dwVtxSize, &dwOffset );

	if( nullptr != m_pIB )
		pContext->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, 0 );

	if( dwInstanceCnt > 1 && nullptr != m_pIB )
		pContext->DrawIndexedInstanced( m_dwIdxCnt, dwInstanceCnt, dwOffset, 0, 0 );

	else if( dwInstanceCnt > 1 && nullptr == m_pIB )
		pContext->DrawInstanced( m_dwVtxCnt, dwInstanceCnt, 0, 0 );

	else if( dwInstanceCnt <=1 && nullptr != m_pIB )
		pContext->DrawIndexed( m_dwIdxCnt, dwOffset, 0 );

	else
		pContext->Draw( m_dwVtxCnt, dwOffset );
}

DWORD CMesh::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	if( 0 == dwRefCnt ) {
		::Safe_Release( m_pVB );
		::Safe_Release( m_pIB );
	}

	return dwRefCnt;
}