#include "stdafx.h"
#include "Define.h"
#include "GraphicDev.h"
#include "Value.h"
#include "Struct.h"
#include "Camera.h"
#include "Function.h"

CCamera::CCamera()
	: CBase()
	, m_pConstantBufferCamera( nullptr )
	, m_vAt( 0.f, 0.f, 0.f )
	, m_vEye( 0.f, 0.f, 0.f )
	, m_fSpeed( 0.f )
{
	ZeroMemory( &m_mtxView, sizeof( XMFLOAT4X4 ) );
	ZeroMemory( &m_mtxProj, sizeof( XMFLOAT4X4 ) );
}

HRESULT CCamera::Initialize( ID3D11Device* pDevice )
{
	XMMATRIX mtxProj = XMMatrixPerspectiveFovLH( XMConvertToRadians( 45.f ), float( g_dwWinsizeX ) / float( g_dwWinsizeY ), 1.f, 1000.f );
	XMStoreFloat4x4( &m_mtxProj, mtxProj );

	CreateConstantBuffer( pDevice );

	return S_OK;
}

void CCamera::Render( ID3D11DeviceContext* pContext )
{
	SetConstantBuffer( pContext );
}

DWORD	CCamera::Release()
{
	CBase::Release();
	::Safe_Release( m_pConstantBufferCamera );

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
	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pConstantBufferCamera );
}

void CCamera::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pConstantBufferCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	CB_CAMERA* pConstantBuffer_Camera = ( CB_CAMERA* )MappedSubresource.pData;
	XMStoreFloat4x4( &pConstantBuffer_Camera->m_mtxView, XMMatrixTranspose( XMLoadFloat4x4( &m_mtxView ) ) );
	XMStoreFloat4x4( &pConstantBuffer_Camera->m_mtxProj, XMMatrixTranspose( XMLoadFloat4x4( &m_mtxProj ) ) );
	memcpy_s( &pConstantBuffer_Camera->m_vCameraPos, sizeof( XMFLOAT4 ), &pConstantBuffer_Camera->m_mtxView.m[ 3 ], sizeof( XMFLOAT4 ) );

	pContext->Unmap( m_pConstantBufferCamera, 0 );
	pContext->VSSetConstantBuffers( SLOT_CAMERA, 1, &m_pConstantBufferCamera );
	pContext->PSSetConstantBuffers( SLOT_CAMERA, 1, &m_pConstantBufferCamera );
}

void CCamera::CalculateViewMatrix( XMFLOAT4X4* pOut, const XMFLOAT3& vEye, const XMFLOAT3& vAt )
{
	XMVECTOR vLoadEye{ XMLoadFloat3( &vEye ) }, vLoadAt{ XMLoadFloat3( &vAt ) }, vUp{ 0.f, 1.f, 0.f, 0.f };

	XMStoreFloat4x4( pOut, XMMatrixLookAtLH( vLoadEye, vLoadAt, vUp ) );
}
