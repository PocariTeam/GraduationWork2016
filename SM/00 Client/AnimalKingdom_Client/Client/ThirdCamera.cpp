#include "stdafx.h"
#include "ThirdCamera.h"
#include "InputMgr.h"
#include "Value.h"
#include <NxActor.h>

CThirdCamera::CThirdCamera()
	: CCamera()
	, m_pDestWorldTranspose( nullptr )
	, m_vOffset( 0.f, 0.f, 0.f )
	, m_vCurrent( 0.f, 0.f, 0.f )
{
}

CThirdCamera::~CThirdCamera()
{
}

HRESULT CThirdCamera::Initialize( ID3D11Device* pDevice, XMFLOAT4X4* pWorldTranspose, XMFLOAT3& vOffset )
{
	m_pDestWorldTranspose = pWorldTranspose;
	m_vOffset = vOffset;
	m_vAt = XMFLOAT3( m_pDestWorldTranspose->_14, m_pDestWorldTranspose->_24, m_pDestWorldTranspose->_34 );
	m_vCurrent = XMFLOAT3( m_pDestWorldTranspose->_14, m_pDestWorldTranspose->_24, m_pDestWorldTranspose->_34 );
	m_vEye = XMFLOAT3( m_vAt.x + m_vOffset.x, m_vAt.y + m_vOffset.y, m_vAt.z + m_vOffset.z );
	
	CalculateViewMatrix( &m_mtxView, m_vEye, m_vCurrent );
	m_fSpeed = 1.f;

	return CCamera::Initialize( pDevice );
}


CThirdCamera* CThirdCamera::Create( ID3D11Device* pDevice, XMFLOAT4X4* pWorldTranspose, XMFLOAT3& vOffset )
{
	CThirdCamera* pThirdCamera = new CThirdCamera;

	if( FAILED( pThirdCamera->Initialize( pDevice, pWorldTranspose, vOffset ) ) )
	{
		pThirdCamera->Release();
		pThirdCamera = nullptr;
	}

	return pThirdCamera;
}

int CThirdCamera::Update( const float& fTimeDelta )
{
	XMVECTOR	vAt{ XMLoadFloat3( &XMFLOAT3( m_pDestWorldTranspose->_14, m_pDestWorldTranspose->_24, m_pDestWorldTranspose->_34 ) ) };
	XMVECTOR	vCurrent{ XMLoadFloat3( &m_vCurrent ) };
	XMVECTOR	vEye{ XMLoadFloat3( &m_vOffset ) };

	vCurrent = XMVectorLerp( vAt, vCurrent, fTimeDelta * fTimeDelta );

	vEye += vCurrent;

	XMStoreFloat3( &m_vEye, vEye );
	XMStoreFloat3( &m_vAt, vAt );
	XMStoreFloat3( &m_vCurrent, vCurrent );

	CalculateViewMatrix( &m_mtxView, m_vEye, m_vCurrent );

	return 0;
}

DWORD CThirdCamera::Release( void )
{
	CCamera::Release();

	delete this;

	return 0;
}
