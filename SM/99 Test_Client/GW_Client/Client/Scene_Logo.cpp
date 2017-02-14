#include "stdafx.h"
#include "Scene_Logo.h"
#include "Function.h"
#include "Loader.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "ResourceMgr.h"
#include "Renderer.h"
#include "Texture.h"

HRESULT CScene_Logo::Initialize( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext )
{
	m_hWnd = hWnd;
	m_pLoader = CLoader::Create( pDevice, pContext );

	return S_OK;
}

int CScene_Logo::Update( const float & fTimeDelta )
{
	CScene::Update( fTimeDelta );

	if( CLoader::m_bLoading )
		if( GetAsyncKeyState( VK_RETURN ) )
				return 1;
	return 0;
}

void CScene_Logo::Render( ID3D11DeviceContext * pContext )
{
}

void CScene_Logo::Release( void )
{
	::Safe_Release( m_pLoader );
	CScene::Release();
	CRenderer::GetInstance()->Clear_RenderGroup();

	delete this;
}

CScene * CScene_Logo::Create( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext )
{
	CScene_Logo* pScene = new CScene_Logo;

	if( FAILED( pScene->Initialize( hWnd, pDevice, pContext ) ) ) {
		pScene->Release();
		pScene = nullptr;
	}

	return pScene;
}
