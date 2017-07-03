#include "stdafx.h"
#include "MeshMgr.h"
#include "Functor.h"
#include "Mesh.h"
#include "ClothMesh.h"
#include "Dot.h"
#include "StaticMesh.h"

CMeshMgr*	CSingleton<CMeshMgr>::m_pInstance;

DWORD CMeshMgr::Release( void )
{
	for_each( m_mapMesh.begin(), m_mapMesh.end(), ReleasePair() );
	delete this;
	return 0;
}

CMesh* CMeshMgr::Clone( const char* pKey )
{
	CMesh*		pMesh = Find( pKey );
	if( nullptr == pMesh ) return nullptr;
	return pMesh->Clone();
}

CMesh* CMeshMgr::Find( const char* pKey )
{
	MESHCONTAINER::iterator		iter_Find = m_mapMesh.find( pKey );
	if( iter_Find == m_mapMesh.end() )  return nullptr;
	return iter_Find->second;
}

HRESULT CMeshMgr::Add( ID3D11Device* pDevice, const char* pKey, CMesh* pMesh )
{
	CMesh*		pFind = Find( pKey );

	if( nullptr != pFind )
	{
		MessageBox( nullptr, "Already Mesh Exists", nullptr, MB_OK );
		return E_FAIL;
	}

	if( nullptr == pMesh ) return E_FAIL;

	m_mapMesh.insert( make_pair( pKey, pMesh ) );
	return S_OK;
}

HRESULT CMeshMgr::Add( ID3D11Device* pDevice, const char* pKey, const char* pFilePath )
{
	CMesh*		pMesh = Find( pKey );

	if( nullptr != pMesh )
	{
		MessageBox( nullptr, "Already Mesh Exists", nullptr, MB_OK );
		return E_FAIL;
	}
	
	pMesh = CStaticMesh::Create( pDevice, pFilePath );
	if( nullptr == pMesh ) return E_FAIL;

	m_mapMesh.insert( make_pair( pKey, pMesh ) );
	return S_OK;
}

HRESULT CMeshMgr::Add( ID3D11Device* pDevice, const char* pKey, LPVOID pVertexArray, DWORD dwVertexCnt, DWORD* pIndexArray, DWORD dwIdxCnt )
{
	CMesh*		pMesh = Find( pKey );

	if( nullptr != pMesh )
	{
		MessageBox( nullptr, "Already Mesh Exists", nullptr, MB_OK );
		return E_FAIL;
	}

	pMesh = CClothMesh::Create( pDevice, ( VERTEX_PNT* )pVertexArray, dwVertexCnt, pIndexArray, dwIdxCnt );
	if( nullptr == pMesh ) return E_FAIL;

	m_mapMesh.insert( make_pair( pKey, pMesh ) );
	return S_OK;
}

HRESULT CMeshMgr::Pop( const char* pKey )
{
	MESHCONTAINER::iterator		iter_Find = m_mapMesh.find( pKey );
	if( iter_Find == m_mapMesh.end() )  return E_FAIL;
	
	iter_Find->second->Release();
	m_mapMesh.erase( pKey );
	return S_OK;
}

HRESULT CMeshMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
{
	HRESULT		hr{};
	ifstream	pIn{ pFilePath };
	int			iAllCnt{};
	char		szKey[ MAX_PATH ]{ "" }, szPath[ MAX_PATH ]{ "" };

	pIn >> iAllCnt;
	for( int i = 0; i < iAllCnt; ++i )
	{
		pIn >> szKey;
		pIn >> szPath;

		Add( pDevice, szKey, szPath );
	}
	pIn.close();

	/*                Default Primitive                   */
	CMesh*	pMesh = CDot::Create( pDevice );
	Add( pDevice, "Mesh_Dot", pMesh );

	return S_OK;
}
