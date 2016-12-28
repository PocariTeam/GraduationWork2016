#include "stdafx.h"
#include "Define.h"
#include "GraphicDev.h"
#include "Camera.h"

HRESULT CCamera::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	m_pCbMtxCamera = nullptr;

	/* 원근투영 행렬 구성 */
	XMMATRIX mtxProj = XMMatrixPerspectiveFovLH( XMConvertToRadians( 45.f ), float( WIN_WIDTH ) / float( WIN_HEIGHT ), 1.f, 1000.f );
	XMStoreFloat4x4A( &m_mtxProj, mtxProj );

	CreateConstantBuffer( pDevice );

	return S_OK;
}

DWORD	CCamera::Release()
{
	m_pCbMtxCamera->Release();

	return 0;
}

void CCamera::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( VS_CB_CAMERA );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCbMtxCamera );
}

void CCamera::SetConstantBuffer( CGraphicDev* pGraphicDev )
{
	ID3D11DeviceContext* pContext = pGraphicDev->Get_Context();
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pCbMtxCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	VS_CB_CAMERA* pCbMtxViewProj = ( VS_CB_CAMERA * )MappedSubresource.pData;
	XMStoreFloat4x4A( &pCbMtxViewProj->m_mtxView, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxView ) ) );
	XMStoreFloat4x4A( &pCbMtxViewProj->m_mtxProj, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxProj ) ) );
	pCbMtxViewProj->m_vCameraPos = XMFLOAT4A( m_vEye.x, m_vEye.y, m_vEye.z, 0.f );

	pContext->Unmap( m_pCbMtxCamera, 0 );
	pContext->VSSetConstantBuffers( VS_SLOT_CAMERA, 1, &m_pCbMtxCamera );
	pContext->GSSetConstantBuffers( VS_SLOT_CAMERA, 1, &m_pCbMtxCamera );
}

void CCamera::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pCbMtxCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	VS_CB_CAMERA* pCbMtxViewProj = ( VS_CB_CAMERA * )MappedSubresource.pData;
	XMStoreFloat4x4A( &pCbMtxViewProj->m_mtxView, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxView ) ) );
	XMStoreFloat4x4A( &pCbMtxViewProj->m_mtxProj, XMMatrixTranspose( XMLoadFloat4x4A( &m_mtxProj ) ) );
	pCbMtxViewProj->m_vCameraPos = XMFLOAT4A( m_vEye.x, m_vEye.y, m_vEye.z, 0.f );

	pContext->Unmap( m_pCbMtxCamera, 0 );
	pContext->VSSetConstantBuffers( VS_SLOT_CAMERA, 1, &m_pCbMtxCamera );
	pContext->GSSetConstantBuffers( VS_SLOT_CAMERA, 1, &m_pCbMtxCamera );
}