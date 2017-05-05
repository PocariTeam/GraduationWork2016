#include "stdafx.h"
#include "Chameleon.h"
#include "AnimateMeshMgr.h"
#include "TextureMgr.h"
#include "AnimationMgr.h"
#include "Animator.h"
#include "Player.h"

CChameleon::CChameleon()
	: CPlayer()
{
}

CChameleon::~CChameleon()
{
}

HRESULT CChameleon::Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose )
{
	CPlayer::Initialize( pDevice, pCharacterController, pActorOriginPose );

	// Mesh, Texture, AnimationController »ý¼º
	m_pMesh = CAnimateMeshMgr::GetInstance()->Clone( "Mesh_Chameleon" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Chameleon" );
	m_pAnimator = CAnimationMgr::GetInstance()->Clone( CHARACTER_CHM );
	m_dwActorCnt = 7;

	return S_OK;
}

int CChameleon::Update( const float& fTimeDelta )
{
	CPlayer::Update( fTimeDelta );

	return 0;
}

void CChameleon::Render( ID3D11DeviceContext* pContext )
{
	CPlayer::Render( pContext );
}

DWORD CChameleon::Release( void )
{
	CPlayer::Release();

	delete this;

	return 0;
}

void CChameleon::Attack( STATE eState )
{
	if( !m_bSweap ) return;

	NxActor*	pActor{};
	switch( eState )
	{
	case STATE_ATT1:
		pActor = ( ( NxActor** )m_pCharacterController->getUserData() )[ 1 ];
		break;
	case STATE_ATT2:
		pActor = ( ( NxActor** )m_pCharacterController->getUserData() )[ 3 ];
		break;
	default:
		return ;
	}

	NxSweepQueryHit result[ 100 ];
	UINT MyGroup = UINT( pActor->getGroup() );
	LPVOID pUserdata = LPVOID( LONGLONG( MyGroup ) );

	/*pActor->linearSweep( pActor->getShapes()[ 0 ]->getGlobalPosition(), NX_SF_DYNAMICS, pUserdata, 50, result,
		( NxUserEntityReport<NxSweepQueryHit>* )&CPhysics::m_EntityReport );*/

	/*NxSphere tRealShape;
	UINT ActiveGroup = ( COL_PLAYER1 | COL_PLAYER2 | COL_PLAYER3 | COL_PLAYER4 ) ^ MyGroup;
	pActor->getShapes()[ 0 ]->isSphere()->getWorldSphere( tRealShape );
	NxCapsule tColBody;
	tColBody.radius = tRealShape.radius;
	tColBody.p0 = tRealShape.center;
	tColBody.p1 = tRealShape.center;

	CPhysics::GetInstance()->GetScene()->linearCapsuleSweep( tColBody, pActor->getShapes()[ 0 ]->getGlobalPosition(), NX_SF_DYNAMICS, pUserdata, 50, result,
		( NxUserEntityReport<NxSweepQueryHit>* )&CPhysics::m_EntityReport, ActiveGroup );*/
}

CChameleon* CChameleon::Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose )
{
	CChameleon* pChameleon = new CChameleon;

	if ( FAILED( pChameleon->Initialize( pDevice, pCharacterController, pActorOriginPose ) ) )
	{
		pChameleon->Release();
		pChameleon = nullptr;
	}

	return pChameleon;
}
