#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Function.h"
#include "Functor.h"
#include "Camera.h"
#include "Logo.h"
#include "Jungle.h"
#include "Lobby.h"
#include "Shader.h"
#include "Room.h"
#include "NetworkMgr.h"

CScene * CScene::Create( HWND hWnd, ID3D11Device * pDevice, BYTE byStageNum )
{
	CScene* pScene = nullptr;

	switch( byStageNum )
	{
	case SCENE_LOGO:
		pScene = CLogo::Create( hWnd, pDevice );
		break;
	case SCENE_LOBBY:
		pScene = CLobby::Create( hWnd, pDevice );
		break;
	case SCENE_ROOM:
		pScene = CRoom::Create( hWnd, pDevice );
		break;
	case SCENE_JUNGLE:
		pScene = CJungle::Create( hWnd, pDevice );
		break;
	}

	if( nullptr == pScene ) {
		MessageBox( hWnd, "Scene Create Fail", nullptr, MB_OK );
	}
	return pScene;
}


CScene::CScene()
	:m_hWnd( nullptr )
	, m_pCamera( nullptr )
	, m_pDevice( nullptr )
	, m_fPlayingTime( GAME_PLAYING_SEC )
{
}

HRESULT CScene::Initialize( HWND hWnd, ID3D11Device * pDevice )
{
	m_hWnd = hWnd;
	if( nullptr == m_hWnd )
		return E_FAIL;

	CNetworkMgr::GetInstance()->setScene( this );

	return S_OK;
}

int CScene::Update( const float& fTimeDelta )
{
	if( nullptr != m_pCamera )
		m_pCamera->Update( fTimeDelta );

	for( DWORD i = 0; i < ( DWORD )RENDER_END; ++i )
		for_each( m_listShader[ i ].begin(), m_listShader[ i ].end(), [&fTimeDelta]( CShader* pShader ) { pShader->Update( fTimeDelta ); } );

	return 0;
}

DWORD CScene::Release( void )
{
	::Safe_Release( m_pCamera );

	for( DWORD i = 0; i < ( DWORD )RENDER_END; ++i )
		for_each( m_listShader[ i ].begin(), m_listShader[ i ].end(), ReleaseElement() );

	return 0;
}

void CScene::Render( ID3D11DeviceContext* pContext )
{
	if( nullptr != m_pCamera )
		m_pCamera->Render( pContext );
}

POINT CScene::GetMousePosition( HWND hWnd )
{
	POINT	ptMouse;
	::GetCursorPos( &ptMouse );
	::ScreenToClient( hWnd, &ptMouse );

	return ptMouse;
}