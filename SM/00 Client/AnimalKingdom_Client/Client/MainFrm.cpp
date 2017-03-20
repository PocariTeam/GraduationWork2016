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

	ID3D11Device*	pDevice = m_pGraphicDev->Get_Device();

	if( FAILED( CRenderTargetMgr::GetInstance()->Initialize( pDevice, m_pGraphicDev->Get_SwapChain(), g_wWinsizeX, g_wWinsizeY ) ) )
		return E_FAIL;
	CRenderTargetMgr::GetInstance()->SetRenderTargetView( m_pGraphicDev->Get_Context(), CRenderTargetMgr::RT_BACK, 1 );

	if( FAILED( CInputMgr::GetInstance()->Initialize( hInst, hWnd ) ) )
		return E_FAIL;

	if( FAILED( CThreadMgr::GetInstance()->Initialize( hWnd ) ) )
		return E_FAIL;

	if( FAILED( CRenderer::GetInstance()->Initialize( pDevice ) ) )
		return E_FAIL;

	if( FAILED( Ready_Logo( pDevice ) ) )
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
	if( ::GetActiveWindow() == m_hWnd )	CInputMgr::GetInstance()->Check_Input();

	int iRetVal{ 0 };
	
	m_fAccTime += fTimeDelta;

	Check_Key();

	if( iRetVal = m_pScene->Update( fTimeDelta ) ) {
		m_bySceneNum = iRetVal;
		Change_Scene();
	}

	return 0;
}

void CMainFrm::Render( void )
{
	++m_dwCnt;
	if( m_fAccTime >= 1.f )
	{
		wsprintf( m_szFPS, "Animal Kingdom - FPS : %d", m_dwCnt );
		::SetWindowTextW( m_hWnd, ( LPCWSTR )m_szFPS );
		m_fAccTime = 0.f;
		m_dwCnt = 0;
	}

	ID3D11DeviceContext* pContext = m_pGraphicDev->Get_Context();

	CRenderTargetMgr::GetInstance()->ClearRenderTargetView( pContext );
	CRenderTargetMgr::GetInstance()->ClearDepthStencilView( pContext );

	m_pScene->Render( pContext );

	m_pGraphicDev->EndScene();
}

DWORD CMainFrm::Release( void )
{
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
	CRenderTargetMgr::GetInstance()->ResizeRenderTarget( m_pGraphicDev->Get_Device(), m_pGraphicDev->Get_Context(), m_pGraphicDev->Get_SwapChain(), wSizeX, wSizeY );
	CRenderTargetMgr::GetInstance()->SetRenderTargetView( m_pGraphicDev->Get_Context(), 0, 1 );

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

	m_pScene = CScene::Create( m_hWnd, m_pGraphicDev->Get_Device(), m_bySceneNum );

	if( nullptr == m_pScene )
		return E_FAIL;

	return S_OK;
}

void CMainFrm::Check_Key( void )
{
	/* Wireframe */
	if( ( CInputMgr::GetInstance()->Get_KeyboardState( DIK_F1 ) & 0x80 ) && m_bOverlapped )
	{
		CRenderer::GetInstance()->SetWireframe();
		m_bOverlapped = false;
	}

	/* Debug RenderTarget */
	else if( ( CInputMgr::GetInstance()->Get_KeyboardState( DIK_F2 ) & 0x80 ) && m_bOverlapped )
	{
		CRenderer::GetInstance()->SetRenderTargetDebug();
		m_bOverlapped = false;
	}

	else if( !( CInputMgr::GetInstance()->Get_KeyboardState( DIK_F1 ) & 0x80 ) 
		&& !( CInputMgr::GetInstance()->Get_KeyboardState( DIK_F2 ) & 0x80 ) )
		m_bOverlapped = true;
}

CMainFrm::CMainFrm()
	: m_pGraphicDev( nullptr )
	, m_pScene( nullptr )
	, m_bySceneNum( CScene::SCENE_LOGO )
	, m_dwCnt( 0 )
	, m_dwFrameCnt( 0 )
	, m_fAccTime( 0.f )
	, m_bOverlapped( true )
{
	
	ZeroMemory( m_szFPS, sizeof( char ) * MAX_PATH );
}

CMainFrm::~CMainFrm()
{
}
