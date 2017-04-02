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
	// FIXME: �ӽ÷� �޾ƿͼ� ������ player ��ü�� ����Լ����� ó���ϸ� �� ���� �� �ϴ�.
	NxVec3	newDir;
	newDir.x = vDir.x;
	newDir.y = vDir.y;
	newDir.z = vDir.z;

	CPlayer* player = m_mapPlayer.find(id)->second;
	auto cct = player->GetCharacterController();
	newDir.normalize();
	if (false == newDir.isZero()) // ������ȯ
	{
		NxVec3 oldLook = cct->getActor()->getGlobalPose().M.getColumn(2);
		NxReal rotAngle = acos(oldLook.dot(newDir));
		NxVec3 cross = oldLook;
		cross = cross.cross(newDir);
		rotAngle *= (cross.y >= 0.0f) ? -1.0f : 1.0f;
		player->setRotateY(rotAngle);
	}

	//system_clock::time_point packetTick = system_clock::from_time_t(tick);
	//duration<double> lagTick = system_clock::now() - packetTick;
	//printf("�����ð�: %lf ��\n", lagTick.count());

	//NxU32	dwCollisionFlag;
	//// ������ �ٲ���� �ð�����ŭ �ǵ��ư���.
	//NxVec3 pastDir = player->m_vMoveDir * player->m_fSpeed * lagTick.count();
	////pastDir.y += -GRAVITY * GRAVITY * lagTick.count();
	//cct->move(-pastDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag);

	//// �ð�����ŭ �ٽ� ���� ��ġ�� ���ư���.
	//NxVec3 curDir = newDir * player->m_fSpeed * lagTick.count();
	////curDir.y += -GRAVITY * GRAVITY * lagTick.count();
	//cct->move(curDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag);
	
	player->m_vMoveDir = newDir;
	player->ChangeState(state);
	
	
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
