#include "stdafx.h"
#include "AnimateMesh.h"
#include "Function.h"
#include "Struct.h"
#include <fstream>

void operator >> ( ifstream& pIn, VERTEX_PNTTB& tVertex_PNTTB );

CAnimateMesh::CAnimateMesh()
	: CMesh()
{
}


CAnimateMesh::CAnimateMesh( const CAnimateMesh& Instance )
	: CMesh( Instance )
{
}

CAnimateMesh::~CAnimateMesh()
{
}

CMesh* CAnimateMesh::Clone( void )
{
	return new CAnimateMesh( *this );
}

HRESULT CAnimateMesh::CreateBuffer( ID3D11Device* pDevice, const char* pFilePath )
{
	ifstream	pIn{ pFilePath };

	char		szType{};
	char		szInformation{};

	pIn >> szType;

	if( 'a' == szType )
		while( !pIn.eof() )
		{
			pIn >> szInformation;
			switch( szInformation )
			{
			case 'v':
			{
				pIn >> m_dwVtxCnt;
				m_dwVtxSize = sizeof( VERTEX_PNTTB );
				VERTEX_PNTTB* pVertex = new VERTEX_PNTTB[ m_dwVtxCnt ];
				for( DWORD i = 0; i < m_dwVtxCnt; ++i )
				{
					pIn >> pVertex[ i ];
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
				szInformation = 0;
			}
				break;
			case 'i':
			{
				pIn >> m_dwIdxCnt;
				m_dwIdxSize = sizeof( DWORD );
				DWORD*	pIndex = new DWORD[ m_dwIdxCnt ];
				for( DWORD i = 0; i < m_dwIdxCnt; ++i )
				{
					pIn >> pIndex[ i ];
				}

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
				szInformation = 0;
			}
			break;
			}
		}

	pIn.close();

	return S_OK;
}

CAnimateMesh* CAnimateMesh::Create( ID3D11Device* pDevice, const char* pFilePath )
{
	CAnimateMesh*	pAnimateMesh = new CAnimateMesh;

	if( FAILED( pAnimateMesh->CreateBuffer( pDevice, pFilePath ) ) )
	{
		pAnimateMesh->Release();
		pAnimateMesh = nullptr;
	}

	return pAnimateMesh;
}

DWORD CAnimateMesh::Release( void )
{
	DWORD dwRefCnt = CMesh::Release();
	delete this;

	return dwRefCnt;
}

void operator >> ( ifstream& pIn, VERTEX_PNTTB& tVertex_PNTTB )
{
	pIn >> tVertex_PNTTB.m_vPos.x;	pIn >> tVertex_PNTTB.m_vPos.y;	pIn >> tVertex_PNTTB.m_vPos.z;
	pIn >> tVertex_PNTTB.m_vNormal.x;	pIn >> tVertex_PNTTB.m_vNormal.y;	pIn >> tVertex_PNTTB.m_vNormal.z;
	pIn >> tVertex_PNTTB.m_vTangent.x; pIn >> tVertex_PNTTB.m_vTangent.y; pIn >> tVertex_PNTTB.m_vTangent.z;
	pIn >> tVertex_PNTTB.m_vUV.x;		 pIn >> tVertex_PNTTB.m_vUV.y;
	pIn >> tVertex_PNTTB.m_vIndex[ 0 ]; pIn >> tVertex_PNTTB.m_vWeight.x;
	pIn >> tVertex_PNTTB.m_vIndex[ 1 ]; pIn >> tVertex_PNTTB.m_vWeight.y;
	pIn >> tVertex_PNTTB.m_vIndex[ 2 ]; pIn >> tVertex_PNTTB.m_vWeight.z;
	pIn >> tVertex_PNTTB.m_vIndex[ 3 ]; pIn >> tVertex_PNTTB.m_vWeight.w;
}

