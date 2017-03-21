#include "stdafx.h"
#include "Button_UI.h"
#include "Window_UI.h"
#include "Value.h"
#include "Texture.h"

CButton_UI::CButton_UI()
	: m_pOwner( nullptr )
	, m_vOffset( 0.f, 0.f, 0.f, 0.f )
	, m_eState( BTN_NORMAL )
{
}


CButton_UI::~CButton_UI()
{
}

HRESULT CButton_UI::Initialize( CWindow_UI* pOwner, CTexture* pTexture, const XMFLOAT4& vOffset )
{
	m_pOwner = pOwner;
	m_pTexture = pTexture;
	m_vOffset = vOffset;

	return S_OK;
}

CButton_UI* CButton_UI::Create( CWindow_UI* pOwner, CTexture* pTexture, const XMFLOAT4& vOffset )
{
	CButton_UI*	pButton_UI = new CButton_UI;

	if( FAILED( pButton_UI->Initialize( pOwner, pTexture, vOffset ) ) )
	{
		pButton_UI->Release();
		pButton_UI = nullptr;
	}

	return pButton_UI;
}

void CButton_UI::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext, m_eState );
	pContext->Draw( 6, 0 );
}

int CButton_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	XMFLOAT4 vParent = m_pOwner->GetPosSize();

	m_mtxPosSize._11 = vParent.x + m_vOffset.x;
	m_mtxPosSize._21 = vParent.y + m_vOffset.y;
	m_mtxPosSize._31 = vParent.z * m_vOffset.z;
	m_mtxPosSize._41 = vParent.w * m_vOffset.w;

	return 0;
}

DWORD CButton_UI::Release( void )
{
	CUserInterface::Release();

	delete this;

	return 0;
}

bool CButton_UI::isCollide( POINT& ptMouse, bool bClick )
{
	if( m_eState == BTN_PLAYING ) return false;
	RECT	rcCollision;

	rcCollision.top = long( ( -1.f * m_mtxPosSize._21 + 1.f ) * ( float )g_wWinsizeY * 0.5f );
	rcCollision.left = long( ( m_mtxPosSize._11 + 1.f ) * ( float )g_wWinsizeX * 0.5f );
	rcCollision.bottom = long( ( -1.f *m_mtxPosSize._21 + 1.f + m_mtxPosSize._41 ) * ( float )g_wWinsizeY * 0.5f );
	rcCollision.right = long( ( m_mtxPosSize._11 + 1.f + m_mtxPosSize._31 ) * ( float )g_wWinsizeX * 0.5f );

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