#include "stdafx.h"
#include "Lobby.h"
#include "InputMgr.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "Button_UI.h"
#include "NetworkMgr.h"
#include "Value.h"
#include "MeshMgr.h"
#include "Wallpaper.h"
#include "Renderer.h"
#include "Normal_UI.h"

CLobby::CLobby()
	: CScene()
	, m_dpBtns( nullptr )
	, m_pRoomInfo( nullptr )
	, m_bOverlapped( true )
	, m_dwMaxPageCnt( 1 )
	, m_dwCurrentPage( 1 )
	, m_dwMaxRoomCnt( 0 )
	, m_pRenderer( nullptr )
	, m_pInputMgr( nullptr )
	, m_bEnter( false )
{
}

CLobby::~CLobby()
{
}

HRESULT CLobby::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	CTextureMgr*	pTextureMgr = CTextureMgr::GetInstance();
	m_pRenderer = CRenderer::GetInstance();
	m_pInputMgr = CInputMgr::GetInstance();

	// Background
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );
	CTexture* pTexture = pTextureMgr->Clone( "Texture_Lobby" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	pShader->Add_RenderObject( CWallpaper::Create( pDevice, pMesh, pTexture ) );
	m_listShader[ RENDER_BACKGROUND ].push_back( pShader );

	// Mesh
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );

	CNormal_UI* pWnd_UI = CNormal_UI::Create( pTextureMgr->Clone( "Texture_Wnd" ), XMFLOAT4( -0.63f, 0.9f, 1.26f, 2.24f ), 4.f );
	pShader->Add_RenderObject( pWnd_UI );

	m_dpBtns = new CButton_UI*[ BTN_END ];

	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 2; ++j )
			pShader->Add_RenderObject( m_dpBtns[ i * 2 + j ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_L_Room" ), XMFLOAT4( -0.3f + float( j ) * 0.3f, -0.2f - float( i ) * 0.17f, 0.28f, 0.16f ) ) );

	pShader->Add_RenderObject( m_dpBtns[ BTN_REFRESH ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Refresh" ), XMFLOAT4( -0.25f, 0.18f, 0.09f, 0.16f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_PREVIOUS ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Previous" ), XMFLOAT4( -0.15f, 0.18f, 0.09f, 0.16f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_NEXT ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Next" ), XMFLOAT4( -0.05f, 0.18f, 0.09f, 0.16f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_EXIT ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Exit" ), XMFLOAT4( 0.05f, 0.18f, 0.18f, 0.16f ) ) );

	m_listShader[ RENDER_UI ].push_back( pShader );
	m_pRenderer->Copy_RenderGroup( m_listShader );

	CScene::Initialize( hWnd, pDevice );
	CNetworkMgr::GetInstance()->sendRequestRoomList();

	return S_OK;
}

int CLobby::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );

	return Check_Key();
}

DWORD CLobby::Release( void )
{
	CScene::Release();
	m_pRenderer->Clear_RenderGroup();
	m_pRenderer = nullptr;
	m_pInputMgr = nullptr;

	delete[] m_dpBtns;
	m_dpBtns = nullptr;

	delete this;

	return 0;
}

void CLobby::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	m_pRenderer->Render_Background( pContext );
	m_pRenderer->Render_UI( pContext );
}

void CLobby::NotifyRoomInfo( S_RoomList* pRoomlistPacket )
{
	m_pRoomInfo = pRoomlistPacket->roomInfo;
	m_dwMaxRoomCnt = ( pRoomlistPacket->header.size - sizeof( HEADER ) ) / sizeof( RoomInfo );
	m_dwMaxPageCnt = m_dwMaxRoomCnt / ( UINT )ROOM_PRESENT_CNT + ( ( m_dwMaxRoomCnt % ( UINT )ROOM_PRESENT_CNT ) ? 1 : 0 );
	m_dwCurrentPage = 1;

	for( int i = 0; i < ( int )m_dwMaxRoomCnt; ++i )
	{
		if( i < ROOM_PRESENT_CNT )
		{
			if( TRUE == m_pRoomInfo[ i ].playing || PLAYER_CAPACITY == m_pRoomInfo[ i ].playerCount ) m_dpBtns[ i ]->Disable();
			else	m_dpBtns[ i ]->Normal();
		}
		else
			m_dpBtns[ i ]->Hide();
	}
		

	m_dpBtns[ BTN_PREVIOUS ]->Disable();
	if( 1 == m_dwMaxPageCnt ) m_dpBtns[ BTN_NEXT ]->Disable();
	else m_dpBtns[ BTN_NEXT ]->Normal();
}

void CLobby::NotifyEnterRoom()
{
	m_bEnter = true;
}

int	 CLobby::Check_Key( void )
{
	POINT ptMouse = CScene::GetMousePosition( m_hWnd );

	if( m_pInputMgr->Get_MouseState( CInputMgr::CLICK_LBUTTON ) && m_bOverlapped )
		m_bOverlapped = false;

	else if( !m_pInputMgr->Get_MouseState( CInputMgr::CLICK_LBUTTON ) )
		m_bOverlapped = true;

	for( int i = 0; i < BTN_END; ++i )
		if( m_dpBtns[ i ]->isCollide( ptMouse, !m_bOverlapped ) )
		{
			switch( i )
			{
			case BTN_REFRESH:
				CNetworkMgr::GetInstance()->sendRequestRoomList();
				break;
			case BTN_PREVIOUS:
				m_dwCurrentPage--;
				if( 1 == m_dwCurrentPage ) m_dpBtns[ BTN_PREVIOUS ]->Disable();
				m_dpBtns[ BTN_NEXT ]->Normal();
				Change_Page();
				break;
			case BTN_NEXT:
				m_dwCurrentPage++;
				if( 0 == m_dwMaxPageCnt - m_dwCurrentPage ) m_dpBtns[ BTN_NEXT ]->Disable();
				m_dpBtns[ BTN_PREVIOUS ]->Normal();
				Change_Page();
				break;
			case BTN_EXIT:
				PostQuitMessage( 1 );
				break;
			default:
				if( PLAYER_CAPACITY == m_pRoomInfo[ i + ( ( int )m_dwCurrentPage - 1 ) * ( int )ROOM_PRESENT_CNT ].playerCount
					|| m_pRoomInfo[ i + ( ( int )m_dwCurrentPage - 1 ) * ( int )ROOM_PRESENT_CNT ].playing )
					break;
				CNetworkMgr::GetInstance()->sendEnterRoom( i );
				break;
			}
		}

	if( m_bEnter )
		return SCENE_ROOM;

	return 0;
}

void CLobby::Change_Page( void )
{
	int iPresent_Start = int( ( m_dwCurrentPage - 1 ) * ROOM_PRESENT_CNT );
	for( int i = iPresent_Start; i < iPresent_Start + ( int )ROOM_PRESENT_CNT; ++i )
	{
		if( i < ( signed )m_dwMaxRoomCnt )
		{
			if( TRUE == m_pRoomInfo[ i ].playing || PLAYER_CAPACITY == m_pRoomInfo[ i ].playerCount ) m_dpBtns[ i ]->Disable();
			else	m_dpBtns[ i ]->Normal();
		}

		else m_dpBtns[ i ]->Hide();
	}
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