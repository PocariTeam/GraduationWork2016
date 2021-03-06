#include "stdafx.h"
#include "Monkey.h"
#include "AnimateMeshMgr.h"
#include "TextureMgr.h"
#include "AnimationMgr.h"
#include "Animator.h"
#include "Mathematics.h"
#include "Player.h"

CMonkey::CMonkey()
	: CPlayer()
{
}

CMonkey::~CMonkey()
{
}

HRESULT CMonkey::Initialize( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose )
{
	CPlayer::Initialize( pDevice, pCharacterController, pActorOriginPose );

	// Mesh, Texture, AnimationController ����
	m_pMesh = CAnimateMeshMgr::GetInstance()->Clone( "Mesh_Monkey" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Monkey" );
	m_pAnimator = CAnimationMgr::GetInstance()->Clone( CHARACTER_MON );
	m_dwActorCnt = 3;

	return S_OK;
}

int CMonkey::Update( const float& fTimeDelta )
{
	CPlayer::Update( fTimeDelta );

	return 0;
}

void CMonkey::Render( ID3D11DeviceContext* pContext )
{
	CPlayer::Render( pContext );
}

DWORD CMonkey::Release( void )
{
	CPlayer::Release();

	delete this;

	return 0;
}

DirectX::XMFLOAT4X4 CMonkey::GetWorld()
{
	XMMATRIX mtxWorld;
	XMFLOAT4X4 mtxStoreWorld = CMathematics::ConvertToXMFloat4x4( &m_pCharacterController->getActor()->getGlobalPose() );
	mtxStoreWorld._24 -= 6.8f;
	mtxWorld = XMMatrixMultiply( XMLoadFloat4x4( &mtxStoreWorld ), XMMatrixRotationY( m_vRotate.y ) * XMMatrixScaling( 21.209435f, 21.209435f, 21.209435f ) );

	XMFLOAT4X4 Out;

	XMStoreFloat4x4( &Out, mtxWorld );

	return Out;
}

void CMonkey::Create_Banana()
{
	XMFLOAT4X4	mtxWorld = GetWorld();
	XMVECTOR vDir = XMVector3Normalize( XMLoadFloat3( &XMFLOAT3( mtxWorld._13, mtxWorld._23, mtxWorld._33 ) ) );
	XMFLOAT3 vNormalDir;
	XMStoreFloat3( &vNormalDir, vDir );
	CPhysics::GetInstance()->CreateBanana( NxVec3( mtxWorld._14 + vNormalDir.x * 10.f, mtxWorld._24 + vNormalDir.y * 10.f, mtxWorld._34 + vNormalDir.z * 10.f ), NxVec3( vNormalDir.x, vNormalDir.y, vNormalDir.z ), static_cast<COL_GROUP>( m_pCharacterController->getActor()->getGroup() ) );
}

CMonkey* CMonkey::Create( ID3D11Device* pDevice, NxController* pCharacterController, NxMat34* pActorOriginPose )
{
	CMonkey* pChameleon = new CMonkey;

	if( FAILED( pChameleon->Initialize( pDevice, pCharacterController, pActorOriginPose ) ) )
	{
		pChameleon->Release();
		pChameleon = nullptr;
	}

	return pChameleon;
}
