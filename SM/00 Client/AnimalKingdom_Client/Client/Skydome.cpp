#include "stdafx.h"
#include "Skydome.h"
#include "Mesh.h"
#include "Texture.h"

CSkydome::CSkydome()
	: CGameObject()
	, m_fAngle( 0.f )
	, m_pDirectionLight( nullptr )
	, m_fDestDiffuse( 1.f )
{
}

CSkydome::~CSkydome()
{
}

HRESULT CSkydome::Initialize( CMesh* pMesh, CTexture* pTexture, LIGHT* pLight )
{
	m_pMesh = pMesh;
	m_pTexture = pTexture;
	m_pDirectionLight = pLight;

	return S_OK;
}

CSkydome* CSkydome::Create( CMesh* pMesh, CTexture* pTexture, LIGHT* pLight )
{
	CSkydome*	pSkyBox = new CSkydome;

	if( FAILED( pSkyBox->Initialize( pMesh, pTexture, pLight ) ) )
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
	m_fDestDiffuse = ( m_vOption.y > 2.3f && m_vOption.y < 3.7f ) ? 0.f : 2.f ;

	float fNewTime = fTimeDelta;
	float fDiffuseColor = ( fNewTime * m_fDestDiffuse ) + ( 1.f - fNewTime ) * m_pDirectionLight->m_vDiffuse.x;
	m_pDirectionLight->m_vDiffuse = XMFLOAT4( fDiffuseColor, fDiffuseColor, fDiffuseColor, 1.f );
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
	m_pDirectionLight = nullptr;

	delete this;
	
	return 0;
}
