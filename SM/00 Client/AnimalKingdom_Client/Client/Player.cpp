#include "stdafx.h"
#include "Player.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"
#include "Chameleon.h"
#include "AnimationController.h"

CPlayer::CPlayer()
	: CGameObject()
	, m_pAnimationController( nullptr )
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Initialize( ID3D11Device* pDevice )
{
	return S_OK;
}

void CPlayer::Check_Key( const float& fTimeDelta )
{

}

int CPlayer::Update( const float& fTimeDelta )
{
	return 0;
}

void CPlayer::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CPlayer::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pAnimationController );
	::Safe_Release( m_pTexture );
	::Safe_Release( m_pMesh );

	return 0;
}

CPlayer* CPlayer::Create( ID3D11Device* pDevice, CAnimationMgr::CHARACTER_TYPE eType )
{
	CPlayer* pPlayer{ nullptr };

	switch( eType )
	{
	case CAnimationMgr::CHARACTER_CHM :
		pPlayer = CChameleon::Create( pDevice );
		break;
	case CAnimationMgr::CHARACTER_MON :
		break;
	}

	return pPlayer;
}

