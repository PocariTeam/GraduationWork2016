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
#include "NetworkMgr.h"

CLogo::CLogo()
	: CScene()
	, m_pTexture( nullptr )
	, m_pShader( nullptr )
	, m_pMesh( nullptr )
	, m_pThreadMgr( nullptr )
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

	m_pThreadMgr = CThreadMgr::GetInstance();
	CPhysics::GetInstance()->Initialize( pDevice );
	m_pThreadMgr->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_Textures", "../Executable/Resources/List/Textures.txt", pDevice, CThreadMgr::LOAD_TEXTURE );
	m_pThreadMgr->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_StaticMeshes", "../Executable/Resources/List/StaticMeshes.txt", pDevice, CThreadMgr::LOAD_MESH );
	m_pThreadMgr->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_Shaders", "../Executable/Resources/List/Shaders.txt", pDevice, CThreadMgr::LOAD_SHADER );
	m_pThreadMgr->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_AnimateMeshes", "../Executable/Resources/List/AnimateMeshes.txt", pDevice, CThreadMgr::LOAD_ANIMATEMESH );
	m_pThreadMgr->Create_Thread( CThreadMgr::THREAD_LOAD, "Thread_Load_Animation", "../Executable/Resources/List/Animations.txt", pDevice, CThreadMgr::LOAD_ANIMATION );

	CNetworkMgr::GetInstance()->connectServer( hWnd );

	return CScene::Initialize( hWnd, pDevice );
}

int CLogo::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( m_pThreadMgr->Check_EndEvent( 1 ) ) return CScene::SCENE_LOBBY;

	return 0;
}

DWORD CLogo::Release( void )
{
	CScene::Release();

	m_pThreadMgr = nullptr;
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
