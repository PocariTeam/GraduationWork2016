#include "stdafx.h"
#include "Camera_Third.h"
#include "Transform.h"

CCamera_Third::CCamera_Third()
{
}


CCamera_Third::~CCamera_Third()
{
}

HRESULT	CCamera_Third::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CTransform* pTarget )
{
	if( FAILED( CCamera::Initialize( pDevice, pDeviceContext ) ) )
		return E_FAIL;

	m_pTarget = pTarget;
	Update( 0.f );

	return S_OK;
}

CCamera_Third* CCamera_Third::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTransform* pTarget )
{
	CCamera_Third* pThirdCam = new CCamera_Third;

	if( FAILED( pThirdCam->Initialize( pDevice, pContext, pTarget ) ) )
	{
		pThirdCam->Release();
		pThirdCam = nullptr;
	}

	return pThirdCam;
}

int CCamera_Third::Update( float fTimeDelta )
{
	if( nullptr == m_pTarget ) return 0;

	XMVECTOR	vUp{ 0.f, 1.f, 0.f, 0.f };
	XMVECTOR	vAt = XMLoadFloat3( &m_pTarget->m_vPos );
	XMVECTOR	vEye = vAt;

	Check_Key( fTimeDelta );

	XMVECTOR	vOffset = XMLoadFloat3( &m_vOffset );
	XMMATRIX	mtxRotX = XMMatrixRotationX( m_fAngle );
	vOffset = XMVector3TransformNormal( vOffset, mtxRotX );
	vEye = XMVectorAdd( vAt, vOffset );

	XMStoreFloat4x4A( &m_mtxView, XMMatrixLookAtLH( vEye, vAt, vUp ) );
	XMStoreFloat3( &m_vEye, vEye );
	XMStoreFloat3( &m_vAt, vAt );

	return 0;
}

DWORD CCamera_Third::Release()
{
	DWORD dwRefCnt = CCamera::Release();
	delete this;
	return dwRefCnt;
}

void CCamera_Third::Check_Key( float fTimeDelta )
{
	float fRealSpeed = m_fSpeed * fTimeDelta;

	if( GetAsyncKeyState( 'Z' ) & 0x8000 )
	{
		m_fAngle -= XM_PIDIV4 * fTimeDelta;
	}

	if( GetAsyncKeyState( 'X' ) & 0x8000 )
	{
		m_fAngle += XM_PIDIV4 * fTimeDelta;
	}

	if( m_fAngle <= 0.01f ) m_fAngle = 0.01f;
	else if( m_fAngle >= XM_PIDIV2 ) m_fAngle = XM_PIDIV2 - 0.01f;
}
