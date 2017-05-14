#include "stdafx.h"
#include "EventCamera.h"
#include "InputMgr.h"
#include "Value.h"
#include <NxActor.h>

CEventCamera::CEventCamera()
	: CCamera()
	, m_vDest( 0.f, 0.f, 0.f )
{
}

CEventCamera::~CEventCamera()
{
}

HRESULT CEventCamera::Initialize( ID3D11Device* pDevice, XMFLOAT3& vAt, XMFLOAT3& vDest, XMFLOAT3& vCurrent )
{
	m_vDest = vDest;
	m_vAt = vAt;
	m_vEye = vCurrent;

	CalculateViewMatrix( &m_mtxView, m_vEye, m_vAt );
	m_fSpeed = 0.5f;

	return CCamera::Initialize( pDevice );
}


CEventCamera* CEventCamera::Create( ID3D11Device* pDevice, XMFLOAT3& vAt, XMFLOAT3& vDest, XMFLOAT3& vCurrent )
{
	CEventCamera* pEventCamera = new CEventCamera;

	if( FAILED( pEventCamera->Initialize( pDevice, vAt, vDest, vCurrent ) ) )
	{
		pEventCamera->Release();
		pEventCamera = nullptr;
	}

	return pEventCamera;
}

int CEventCamera::Update( const float& fTimeDelta )
{
	XMVECTOR	vAt{ XMLoadFloat3( &m_vAt ) };
	XMVECTOR	vCurrent{ XMLoadFloat3( &m_vEye ) };
	XMVECTOR	vDest{ XMLoadFloat3( &m_vDest ) };

	vCurrent = XMVectorLerp( vCurrent, vDest, fTimeDelta * m_fSpeed );

	XMStoreFloat3( &m_vEye, vCurrent );
	XMStoreFloat3( &m_vAt, vAt );

	CalculateViewMatrix( &m_mtxView, m_vEye, m_vAt );

	return 0;
}

DWORD CEventCamera::Release( void )
{
	CCamera::Release();

	delete this;

	return 0;
}