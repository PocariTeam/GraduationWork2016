#include "stdafx.h"
#include "Player.h"
#include "Texture.h"
#include "AnimationMgr.h"
#include "Mesh.h"
#include "Function.h"
#include "Chameleon.h"
#include "Animator.h"
#include "StateMachine.h"
#include "InputMgr.h"
#include <NxVec3.h>
#include "Mathematics.h"
#include <NxControllerManager.h>
#include <NxQuat.h>
#include "NetworkMgr.h"

CPlayer::CPlayer()
	: CGameObject()
	, m_pStateMachine( nullptr )
	, m_pAnimator( nullptr )
	, m_pCharacterController( nullptr )
	, m_dwActorCnt( 0 )
	, m_vRotate( 0.f, 0.f, 0.f )
	, m_pInputMgr( CInputMgr::GetInstance() )
	, m_fSpeed( 50.f )
	, m_vDir( 0.f, 0.f, 0.f )
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Initialize( ID3D11Device* pDevice, NxController* pCharacterController )
{
	m_pCharacterController = pCharacterController;
	m_pActor = ( NxActor* )m_pCharacterController->getUserData();
	m_pStateMachine = CStateMachine::Create( this );

	return S_OK;
}

void CPlayer::Check_Key( const float& fTimeDelta )
{
	NxVec3 vDir{ 0.f, 0.f, 0.f };

	/*if( ( m_pInputMgr->Get_KeyboardState( DIK_UP ) )
		|| m_pInputMgr->Get_KeyboardState( DIK_DOWN )
		|| m_pInputMgr->Get_KeyboardState( DIK_LEFT )
		|| m_pInputMgr->Get_KeyboardState( DIK_RIGHT ) )
		eState = STATE_RUN;*/

	if( m_pInputMgr->Get_KeyboardState( DIK_UP ) )
		vDir += NxVec3{ 0.f, 0.f, 1.f };

	if( m_pInputMgr->Get_KeyboardState( DIK_DOWN ) )
		vDir += NxVec3{ 0.f, 0.f, -1.f };

	if( m_pInputMgr->Get_KeyboardState( DIK_LEFT ) )
		vDir += NxVec3{ -1.f, 0.f, 0.f };

	if( m_pInputMgr->Get_KeyboardState( DIK_RIGHT ) )
		vDir += NxVec3{ 1.f, 0.f, 0.f };

	STATE eState = m_pStateMachine->GetCurrentState();

	if( m_pInputMgr->Get_KeyboardState( DIK_S ) )
		eState = STATE_ATT1;
	else if( m_pInputMgr->Get_KeyboardState( DIK_D ) )
		eState = STATE_BEATEN1;
	else if( m_pInputMgr->Get_KeyboardState( DIK_A ) )
		eState = STATE_DEFEND;
	else if( m_pInputMgr->Get_KeyboardState( DIK_F ) )
		eState = STATE_JUMP;
	else if( false == vDir.isZero() )
		eState = STATE_RUN;
	else if( m_pAnimator->GetCurrentAnimationFinished() )
		eState = STATE_IDLE;

	bool bNeedToSendPacket = false;

	if( eState != m_pStateMachine->GetCurrentState() )
	{
		bNeedToSendPacket = true;
		m_pStateMachine->Change_State( eState );
	}

	vDir.normalize();
	if( m_vDir != vDir )
	{
		bNeedToSendPacket = true;
		m_vDir = vDir;
		if( false == vDir.isZero() )
		{
			NxVec3 oldLook = m_pCharacterController->getActor()->getGlobalPose().M.getColumn( 2 );
			NxReal rotAngle = acos( oldLook.dot( vDir ) );
			NxVec3 cross = oldLook;
			cross = cross.cross( vDir );
			rotAngle *= ( cross.y >= 0.0f ) ? -1.0f : 1.0f;
			m_vRotate.y = rotAngle;
		}
	}

	if( bNeedToSendPacket )
	{
		CNetworkMgr::GetInstance()->sendMoveCharacter( m_vDir, eState );
		// printf("[time:%d] 이동패킷 전송 \n", chrono::system_clock::now());
	}

}

void CPlayer::Jump( const float& fTimeDelta )
{
}

int CPlayer::Update( const float& fTimeDelta )
{
	NxVec3	vDir = m_vDir * m_fSpeed * fTimeDelta;
	vDir.y += -GRAVITY * GRAVITY * fTimeDelta;

	NxU32	dwCollisionFlag;
	m_pCharacterController->move( vDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
	m_pStateMachine->Update( fTimeDelta );

	return 0;
}

void CPlayer::Render( ID3D11DeviceContext* pContext )
{
	m_pAnimator->Render( pContext );
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CPlayer::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pAnimator );
	::Safe_Release( m_pStateMachine );

	for( DWORD i = 0; i < m_dwActorCnt; ++i )
		delete ( ( NxActor** )m_pCharacterController->getUserData() )[ i ]->userData;
	delete[]( ( NxActor** )m_pCharacterController->getUserData() );

	return 0;
}

XMFLOAT4X4 CPlayer::GetWorld()
{
	XMMATRIX mtxWorld;
	XMFLOAT4X4 mtxStoreWorld = CMathematics::ConvertToXMFloat4x4( &m_pCharacterController->getActor()->getGlobalPose() );
	mtxStoreWorld._24 += 2.f;
	mtxWorld = XMMatrixMultiply( XMLoadFloat4x4( &mtxStoreWorld ), XMMatrixRotationY( m_vRotate.y ) );

	XMFLOAT4X4 Out;

	XMStoreFloat4x4( &Out, mtxWorld );

	return Out;
}

void CPlayer::Move( NxVec3& vDir, STATE eState )
{
	NxVec3	vDefault_Dir{ 0.f, 0.f, 1.f };

	if( false == vDir.isZero() )
	{
		NxReal fRotateY = acos( vDefault_Dir.dot( vDir ) );
		NxVec3 vCross = vDefault_Dir;
		vCross = vCross.cross( vDir );
		fRotateY *= ( vCross.y >= 0.0f ) ? -1.0f : 1.0f;
		m_vRotate.y = fRotateY;
	}

	m_vDir = vDir;
	m_pStateMachine->Change_State( eState );
}

void CPlayer::Sync( NxVec3& vPos, float fRotateY )
{
	vPos.subtract( vPos, m_pCharacterController->getActor()->getGlobalPosition() );

	NxU32	dwCollisionFlag;
	m_pCharacterController->move( vPos, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
	m_vRotate.y = fRotateY;
}

CPlayer* CPlayer::Create( ID3D11Device* pDevice, NxController* pCharacterController, CHARACTER eType )
{
	CPlayer* pPlayer{ nullptr };

	switch( eType )
	{
	case CHARACTER_CHM:
		pPlayer = CChameleon::Create( pDevice, pCharacterController );
		break;
	case CHARACTER_MON:
		break;
	}

	return pPlayer;
}

