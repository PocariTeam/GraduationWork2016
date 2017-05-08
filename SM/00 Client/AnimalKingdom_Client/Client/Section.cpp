#include "stdafx.h"
#include "Section.h"
#include "Texture.h"
#include "Mesh.h"
#include "Function.h"

CSection::CSection()
	: CGameObject()
	, m_vPos( 0.f, 0.f, 0.f )
	, m_vScale( 1.f, 1.f, 1.f )
{
	m_vPlane[ 0 ] = XMFLOAT4( 0.f, 0.f, -1.f, 0.f );
	m_vPlane[ 1 ] = XMFLOAT4( 0.f, 0.f, 1.f, 0.f );
	m_vPlane[ 2 ] = XMFLOAT4( -1.f, 0.f, 0.f, 0.f );
	m_vPlane[ 3 ] = XMFLOAT4( 1.f, 0.f, 0.f, 0.f );
	m_vPlane[ 4 ] = XMFLOAT4( 0.f, -1.f, 0.f, 0.f );
	m_vPlane[ 5 ] = XMFLOAT4( 0.f, 1.f, 0.f, 0.f );
}

CSection::~CSection()
{
}

HRESULT CSection::Initialize( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos, XMFLOAT3& vScale )
{
	m_pMesh = pMesh;
	m_pTexture = pTexture;
	m_vPos = vPos;
	m_vScale = vScale;

	XMMATRIX mtxWorld = XMMatrixMultiply( XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z ), XMMatrixTranslation( m_vPos.x, m_vPos.y, m_vPos.z ) );
	XMStoreFloat4x4( &m_mtxWorld, mtxWorld );

	XMVECTOR vPlane, vTransform;
	for( int i = 0; i < 6; ++i )
	{
		vPlane = vTransform = XMLoadFloat4( &m_vPlane[ i ] );
		vTransform = XMVector3TransformCoord( vTransform, mtxWorld );
		vPlane = XMPlaneFromPointNormal( vTransform, vPlane );
		vPlane = XMPlaneNormalize( vPlane );
		XMStoreFloat4( &m_vPlane[ i ], vPlane );
	}

	XMStoreFloat4x4( &m_mtxWorld, XMMatrixTranspose( XMLoadFloat4x4( &m_mtxWorld ) ) );

	return S_OK;
}

CSection* CSection::Create( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos, XMFLOAT3& vScale )
{
	CSection* pEnvironment = new CSection;

	if( FAILED( pEnvironment->Initialize( pDevice, pMesh, pTexture, vPos, vScale ) ) )
	{
		pEnvironment->Release();
		pEnvironment = nullptr;
	}
	return pEnvironment;
}

void CSection::Render( ID3D11DeviceContext* pContext )
{
	// m_pTexture->Render( pContext );
	// m_pMesh->Render( pContext );
}

DWORD CSection::Release( void )
{
	CGameObject::Release();

	delete this;

	return 0;
}

bool CSection::Check_InPlane( XMFLOAT3 vPos )
{
	bool bResult{ true };
	XMVECTOR vPlane, vInputPos;
	float fDot{};

	vInputPos = XMLoadFloat3( &vPos );

	for( int i = 0; i < 6; ++i )
	{
		vPlane = XMLoadFloat4( &m_vPlane[ i ] );
		vPlane = XMPlaneDotCoord( vPlane, vInputPos );
		XMStoreFloat( &fDot, vPlane );
		if( 0.f < fDot )
			bResult = false;
	}

	return bResult;
}
