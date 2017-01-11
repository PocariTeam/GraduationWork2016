#include "stdafx.h"
#include "StaticMesh.h"

CStaticMesh::CStaticMesh()
	: CMesh()
{
}

HRESULT CStaticMesh::CreateBuffer( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex )
{
	m_dwVtxSize = sizeof( VERTEX );
	m_dwVtxCnt = ( DWORD )pvecCtrlPoint->size();
	m_dwIdxSize = sizeof( DWORD );
	m_dwIdxCnt = ( DWORD )pvecIndex->size();

	VERTEX* pVertex = new VERTEX[m_dwVtxCnt];

	for( DWORD i = 0; i < m_dwVtxCnt; ++i )
	{
		pVertex[i].vPos = ( *pvecCtrlPoint )[i].tVertex.vPos;
		pVertex[i].vNormal = ( *pvecCtrlPoint )[i].tVertex.vNormal;
		pVertex[i].vTangent = ( *pvecCtrlPoint )[i].tVertex.vTangent;
		pVertex[i].vUV = ( *pvecCtrlPoint )[i].tVertex.vUV;
	}

	DWORD*	pIndex = new DWORD[ m_dwIdxCnt ];

	for( DWORD i = 0; i < m_dwIdxCnt; ++i )
		pIndex[ i ] = ( DWORD )( *pvecIndex )[ i ];

	if( FAILED( CMesh::CreateBuffer( pDevice, pVertex, pIndex ) ) )
		return E_FAIL;

	delete[] pVertex;
	delete[] pIndex;

	return S_OK;
}

CMesh* CStaticMesh::Create( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex )
{
	CStaticMesh*		pAnimateMesh = new CStaticMesh;

	if( FAILED( pAnimateMesh->CreateBuffer( pDevice, pvecCtrlPoint, pvecIndex ) ) ) {
		pAnimateMesh->Release();
		pAnimateMesh = nullptr;
	}

	return pAnimateMesh;
}

void CStaticMesh::Release( void )
{
	CMesh::Release();

	delete this;
}