#include "stdafx.h"
#include "Terrain.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"

CTerrain::CTerrain()
	: CGameObject()
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture )
{
	m_pActor = pActor;
	m_pMesh = pMesh;
	m_pTexture = pTexture;

	return S_OK;
}

CTerrain* CTerrain::Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture )
{
	CTerrain* pTerrain = new CTerrain;

	if( FAILED( pTerrain->Initialize( pDevice, pActor, pMesh, pTexture ) ) )
	{
		pTerrain->Release();
		pTerrain = nullptr;
	}
	return pTerrain;
}

void CTerrain::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CTerrain::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pTexture );
	::Safe_Release( m_pMesh );

	delete this;

	return 0;
}
