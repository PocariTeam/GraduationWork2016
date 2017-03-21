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

CRoom::CRoom()
	: CScene()
	, m_bStart( false )
	, m_bOverlapped( true )
	, m_pPlayerInfo( nullptr )
	, m_dwPlayerCnt( 0 )
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
	CWindow_UI* pWnd_UI = CWindow_UI::Create( CWindow_UI::WND_LOBBY );
	pShader->Add_RenderObject( pWnd_UI );

	m_dpBtns = new CButton_UI*[ 1 ];
	for( int i = 0; i < 1; ++i )
	{
		pShader->Add_RenderObject( m_dpBtns[ i ] = CButton_UI::Create( pWnd_UI, CTextureMgr::GetInstance()->Clone( "Texture_Ready" ), XMFLOAT4( 0.08f + 0.67f, -0.23f - float( i ) * 0.3f, 0.21f, 0.06f ) ) );
	}
			

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
	delete[] m_dpBtns;
	m_dpBtns = nullptr;

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

	for( int i = 0; i < 1; ++i )
		if( m_dpBtns[ i ]->isCollide( ptMouse, !m_bOverlapped ) )
		{
		CNetworkMgr::GetInstance()->sendSelectCharacter( CHAMEL );
		if( CNetworkMgr::GetInstance()->isMaster() )
			CNetworkMgr::GetInstance()->sendStartRoom();
		else
			CNetworkMgr::GetInstance()->sendReadyRoom();
		break;
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

void CRoom::NotifyPlayerInfo( PlayerInfo* pPlayerInfo )
{
	m_pPlayerInfo = pPlayerInfo;
	if( CNetworkMgr::GetInstance()->isMaster() )
		m_dpBtns[ READY ]->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Start" ) );
}
