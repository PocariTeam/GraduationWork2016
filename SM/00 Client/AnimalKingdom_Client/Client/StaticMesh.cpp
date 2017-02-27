#include "stdafx.h"
#include "StaticMesh.h"
#include "Function.h"
#include "Struct.h"
#include <fstream>

void operator >> ( ifstream& pIn, VERTEX_PNTT& tVertex_PNTT );

CStaticMesh::CStaticMesh()
	: CMesh()
{
}


CStaticMesh::CStaticMesh( const CStaticMesh& Instance )
	: CMesh( Instance )
{
}

CStaticMesh::~CStaticMesh()
{
}

CMesh* CStaticMesh::Clone( void )
{
	return new CStaticMesh( *this );
}

HRESULT CStaticMesh::CreateBuffer( ID3D11Device* pDevice, const char* pFilePath )
{
	ifstream	pIn{ pFilePath };

	char		szType{};
	char		szInformation{};

	pIn >> szType;

	if( szType == 's' )
	{
		pIn >> szInformation;
		if( szInformation == 'v' )
		{
			pIn >> m_dwVtxCnt;
			m_dwVtxSize = sizeof( VERTEX_PNTT );
			VERTEX_PNTT* pVertex = new VERTEX_PNTT[ m_dwVtxCnt ];
			for( DWORD i = 0; i < m_dwVtxCnt; ++i )
				pIn >> pVertex[ i ];

			D3D11_BUFFER_DESC			VTXBuffer_Desc;
			D3D11_SUBRESOURCE_DATA		VTXData;

			ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
			VTXBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
			VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
			VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
			VTXData.pSysMem = pVertex;

			if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
				return E_FAIL;
			::Safe_Delete_Array( pVertex );
		}
	}

	else if( szType == 'd' )
	{
		pIn >> szInformation;
		if( szInformation == 'v' )
		{
			pIn >> m_dwVtxCnt;
			m_dwVtxSize = sizeof( XMFLOAT3 );
			XMFLOAT3* pVertex = new XMFLOAT3[ m_dwVtxCnt ];
			for( DWORD i = 0; i < m_dwVtxCnt; ++i )
			{
				pIn >> pVertex[ i ].x;
				pIn >> pVertex[ i ].y;
				pIn >> pVertex[ i ].z;
			}

			D3D11_BUFFER_DESC			VTXBuffer_Desc;
			D3D11_SUBRESOURCE_DATA		VTXData;

			ZeroMemory( &VTXBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
			VTXBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
			VTXBuffer_Desc.ByteWidth = m_dwVtxSize * m_dwVtxCnt;
			VTXBuffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			ZeroMemory( &VTXData, sizeof( D3D11_SUBRESOURCE_DATA ) );
			VTXData.pSysMem = pVertex;

			if( FAILED( pDevice->CreateBuffer( &VTXBuffer_Desc, &VTXData, &m_pVB ) ) )
				return E_FAIL;
			::Safe_Delete_Array( pVertex );
		}
	}

	pIn >> szInformation;
	if( szInformation == 'i' )
	{
		pIn >> m_dwIdxCnt;
		m_dwIdxSize = sizeof( DWORD );
		DWORD*	pIndex = new DWORD[ m_dwIdxCnt ];
		for( DWORD i = 0; i < m_dwIdxCnt; ++i )
			pIn >> pIndex[ i ];

		D3D11_BUFFER_DESC			IdxBuffer_Desc;
		D3D11_SUBRESOURCE_DATA		IdxData;

		ZeroMemory( &IdxBuffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
		IdxBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
		IdxBuffer_Desc.ByteWidth = m_dwIdxSize * m_dwIdxCnt;
		IdxBuffer_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory( &IdxData, sizeof( D3D11_SUBRESOURCE_DATA ) );
		IdxData.pSysMem = pIndex;

		if( FAILED( pDevice->CreateBuffer( &IdxBuffer_Desc, &IdxData, &m_pIB ) ) )
			return E_FAIL;
		::Safe_Delete_Array( pIndex );
	}

	pIn.close();

	return S_OK;
}

CStaticMesh* CStaticMesh::Create( ID3D11Device* pDevice, const char* pFilePath )
{
	CStaticMesh*	pStaticMesh = new CStaticMesh;

	if( FAILED( pStaticMesh->CreateBuffer( pDevice, pFilePath ) ) )
	{
		pStaticMesh->Release();
		pStaticMesh = nullptr;
	}

	return pStaticMesh;
}

DWORD CStaticMesh::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}

void operator >> ( ifstream& pIn, VERTEX_PNTT& tVertex_PNTT )
{
	pIn >> tVertex_PNTT.m_vPos.x;	pIn >> tVertex_PNTT.m_vPos.y;	pIn >> tVertex_PNTT.m_vPos.z;
	pIn >> tVertex_PNTT.m_vNormal.x;	pIn >> tVertex_PNTT.m_vNormal.y;	pIn >> tVertex_PNTT.m_vNormal.z;
	pIn >> tVertex_PNTT.m_vTangent.x; pIn >> tVertex_PNTT.m_vTangent.y; pIn >> tVertex_PNTT.m_vTangent.z;
	pIn >> tVertex_PNTT.m_vUV.x;		 pIn >> tVertex_PNTT.m_vUV.y;
}
