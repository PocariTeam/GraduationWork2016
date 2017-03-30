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
#include "Button_UI.h"
#include "Normal_UI.h"
#include "OrthoCamera.h"
#include "AnimateMeshMgr.h"
#include "AnimationMgr.h"
#include "ThreeD_UI.h"

CRoom::CRoom()
	: CScene()
	, m_bStart( false )
	, m_bOverlapped( true )
	, m_pPlayerInfo( nullptr )
	, m_dpReady( nullptr )
	, m_dpBtns( nullptr )
	, m_dwPlayerCnt( 0 )
	, m_pInputMgr( nullptr )
	, m_pNetworkMgr( nullptr )
	, m_pRenderer( nullptr )
	, m_iSelectCharacter( -1 )
{
}

CRoom::~CRoom()
{
}

HRESULT CRoom::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	CScene::Initialize( hWnd, pDevice );

	m_pNetworkMgr = CNetworkMgr::GetInstance();
	m_pRenderer = CRenderer::GetInstance();
	m_pInputMgr = CInputMgr::GetInstance();
	CTextureMgr*	pTextureMgr = CTextureMgr::GetInstance();

	m_pCamera = COrthoCamera::Create( hWnd, pDevice );

	// Background
	CShader* pShader = CShaderMgr::GetInstance()->Clone( "Shader_Background" );
	CTexture* pTexture = pTextureMgr->Clone( "Texture_Room" );
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Background" );
	pShader->Add_RenderObject( CWallpaper::Create( pDevice, pMesh, pTexture ) );
	m_listShader[ RENDER_BACKGROUND ].push_back( pShader );

	// Mesh
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );

	m_dpBtns = new CButton_UI*[ BTN_END ];
	m_dpReady = new CNormal_UI*[ PLAYER_CAPACITY ];

	pShader->Add_RenderObject( m_dpBtns[ BTN_READY ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Ready" ), XMFLOAT4( -1.f, -0.5f, 0.6f, 0.3f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_BACK ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Back" ), XMFLOAT4( -1.f, -0.8f, 0.6f, 0.2f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_MAP_NEXT ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Back" ), XMFLOAT4( -0.4f, 0.2f, 0.2f, 0.55f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_MAP_PREVIOUS ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Back" ), XMFLOAT4( 0.8f, 0.2f, 0.2f, 0.55f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_CHAMELEON ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Select_Chameleon" ), XMFLOAT4( -0.4f, -0.35f, 0.28f, 0.65f ) ) );
	pShader->Add_RenderObject( m_dpBtns[ BTN_MONKEY ] = CButton_UI::Create( pTextureMgr->Clone( "Texture_Select_Monkey" ), XMFLOAT4( -0.12f, -0.35f, 0.28f, 0.65f ) ) );
	
	pShader->Add_RenderObject( CNormal_UI::Create( pTextureMgr->Clone( "Texture_Slot" ), XMFLOAT4( -0.35f, 1.f, 0.45f, 0.8f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( pTextureMgr->Clone( "Texture_Slot" ), XMFLOAT4( 0.1f, 1.f, 0.45f, 0.8f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( pTextureMgr->Clone( "Texture_Slot" ), XMFLOAT4( 0.55f, 1.f, 0.45f, 0.8f ) ) );
	pShader->Add_RenderObject( CNormal_UI::Create( pTextureMgr->Clone( "Texture_Preview_Jungle" ), XMFLOAT4( -0.2f, 0.2f, 1.f, 0.55f ) ) );

	m_listShader[ RENDER_UI ].push_back( pShader );

	// 3D_UI
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_3D_UI" );
	
	pMesh = CAnimateMeshMgr::GetInstance()->Clone( "Mesh_Chameleon" );
	pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Chameleon" );
	CAnimator* pAnimator = CAnimationMgr::GetInstance()->Clone( CHARACTER_CHM );
	
	pShader->Add_RenderObject( CThreeD_UI::Create( pMesh, pTexture, pAnimator, XMFLOAT4( -0.73f, 0.1f, 0.0415f, 0.072f ) ) );
	m_listShader[ RENDER_UI ].push_back( pShader );

	/* Ready */
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );

	pShader->Add_RenderObject( m_dpReady[ 0 ] = CNormal_UI::Create( pTextureMgr->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( -0.95f, -0.2f, 0.5f, 0.3f ) ) );
	pShader->Add_RenderObject( m_dpReady[ 1 ] = CNormal_UI::Create( pTextureMgr->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( -0.3f, 0.45f, 0.35f, 0.2f ) ) );
	pShader->Add_RenderObject( m_dpReady[ 2 ] = CNormal_UI::Create( pTextureMgr->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( 0.15f, 0.45f, 0.35f, 0.2f ) ) );
	pShader->Add_RenderObject( m_dpReady[ 3 ] = CNormal_UI::Create( pTextureMgr->Clone( "Texture_Billboard_Ready" ), XMFLOAT4( 0.6f, 0.45f, 0.35f, 0.2f ) ) );

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

	m_pRenderer->Clear_RenderGroup();

	delete[] m_dpBtns;
	m_dpBtns = nullptr;

	delete[] m_dpReady;
	m_dpReady = nullptr;

	m_pRenderer = nullptr;
	m_pInputMgr = nullptr;
	m_pNetworkMgr = nullptr;

	delete this;

	return 0;
}

void CRoom::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	m_pRenderer->Render_Background( pContext );
	m_pRenderer->Render_UI( pContext );
}

int CRoom::Check_Key( void )
{
	POINT ptMouse = CScene::GetMousePosition( m_hWnd );

	if( ( m_pInputMgr->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) && m_bOverlapped )
		m_bOverlapped = false;

	else if( !( m_pInputMgr->Get_MouseState( CInputMgr::CLICK_LBUTTON ) & 0x80 ) )
		m_bOverlapped = true;

	for( int i = 0; i < BTN_END; ++i )
		if( m_dpBtns[ i ]->isCollide( ptMouse, !m_bOverlapped ) )
		{
			switch( i )
			{
			case BTN_READY:
				if( m_pNetworkMgr->isMaster() && -1 != m_iSelectCharacter )
					m_pNetworkMgr->sendStartRoom();
				else if( -1 != m_iSelectCharacter )
					m_pNetworkMgr->sendReadyRoom();
				break;
			case BTN_BACK:
				m_pNetworkMgr->sendExitRoom();
				return SCENE_LOBBY;
			case BTN_CHAMELEON:
				m_pNetworkMgr->sendSelectCharacter( CHAMEL );
				m_dpBtns[ BTN_MONKEY ]->Normal();
				m_dpBtns[ BTN_CHAMELEON ]->Fix();
				m_iSelectCharacter = CHARACTER_CHM;
				break;
			case BTN_MONKEY:
				m_pNetworkMgr->sendSelectCharacter( CHAMEL );
				m_dpBtns[ BTN_CHAMELEON ]->Normal();
				m_dpBtns[ BTN_MONKEY ]->Fix();
				m_iSelectCharacter = CHARACTER_CHM;
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
	for( int i = 0; i < PLAYER_CAPACITY; ++i )
		m_dpReady[ i ]->Hide();

	m_dwPlayerCnt = dwPlayerCnt;
	m_pPlayerInfo = pPlayerInfo;

	if( m_pNetworkMgr->isMaster() )
		m_dpBtns[ BTN_READY ]->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Start" ) );

	PlayerInfo* pTempArray = new PlayerInfo[ m_dwPlayerCnt ];

	for( UINT i = 0, j = 1; i < m_dwPlayerCnt; ++i )
	{
		if( m_pNetworkMgr->getID() == m_pPlayerInfo[ i ].id )
			memcpy_s( &pTempArray[ 0 ], sizeof( PlayerInfo ), &m_pPlayerInfo[ i ], sizeof( PlayerInfo ) );
		else
			memcpy_s( &pTempArray[ j++ ], sizeof( PlayerInfo ), &m_pPlayerInfo[ i ], sizeof( PlayerInfo ) );
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
		{
			m_dpReady[ i ]->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Billboard_Ready" ) );
			m_dpReady[ i ]->Show();
		}
	}

	delete[] pTempArray;
	pTempArray = nullptr;
}
