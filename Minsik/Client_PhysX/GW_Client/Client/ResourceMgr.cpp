#include "stdafx.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "Cube.h"
#include "Texture.h"
#include "SingleTexture.h"
#include "Point.h"

CResourceMgr*	CResourceMgr::m_pInstance = nullptr;

CResourceMgr::CResourceMgr()
{
}

CResourceMgr::~CResourceMgr()
{
}

CResourceMgr*	CResourceMgr::GetInstance()
{
	if( nullptr == m_pInstance )
		m_pInstance = new CResourceMgr;

	return m_pInstance;
}
void CResourceMgr::DestroyInstance()
{
	if( nullptr != m_pInstance )
	{
		m_pInstance->Release();
		m_pInstance = nullptr;
	}
}

CMesh* CResourceMgr::FindMesh( const TCHAR* pMeshKey )
{
	MESHCONTAINER::iterator	Find_Iter = m_mapMesh.find( pMeshKey );

	if( Find_Iter == m_mapMesh.end() )
		return nullptr;

	return Find_Iter->second;
}

CMesh* CResourceMgr::CloneMesh( const TCHAR* pMeshKey )
{
	CMesh*		pMesh = FindMesh( pMeshKey );
	if( nullptr == pMesh )
		return nullptr;

	return pMesh->Clone();
}

CTexture* CResourceMgr::FindTexture( const TCHAR * pTextureKey )
{
	TEXTURECONTAINER::iterator	Find_Iter = m_mapTexture.find( pTextureKey );

	if( Find_Iter == m_mapTexture.end() )
		return nullptr;

	return Find_Iter->second;
}

CTexture * CResourceMgr::CloneTexture( const TCHAR * pTextrueKey )
{
	CTexture*		pTexture = FindTexture( pTextrueKey );
	if( nullptr == pTexture )
		return nullptr;

	return pTexture->Clone();
}

HRESULT CResourceMgr::AddMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext
	, MESH_TYPE eMeshType, const TCHAR* pMeshKey, XMFLOAT3& vSizeData /*= XMFLOAT3( 0.f, 0.f, 1.f )*/, const TCHAR* pObjPath/* = nullptr*/ )
{
	CMesh*		pMesh = FindMesh( pMeshKey );
	if( nullptr != pMesh )
	{
		MessageBox( nullptr, "이미 만들어진 키값입니다.", nullptr, MB_OK );
		return E_FAIL;
	}

	switch( eMeshType )
	{
	case MESH_RECT:
		// pMesh = CRectangle::Create( pDevice, pContext );
		break;

	case MESH_CUBE:
		pMesh = CCube::Create( pDevice, pContext );
		break;

	case MESH_SPHERE:
		// pMesh = CSphere::Create( pDevice, pContext, vSizeData );
		break;

	case MESH_HEIGHTMAP:
		// pMesh = CHeightMap::Create( pDevice, pContext, vSizeData );
		break;

	case MESH_PARTICLE:
		// pMesh = CParticleMesh::Create( pDevice, pContext );
		break;

	case MESH_POINT:
		pMesh = CPoint::Create( pDevice, pContext );
		break;

	case MESH_BACKGROUND:
		// pMesh = CBackground::Create( pDevice, pContext );
		break;

	case MESH_OBJ:
		// pMesh = CObjMesh::Create( pDevice, pContext, pObjPath );
		break;
	}

	if( nullptr == pMesh ) {
		MessageBox( nullptr, "Mesh 생성 실패", nullptr, MB_OK );
		return E_FAIL;
	}

	m_mapMesh.insert( make_pair( pMeshKey, pMesh ) );
	return S_OK;
}

HRESULT CResourceMgr::AddTexture( ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TEXTURE_TYPE eTextureType, const TCHAR * pPath, const TCHAR * pTextureKey, int iTextureCnt/* = 1*/ )
{
	CTexture*		pTexture = FindTexture( pTextureKey );

	if( nullptr != pTexture )
	{
		MessageBox( nullptr, "이미 만들어진 키값입니다.", nullptr, MB_OK );
		return E_FAIL;
	}

	switch( eTextureType )
	{
	case TEXTURE_SINGLE:
		pTexture = CSingleTexture::Create( pDevice, pContext, pPath );
		break;
	case TEXTURE_MULTI:
		// pTexture = CMultiTexture::Create( pDevice, pContext, pPath, iTextureCnt );
		break;
	}
	
	if( nullptr == pTexture ) {
		MessageBox( nullptr, "Textures 생성 실패", nullptr, MB_OK );
		return E_FAIL;
	}

	m_mapTexture.insert( make_pair( pTextureKey, pTexture ) );

	return S_OK;
}

void CResourceMgr::Release( void )
{
	MESHCONTAINER::iterator Adv_Iter = m_mapMesh.begin();

	for( ; Adv_Iter != m_mapMesh.end(); ) {
		Adv_Iter->second->Release();
		Adv_Iter = m_mapMesh.erase( Adv_Iter );
	}
	m_mapMesh.clear();

	TEXTURECONTAINER::iterator Adv_Iter2 = m_mapTexture.begin();
	for( ; Adv_Iter2 != m_mapTexture.end(); ) {
		Adv_Iter2->second->Release();
		Adv_Iter2 = m_mapTexture.erase( Adv_Iter2 );
	}
	m_mapTexture.clear();

	delete this;
}