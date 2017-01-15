#include "stdafx.h"
#include "Transform.h"


CTransform::CTransform()
	: m_pCBWorld( nullptr )
	, m_fAngle( 0.f )
{
	ZeroMemory( &m_mtxWorld, sizeof( XMFLOAT4X4 ) );
}

CTransform::~CTransform()
{
}

CTransform* CTransform::Create( ID3D11Device* pDevice )
{
	CTransform* pTransform = new CTransform;

	if( FAILED( pTransform->Initialize( pDevice ) ) )
	{
		pTransform->Release();
		pTransform = nullptr;
	}

	return pTransform;
}

HRESULT CTransform::Initialize( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( CB_WORLD );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCBWorld );

	return S_OK;
}

void CTransform::Update( float fTimeDelta )
{
	// m_fAngle += fTimeDelta;
	//if( m_fAngle > XM_PI * 2.f ) m_fAngle = 0.f;

	XMMATRIX mtxWorld{ XMLoadFloat4x4A( &m_mtxWorld ) };
	mtxWorld = XMMatrixIdentity();

	// mtxWorld *= XMMatrixRotationX( -XM_PIDIV2 );

	XMStoreFloat4x4A( &m_mtxWorld, mtxWorld );
	
}

void CTransform::Render( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pCBWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	CB_WORLD* pCBWorld = ( CB_WORLD* )MappedSubresource.pData;
	XMStoreFloat4x4A( &pCBWorld->m_mtxWorld, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxWorld ) ) );

	pContext->Unmap( m_pCBWorld, 0 );
	pContext->VSSetConstantBuffers( SLOT_WORLD, 1, &m_pCBWorld );
}

void CTransform::Release()
{
	if( m_pCBWorld ) {
		m_pCBWorld->Release();
		m_pCBWorld = nullptr;
	}

	delete this;
}
