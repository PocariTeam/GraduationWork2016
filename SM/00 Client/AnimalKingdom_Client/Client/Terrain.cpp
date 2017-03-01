#include "stdafx.h"
#include "Terrain.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"
#include <NxSimpleTypes.h>
#include "NxActor.h"

CTerrain::CTerrain()
	: CGameObject()
	, m_vScale( 1.f, 1.f, 1.f )
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale )
{
	m_pActor = pActor;
	m_pMesh = pMesh;
	m_pTexture = pTexture;
	m_vScale = vScale;

	return S_OK;
}

CTerrain* CTerrain::Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale )
{
	CTerrain* pTerrain = new CTerrain;

	if( FAILED( pTerrain->Initialize( pDevice, pActor, pMesh, pTexture, vScale ) ) )
	{
		pTerrain->Release();
		pTerrain = nullptr;
	}
	return pTerrain;
}

XMFLOAT4X4 CTerrain::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMMATRIX mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
	mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
	XMStoreFloat4x4( &Out, mtxScale );

	return Out;
}

void CTerrain::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CTerrain::Release( void )
{
	CGameObject::Release();

	::Safe_Release( m_pTexture );
	::Safe_Release( m_pMesh );

	delete this;

	return 0;
}
