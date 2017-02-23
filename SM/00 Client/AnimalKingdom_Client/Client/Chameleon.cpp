#include "stdafx.h"
#include "Chameleon.h"
#include "AnimateMeshMgr.h"
#include "TextureMgr.h"
#include "AnimationMgr.h"
#include "AnimationController.h"
#include "Player.h"

CChameleon::CChameleon()
	: CPlayer()
{
}

CChameleon::~CChameleon()
{
}

HRESULT CChameleon::Initialize( ID3D11Device* pDevice )
{
	CPlayer::Initialize( pDevice );

	// Mesh, Texture, AnimationController »ý¼º
	m_pMesh = CAnimateMeshMgr::GetInstance()->Clone( "Mesh_Chameleon" );
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Chameleon" );
	m_pAnimationController = CAnimationMgr::GetInstance()->Clone( CAnimationMgr::CHARACTER_CHM, this );

	return S_OK;
}

int CChameleon::Update( const float& fTimeDelta )
{
	m_pAnimationController->Update( fTimeDelta );

	return 0;
}

void CChameleon::Render( ID3D11DeviceContext* pContext )
{
	m_pAnimationController->Render( pContext );
	CPlayer::Render( pContext );
}

DWORD CChameleon::Release( void )
{
	CPlayer::Release();

	delete this;

	return 0;
}

CChameleon* CChameleon::Create( ID3D11Device* pDevice )
{
	CChameleon* pChameleon = new CChameleon;

	if ( FAILED( pChameleon->Initialize( pDevice ) ) )
	{
		pChameleon->Release();
		pChameleon = nullptr;
	}

	return pChameleon;
}
