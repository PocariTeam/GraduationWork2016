#include "stdafx.h"
#include "Jungle.h"
#include "DebugCamera.h"
#include "TextureMgr.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Renderer.h"
#include "Renderer.h"
#include "Terrain.h"
#include "LightMgr.h"
#include "Wallpaper.h"

CJungle::CJungle()
	: CScene()
{
}

CJungle::~CJungle()
{
}

HRESULT CJungle::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pCamera = CDebugCamera::Create( hWnd, pDevice );

	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_Mesh" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Terrain" );
	CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Terrain" );
	m_pTerrain = CTerrain::Create( pDevice, pMesh, pTexture );

	/* 조명 - 현재는 테스트, 나중에는 스레드에서 생성 */
	CLightMgr::GetInstance()->Initialize( pDevice );
	pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	m_pLightShader = CShaderMgr::GetInstance()->Clone( "Shader_Light" );

	m_pShader->Add_RenderObject( m_pTerrain );
	CRenderer::GetInstance()->Add_RenderGroup( CRenderer::RENDER_DEPTHTEST, m_pShader );
	m_pScreen = CWallpaper::Create( pDevice, pMesh );

	m_pLightShader->Add_RenderObject( m_pScreen );
	CRenderer::GetInstance()->Add_RenderGroup( CRenderer::RENDER_LIGHT, m_pLightShader );

	m_pBlendShader = CShaderMgr::GetInstance()->Clone( "Shader_Blend" );
	m_pBlendShader->Add_RenderObject( m_pScreen );
	CRenderer::GetInstance()->Add_RenderGroup( CRenderer::RENDER_BLEND, m_pBlendShader );

	return CScene::Initialize( hWnd, pDevice );
}

int CJungle::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );

	return 0;
}

DWORD CJungle::Release( void )
{
	CScene::Release();

	::Safe_Release( m_pLightShader );
	::Safe_Release( m_pBlendShader );
	::Safe_Release( m_pScreen );
	::Safe_Release( m_pTerrain );
	::Safe_Release( m_pShader );

	delete this;

	return 0;
}

void CJungle::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );
	CRenderer::GetInstance()->Render( pContext );
}

CScene* CJungle::Create( HWND hWnd, ID3D11Device* pDevice )
{
	CJungle* pJungle = new CJungle;

	if( FAILED( pJungle->Initialize( hWnd, pDevice ) ) )
	{
		pJungle->Release();
		pJungle = nullptr;
	}

	return pJungle;
}
