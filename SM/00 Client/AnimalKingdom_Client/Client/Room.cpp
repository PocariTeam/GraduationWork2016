#include "stdafx.h"
#include "Room.h"
#include "InputMgr.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "UserInterface.h"
#include "NetworkMgr.h"
#include "Value.h"
#include "Renderer.h"
#include "Wallpaper.h"
#include "MeshMgr.h"
#include "Window_UI.h"
#include "Button_UI.h"
#include "Normal_UI.h"

CRoom::CRoom()
	: CScene()
	, m_bStart( false )
	, m_bOverlapped( true )
	, m_pPlayerInfo( nullptr )
	, m_dpReady( nullptr )
	, m_dpBtns( nullptr )
	, m_dwPlayerCnt( 0 )
{
}

CRoom::~CRoom()
{
}

HRESULT CRoom::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	CScene::Initialize( hWnd, pDevice );

	// Background
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );
	CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Room" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	pShader->Add_RenderObject( CWallpaper::Create( pDevice, pMesh, pTexture ) );
	m_listShader[ RENDER_BACKGROUND ].push_back( pShader );

	// Mesh
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );
	CWindow_UI* pWnd_UI = CWindow_UI::Create( CWindow_UI::WND_ROOM );
	pShader->Add_RenderObject( pWnd_UI );

	m_dpBtns = new CButton_UI*[ BTN_END ];
	m_dpReady = new CNormal_UI*[ PLAYER_CAPACITY ];

	pShader->Add_RenderObject( m_dpBtns[ BTN_READY ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Ready" ), XMFLOAT4( -1.f, -0.5f, 0.6f, 0.3f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_BACK ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Back" ), XMFLOAT4( -1.f, -0.8f, 0.6f, 0.2f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_MAP_NEXT ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Back" ), XMFLOAT4( -0.4f, 0.2f, 0.2f, 0.55f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_MAP_PREVIOUS ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Back" ), XMFLOAT4( 0.8f, 0.2f, 0.2f, 0.55f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_CHAMELEON ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Select_Chameleon" ), XMFLOAT4( -0.4f, -0.35f, 0.28f, 0.65f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_MONKEY ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Select_Monkey" ), XMFLOAT4( -0.12f, -0.35f, 0.28f, 0.65f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Slot" ), XMFLOAT4( -0.35f, 1.f, 0.45f, 0.8f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Slot" ), XMFLOAT4( 0.1f, 1.f, 0.45f, 0.8f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Slot" ), XMFLOAT4( 0.55f, 1.f, 0.45f, 0.8f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Preview_Jungle" ), XMFLOAT4( -0.2f, 0.2f, 1.f, 0.55f ) ) );

	pShader->Add_RenderObject( m_dpReady[ 0 ] = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( -0.95f, -0.2f, 0.5f, 0.3f ) ) );
	pShader->Add_RenderObject( m_dpReady[ 1 ] = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( -0.3f, 0.45f, 0.35f, 0.2f ) ) );
	pShader->Add_RenderObject( m_dpReady[ 2 ] = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( 0.15f, 0.45f, 0.35f, 0.2f ) ) );
	pShader->Add_RenderObject( m_dpReady[ 3 ] = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( 0.6f, 0.45f, 0.35f, 0.2f ) ) );

	for( int i = 0; i < PLAYER_CAPACITY; ++i )
		m_dpReady[ i ]->Hide();

	m_listShader[ RENDER_UI ].push_back( pShader );

	CRenderer::GetInstance()->Copy_RenderGroup( m_listShader );

	return S_OK;
}

int CRoom::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( m_bStart ) return SCENE_JUNGLE;

	return Check_Key();
}

DWORD CRoom::Release( void )
{
	CScene::Release();
	CRenderer::GetInstance()->Clear_RenderGroup();
	delete[] m_dpBtns;
	m_dpBtns = nullptr;
	delete[] m_dpReady;
	m_dpReady = nullptr;

	delete this;

	return 0;
}

void CRoom::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	CRenderer::GetInstance()->Render_Background( pContext );
	CRenderer::GetInstance()->Render_UI( pContext );

}

int CRoom::Check_Key( void )
{
	POINT ptMouse = CScene::GetMousePosition( m_hWnd );

	if( ( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) && m_bOverlapped )
		m_bOverlapped = false;

	else if( !( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) )
		m_bOverlapped = true;

	for( int i = 0; i < BTN_END; ++i )
		if( m_dpBtns[ i ]->isCollide( ptMouse, !m_bOverlapped ) )
		{
			switch( i )
			{
			case BTN_READY:
				if( CNetworkMgr::GetInstance()->isMaster() )
					CNetworkMgr::GetInstance()->sendStartRoom();
				else
					CNetworkMgr::GetInstance()->sendReadyRoom();
				break;
			case BTN_BACK:
				return SCENE_LOBBY;
			case BTN_CHAMELEON:
				CNetworkMgr::GetInstance()->sendSelectCharacter( CHAMEL );
				m_dpBtns[ BTN_MONKEY ]->Normal();
				m_dpBtns[ BTN_CHAMELEON ]->Fix();
				break;
			case BTN_MONKEY:
				CNetworkMgr::GetInstance()->sendSelectCharacter( CHAMEL );
				m_dpBtns[ BTN_CHAMELEON ]->Normal();
				m_dpBtns[ BTN_MONKEY ]->Fix();
				break;
			case BTN_MAP_NEXT:
			case BTN_MAP_PREVIOUS:
				break;
			}
		}

	return 0;
}

CScene* CRoom::Create( HWND hWnd, ID3D11Device* pDevice )
{
	CRoom*	pRoom = new CRoom;

	if( FAILED( pRoom->Initialize( hWnd, pDevice ) ) )
	{
		pRoom->Release();
		pRoom = nullptr;
	}

	return pRoom;
}

void CRoom::NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt )
{
	m_dwPlayerCnt = dwPlayerCnt;
	m_pPlayerInfo = pPlayerInfo;

	if( CNetworkMgr::GetInstance()->isMaster() )
		m_dpBtns[ BTN_READY ]->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Start" ) );

	PlayerInfo* pTempArray = new PlayerInfo[ m_dwPlayerCnt ];

	for( UINT i = 0, j = 1; i < m_dwPlayerCnt; ++i )
	{
		if( CNetworkMgr::GetInstance()->getID() == m_pPlayerInfo[ i ].id )
			memcpy_s( &pTempArray[ 0 ], sizeof( PlayerInfo ), &m_pPlayerInfo[ i ], sizeof( PlayerInfo ) );
		else
		{
			memcpy_s( &pTempArray[ j ], sizeof( PlayerInfo ), &m_pPlayerInfo[ i ], sizeof( PlayerInfo ) );
			j++;
		}
	}

	for( UINT i = 0; i < m_dwPlayerCnt; ++i )
	{
		m_dpReady[ i ]->Hide();

		if( pTempArray[ i ].isMaster )
		{
			m_dpReady[ i ]->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Billboard_Master" ) );
			m_dpReady[ i ]->Show();
		}

		else if( pTempArray[ i ].isReady )
			m_dpReady[ i ]->Show();
	}

	delete[] pTempArray;
	pTempArray = nullptr;
}
