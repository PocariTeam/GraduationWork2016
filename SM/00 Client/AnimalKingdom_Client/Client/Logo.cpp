#include "stdafx.h"
#include "Logo.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Mesh.h"
#include "ShaderMgr.h"
#include "Shader.h"
#include "ThreadMgr.h"

CLogo::CLogo()
	: CScene()
	, m_pTexture( nullptr )
	, m_pShader( nullptr )
	, m_pMesh( nullptr )
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Logo" );
	m_pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );

	CThreadMgr::GetInstance()->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_Textures", "../Executable/Resources/List/Textures.txt", pDevice, CThreadMgr::LOAD_TEXTURE );
	CThreadMgr::GetInstance()->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_StaticMeshes", "../Executable/Resources/List/StaticMeshes.txt", pDevice, CThreadMgr::LOAD_MESH );
	CThreadMgr::GetInstance()->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_Shaders", "../Executable/Resources/List/Shaders.txt", pDevice, CThreadMgr::LOAD_SHADER );
	CThreadMgr::GetInstance()->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_AnimateMeshes", "../Executable/Resources/List/AnimateMeshes.txt", pDevice, CThreadMgr::LOAD_ANIMATEMESH );

	return CScene::Initialize( hWnd, pDevice );
}

int CLogo::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( CThreadMgr::GetInstance()->Check_EndEvent( 1 ) ) return CScene::SCENE_JUNGLE;

	return 0;
}

DWORD CLogo::Release( void )
{
	CScene::Release();

	Safe_Release( m_pShader );
	Safe_Release( m_pTexture );
	Safe_Release( m_pMesh );

	delete this;

	return 0;
}

void CLogo::Render( ID3D11DeviceContext* pContext )
{
	m_pShader->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

CScene* CLogo::Create( HWND hWnd, ID3D11Device* pDevice )
{
	CLogo* pLogo = new CLogo;

	if( FAILED( pLogo->Initialize( hWnd, pDevice ) ) )
	{
		pLogo->Release();
		pLogo = nullptr;
	}

	return pLogo;
}
