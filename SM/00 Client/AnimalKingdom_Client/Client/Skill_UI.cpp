#include "stdafx.h"
#include "Skill_UI.h"
#include "Texture.h"

CSkill_UI::CSkill_UI()
	: CUserInterface()
	, m_bHide( false )
	, m_pData( nullptr )
	, m_fSpeed( 0.f )
	, m_eCharacterType( CHARACTER_CHM )
{
}

CSkill_UI::~CSkill_UI()
{
}

HRESULT CSkill_UI::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, float* pData, CHARACTER eType, float fSpeed )
{
	m_pTexture = pTexture;
	m_vPosSize_Dest = vPosSize;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );
	m_pData = pData;
	m_fSpeed = fSpeed;
	m_eCharacterType = eType;

	return S_OK;
}

CSkill_UI* CSkill_UI::Create( CTexture* pTexture, const XMFLOAT4& vPosSize, float* pData, CHARACTER eType, float fSpeed )
{
	CSkill_UI*	pSkill_UI = new CSkill_UI;

	if( FAILED( pSkill_UI->Initialize( pTexture, vPosSize, pData, eType, fSpeed ) ) )
	{
		pSkill_UI->Release();
		pSkill_UI = nullptr;
	}

	return pSkill_UI;
}

void CSkill_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_bHide ) return;

	m_pTexture->Render( pContext, m_eCharacterType );
	pContext->Draw( 6, 0 );
}

int CSkill_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_fLerpTime == 1.f )
	{
		m_vOption.w = *m_pData;
		return 0;
	}

	m_fLerpTime += m_fSpeed * fTimeDelta;
	if( m_fLerpTime > 1.f )	m_fLerpTime = 1.f;

	XMVECTOR vLerp = DirectX::XMVectorLerp( XMLoadFloat4( &m_vPosSize_Src ), XMLoadFloat4( &m_vPosSize_Dest ), m_fLerpTime );
	XMFLOAT4 vLerp_Store;
	XMStoreFloat4( &vLerp_Store, vLerp );

	m_mtxWorld._11 = vLerp_Store.x;
	m_mtxWorld._21 = vLerp_Store.y;
	m_mtxWorld._31 = vLerp_Store.z;
	m_mtxWorld._41 = vLerp_Store.w;

	return 0;
}

DWORD CSkill_UI::Release( void )
{
	if( 0 == CUserInterface::Release() )
		delete this;

	return 0;
}
