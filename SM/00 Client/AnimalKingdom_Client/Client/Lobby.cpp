#include "stdafx.h"
#include "Lobby.h"
#include "InputMgr.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "UserInterface.h"
#include "NetworkMgr.h"
#include "Value.h"

CLobby::CLobby()
	: CScene()
	, m_pShader( nullptr )
{
}

CLobby::~CLobby()
{
}

HRESULT CLobby::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );

	for( int i = 0; i < 3; ++i )
		for( int j = 0; j < 2; ++j )
		{
			m_pShader->Add_RenderObject( CUserInterface::Create( CTextureMgr::GetInstance()->Clone( "Texture_L_Room" ), XMFLOAT4( -0.6f + float( j ) * 0.7f, 0.8f - float( i ) * 0.3f, 0.6f, 0.2f ), 0.f ) );
			m_rcRoom[ i * 2 + j ].top = long( ( -1.f * ( 0.8f - float( i ) * 0.3f ) + 1.f ) * ( float )g_wWinsizeY * 0.5f );
			m_rcRoom[ i * 2 + j ].left = long( ( -0.6f + float( j ) * 0.7f + 1.f ) * ( float )g_wWinsizeX * 0.5f );
			m_rcRoom[ i * 2 + j ].bottom = long( ( -1.f * ( 0.8f - float( i ) * 0.3f ) + 1.f ) * ( float )g_wWinsizeY * 0.5f + 0.2f * 0.5f * ( float )g_wWinsizeY );
			m_rcRoom[ i * 2 + j ].right = long( ( -0.6f + float( j ) * 0.7f + 1.f ) * ( float )g_wWinsizeX * 0.5f + 0.6f * 0.5f * ( float )g_wWinsizeX );
		}


	return CScene::Initialize( hWnd, pDevice );
}

int CLobby::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );

	if( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 )
	{
		POINT ptMouse = CScene::GetMousePosition( m_hWnd );
		for( int i = 0; i < MAX_ROOM_CNT; ++i )
		{
			if( PtInRect( &m_rcRoom[ i ], ptMouse ) )
			{
				CNetworkMgr::GetInstance()->sendEnterRoom( i );
				return CScene::SCENE_JUNGLE;
			}
		}
	}
	

	return 0;
}

DWORD CLobby::Release( void )
{
	CScene::Release();

	::Safe_Release( m_pShader );

	delete this;

	return 0;
}

void CLobby::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	m_pShader->Render( pContext );
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