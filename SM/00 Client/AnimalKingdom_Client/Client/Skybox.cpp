#include "stdafx.h"
#include "Skybox.h"
#include "Mesh.h"
#include "Texture.h"

CSkybox::CSkybox()
	: CGameObject()
{
}

CSkybox::~CSkybox()
{
}

HRESULT CSkybox::Initialize( CMesh* pMesh, CTexture* pTexture )
{
	m_pMesh = pMesh;
	m_pTexture = pTexture;

	return S_OK;
}

CSkybox* CSkybox::Create( CMesh* pMesh, CTexture* pTexture )
{
	CSkybox*	pSkyBox = new CSkybox;

	if( FAILED( pSkyBox->Initialize( pMesh, pTexture ) ) )
	{
		pSkyBox->Release();
		pSkyBox = nullptr;
	}

	return pSkyBox;
}

XMFLOAT4X4 CSkybox::GetWorld()
{
	XMFLOAT4X4	Out;
	XMStoreFloat4x4( &Out, XMMatrixIdentity() );
	return Out;
}

void CSkybox::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CSkybox::Release( void )
{
	CGameObject::Release();
	
	delete this;
	
	return 0;
}
