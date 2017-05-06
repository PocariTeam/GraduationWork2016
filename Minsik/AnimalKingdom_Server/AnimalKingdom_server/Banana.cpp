#include "stdafx.h"
#include "Banana.h"
#include "NxActor.h"

CBanana::CBanana()
{
}

CBanana::~CBanana()
{
}

HRESULT CBanana::Initialize( NxActor* pActor, NxVec3& vDir, COL_GROUP eMaster )
{
	m_pActor = pActor;
	m_pActor->setLinearVelocity( vDir * 200.f );
	m_pActor->setAngularVelocity( NxVec3( 90.f, 0.f, 180.f ) );
	m_eMasterGroup = eMaster;

	return S_OK;
}

CBanana* CBanana::Create( NxActor* pActor, NxVec3& vDir, COL_GROUP eMaster )
{
	CBanana* pBanana = new CBanana;

	if( FAILED( pBanana->Initialize( pActor, vDir, eMaster ) ) )
	{
		pBanana->Release();
		pBanana = nullptr;
	}
	return pBanana;
}

XMFLOAT4X4 CBanana::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out;
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMMATRIX mtxScale = XMMatrixScaling( 0.06f, 0.06f, 0.06f );
	mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
	XMStoreFloat4x4( &Out, mtxScale );

	return Out;
}


COL_GROUP CBanana::GetMasterCollisionGroup()
{
	return m_eMasterGroup;
}

DWORD CBanana::Release( void )
{
	delete this;

	return 0;
}
