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
#include "Physics.h"
#include <NxControllerManager.h>

CPlayer::CPlayer()
	: CGameObject()
	, m_pStateMachine( nullptr )
	, m_pAnimator( nullptr )
	, m_pCharacterController( nullptr )
	, m_dwActorCnt( 0 )
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
	NxVec3	vDir{ 0.f, 0.f, 0.f };

	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_UP ) & 0x80 )
	{
		m_pStateMachine->Change_State( STATE_RUN );
		vDir = NxVec3{ 0.f, 0.f, 1.f };
	}
		

	else if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_DOWN ) & 0x80 )
	{
		m_pStateMachine->Change_State( STATE_RUN );
		vDir = NxVec3{ 0.f, 0.f, -1.f };
	}
		

	else if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_LEFT ) & 0x80 )
	{
		m_pStateMachine->Change_State( STATE_RUN );
		vDir = NxVec3{ -1.f, 0.f, 0.f };
	}

	else if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_RIGHT ) & 0x80 )
	{
		m_pStateMachine->Change_State( STATE_RUN );
		vDir = NxVec3{ 1.f, 0.f, 0.f };
	}

	else
		m_pStateMachine->Change_State( STATE_IDLE );

	vDir.y += GRAVITY * -GRAVITY * fTimeDelta;

	NxU32	dwCollisionFlag;
	m_pCharacterController->move( vDir, COLLIDABLE_MASK, 0.0001f, dwCollisionFlag );
	CPhysics::m_pCharacterControllerMgr->updateControllers();
	NxExtendedVec3 vPos = m_pCharacterController->getPosition();
	NxActor** dpActorArray = ( NxActor** )m_pCharacterController->getUserData();
	for( int i = 0; i < 1; ++i )
		dpActorArray[ i ]->moveGlobalPosition( NxVec3( ( NxReal )vPos.x, ( NxReal )vPos.y, ( NxReal )vPos.z ) );

}

int CPlayer::Update( const float& fTimeDelta )
{
	Check_Key( fTimeDelta );
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
	::Safe_Release( m_pTexture );
	::Safe_Release( m_pMesh );

	for( DWORD i = 0; i < m_dwActorCnt; ++i )
	{
		delete ( ( NxActor** )m_pCharacterController->getUserData() )[ i ]->userData;
	}
	delete[]( ( NxActor** )m_pCharacterController->getUserData() );

	return 0;
}

XMFLOAT4X4 CPlayer::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	m_pCharacterController->getActor()->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	Out._24 += 8.f;
	return Out;
}

CPlayer* CPlayer::Create( ID3D11Device* pDevice, NxController* pCharacterController, CHARACTER eType )
{
	CPlayer* pPlayer{ nullptr };

	switch( eType )
	{
	case CHARACTER_CHM :
		pPlayer = CChameleon::Create( pDevice, pCharacterController );
		break;
	case CHARACTER_MON :
		break;
	}

	return pPlayer;
}

