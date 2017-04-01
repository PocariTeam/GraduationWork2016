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
	if( -1 != m_iPlayerID )
		m_mapPlayer.find( m_iPlayerID )->second->Check_Key( fTimeDelta );
		
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
	CBeatenState::DestroyInstance();
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

void CJungle::Move(UINT32 id, time_t tick, XMFLOAT3 vDir, STATE state)
{
	// FIXME: 임시로 받아와서 쓰지만 player 자체의 멤버함수에서 처리하면 더 좋을 듯 하다.

	NxU32	dwCollisionFlag;
	NxVec3	dir;
	dir.x = vDir.x;
	dir.y = vDir.y;
	dir.z = vDir.z;

	//time_t difference = chrono::system_clock::to_time_t(chrono::system_clock::now()) - tick;
	//dir *= difference; 일단 틱계산을 안 해보자..
	auto cct = m_mapPlayer.find(id)->second->GetCharacterController();

	dir.normalize();
	if (false == dir.isZero())
	{
		NxVec3 oldLook = cct->getActor()->getGlobalPose().M.getColumn(2);
		NxReal rotAngle = acos(oldLook.dot(dir));
		NxVec3 cross = oldLook;
		cross = cross.cross(dir);
		rotAngle *= (cross.y >= 0.0f) ? -1.0f : 1.0f;
		m_mapPlayer.find(id)->second->setRotateY(rotAngle);
	}
	m_mapPlayer.find(id)->second->m_vMoveDir = dir;
	m_mapPlayer.find(id)->second->ChangeState(state);
	
	
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
