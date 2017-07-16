#include "stdafx.h"
#include "Normal_UI.h"
#include "Texture.h"

CNormal_UI::CNormal_UI()
	: CUserInterface()
	, m_bHide( false )
	, m_fSpeed( 0.f )
{
}

CNormal_UI::~CNormal_UI()
{
}

HRESULT CNormal_UI::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, float fSpeed )
{
	m_pTexture = pTexture;
	m_vPosSize_Dest = vPosSize;
	m_fSpeed = fSpeed;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );

	return S_OK;
}

CNormal_UI* CNormal_UI::Create( CTexture* pTexture, const XMFLOAT4& vPosSize, float fSpeed )
{
	CNormal_UI*	pNormal_UI = new CNormal_UI;

	if( FAILED( pNormal_UI->Initialize( pTexture, vPosSize, fSpeed ) ) )
	{
		pNormal_UI->Release();
		pNormal_UI = nullptr;
	}

	return pNormal_UI;
}

void CNormal_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_bHide ) return;
	
	m_pTexture->Render( pContext );
	pContext->Draw( 6, 0 );
}

int CNormal_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_fLerpTime == 1.f )
		return 0;

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

DWORD CNormal_UI::Release( void )
{
	if( 0 == CUserInterface::Release() )
		delete this;

	return 0;
}
