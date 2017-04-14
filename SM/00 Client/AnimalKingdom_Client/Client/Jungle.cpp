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

CJungle::CJungle()
	: CScene()
	, m_iPlayerID( -1 )
	, m_pPlayerInfo( nullptr )
	, m_dwPlayerCnt( 0 )
{
}

CJungle::~CJungle()
{
}

HRESULT CJungle::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_pCamera = CDebugCamera::Create( hWnd, pDevice );
	CLightMgr::GetInstance()->Initialize( pDevice );
	CPhysics::GetInstance()->Load_Scene( pDevice, m_listShader, &m_mapPlayer, "../Executable/Resources/Scene/Jungle_player4.xml" );
	m_iPlayerID = CNetworkMgr::GetInstance()->getID();

	return CScene::Initialize( hWnd, pDevice );
}

int CJungle::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );
	if( -1 != m_iPlayerID )	m_mapPlayer.find( m_iPlayerID )->second->Check_Key( fTimeDelta );

	CPhysics::GetInstance()->Update( fTimeDelta );

	return 0;
}

DWORD CJungle::Release( void )
{
	m_mapPlayer.erase( m_mapPlayer.begin(), m_mapPlayer.end() );

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
