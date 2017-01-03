#include "stdafx.h"
#include "Camera_Dynamic.h"
#include "Transform.h"
#include "Define.h"

HRESULT	CCamera_Dynamic::Initialize( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
{
	if( FAILED( CCamera::Initialize( pDevice, pDeviceContext ) ) )
		return E_FAIL;

	m_hWnd = hWnd;
	m_vEye = XMFLOAT3( 0.f, 10.f, -20.f );
	m_vAt = XMFLOAT3( 0.f, 0.f, 0.f );

	return S_OK;
}

CCamera_Dynamic* CCamera_Dynamic::Create( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	CCamera_Dynamic* pDynamicCam = new CCamera_Dynamic;

	if( FAILED( pDynamicCam->Initialize( hWnd, pDevice, pContext ) ) )
	{
		pDynamicCam->Release();
		pDynamicCam = nullptr;
	}

	return pDynamicCam;
}

int CCamera_Dynamic::Update( float fTimeDelta )
{
	XMVECTOR	vEye = XMLoadFloat3( &m_vEye );
	XMVECTOR	vAt  = XMLoadFloat3( &m_vAt );

	Check_Key( vEye, vAt, fTimeDelta );

	XMMATRIX mtxView{ XMMatrixLookAtLH( vEye, vAt, XMVECTOR{ 0.f, 1.f, 0.f, 0.f } ) };
	XMStoreFloat4x4A( &m_mtxView, mtxView );
	XMStoreFloat3( &m_vEye, vEye );
	XMStoreFloat3( &m_vAt, vAt );

	return 0;
}

DWORD CCamera_Dynamic::Release()
{
	DWORD dwRefCnt = CCamera::Release();

	delete this;
	return dwRefCnt;
}

void CCamera_Dynamic::Check_Key( XMVECTOR& vEye, XMVECTOR& vAt, float fTimeDelta )
{
	float fRealSpeed = m_fSpeed * fTimeDelta;

	XMVECTOR vRight{}, vUp{}, vLook{};
	XMMATRIX mtxCameraWorld{ XMLoadFloat4x4A( &m_mtxView ) };
	mtxCameraWorld = XMMatrixTranspose( mtxCameraWorld );
	XMStoreFloat4x4A( &m_mtxView, mtxCameraWorld );

	memcpy( &vRight, &m_mtxView.m[0], sizeof( XMVECTOR ) );
	memcpy( &vUp, &m_mtxView.m[1], sizeof( XMVECTOR ) );
	memcpy( &vLook, &m_mtxView.m[2], sizeof( XMVECTOR ) );

	if( GetAsyncKeyState( 'W' ) & 0x8000 )
	{
		vEye += vLook * fRealSpeed;
		vAt += vLook * fRealSpeed;
	}

	if( GetAsyncKeyState( 'S' ) & 0x8000 )
	{
		vEye -= vLook * fRealSpeed;
		vAt -= vLook * fRealSpeed;
	}

	if( GetAsyncKeyState( 'A' ) & 0x8000 )
	{
		vEye -= vRight * fRealSpeed;
		vAt -= vRight * fRealSpeed;
	}

	if( GetAsyncKeyState( 'D' ) & 0x8000 )
	{
		vEye += vRight * fRealSpeed;
		vAt += vRight * fRealSpeed;
	}

	if( GetAsyncKeyState( VK_RBUTTON ) && false == m_bOnClick )
	{
		m_ptMouse.x = WIN_WIDTH / 2;
		m_ptMouse.y = WIN_HEIGHT / 2;

		::ClientToScreen( m_hWnd, &m_ptMouse );
		::SetCursorPos( m_ptMouse.x, m_ptMouse.y );
		::ShowCursor( false );
		m_bOnClick = true;
	}

	if( GetAsyncKeyState( VK_RBUTTON ) && true == m_bOnClick )
	{
		POINT	ptUpdateMouse;
		::GetCursorPos( &ptUpdateMouse );

		float fDeltaX = float( ptUpdateMouse.x - m_ptMouse.x );
		float fDeltaY = float( ptUpdateMouse.y - m_ptMouse.y );

		
		XMMATRIX mtxRotAxisUp = XMMatrixRotationAxis( vUp, fDeltaX / 15.f * fTimeDelta );
		XMMATRIX mtxRotAxisRight = XMMatrixRotationAxis( vRight, fDeltaY / 15.f * fTimeDelta );
		XMMATRIX mtxRotation{ mtxRotAxisRight * mtxRotAxisUp };

		vLook = XMVector3TransformNormal( vLook, mtxRotation );
		vAt = vEye + vLook;
		
		::SetCursorPos( m_ptMouse.x, m_ptMouse.y );
	}

	if( !GetAsyncKeyState( VK_RBUTTON ) && true == m_bOnClick )
	{
		m_bOnClick = false;
		::ShowCursor( true );
	}
}
