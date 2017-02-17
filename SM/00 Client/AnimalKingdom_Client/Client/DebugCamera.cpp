#include "stdafx.h"
#include "DebugCamera.h"
#include "InputMgr.h"
#include "Value.h"

CDebugCamera::CDebugCamera()
	: CCamera()
	, m_hWnd( nullptr )
	, m_bClicked( false )
{
	ZeroMemory( &m_ptMouse, sizeof( POINT ) );
}

CDebugCamera::~CDebugCamera()
{
}

HRESULT CDebugCamera::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_hWnd = hWnd;

	m_fSpeed = 10.f;
	m_vAt = XMFLOAT3( 0.f, 0.f, 0.f );
	m_vEye = XMFLOAT3( 0.f, 40.f, -60.f );
	CalculateViewMatrix( &m_mtxView, XMFLOAT3{ 0.f, 100.f, -120.f }, XMFLOAT3{ 0.f, 0.f, 0.f } );

	return CCamera::Initialize( pDevice );
}

void CDebugCamera::Check_Key( XMVECTOR& vEye, XMVECTOR& vAt, const float& fTimeDelta )
{
	float fRealSpeed = m_fSpeed * fTimeDelta;

	XMVECTOR vRight{}, vUp{}, vLook{};
	XMMATRIX mtxCameraWorld{ XMLoadFloat4x4( &m_mtxView ) };
	mtxCameraWorld = XMMatrixTranspose( mtxCameraWorld );
	XMStoreFloat4x4( &m_mtxView, mtxCameraWorld );

	memcpy( &vRight, &m_mtxView.m[ 0 ], sizeof( XMVECTOR ) );
	memcpy( &vUp, &m_mtxView.m[ 1 ], sizeof( XMVECTOR ) );
	memcpy( &vLook, &m_mtxView.m[ 2 ], sizeof( XMVECTOR ) );

	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_W ) & 0x80 )
	{
		vEye += vLook * fRealSpeed;
		vAt += vLook * fRealSpeed;
	}

	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_S ) & 0x80 )
	{
		vEye -= vLook * fRealSpeed;
		vAt -= vLook * fRealSpeed;
	}

	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_A ) & 0x80 )
	{
		vEye -= vRight * fRealSpeed;
		vAt -= vRight * fRealSpeed;
	}

	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_D ) & 0x80 )
	{
		vEye += vRight * fRealSpeed;
		vAt += vRight * fRealSpeed;
	}

	if( ( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_RBUTTON ) & 0x80 ) && false == m_bClicked )
	{
		m_ptMouse.x = g_wWinsizeX / 2;
		m_ptMouse.y = g_wWinsizeY / 2;

		::ClientToScreen( m_hWnd, &m_ptMouse );
		::SetCursorPos( m_ptMouse.x, m_ptMouse.y );
		::ShowCursor( false );
		m_bClicked = true;
	}

	if( ( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_RBUTTON ) & 0x80 ) && true == m_bClicked )
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

	if( !( CInputMgr::GetInstance()->Get_MouseState( CInputMgr::CLICK_RBUTTON ) & 0x80 ) && true == m_bClicked )
	{
		m_bClicked = false;
		::ShowCursor( true );
	}
}

CDebugCamera* CDebugCamera::Create( HWND hWnd, ID3D11Device* pDevice )
{
	CDebugCamera* pDebugCamera = new CDebugCamera;

	if( FAILED( pDebugCamera->Initialize( hWnd, pDevice ) ) )
	{
		pDebugCamera->Release();
		pDebugCamera = nullptr;
	}

	return pDebugCamera;
}

int CDebugCamera::Update( const float& fTimeDelta )
{
	XMVECTOR	vEye{ XMLoadFloat3( &m_vEye ) };
	XMVECTOR	vAt{ XMLoadFloat3( &m_vAt ) };

	Check_Key( vEye, vAt, fTimeDelta );

	XMStoreFloat3( &m_vEye, vEye );
	XMStoreFloat3( &m_vAt, vAt );

	CalculateViewMatrix( &m_mtxView, m_vEye, m_vAt );

	return 0;
}

DWORD CDebugCamera::Release( void )
{
	CCamera::Release();
	m_hWnd = nullptr;

	delete this;

	return 0;
}
