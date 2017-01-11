#include "stdafx.h"
#include "GraphicDev.h"
#include "Camera.h"

HRESULT CCamera::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	/* 원근투영 행렬 구성 */
	XMMATRIX mtxProj = XMMatrixPerspectiveFovLH( XMConvertToRadians( 45.f ), float( C_WINSIZE_WIDTH ) / float( C_WINSIZE_HEIGHT ), 1.f, 1000.f );
	XMStoreFloat4x4A( &m_mtxProj, mtxProj );

	/* 카메라 변환 행렬 구성 */
	XMMATRIX mtxView = XMMatrixLookAtLH( XMLoadFloat3( &m_vEye ), XMLoadFloat3( &m_vAt ), XMLoadFloat3( &XMFLOAT3( 0.f, 1.f, 0.f ) ) );
	XMStoreFloat4x4A( &m_mtxView, mtxView );

	CreateConstantBuffer( pDevice );
	SetConstantBuffer( pDeviceContext );

	return S_OK;
}

DWORD	CCamera::Release()
{
	m_pCbMtxCamera->Release();
	m_pCbMtxCamera = nullptr;

	delete this;

	return 0;
}

void CCamera::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( CB_CAMERA );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCbMtxCamera );
}

void CCamera::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pCbMtxCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	CB_CAMERA* pCbMtxViewProj = ( CB_CAMERA * )MappedSubresource.pData;
	XMStoreFloat4x4A( &pCbMtxViewProj->m_mtxView, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxView ) ) );
	XMStoreFloat4x4A( &pCbMtxViewProj->m_mtxProj, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxProj ) ) );
	pCbMtxViewProj->m_vCameraPos = XMFLOAT4A( m_vEye.x, m_vEye.y, m_vEye.z, 0.f );

	pContext->Unmap( m_pCbMtxCamera, 0 );
	pContext->VSSetConstantBuffers( SLOT_CAMERA, 1, &m_pCbMtxCamera );
	pContext->PSSetConstantBuffers( SLOT_CAMERA, 1, &m_pCbMtxCamera );
}

void CCamera::Update()
{
	/* 카메라 변환 행렬 구성 */
	XMMATRIX mtxView = XMMatrixLookAtLH( XMLoadFloat3( &m_vEye ), XMLoadFloat3( &m_vAt ), XMLoadFloat3( &XMFLOAT3( 0.f, 1.f, 0.f ) ) );
	XMStoreFloat4x4A( &m_mtxView, mtxView );
}

CCamera* CCamera::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	CCamera* pCamera = new CCamera;

	if( FAILED( pCamera->Initialize( pDevice, pDeviceContext ) ) )
	{
		pCamera->Release();
		pCamera = nullptr;
	}

	return pCamera;
}
