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

CJungle::CJungle()
	: CScene()
	, m_iPlayerID( -1 )
	, m_pPlayerInfo( nullptr )
	, m_dwPlayerCnt( 0 )
	, m_fAccTime( 180.f )
	, m_bOverlapped( true )
	, m_bDebug( true )
{
}

CJungle::~CJungle()
{
}

HRESULT CJungle::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pDevice = pDevice;
	m_hWnd = hWnd;

	m_pCamera = CDebugCamera::Create( hWnd, pDevice );
	CLightMgr::GetInstance()->Initialize( pDevice );
	CPhysics::GetInstance()->Load_Scene( pDevice, m_listShader, &m_mapPlayer, "../Executable/Resources/Scene/Jungle.xml" );
	m_iPlayerID = CNetworkMgr::GetInstance()->getID();

	CShader*	pShader = CShaderMgr::GetInstance()->Clone( "Shader_Number_UI" );
	m_dpTime_UI = new CNumber_UI*[ NUM_END ];
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_TEN ] = CNumber_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Number" ), XMFLOAT4( -0.1f, 0.9f, 0.05f, 0.1f ), 0 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_ONE ] = CNumber_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Number" ), XMFLOAT4( -0.05f, 0.9f, 0.05f, 0.1f ), 0 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_COLON ] = CNumber_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Number" ), XMFLOAT4( 0.f, 0.9f, 0.05f, 0.1f ), 10 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_PROCENT ] = CNumber_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Number" ), XMFLOAT4( 0.05f, 0.9f, 0.05f, 0.1f ), 0 ) );
	pShader->Add_RenderObject( m_dpTime_UI[ NUM_CENTI ] = CNumber_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Number" ), XMFLOAT4( 0.1f, 0.9f, 0.05f, 0.1f ), 0 ) );

	m_listShader[ RENDER_UI ].push_back( pShader );

	pShader = CShaderMgr::GetInstance()->Clone( "Shader_UI" );
	CGameObject* pTime = CNormal_UI::Create( CTextureMgr::GetInstance()->Clone( "Texture_Time" ), XMFLOAT4( -0.3f, 0.9f, 0.15f, 0.1f ) );
	pShader->Add_RenderObject( pTime );

	m_listShader[ RENDER_UI ].push_back( pShader );

	return CScene::Initialize( hWnd, pDevice );
}

void CJungle::AccumulateTime( const float& fTimeDelta )
{
	UINT	iInput = UINT( m_fAccTime );

	m_dpTime_UI[ NUM_TEN ]->SetNumber( iInput / 600 );
	m_dpTime_UI[ NUM_ONE ]->SetNumber( ( iInput / 60 ) % 10 );
	m_dpTime_UI[ NUM_PROCENT ]->SetNumber( ( iInput % 60 ) / 10 );
	m_dpTime_UI[ NUM_CENTI ]->SetNumber( iInput % 10 );

	if( iInput == 0 ) return; // 게임 종료
	m_fAccTime -= fTimeDelta;
}

int CJungle::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( -1 != m_iPlayerID )	m_mapPlayer.find( m_iPlayerID )->second->Check_Key( fTimeDelta );

	CPhysics::GetInstance()->Update( fTimeDelta );
	AccumulateTime( fTimeDelta );
	Check_Key( fTimeDelta );

	return 0;
}

DWORD CJungle::Release( void )
{
	m_mapPlayer.erase( m_mapPlayer.begin(), m_mapPlayer.end() );

	delete[] m_dpTime_UI;
	m_dpTime_UI = nullptr;

	CScene::Release();

	CGlobalState::DestroyInstance();
	CIdleState::DestroyInstance();
	CJumpState::DestroyInstance();
	CRunState::DestroyInstance();
	CAttackState::DestroyInstance();
	CAttackState2::DestroyInstance();
	CBeatenState::DestroyInstance();
	CBeatenState2::DestroyInstance();
	CDefendState::DestroyInstance();

	delete this;

	return 0;
}

void CJungle::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 1 );
	CPhysics::GetInstance()->Render( pContext );

	CRenderer::GetInstance()->Render( pContext );
}

void CJungle::Sync( UINT id, XMFLOAT3 vPos, float fRotateY, STATE state )
{
	auto find_iter = m_mapPlayer.find( id );
	if( find_iter == m_mapPlayer.end() ) return;
	
	NxVec3	vPosition{ vPos.x, vPos.y, vPos.z };

	find_iter->second->Sync( vPosition, fRotateY, state);
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
	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_SPACE ) && m_bOverlapped )
	{
		// 카메라 전환
		::Safe_Release( m_pCamera );
		m_bDebug = !m_bDebug;

		if( m_bDebug )
			m_pCamera = CDebugCamera::Create( m_hWnd, m_pDevice );
		else
			m_pCamera = CThirdCamera::Create( m_pDevice, m_mapPlayer[ CNetworkMgr::GetInstance()->getID() ]->GetCharacterController()->getActor(), XMFLOAT3( 0.f, 100.f, -200.f ) );
		
		m_bOverlapped = false;
	}

	else if( !CInputMgr::GetInstance()->Get_KeyboardState( DIK_SPACE ) )
		m_bOverlapped = true;
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
