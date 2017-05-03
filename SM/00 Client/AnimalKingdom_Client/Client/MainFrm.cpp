#include "stdafx.h"
#include "MainFrm.h"
#include "Value.h"
#include "GraphicDev.h"
#include "Function.h"
#include "Scene.h"
#include "ThreadMgr.h"
#include "TextureMgr.h"
#include "MeshMgr.h"
#include "Mesh.h"
#include "Background.h"
#include "ShaderMgr.h"
#include "Shader.h"
#include "InputMgr.h"
#include "RenderTargetMgr.h"
#include "Renderer.h"
#include "LightMgr.h"
#include "AnimateMeshMgr.h"
#include "AnimationMgr.h"
#include "Physics.h"
#include "NetworkMgr.h"

HRESULT CMainFrm::Initialize( const HINSTANCE hInst, const HWND hWnd )
{
	m_hWnd = hWnd;
	m_pGraphicDev = CGraphicDev::Create( CGraphicDev::MODE_WIN, hWnd, g_wWinsizeX, g_wWinsizeY );
	if( nullptr == m_pGraphicDev ) return E_FAIL;

	m_pDevice = m_pGraphicDev->Get_Device();
	m_pContext = m_pGraphicDev->Get_Context();

	m_pRenderTargetMgr = CRenderTargetMgr::GetInstance();
	m_pInputMgr = CInputMgr::GetInstance();
	m_pRenderer = CRenderer::GetInstance();

	if( FAILED( m_pRenderTargetMgr->Initialize( m_pDevice, m_pGraphicDev->Get_SwapChain(), g_wWinsizeX, g_wWinsizeY ) ) )
		return E_FAIL;
	m_pRenderTargetMgr->SetRenderTargetView( m_pContext, CRenderTargetMgr::RT_BACK, 1 );

	if( FAILED( m_pInputMgr->Initialize( hInst, hWnd ) ) )
		return E_FAIL;

	if( FAILED( CThreadMgr::GetInstance()->Initialize( hWnd ) ) )
		return E_FAIL;

	if( FAILED( m_pRenderer->Initialize( m_pDevice ) ) )
		return E_FAIL;

	if( FAILED( Ready_Logo( m_pDevice ) ) )
		return E_FAIL;
	
	if( FAILED( Change_Scene() ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CMainFrm::Ready_Logo( ID3D11Device* pDevice )
{
	/* Add Logo Image */
	CTextureMgr::GetInstance()->Add( pDevice, "Texture_Logo", "../Executable/Resources/Texture/Background/Logo.jpg" );

	/* Add Logo Mesh */
	CMesh*	pMesh = CBackground::Create();
	CMeshMgr::GetInstance()->Add( pDevice, "Mesh_Background", pMesh );

	/* Add Logo Shader */
	CShaderMgr::GetInstance()->Add( pDevice, CShaderMgr::SHADER_NORMAL, CShader::INPUT_NO, "Shader_Background", "../Executable/Resources/Shader/Background.fx" );
		
	return S_OK;
}

int CMainFrm::Update( const float& fTimeDelta )
{
	// 윈도우 비활성화시에는 키 체크하지 않겠다
	if( ::GetActiveWindow() == m_hWnd )	m_pInputMgr->Check_Input();

	int iRetVal{ 0 };
	
	m_fAccTime += fTimeDelta;
	++m_dwCnt;

	Check_Key();

	if( iRetVal = m_pScene->Update( fTimeDelta ) ) {
		m_bySceneNum = iRetVal;
		Change_Scene();
	}

	return 0;
}

void CMainFrm::Render( void )
{
	if( m_fAccTime >= 1.f )
	{
		wsprintf( m_szFPS, "Animal Kingdom - FPS : %d", m_dwCnt );
		::SetWindowTextW( m_hWnd, ( LPCWSTR )m_szFPS );
		m_fAccTime = 0.f;
		m_dwCnt = 0;
	}

	m_pRenderTargetMgr->ClearRenderTargetView( m_pContext );
	m_pRenderTargetMgr->ClearDepthStencilView( m_pContext );

	m_pScene->Render( m_pContext );

	m_pGraphicDev->EndScene();
}

DWORD CMainFrm::Release( void )
{
	m_pInputMgr = nullptr;
	m_pRenderTargetMgr = nullptr;
	m_pRenderer = nullptr;
	
	::Safe_Release( m_pScene );

	CPhysics::DestroyInstance();
	CAnimationMgr::DestroyInstance();
	CLightMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CRenderTargetMgr::DestroyInstance();
	CAnimateMeshMgr::DestroyInstance();
	CMeshMgr::DestroyInstance();
	CTextureMgr::DestroyInstance();
	CShaderMgr::DestroyInstance();
	CThreadMgr::DestroyInstance();
	CInputMgr::DestroyInstance();
	CNetworkMgr::DestroyInstance();

	::Safe_Release( m_pGraphicDev );
	m_hWnd = nullptr;

	delete this;

	return 0;
}

void CMainFrm::ResizeRenderTarget( const WORD& wSizeX, const WORD& wSizeY )
{
	m_pRenderTargetMgr->ResizeRenderTarget( m_pDevice, m_pContext, m_pGraphicDev->Get_SwapChain(), g_wWinsizeX, g_wWinsizeY );
	m_pRenderTargetMgr->SetRenderTargetView( m_pContext, 0, 1 );
}

CMainFrm* CMainFrm::Create( const HINSTANCE hInst, const HWND hWnd )
{
	CMainFrm*	pMainFrm = new CMainFrm;

	if( FAILED( pMainFrm->Initialize( hInst, hWnd ) ) )
	{
		pMainFrm->Release();
		pMainFrm = nullptr;
	}

	return pMainFrm;
}

HRESULT CMainFrm::Change_Scene( void )
{
	::Safe_Release( m_pScene );

	m_pScene = CScene::Create( m_hWnd, m_pDevice, m_bySceneNum );

	if( nullptr == m_pScene )
		return E_FAIL;

	return S_OK;
}

void CMainFrm::Check_Key( void )
{
	/* Wireframe */
	if( ( m_pInputMgr->Get_KeyboardState( DIK_F1 ) ) && m_bOverlapped )
	{
		m_pRenderer->SetWireframe();
		m_bOverlapped = false;
	}

	/* Debug RenderTarget */
	else if( ( m_pInputMgr->Get_KeyboardState( DIK_F2 ) ) && m_bOverlapped )
	{
		m_pRenderer->SetRenderTargetDebug();
		m_bOverlapped = false;
	}

	else if( !( m_pInputMgr->Get_KeyboardState( DIK_F1 ) )
		&& !( m_pInputMgr->Get_KeyboardState( DIK_F2 ) ) )
		m_bOverlapped = true;
}

CMainFrm::CMainFrm()
	: m_pGraphicDev( nullptr )
	, m_pDevice( nullptr )
	, m_pContext( nullptr )
	, m_pScene( nullptr )
	, m_bySceneNum( CScene::SCENE_LOGO )
	, m_dwCnt( 0 )
	, m_dwFrameCnt( 0 )
	, m_fAccTime( 0.f )
	, m_bOverlapped( true )
	, m_pInputMgr( nullptr )
	, m_pRenderTargetMgr( nullptr )
	, m_pRenderer( nullptr )
{
	
	ZeroMemory( m_szFPS, sizeof( char ) * MAX_PATH );
}

CMainFrm::~CMainFrm()
{
}
