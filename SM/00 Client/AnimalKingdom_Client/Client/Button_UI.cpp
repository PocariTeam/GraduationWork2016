#include "stdafx.h"
#include "Button_UI.h"
#include "Window_UI.h"
#include "Value.h"

CButton_UI::CButton_UI()
	: m_pOwner( nullptr )
	, m_vOffset( 0.f, 0.f, 0.f, 0.f )
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
	CUserInterface::Render( pContext );
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

RECT CButton_UI::GetCollisionRect( void )
{
	RECT	rcCollision;

	rcCollision.top = long( ( -1.f * m_mtxPosSize._21 + 1.f ) * ( float )g_wWinsizeY * 0.5f );
	rcCollision.left = long( ( m_mtxPosSize._11 + 1.f ) * ( float )g_wWinsizeX * 0.5f );
	rcCollision.bottom = long( ( -1.f *m_mtxPosSize._21 + 1.f + m_mtxPosSize._41 ) * ( float )g_wWinsizeY * 0.5f );
	rcCollision.right = long( ( m_mtxPosSize._11 + 1.f + m_mtxPosSize._31 ) * ( float )g_wWinsizeX * 0.5f );

	return rcCollision;
}
