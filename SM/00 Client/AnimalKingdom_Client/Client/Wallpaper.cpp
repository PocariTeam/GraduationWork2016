#include "stdafx.h"
#include "Wallpaper.h"
#include "Function.h"
#include "Mesh.h"
#include "Define.h"
#include "Value.h"

CWallpaper::CWallpaper()
	: CGameObject()
	, m_pMesh( nullptr )
{
}


CWallpaper::~CWallpaper()
{
}

HRESULT CWallpaper::Initialize( ID3D11Device* pDevice, CMesh* pMesh )
{
	m_pMesh = pMesh;

	return S_OK;
}

CWallpaper* CWallpaper::Create( ID3D11Device* pDevice, CMesh* pMesh )
{
	CWallpaper* pWallpater = new CWallpaper;

	if( FAILED( pWallpater->Initialize( pDevice, pMesh ) ) )
	{
		pWallpater->Release();
		pWallpater = nullptr;
	}
	return pWallpater;
}

void CWallpaper::Render( ID3D11DeviceContext* pContext )
{
	m_mtxWorld._11 = ( float )g_wWinsizeX;
	m_mtxWorld._12 = ( float )g_wWinsizeY;

	m_pMesh->Render( pContext );
}

DWORD CWallpaper::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pMesh );

	delete this;

	return 0;
}
