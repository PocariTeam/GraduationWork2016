#include "stdafx.h"
#include "UserInterface.h"
#include "Texture.h"

CUserInterface::CUserInterface()
	: CGameObject()
{
	ZeroMemory( &m_mtxPosSize, sizeof( XMFLOAT4X4 ) );
}

CUserInterface::~CUserInterface()
{
}

//HRESULT CUserInterface::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, const float& fPriority )
//{
//	m_pTexture = pTexture;
//
//	m_mtxWolrd._11 = vPosSize.x;
//	m_mtxWolrd._21 = vPosSize.y;
//	m_mtxWolrd._31 = vPosSize.z;
//	m_mtxWolrd._41 = vPosSize.w;
//
//	m_mtxWolrd._12 = fPriority;
//
//	return S_OK;
//}

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
