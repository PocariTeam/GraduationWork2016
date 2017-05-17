#include "stdafx.h"
#include "Environment.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"
#include <NxSimpleTypes.h>
#include "NxActor.h"

CEnvironment::CEnvironment()
	: CGameObject()
{
}

CEnvironment::~CEnvironment()
{
}

HRESULT CEnvironment::Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale )
{
	m_pActor = pActor;
	m_pMesh = pMesh;
	m_pTexture = pTexture;
	m_vScale = vScale;
	m_vOption.x = 1.f;		// Ä«Å÷ ÇÑ´Ù
	m_vOption.w = 0.1f;

	return S_OK;
}

CEnvironment* CEnvironment::Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale )
{
	CEnvironment* pEnvironment = new CEnvironment;

	if( FAILED( pEnvironment->Initialize( pDevice, pActor, pMesh, pTexture, vScale ) ) )
	{
		pEnvironment->Release();
		pEnvironment = nullptr;
	}
	return pEnvironment;
}

XMFLOAT4X4* CEnvironment::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMMATRIX mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
	mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
	XMStoreFloat4x4( &m_mtxWorld, mtxScale );

	return &m_mtxWorld;
}

void CEnvironment::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

DWORD CEnvironment::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		delete this;
	}

	return 0;
}
