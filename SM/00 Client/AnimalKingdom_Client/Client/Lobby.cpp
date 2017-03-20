#include "stdafx.h"
#include "Lobby.h"
#include "InputMgr.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "UserInterface.h"
#include "NetworkMgr.h"
#include "Value.h"
#include "MeshMgr.h"
#include "Wallpaper.h"
#include "Renderer.h"

CLobby::CLobby()
	: CScene()
	, m_bOverlapped( true )
{
}

CLobby::~CLobby()
{
}

HRESULT CLobby::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	// Background
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );
	CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Lobby" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	pShader->Add_RenderObject( CWallpaper::Create( pDevice, pMesh, pTexture ) );
	m_listShader[ RENDER_BACKGROUND ].push_back( pShader );
		
	// Mesh
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );

	pShader->Add_RenderObject( CUserInterface::Create( CTextureMgr::GetInstance()->Clone( "Texture_Wnd" ), XMFLOAT4( -0.7f, 0.9f, 1.5f, 1.7f ), 0.f ) );

	for( int i = 0; i < 4; ++i )
		for( int j = 0; j < 2; ++j )
		{
			pShader->Add_RenderObject( CUserInterface::Create( CTextureMgr::GetInstance()->Clone( "Texture_L_Room" ), XMFLOAT4( -0.6f + float( j ) * 0.7f, 0.7f - float( i ) * 0.3f, 0.6f, 0.2f ), 0.f ) );
			m_rcRoom[ i * 2 + j ].top = long( ( -1.f * ( 0.7f - float( i ) * 0.3f ) + 1.f ) * ( float )g_wWinsizeY * 0.5f );
			m_rcRoom[ i * 2 + j ].left = long( ( -0.6f + float( j ) * 0.7f + 1.f ) * ( float )g_wWinsizeX * 0.5f );
			m_rcRoom[ i * 2 + j ].bottom = long( ( -1.f * ( 0.7f - float( i ) * 0.3f ) + 1.f ) * ( float )g_wWinsizeY * 0.5f + 0.2f * 0.5f * ( float )g_wWinsizeY );
			m_rcRoom[ i * 2 + j ].right = long( ( -0.6f + float( j ) * 0.7f + 1.f ) * ( float )g_wWinsizeX * 0.5f + 0.6f * 0.5f * ( float )g_wWinsizeX );
		}

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

	delete this;

	return 0;
}

void CLobby::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	CRenderer::GetInstance()->Render_Background( pContext );
	CRenderer::GetInstance()->Render_UI( pContext );
}

int	 CLobby::Check_Key( void )
{
	if( ( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) && m_bOverlapped )
	{
		POINT ptMouse = CScene::GetMousePosition( m_hWnd );
		m_bOverlapped = false;
		for( int i = 0; i < 8; ++i )
		{
			if( PtInRect( &m_rcRoom[ i ], ptMouse ) )
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