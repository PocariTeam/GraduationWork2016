#include "stdafx.h"
#include "Window_UI.h"
#include "TextureMgr.h"

CWindow_UI::CWindow_UI()
	: CUserInterface()
{
}

CWindow_UI::~CWindow_UI()
{
}

HRESULT CWindow_UI::Initialize( WND_TYPE eWndType )
{
	switch( eWndType )
	{
	case WND_LOBBY:
		m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Wnd" );
		m_vPosSize_Dest = XMFLOAT4( -0.75f, 0.9f, 1.5f, 2.f );
		m_vPosSize_Src = XMFLOAT4( 0.05f, 0.05f, 0.f, 0.f );
		break;
	case WND_ROOM:
		m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Wnd" );
		m_vPosSize_Dest = XMFLOAT4( -0.75f, 0.9f, 1.5f, 2.f );
		m_vPosSize_Src = XMFLOAT4( 0.05f, 0.05f, 0.f, 0.f );
		break;
	}

	return S_OK;
}

CWindow_UI* CWindow_UI::Create( WND_TYPE eWndType )
{
	CWindow_UI*	pWindow_UI = new CWindow_UI;

	if( FAILED( pWindow_UI->Initialize( eWndType ) ) )
	{
		pWindow_UI->Release();
		pWindow_UI = nullptr;
	}

	return pWindow_UI;
}

void CWindow_UI::Render( ID3D11DeviceContext* pContext )
{
	CUserInterface::Render( pContext );
}

int CWindow_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_fLerpTime == 1.f )
		return 0;

	m_fLerpTime += 4.f * fTimeDelta;
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

DWORD CWindow_UI::Release( void )
{
	CUserInterface::Release();

	delete this;

	return 0;
}

XMFLOAT4 CWindow_UI::GetPosSize( void )
{
	XMVECTOR vLerp = DirectX::XMVectorLerp( XMLoadFloat4( &m_vPosSize_Src ), XMLoadFloat4( &m_vPosSize_Dest ), m_fLerpTime );
	XMFLOAT4 vLerp_Store;
	XMStoreFloat4( &vLerp_Store, vLerp );

	return vLerp_Store;
}
