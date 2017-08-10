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
#include "ShaderMgr.h"
#include "TextureMgr.h"
#include "Number_UI.h"
#include "Normal_UI.h"
#include "InputMgr.h"
#include "ThirdCamera.h"
#include "MeshMgr.h"
#include "Section.h"
#include "Bar_UI.h"
#include "Sign_UI.h"
#include "StateMachine.h"
#include "EventCamera.h"
#include "ParticleMgr.h"

bool CJungle::m_bStart = false;

CJungle::CJungle()
	: CScene()
	, m_iPlayerID( -1 )
	, m_pPlayerInfo( nullptr )
	, m_dpSection( nullptr )
	, m_dwPlayerCnt( 0 )
	, m_bOverlapped( true )
	, m_bDebug( false )
	, m_pStateNotify( nullptr )
	, m_dpHP_Bar( nullptr )
	, m_iFocus( 0 )
	, m_bFinished( false )
	, m_dpCrownTime_UI( nullptr )
	, m_pCrownmark_UI( nullptr )
	, m_iCrownIndex( 4 )
{
}

CJungle::~CJungle()
{
}

HRESULT CJungle::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pDevice = pDevice;
	m_hWnd = hWnd;

	m_pCamera = CEventCamera::Create( pDevice, XMFLOAT3( 20.f, 130.f, 0.f ), XMFLOAT3( -50.f, 180.f, -200.f ), XMFLOAT3( 200.f, 260.f, -230.f ) );
	CNetworkMgr::GetInstance()->unreadyAllPlayer();

	CLightMgr::GetInstance()->Initialize( pDevice );
	CParticleMgr::GetInstance()->Initialize( pDevice );
	CPhysics::GetInstance()->Load_Scene( pDevice, m_listShader, &m_mapPlayer, "../Executable/Resources/Scene/Jungle.xml" );
	m_iPlayerID = CNetworkMgr::GetInstance()->getID();
	m_iFocus = m_iPlayerID;

	m_dwPlayerCnt = ( UINT )m_mapPlayer.size();

	// Section 1 ~ 3
	CMesh* pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Test" );
	CTexture* pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Test" );
	m_dpSection = new CSection*[ 3 ];
	m_dpSection[ 0 ] = CSection::Create( pDevice, pMesh, pTexture, XMFLOAT3( -20.f, 130.f, 360.f ), XMFLOAT3( 180.f, 45.f, 45.f ) );
	// m_listShader[ RENDER_DEPTHTEST ].front()->Add_RenderObject( m_dpSection[ 0 ] );
	pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Test" );
	pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Test" );
	m_dpSection[ 1 ] = CSection::Create( pDevice, pMesh, pTexture, XMFLOAT3( -170.f, 120.f, 265.f ), XMFLOAT3( 30.f, 25.f, 50.f ) );
	// m_listShader[ RENDER_DEPTHTEST ].front()->Add_RenderObject( m_dpSection[ 1 ] );
	pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Test" );
	pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Test" );
	m_dpSection[ 2 ] = CSection::Create( pDevice, pMesh, pTexture, XMFLOAT3( 130.f, 120.f, 265.f ), XMFLOAT3( 30.f, 25.f, 50.f ) );
	// m_listShader[ RENDER_DEPTHTEST ].front()->Add_RenderObject( m_dpSection[ 2 ] );

	auto player_iter = m_mapPlayer.begin();
	for( ; player_iter != m_mapPlayer.end(); ++player_iter )
		player_iter->second->SetSection( m_dpSection, 3 );

	// 머리 위의 ID
	player_iter = m_mapPlayer.begin();
	CShader*	pShader = CShaderMgr::GetInstance()->Clone( "Shader_Sign_UI" );
	for( ; player_iter != m_mapPlayer.end(); ++player_iter )
		pShader->Add_RenderObject( CSign_UI::Create( CTextureMgr::GetInstance()->Clone( ( player_iter->first == m_iPlayerID ) ? "Texture_ID2" : "Texture_ID3" ), player_iter->second->GetWorld(), player_iter->first ) );
	m_listShader[ RENDER_ALPHA ].push_front( pShader );

	// Timer UI
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_Number_UI_I" );
	m_dpTime_UI = new CNumber_UI*[ NUM_END ];
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_TEN ] = CNumber_UI::Create( CNumber_UI::NUMBER_TIME, XMFLOAT4( -0.1f, 0.9f, 0.05f, 0.1f ), 0 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_ONE ] = CNumber_UI::Create( CNumber_UI::NUMBER_TIME, XMFLOAT4( -0.05f, 0.9f, 0.05f, 0.1f ), 0 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_COLON ] = CNumber_UI::Create( CNumber_UI::NUMBER_TIME, XMFLOAT4( 0.f, 0.9f, 0.05f, 0.1f ), 10 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_PROCENT ] = CNumber_UI::Create( CNumber_UI::NUMBER_TIME, XMFLOAT4( 0.05f, 0.9f, 0.05f, 0.1f ), 0 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_CENTI ] = CNumber_UI::Create( CNumber_UI::NUMBER_TIME, XMFLOAT4( 0.1f, 0.9f, 0.05f, 0.1f ), 0 ) );
	m_listShader[ RENDER_UI ].push_back( pShader );

	// Crown Time UI
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_Number_UI_I" );
	m_dpCrownTime_UI = new CNumber_UI*[ NUM_END ];
	pShader->Add_RenderObject( m_dpCrownTime_UI[ NUM_TEN ] = CNumber_UI::Create( CNumber_UI::NUMBER_CROWN, XMFLOAT4( -0.775f, -0.62f, 0.035f, 0.07f ), 0 ) );
	pShader->Add_RenderObject( m_dpCrownTime_UI[ NUM_ONE ] = CNumber_UI::Create( CNumber_UI::NUMBER_CROWN, XMFLOAT4( -0.74f, -0.62f, 0.035f, 0.07f ), 0 ) );
	pShader->Add_RenderObject( m_dpCrownTime_UI[ NUM_COLON ] = CNumber_UI::Create( CNumber_UI::NUMBER_CROWN, XMFLOAT4( -0.705f, -0.62f, 0.035f, 0.07f ), 10 ) );
	pShader->Add_RenderObject( m_dpCrownTime_UI[ NUM_PROCENT ] = CNumber_UI::Create( CNumber_UI::NUMBER_CROWN, XMFLOAT4( -0.67f, -0.62f, 0.035f, 0.07f ), 0 ) );
	pShader->Add_RenderObject( m_dpCrownTime_UI[ NUM_CENTI ] = CNumber_UI::Create( CNumber_UI::NUMBER_CROWN, XMFLOAT4( -0.635f, -0.62f, 0.035f, 0.07f ), 0 ) );
	m_listShader[ RENDER_UI ].push_back( pShader );
	m_dpCrownTime_UI[ NUM_TEN ]->Hide();
	m_dpCrownTime_UI[ NUM_ONE ]->Hide();
	m_dpCrownTime_UI[ NUM_COLON ]->Hide();
	m_dpCrownTime_UI[ NUM_PROCENT ]->Hide();
	m_dpCrownTime_UI[ NUM_CENTI ]->Hide();

	// Time 글자
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );
	CGameObject* pTime = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Time" ), XMFLOAT4( -0.3f, 0.9f, 0.15f, 0.1f ) );
	pShader->Add_RenderObject( pTime );
	// Crown 그림
	m_pCrownmark_UI = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Crown_UI" ), XMFLOAT4( -0.88f, -0.55f, 0.1f, 0.15f ) );
	m_pCrownmark_UI->Hide();
	pShader->Add_RenderObject( m_pCrownmark_UI );

	// Status
	for( UINT i = 0; i < m_dwPlayerCnt; ++i )
	{
		CGameObject* pStatus = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Status" ), XMFLOAT4( -0.98f + i * 0.5f, -0.7f, 0.45f, 0.225f ) );
		pShader->Add_RenderObject( pStatus );
	}

	// HP Bar
	m_dpHP_Bar = new CBar_UI*[ m_dwPlayerCnt ];
	player_iter = m_mapPlayer.begin();

	for( UINT i = 0; i < m_dwPlayerCnt; ++i )
	{
		m_dpHP_Bar[ i ] = CBar_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_HP_Bar" ), XMFLOAT4( -0.83f + i * 0.5f, -0.77f, 0.285f, 0.079f ), player_iter->second->GetHP(), 100 );
		pShader->Add_RenderObject( m_dpHP_Bar[ i ] );
		player_iter++;
	}

	// Ready / Start / End 표시
	m_pStateNotify = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_RUReady" ), XMFLOAT4( -1.f, 1.f, 2.f, 2.f ) );
	pShader->Add_RenderObject( m_pStateNotify );
	m_listShader[ RENDER_UI ].push_back( pShader );

	// 체력바 옆 ID 표시
	/* ID */
	pShader = CShaderMgr::GetInstance()->Clone( "Shader_Number_UI" );
	player_iter = m_mapPlayer.begin();
	for( UINT i = 0; i < m_dwPlayerCnt; ++i )
	{
		if( player_iter->first == m_iPlayerID )
			pShader->Add_RenderObject( CNumber_UI::Create( CNumber_UI::NUMBER_MY_ID, XMFLOAT4( -0.985f + i * 0.5f, -0.74f, 0.15f, 0.15f ), m_iPlayerID ) );
		else
			pShader->Add_RenderObject( CNumber_UI::Create( CNumber_UI::NUMBER_OTHER_ID, XMFLOAT4( -0.985f + i * 0.5f, -0.74f, 0.15f, 0.15f ), player_iter->first ) );
		player_iter++;
	}
	m_listShader[ RENDER_UI ].push_back( pShader );

	CRenderer::GetInstance()->Copy_RenderGroup( m_listShader );
	CScene::Initialize( hWnd, pDevice );

	return S_OK;
}

void CJungle::SetPlayingTime( const float& fTime )
{
	UINT	iInput = UINT( m_fPlayingTime );

	m_dpTime_UI[ NUM_TEN ]->SetNumber( iInput / 600 );
	m_dpTime_UI[ NUM_ONE ]->SetNumber( ( iInput / 60 ) % 10 );
	m_dpTime_UI[ NUM_PROCENT ]->SetNumber( ( iInput % 60 ) / 10 );
	m_dpTime_UI[ NUM_CENTI ]->SetNumber( iInput % 10 );

	if( iInput == 0 ) return; // 게임 종료
	m_fPlayingTime = fTime;

	if( ( GAME_PLAYING_SEC - 1.0f ) > m_fPlayingTime && m_fPlayingTime > ( GAME_PLAYING_SEC - 2.0f ) ) m_pStateNotify->Hide();
}

void CJungle::SetWinningTime( const float & fTime )
{
	m_fWinningTime = fTime;

	UINT iInput = UINT( m_fWinningTime * 100 );
	if( iInput == 2000 )	return;

	m_dpCrownTime_UI[ NUM_TEN ]->SetNumber( iInput / 1000 );
	m_dpCrownTime_UI[ NUM_ONE ]->SetNumber( ( iInput % 1000 ) / 100 );
	m_dpCrownTime_UI[ NUM_PROCENT ]->SetNumber( ( iInput % 100 ) / 10 );
	m_dpCrownTime_UI[ NUM_CENTI ]->SetNumber( iInput % 10 );
}

void CJungle::Change_CrownUI_Position( void )
{
	if( nullptr == m_dpCrownTime_UI )	return;
	if( m_iCrownIndex == m_mapPlayer.size() )
	{
		m_dpCrownTime_UI[ NUM_TEN ]->Hide();
		m_dpCrownTime_UI[ NUM_ONE ]->Hide();
		m_dpCrownTime_UI[ NUM_COLON ]->Hide();
		m_dpCrownTime_UI[ NUM_PROCENT ]->Hide();
		m_dpCrownTime_UI[ NUM_CENTI ]->Hide();
		m_pCrownmark_UI->Hide();

		return;
	}
	else
	{
		m_dpCrownTime_UI[ NUM_TEN ]->SetPosition( XMFLOAT2( -0.775f + ( float )m_iCrownIndex * 0.5f, -0.62f ) );
		m_dpCrownTime_UI[ NUM_ONE ]->SetPosition( XMFLOAT2( -0.74f + ( float )m_iCrownIndex * 0.5f, -0.62f ) );
		m_dpCrownTime_UI[ NUM_COLON ]->SetPosition( XMFLOAT2( -0.705f + ( float )m_iCrownIndex * 0.5f, -0.62f ) );
		m_dpCrownTime_UI[ NUM_PROCENT ]->SetPosition( XMFLOAT2( -0.67f + ( float )m_iCrownIndex * 0.5f, -0.62f ) );
		m_dpCrownTime_UI[ NUM_CENTI ]->SetPosition( XMFLOAT2( -0.635f + ( float )m_iCrownIndex * 0.5f, -0.62f ) );
		m_pCrownmark_UI->SetPosition( XMFLOAT2( -0.88f + ( float )m_iCrownIndex * 0.5f, -0.55f ) );

		m_dpCrownTime_UI[ NUM_TEN ]->Show();
		m_dpCrownTime_UI[ NUM_ONE ]->Show();
		m_dpCrownTime_UI[ NUM_COLON ]->Show();
		m_dpCrownTime_UI[ NUM_PROCENT ]->Show();
		m_dpCrownTime_UI[ NUM_CENTI ]->Show();
		m_pCrownmark_UI->Show();
	}
}

int CJungle::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	CParticleMgr::GetInstance()->Update( fTimeDelta );

	auto focus_iter = m_mapPlayer.find( m_iFocus );
	if( m_bStart )
	{
		CPhysics::GetInstance()->Update( fTimeDelta );
		m_mapPlayer[ m_iPlayerID ]->Check_Key( fTimeDelta );
		if( focus_iter != m_mapPlayer.end() )
		{
			if( focus_iter->second->GetAlpha() )
				CRenderer::GetInstance()->InCave();
			else
				CRenderer::GetInstance()->OutCave();
		}
	}

	if( focus_iter != m_mapPlayer.end() && STATE_DEAD == focus_iter->second->GetFSM()->GetCurrentState() && !m_bDebug )
		Change_CameraDest();

	Check_Key( fTimeDelta );

	if( m_bFinished )
		return SCENE_ROOM;

	return 0;
}

DWORD CJungle::Release( void )
{
	m_bStart = false;
	CRenderer::GetInstance()->OutCave();
	m_mapPlayer.erase( m_mapPlayer.begin(), m_mapPlayer.end() );

	delete[] m_dpTime_UI;
	m_dpTime_UI = nullptr;

	delete[] m_dpCrownTime_UI;
	m_dpCrownTime_UI = nullptr;

	for( int i = 0; i < 3; ++i )
		m_dpSection[ i ]->Release();
	delete[] m_dpSection;
	m_dpSection = nullptr;

	delete[] m_dpHP_Bar;
	m_dpHP_Bar = nullptr;

	CScene::Release();

	CParticleMgr::DestroyInstance();
	CLightMgr::DestroyInstance();
	CPhysics::GetInstance()->Release_Scene();

	delete this;

	return 0;
}

void CJungle::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 1 );

	CRenderer::GetInstance()->Render( pContext );
	if( CRenderer::GetInstance()->m_bWireFrame ) CPhysics::GetInstance()->Render( pContext );
}

void CJungle::Sync( UINT id, int hp, XMFLOAT3 vPos, float fRotateY, STATE state )
{
	auto find_iter = m_mapPlayer.find( id );
	if( find_iter == m_mapPlayer.end() ) return;

	NxVec3	vPosition{ vPos.x, vPos.y, vPos.z };

	find_iter->second->Sync( vPosition, hp, fRotateY, state );
}

void CJungle::NotifyPlayerInfo( PlayerInfo* pPlayerInfo, UINT& dwPlayerCnt )
{
	m_pPlayerInfo = pPlayerInfo;

	int* pTemp = new int[ dwPlayerCnt ];
	m_dwPlayerCnt = dwPlayerCnt;

	for( UINT i = 0; i < m_dwPlayerCnt; ++i )
		pTemp[ i ] = ( int )m_pPlayerInfo[ i ].id;

	sort( &pTemp[ 0 ], &pTemp[ dwPlayerCnt - 1 ] );

	auto iter_begin = m_mapPlayer.begin();
	for( UINT i = 0; i < m_dwPlayerCnt; ++i, ++iter_begin )
		if( iter_begin->first != pTemp[ i ] )
		{
			iter_begin = m_mapPlayer.erase( iter_begin );
			break;
		}
	delete[] pTemp;
	pTemp = nullptr;
}

void CJungle::Check_Key( const float& fTimeDelta )
{
	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_RETURN ) && m_bOverlapped )
	{
		// 카메라 전환
		::Safe_Release( m_pCamera );
		m_bDebug = !m_bDebug;

		if( m_bDebug )
			m_pCamera = CDebugCamera::Create( m_hWnd, m_pDevice );
		else
			m_pCamera = CThirdCamera::Create( m_pDevice, m_mapPlayer[ m_iFocus ]->GetWorld(), XMFLOAT3( 0.f, 100.f, -200.f ) );

		m_bOverlapped = false;
	}

	else if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_SPACE ) && m_bOverlapped
		&& STATE_DEAD == m_mapPlayer[ m_iPlayerID ]->GetCurrentState() && !m_bDebug )
	{
		Change_CameraDest();
		m_bOverlapped = false;
	}

	else if( !CInputMgr::GetInstance()->Get_KeyboardState( DIK_RETURN )
		&& !CInputMgr::GetInstance()->Get_KeyboardState( DIK_SPACE ) )
		m_bOverlapped = true;
}

void CJungle::Change_CameraDest( void )
{
	auto player_iter = m_mapPlayer.find( m_iFocus );
	if( player_iter == m_mapPlayer.end() ) return;

	auto advance_iter = player_iter;
	advance_iter++;

	int iFocus{ m_iFocus };

	if( advance_iter != m_mapPlayer.end() )
	{
		if( STATE_DEAD != advance_iter->second->GetCurrentState() )
			iFocus = advance_iter->first;
		else
		{
			while( ++advance_iter != m_mapPlayer.end() )
				if( STATE_DEAD != advance_iter->second->GetCurrentState() )
				{
					iFocus = advance_iter->first;
					break;
				}

			if( advance_iter == m_mapPlayer.end() )
			{
				advance_iter = m_mapPlayer.begin();

				for( ; advance_iter != player_iter; ++advance_iter )
					if( STATE_DEAD != advance_iter->second->GetCurrentState() )
					{
						iFocus = advance_iter->first;
						break;
					}
			}
		}
	}

	else
	{
		advance_iter = m_mapPlayer.begin();

		for( ; advance_iter != player_iter; ++advance_iter )
			if( STATE_DEAD != advance_iter->second->GetCurrentState() )
			{
				iFocus = advance_iter->first;
				break;
			}
	}

	if( m_iFocus != iFocus )
	{
		m_iFocus = iFocus;
		( ( CThirdCamera* )m_pCamera )->SetDestWorldTranspose( m_mapPlayer.find( m_iFocus )->second->GetWorld() );
	}
}

void CJungle::NotifyGameStart( void )
{
	m_pStateNotify->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Go" ) );
	m_bStart = true;
	::Safe_Release( m_pCamera );
	m_pCamera = CThirdCamera::Create( m_pDevice, m_mapPlayer[ CNetworkMgr::GetInstance()->getID() ]->GetWorld(), XMFLOAT3( 0.f, 100.f, -200.f ) );
}

void CJungle::NotifyGameFinished()
{
	m_bFinished = true;
}

void CJungle::NotifyWinner( UINT ID )
{
	CPhysics::GetInstance()->SetCrownOwner( m_mapPlayer[ ID ] );

	m_iFocus = ID;
	::Safe_Release( m_pCamera );
	m_bDebug = false;
	m_pCamera = CThirdCamera::Create( m_pDevice, m_mapPlayer[ m_iFocus ]->GetWorld(), XMFLOAT3( 0.f, 100.f, -200.f ) );
	( ( CThirdCamera* )m_pCamera )->SetDestWorldTranspose( m_mapPlayer[ m_iFocus ]->GetWorld() );
	( ( CThirdCamera* )m_pCamera )->WinnerEvent();
	m_pStateNotify->Show();
	m_pStateNotify->SetTexture( CTextureMgr::GetInstance()->Clone( "Texture_Winner" ) );
}

void CJungle::NotifyCrownOwner( UINT ID )
{
	CPlayer *owner = nullptr;
	auto find_iter = m_mapPlayer.find( ID );
	int iCrownIndex = int( distance( m_mapPlayer.begin(), find_iter ) );

	if( m_iCrownIndex != iCrownIndex )
	{
		m_iCrownIndex = iCrownIndex;
		Change_CrownUI_Position();
	}

	if( find_iter != m_mapPlayer.end() ) {
		owner = find_iter->second;
	}

	CPhysics::GetInstance()->SetCrownOwner( owner );
}

void CJungle::NotifyUseSkill(UINT ID, BOOL use)
{
	auto find_iter = m_mapPlayer.find(ID);

	find_iter->second->SetSkillOn(use);

	printf("%d 플레이어 스킬사용 : %d \n", ID, use);
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
