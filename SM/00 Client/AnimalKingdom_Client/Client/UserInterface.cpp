#include "stdafx.h"
#include "UserInterface.h"
#include "Texture.h"

CUserInterface::CUserInterface()
	: CGameObject()
	, m_vPosSize_Dest( 0.f, 0.f, 0.f, 0.f )
	, m_vPosSize_Src( 0.f, 0.f, 0.f, 0.f )
	, m_fLerpTime( 0.f )
{
	ZeroMemory( &m_mtxPosSize, sizeof( XMFLOAT4X4 ) );
}

CUserInterface::~CUserInterface()
{
}

XMFLOAT4X4 CUserInterface::GetWorld( void )
{
	return m_mtxPosSize;
}

void CUserInterface::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	pContext->Draw( 6, 0 );
}

int CUserInterface::Update( const float& fTimedelta )
{
	return CGameObject::Update( fTimedelta );
}

DWORD CUserInterface::Release( void )
{
	CGameObject::Release();

	return 0;
}

void CUserInterface::SetTexture( CTexture* pTexture )
{
	::Safe_Release( m_pTexture ); m_pTexture = pTexture;
}
