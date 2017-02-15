#include "stdafx.h"
#include "AnimateMeshMgr.h"
#include "Functor.h"
#include "Mesh.h"
#include "AnimateMesh.h"

CAnimateMeshMgr*	CSingleton<CAnimateMeshMgr>::m_pInstance;

DWORD CAnimateMeshMgr::Release( void )
{
	for_each( m_mapMesh.begin(), m_mapMesh.end(), ReleasePair() );
	delete this;
	return 0;
}

CMesh* CAnimateMeshMgr::Clone( const char* pKey )
{
	CMesh*		pMesh = Find( pKey );
	if( nullptr == pMesh ) return nullptr;
	return pMesh->Clone();
}

CMesh* CAnimateMeshMgr::Find( const char* pKey )
{
	MESHCONTAINER::iterator		iter_Find = m_mapMesh.find( pKey );
	if( iter_Find == m_mapMesh.end() )  return nullptr;
	return iter_Find->second;
}

HRESULT CAnimateMeshMgr::Add( ID3D11Device* pDevice, const char* pKey, const char* pFilePath )
{
	CMesh*		pMesh = Find( pKey );

	if( nullptr != pMesh )
	{
		MessageBox( nullptr, "Already Mesh Exists", nullptr, MB_OK );
		return E_FAIL;
	}

	pMesh = CAnimateMesh::Create( pDevice, pFilePath );
	if( nullptr == pMesh ) return E_FAIL;

	m_mapMesh.insert( make_pair( pKey, pMesh ) );
	return S_OK;
}

HRESULT CAnimateMeshMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
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

	return S_OK;
}
