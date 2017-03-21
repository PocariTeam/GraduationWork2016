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

CRoom::CRoom()
	: CScene()
	, m_bStart( false )
	, m_bOverlapped( true )
	, m_pPlayerInfo( nullptr )
	, m_pPlayerCnt( nullptr )
{
}

CRoom::~CRoom()
{
}

HRESULT CRoom::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	// Background
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );
	CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Room" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	pShader->Add_RenderObject( CWallpaper::Create( pDevice, pMesh, pTexture ) );
	m_listShader[ RENDER_BACKGROUND ].push_back( pShader );

	// Mesh
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );

	/*for( int i = 0; i < 1; ++i )
		for( int j = 0; j < 1; ++j )
		{
			pShader->Add_RenderObject( CUserInterface::Create( CTextureMgr::GetInstance()->Clone( "Texture_L_Room" ), XMFLOAT4( -0.6f + float( j ) * 0.7f, 0.8f - float( i ) * 0.3f, 0.6f, 0.2f ), 0.f ) );
			m_rcRoom[ i * 2 + j ].top = long( ( -1.f * ( 0.8f - float( i ) * 0.3f ) + 1.f ) * ( float )g_wWinsizeY * 0.5f );
			m_rcRoom[ i * 2 + j ].left = long( ( -0.6f + float( j ) * 0.7f + 1.f ) * ( float )g_wWinsizeX * 0.5f );
			m_rcRoom[ i * 2 + j ].bottom = long( ( -1.f * ( 0.8f - float( i ) * 0.3f ) + 1.f ) * ( float )g_wWinsizeY * 0.5f + 0.2f * 0.5f * ( float )g_wWinsizeY );
			m_rcRoom[ i * 2 + j ].right = long( ( -0.6f + float( j ) * 0.7f + 1.f ) * ( float )g_wWinsizeX * 0.5f + 0.6f * 0.5f * ( float )g_wWinsizeX );
		}*/
	m_listShader[ RENDER_UI ].push_back( pShader );
	CRenderer::GetInstance()->Copy_RenderGroup( m_listShader );

	return CScene::Initialize( hWnd, pDevice );
}

int CRoom::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	Check_Key();
	
	if( m_bStart ) return SCENE_JUNGLE;


	return 0;
}

DWORD CRoom::Release( void )
{
	CScene::Release();
	CRenderer::GetInstance()->Clear_RenderGroup();

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
	if( ( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) && m_bOverlapped )
	{
		m_bOverlapped = false;
		POINT ptMouse = CScene::GetMousePosition( m_hWnd );
		for( int i = 0; i < 1; ++i )
		{
			if( PtInRect( &m_rcRoom[ i ], ptMouse ) )
			{
				CNetworkMgr::GetInstance()->sendSelectCharacter( CHAMEL );
				if( CNetworkMgr::GetInstance()->isMaster() )
					CNetworkMgr::GetInstance()->sendStartRoom();
				else
					CNetworkMgr::GetInstance()->sendReadyRoom();
				break;
			}
		}
	}

	else if( !( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) )
		m_bOverlapped = true;

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