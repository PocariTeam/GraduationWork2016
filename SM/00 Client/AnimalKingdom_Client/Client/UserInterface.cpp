#include "stdafx.h"
#include "UserInterface.h"
#include "Texture.h"

CUserInterface::CUserInterface()
	: CGameObject()
{
	ZeroMemory( &m_mtxWolrd, sizeof( XMFLOAT4X4 ) );
}

CUserInterface::~CUserInterface()
{
}

HRESULT CUserInterface::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, const float& fPriority )
{
	m_pTexture = pTexture;

	m_mtxWolrd._11 = vPosSize.x;
	m_mtxWolrd._21 = vPosSize.y;
	m_mtxWolrd._31 = vPosSize.z;
	m_mtxWolrd._41 = vPosSize.w;

	m_mtxWolrd._12 = fPriority;

	return S_OK;
}

CUserInterface* CUserInterface::Create( CTexture* pTexture, const XMFLOAT4& vPosSize, const float& fPriority )
{
	CUserInterface*	pUserInterface = new CUserInterface;

	if( FAILED( pUserInterface->Initialize( pTexture, vPosSize, fPriority ) ) )
	{
		pUserInterface->Release();
		pUserInterface = nullptr;
	}

	return pUserInterface;
}

XMFLOAT4X4 CUserInterface::GetWorld( void )
{
	return m_mtxWolrd;
}

void CUserInterface::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	pContext->Draw( 6, 0 );
}

DWORD CUserInterface::Release( void )
{
	CGameObject::Release();

	delete this;

	return 0;
}
