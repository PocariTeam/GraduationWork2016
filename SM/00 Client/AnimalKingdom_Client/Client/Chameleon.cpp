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

HRESULT CChameleon::Initialize( ID3D11Device* pDevice, NxController* pCharacterController )
{
	CPlayer::Initialize( pDevice, pCharacterController );

	// Mesh, Texture, AnimationController ����
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

CChameleon* CChameleon::Create( ID3D11Device* pDevice, NxController* pCharacterController )
{
	CChameleon* pChameleon = new CChameleon;

	if ( FAILED( pChameleon->Initialize( pDevice, pCharacterController ) ) )
	{
		pChameleon->Release();
		pChameleon = nullptr;
	}

	return pChameleon;
}
