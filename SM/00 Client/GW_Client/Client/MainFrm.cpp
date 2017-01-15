#include "stdafx.h"
#include "MainFrm.h"
#include "GraphicDev.h"
#include "Function.h"
#include "Scene.h"
#include "Renderer.h"
#include "ShaderMgr.h"
#include "ResourceMgr.h"
#include "PhysicsMgr.h"

HRESULT CMainFrm::Initialize( const HWND hWnd )
{
	m_hWnd = hWnd;
	/* 그래픽 장치 초기화 및 객체 생성 */
	m_pGraphicDev = CGraphicDev::Create( CGraphicDev::MODE_WIN, hWnd, WIN_WIDTH, WIN_HEIGHT );
	if( nullptr == m_pGraphicDev ) return E_FAIL;

	/* For FPS */
	ZeroMemory( m_szFPS, sizeof( TCHAR ) * MIN_STR );
	m_dwFrameCnt = 0;
	m_fAccTime = 0.f;

	/* Scene 생성 */
	if( FAILED( Change_Scene() ) )
		return E_FAIL;

	return S_OK;
}

int CMainFrm::Update( const float& fTimeDelta )
{
	if( nullptr == m_pScene ) return 1;

	// Frame Check
	m_fAccTime += fTimeDelta;

	int iRetVal{ 0 };

	// Scene Update
	if( iRetVal = m_pScene->Update( fTimeDelta ) ) {
		m_bySceneNum += iRetVal;
		Change_Scene();
	}

	return 0;
}

void CMainFrm::Render( void )
{
	m_pGraphicDev->BeginScene();

	m_pScene->Render( m_pGraphicDev->Get_Context() );

	m_pGraphicDev->EndScene();
}

void CMainFrm::Release( void )
{
	::Safe_Release( m_pScene );
	
	CRenderer::GetInstance()->Release();
	CPhysicsMgr::GetInstance()->Release();
	CShaderMgr::GetInstance()->Release();
	CResourceMgr::GetInstance()->Release();
	::Safe_Release( m_pGraphicDev );
	m_hWnd = nullptr;

	delete this;
}

CMainFrm* CMainFrm::Create( const HWND hWnd )
{
	CMainFrm*	pMainFrm = new CMainFrm;

	if( FAILED( pMainFrm->Initialize( hWnd ) ) )
	{
		pMainFrm->Release();
		pMainFrm = nullptr;
	}

	return pMainFrm;
}

HRESULT CMainFrm::Change_Scene( void )
{
	::Safe_Release( m_pScene );
	m_pScene = CScene::Create( m_hWnd, m_pGraphicDev->Get_Device(), m_pGraphicDev->Get_Context(), m_bySceneNum );
	if( nullptr == m_pScene )
		return E_FAIL;

	return S_OK;
}

CMainFrm::CMainFrm()
	: m_pGraphicDev( nullptr )
	, m_pScene( nullptr )
	, m_bySceneNum( 0 )
{
}

CMainFrm::~CMainFrm()
{
}
