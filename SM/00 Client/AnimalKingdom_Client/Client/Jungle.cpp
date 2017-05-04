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

CJungle::CJungle()
	: CScene()
	, m_iPlayerID( -1 )
	, m_pPlayerInfo( nullptr )
	, m_dwPlayerCnt( 0 )
	, m_fAccTime( 0.f )
{
}

CJungle::~CJungle()
{
}

HRESULT CJungle::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
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
	m_fAccTime += fTimeDelta;

	UINT	iInput = UINT( m_fAccTime * 100 );
	if( iInput > 9999 ) m_fAccTime = 0.f;

	m_dpTime_UI[ NUM_TEN ]->SetNumber( iInput / 1000 );
	m_dpTime_UI[ NUM_ONE ]->SetNumber( ( iInput % 1000 ) / 100 );
	m_dpTime_UI[ NUM_PROCENT ]->SetNumber( ( iInput % 100 ) / 10 );
	m_dpTime_UI[ NUM_CENTI ]->SetNumber( ( iInput % 100 ) % 10 );
}

int CJungle::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( -1 != m_iPlayerID )	m_mapPlayer.find( m_iPlayerID )->second->Check_Key( fTimeDelta );

	CPhysics::GetInstance()->Update( fTimeDelta );
	AccumulateTime( fTimeDelta );


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
