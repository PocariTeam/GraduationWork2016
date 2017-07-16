#include "stdafx.h"
#include "Button_UI.h"
#include "Value.h"
#include "Texture.h"

CButton_UI::CButton_UI()
	: m_eState( BTN_NORMAL )
{
}


CButton_UI::~CButton_UI()
{
}

HRESULT CButton_UI::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize )
{
	m_pTexture = pTexture;
	m_vPosSize_Dest = vPosSize;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );

	return S_OK;
}

CButton_UI* CButton_UI::Create( CTexture* pTexture, const XMFLOAT4& vPosSize )
{
	CButton_UI*	pButton_UI = new CButton_UI;

	if( FAILED( pButton_UI->Initialize( pTexture, vPosSize ) ) )
	{
		pButton_UI->Release();
		pButton_UI = nullptr;
	}

	return pButton_UI;
}

void CButton_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_eState == BTN_HIDE ) return;

	if( m_eState == BTN_FIX )
		m_pTexture->Render( pContext, BTN_ACTIVE );
	else
		m_pTexture->Render( pContext, m_eState );
	pContext->Draw( 6, 0 );
}

int CButton_UI::Update( const float& fTimeDelta )
{
	if( m_fLerpTime == 1.f )
		return 0;

	m_fLerpTime += 2.f * fTimeDelta;
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

DWORD CButton_UI::Release( void )
{
	if( 0 == CUserInterface::Release() )
		delete this;

	return 0;
}

bool CButton_UI::isCollide( POINT& ptMouse, bool bClick )
{
	if( m_eState == BTN_DISABLE
		|| m_eState == BTN_HIDE
		|| m_eState == BTN_FIX ) return false;

	RECT	rcCollision;

	rcCollision.top = long( ( -1.f * m_mtxWorld._21 + 1.f ) * ( float )g_wWinsizeY * 0.5f );
	rcCollision.left = long( ( m_mtxWorld._11 + 1.f ) * ( float )g_wWinsizeX * 0.5f );
	rcCollision.bottom = long( ( -1.f *m_mtxWorld._21 + 1.f + m_mtxWorld._41 ) * ( float )g_wWinsizeY * 0.5f );
	rcCollision.right = long( ( m_mtxWorld._11 + 1.f + m_mtxWorld._31 ) * ( float )g_wWinsizeX * 0.5f );

	if( PtInRect( &rcCollision, ptMouse ) )
	{
		if( bClick ) m_eState = BTN_PUSH;

		else
		{
			if( m_eState == BTN_PUSH )
			{
				m_eState = BTN_ACTIVE;
				return true;
			}

			m_eState = BTN_ACTIVE;
		}
	}

	else
		m_eState = BTN_NORMAL;

	return false;
}