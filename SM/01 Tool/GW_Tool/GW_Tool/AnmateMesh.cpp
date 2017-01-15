#include "stdafx.h"
#include "AnimateMesh.h"

CAnimateMesh::CAnimateMesh()
	: CMesh()
{
}

HRESULT CAnimateMesh::CreateBuffer( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex )
{
	m_dwVtxSize = sizeof( VERTEX_SKINNED );
	m_dwVtxCnt = ( DWORD )pvecCtrlPoint->size();
	m_dwIdxSize = sizeof( DWORD );
	m_dwIdxCnt = ( DWORD )pvecIndex->size();
	VERTEX_SKINNED* pVertex = new VERTEX_SKINNED[m_dwVtxCnt];

	for( DWORD i = 0; i < m_dwVtxCnt; ++i )
	{
		pVertex[i].vPos = ( *pvecCtrlPoint )[i].tVertex.vPos;
		pVertex[i].vNormal = ( *pvecCtrlPoint )[i].tVertex.vNormal;
		pVertex[i].vTangent = ( *pvecCtrlPoint )[i].tVertex.vTangent;
		pVertex[i].vUV = ( *pvecCtrlPoint )[i].tVertex.vUV;
		pVertex[i].dwIndex[0] = ( *pvecCtrlPoint )[i].vecBlendInfo[0].dwIndex;
		pVertex[i].dwIndex[1] = ( *pvecCtrlPoint )[i].vecBlendInfo[1].dwIndex;
		pVertex[i].dwIndex[2] = ( *pvecCtrlPoint )[i].vecBlendInfo[2].dwIndex;
		pVertex[i].dwIndex[3] = ( *pvecCtrlPoint )[i].vecBlendInfo[3].dwIndex;
		pVertex[i].vWeight.x = ( float )( *pvecCtrlPoint )[i].vecBlendInfo[0].dWeight;
		pVertex[i].vWeight.y = ( float )( *pvecCtrlPoint )[i].vecBlendInfo[1].dWeight;
		pVertex[i].vWeight.z = ( float )( *pvecCtrlPoint )[i].vecBlendInfo[2].dWeight;
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

CMesh* CAnimateMesh::Create( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex )
{
	CAnimateMesh*		pAnimateMesh = new CAnimateMesh;

	if( FAILED( pAnimateMesh->CreateBuffer( pDevice, pvecCtrlPoint, pvecIndex ) ) ) {
		pAnimateMesh->Release();
		pAnimateMesh = nullptr;
	}

	return pAnimateMesh;
}

void CAnimateMesh::Release( void )
{
	CMesh::Release();

	delete this;
}