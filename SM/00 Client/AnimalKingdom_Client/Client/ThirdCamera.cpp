#include "stdafx.h"
#include "ThirdCamera.h"
#include "InputMgr.h"
#include "Value.h"
#include <NxActor.h>

CThirdCamera::CThirdCamera()
	: CCamera()
	, m_pDestActor( nullptr )
	, m_vOffset()
{
}

CThirdCamera::~CThirdCamera()
{
}

HRESULT CThirdCamera::Initialize( ID3D11Device* pDevice, NxActor* pActor, XMFLOAT3& vOffset )
{
	m_pDestActor = pActor;
	m_vOffset = vOffset;
	NxVec3 vAt = pActor->getGlobalPosition();
	m_vAt = XMFLOAT3( vAt.x, vAt.y, vAt.z );
	m_vEye = XMFLOAT3( m_vAt.x + m_vOffset.x, m_vAt.y + m_vOffset.y, m_vAt.z + m_vOffset.z );
	CalculateViewMatrix( &m_mtxView, m_vEye, m_vAt );

	return CCamera::Initialize( pDevice );
}


CThirdCamera* CThirdCamera::Create( ID3D11Device* pDevice, NxActor* pActor, XMFLOAT3& vOffset )
{
	CThirdCamera* pThirdCamera = new CThirdCamera;

	if( FAILED( pThirdCamera->Initialize( pDevice, pActor, vOffset ) ) )
	{
		pThirdCamera->Release();
		pThirdCamera = nullptr;
	}

	return pThirdCamera;
}

int CThirdCamera::Update( const float& fTimeDelta )
{
	NxVec3		vActorPos = m_pDestActor->getGlobalPosition();
	XMVECTOR	vAt{ XMLoadFloat3( &XMFLOAT3( vActorPos.x, vActorPos.y, vActorPos.z ) ) };
	XMVECTOR	vEye{ XMLoadFloat3( &m_vOffset ) };

	vEye += vAt;

	XMStoreFloat3( &m_vEye, vEye );
	XMStoreFloat3( &m_vAt, vAt );

	CalculateViewMatrix( &m_mtxView, m_vEye, m_vAt );

	return 0;
}

DWORD CThirdCamera::Release( void )
{
	CCamera::Release();

	delete this;

	return 0;
}
