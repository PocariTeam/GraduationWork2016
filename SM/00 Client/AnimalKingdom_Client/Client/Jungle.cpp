#include "stdafx.h"
#include "Jungle.h"
#include "DebugCamera.h"
#include "Renderer.h"
#include "LightMgr.h"
#include "Physics.h"
#include "RenderTargetMgr.h"
#include "NetworkMgr.h"
#include "Player.h"
#include "State.h"

CJungle::CJungle()
	: CScene()
	, m_iPlayerID( -1 )
{
}

CJungle::~CJungle()
{
}

HRESULT CJungle::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pCamera = CDebugCamera::Create( hWnd, pDevice );
	CLightMgr::GetInstance()->Initialize( pDevice );
	CPhysics::GetInstance()->Load_Scene( pDevice, m_listShader, &m_mapPlayer, "../Executable/Resources/Scene/Jungle_player4.xml" );
	CNetworkMgr::GetInstance()->setScene( this );
	m_iPlayerID = CNetworkMgr::GetInstance()->getID();

	return CScene::Initialize( hWnd, pDevice );
}

int CJungle::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( -1 != m_iPlayerID )
		m_mapPlayer.find( m_iPlayerID )->second->Check_Key( fTimeDelta );
		
	CPhysics::GetInstance()->Update( fTimeDelta );

	return 0;
}

DWORD CJungle::Release( void )
{
	CScene::Release();

	m_mapPlayer.erase( m_mapPlayer.begin(), m_mapPlayer.end() );

	CGlobalState::DestroyInstance();
	CIdleState::DestroyInstance();
	CJumpState::DestroyInstance();
	CRunState::DestroyInstance();
	CAttackState::DestroyInstance();
	CBeatenState::DestroyInstance();
	CDefendState::DestroyInstance();

	delete this;

	return 0;
}

void CJungle::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 1 );
	CPhysics::GetInstance()->Render( pContext );

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
