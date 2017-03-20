#include "stdafx.h"
#include "Wallpaper.h"
#include "Mesh.h"
#include "Value.h"
#include "Texture.h"

CWallpaper::CWallpaper()
	: CGameObject()
{
}


CWallpaper::~CWallpaper()
{
}

HRESULT CWallpaper::Initialize( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture )
{
	m_pTexture = pTexture;
	m_pMesh = pMesh;

	return S_OK;
}

CWallpaper* CWallpaper::Create( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture )
{
	CWallpaper* pWallpater = new CWallpaper;

	if( FAILED( pWallpater->Initialize( pDevice, pMesh, pTexture ) ) )
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
	if( nullptr != m_pTexture ) m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CWallpaper::Release( void )
{
	CGameObject::Release();

	delete this;

	return 0;
}
