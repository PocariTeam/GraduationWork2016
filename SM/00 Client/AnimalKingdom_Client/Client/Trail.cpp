#include "stdafx.h"
#include "Trail.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"

CTrail::CTrail()
	: CGameObject()
{
}

CTrail::~CTrail()
{
}

HRESULT CTrail::Initialize( ID3D11Device* pDevice, XMFLOAT3& vScale )
{
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Fire" );
	m_vScale = vScale;
	m_vOption.x = 0.5f;		// Ä«Å÷ ¾ÈÇÑ´Ù
	m_vOption.w = 0.1f;

	return S_OK;
}

CTrail* CTrail::Create( ID3D11Device* pDevice, XMFLOAT3& vScale )
{
	CTrail* pTrail = new CTrail;

	if( FAILED( pTrail->Initialize( pDevice, vScale ) ) )
	{
		pTrail->Release();
		pTrail = nullptr;
	}
	return pTrail;
}

XMFLOAT4X4* CTrail::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	/*m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMMATRIX mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
	mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
	XMStoreFloat4x4( &m_mtxWorld, mtxScale );*/

	return &m_mtxWorld;
}

void CTrail::Render( ID3D11DeviceContext* pContext )
{
	m_pTexture->Render( pContext );
	pContext->Draw( 6, 0 );
}

DWORD CTrail::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		delete this;
	}

	return 0;
}
