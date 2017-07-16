#include "stdafx.h"
#include "Sign_UI.h"
#include "Texture.h"

CSign_UI::CSign_UI()
	: CUserInterface()
	, m_bHide( false )
	, m_pDestWorld( nullptr )
	, m_fOffset( 30.f )
	, m_iValue( 999 )
{
}

CSign_UI::~CSign_UI()
{
}

HRESULT CSign_UI::Initialize( CTexture* pTexture, XMFLOAT4X4* pDest, UINT iID )
{
	m_pTexture = pTexture;
	m_pDestWorld = pDest;
	SetNumber( iID );

	return S_OK;
}

CSign_UI* CSign_UI::Create( CTexture* pTexture, XMFLOAT4X4* pDest, UINT iID )
{
	CSign_UI*	pSign_UI = new CSign_UI;

	if( FAILED( pSign_UI->Initialize( pTexture, pDest, iID ) ) )
	{
		pSign_UI->Release();
		pSign_UI = nullptr;
	}

	return pSign_UI;
}

void CSign_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_bHide ) return;

	m_pTexture->Render( pContext );
	pContext->Draw( 6, 0 );
}

int CSign_UI::Update( const float& fTimeDelta )
{
	if( m_bHide ) return 0;

	XMMATRIX mtxWorld = XMMatrixScaling( 10.f, 10.f, 10.f) * XMMatrixTranslation( m_pDestWorld->_14, m_pDestWorld->_24 + m_fOffset, m_pDestWorld->_34 );
	mtxWorld = XMMatrixTranspose( mtxWorld );
	XMStoreFloat4x4( &m_mtxWorld, mtxWorld );

	return 0;
}

DWORD CSign_UI::Release( void )
{
	if( 0 == CUserInterface::Release() )
		delete this;

	return 0;
}

void CSign_UI::SetNumber( UINT iNum )
{
	if( m_iValue == iNum ) return;
	( iNum > 99 ) ? m_iValue = 99 : m_iValue = iNum;
	m_vOption.x = float( m_iValue % 10 ) * 0.1f;
	m_vOption.y = m_vOption.x + 1.f / 10.f;
	m_vOption.z = float( int( m_iValue * 0.1f ) * 0.1f );
	m_vOption.w = m_vOption.z + 1.f / 10.f;
}
