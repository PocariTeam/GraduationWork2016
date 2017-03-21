#include "stdafx.h"
#include "Lobby.h"
#include "InputMgr.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "Window_UI.h"
#include "Button_UI.h"
#include "NetworkMgr.h"
#include "Value.h"
#include "MeshMgr.h"
#include "Wallpaper.h"
#include "Renderer.h"

CLobby::CLobby()
	: CScene()
	, m_bOverlapped( true )
	, m_dpBtns( nullptr )
{
}

CLobby::~CLobby()
{
}

HRESULT CLobby::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	CNetworkMgr::GetInstance()->Initialize();
	CNetworkMgr::GetInstance()->connectServer( hWnd );

	// Background
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );
	CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Lobby" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	pShader->Add_RenderObject( CWallpaper::Create( pDevice, pMesh, pTexture ) );
	m_listShader[ RENDER_BACKGROUND ].push_back( pShader );


	// Mesh
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );
	CWindow_UI* pWnd_UI = CWindow_UI::Create( CWindow_UI::WND_LOBBY );
	pShader->Add_RenderObject( pWnd_UI );

	m_dpBtns = new CButton_UI*[ 8 ];
	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 2; ++j )
			pShader->Add_RenderObject( m_dpBtns[ i * 2 + j ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_L_Room" ), XMFLOAT4( 0.08f + float( j ) * 0.67f, -0.23f - float( i ) * 0.3f, 0.42f, 0.13f ) ) );

	m_listShader[ RENDER_UI ].push_back( pShader );

	CRenderer::GetInstance()->Copy_RenderGroup( m_listShader );

	return CScene::Initialize( hWnd, pDevice );
}

int CLobby::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );

	return Check_Key();
}

DWORD CLobby::Release( void )
{
	CScene::Release();
	CRenderer::GetInstance()->Clear_RenderGroup();
	delete[] m_dpBtns;
	m_dpBtns = nullptr;

	delete this;

	return 0;
}

void CLobby::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	CRenderer::GetInstance()->Render_Background( pContext );
	CRenderer::GetInstance()->Render_UI( pContext );
}

void CLobby::NotifyRoomInfo( S_RoomList* pRoomlistArray )
{
	m_pRoomInfo = pRoomlistArray;
	UINT iArraySize = sizeof( pRoomlistArray ) / sizeof( S_RoomList );
	int i = 0;
}

int	 CLobby::Check_Key( void )
{
	if( ( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) && m_bOverlapped )
	{
		POINT ptMouse = CScene::GetMousePosition( m_hWnd );
		m_bOverlapped = false;
		for( int i = 0; i < 8; ++i )
		{
			if( PtInRect( &m_dpBtns[ i ]->GetCollisionRect(), ptMouse ) )
			{
				CNetworkMgr::GetInstance()->sendEnterRoom( i );
				return CScene::SCENE_ROOM;
			}
		}
	}

	else if( !( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) )
		m_bOverlapped = true;

	return 0;
}

CScene* CLobby::Create( HWND hWnd, ID3D11Device* pDevice )
{
	CLobby*	pLobby = new CLobby;

	if( FAILED( pLobby->Initialize( hWnd, pDevice ) ) )
	{
		pLobby->Release();
		pLobby = nullptr;
	}

	return pLobby;
}