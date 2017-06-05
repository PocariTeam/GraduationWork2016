#include "stdafx.h"
#include "Skydome.h"
#include "Mesh.h"
#include "Texture.h"

CSkydome::CSkydome()
	: CGameObject()
	, m_fAngle( 0.f )
{
}

CSkydome::~CSkydome()
{
}

HRESULT CSkydome::Initialize( CMesh* pMesh, CTexture* pTexture )
{
	m_pMesh = pMesh;
	m_pTexture = pTexture;

	return S_OK;
}

CSkydome* CSkydome::Create( CMesh* pMesh, CTexture* pTexture )
{
	CSkydome*	pSkyBox = new CSkydome;

	if( FAILED( pSkyBox->Initialize( pMesh, pTexture ) ) )
	{
		pSkyBox->Release();
		pSkyBox = nullptr;
	}

	return pSkyBox;
}

int CSkydome::Update( const float& fTimeDelta )
{
	m_fAngle += fTimeDelta * 2.f;
	m_vOption.y = fmod( m_fAngle * 0.05f, 5.f );
	XMStoreFloat4x4( &m_mtxWorld, XMMatrixRotationY( XMConvertToRadians( m_fAngle ) ) );

	return 0;
}

void CSkydome::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CSkydome::Release( void )
{
	CGameObject::Release();
	
	delete this;
	
	return 0;
}
