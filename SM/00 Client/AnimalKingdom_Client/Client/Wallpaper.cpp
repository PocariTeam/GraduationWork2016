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

XMFLOAT4X4 CWallpaper::GetWorld()
{
	XMFLOAT4X4 Out;
	Out._11 = ( float )g_wWinsizeX;
	Out._12 = ( float )g_wWinsizeY;

	return Out;
}

void CWallpaper::Render( ID3D11DeviceContext* pContext )
{
	m_pMesh->Render( pContext );
}

DWORD CWallpaper::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pMesh );

	delete this;

	return 0;
}
