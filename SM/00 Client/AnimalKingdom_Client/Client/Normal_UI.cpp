#include "stdafx.h"
#include "Normal_UI.h"
#include "Texture.h"


CNormal_UI::CNormal_UI()
	:m_bHide( false )
{
}


CNormal_UI::~CNormal_UI()
{
}

HRESULT CNormal_UI::Initialize( CTexture* pTexture, const XMFLOAT4& vOffset )
{
	m_pTexture = pTexture;
	m_vPosSize_Dest = vOffset;
	m_vPosSize_Src = XMFLOAT4( vOffset.x + vOffset.z * 0.5f, vOffset.y - vOffset.w * 0.5f, 0.f, 0.f );

	return S_OK;
}

CNormal_UI* CNormal_UI::Create( CTexture* pTexture, const XMFLOAT4& vOffset )
{
	CNormal_UI*	pNormal_UI = new CNormal_UI;

	if( FAILED( pNormal_UI->Initialize( pTexture, vOffset ) ) )
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

	m_fLerpTime += 2.f * fTimeDelta;
	if( m_fLerpTime > 1.f )	m_fLerpTime = 1.f;

	XMVECTOR vLerp = DirectX::XMVectorLerp( XMLoadFloat4( &m_vPosSize_Src ), XMLoadFloat4( &m_vPosSize_Dest ), m_fLerpTime );
	XMFLOAT4 vLerp_Store;
	XMStoreFloat4( &vLerp_Store, vLerp );

	m_mtxPosSize._11 = vLerp_Store.x;
	m_mtxPosSize._21 = vLerp_Store.y;
	m_mtxPosSize._31 = vLerp_Store.z;
	m_mtxPosSize._41 = vLerp_Store.w;

	return 0;
}

DWORD CNormal_UI::Release( void )
{
	CUserInterface::Release();

	delete this;

	return 0;
}
