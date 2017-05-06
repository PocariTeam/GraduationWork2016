#include "stdafx.h"
#include "Banana.h"
#include "NxActor.h"

CBanana::CBanana()
{
}

CBanana::~CBanana()
{
}

HRESULT CBanana::Initialize( NxActor* pActor, COL_GROUP eMaster, UINT iSceneNum )
{
	m_pActor = pActor;
	m_eMasterGroup = eMaster;
	m_iSceneNum = iSceneNum;

	return S_OK;
}

CBanana* CBanana::Create( NxActor* pActor, COL_GROUP eMaster, UINT iSceneNum )
{
	CBanana* pBanana = new CBanana;

	if( FAILED( pBanana->Initialize( pActor, eMaster, iSceneNum ) ) )
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


COL_GROUP CBanana::GetMasterCollisionGroup() const
{
	return m_eMasterGroup;
}

void CBanana::Throw( NxVec3& vPos, NxVec3& vDir, COL_GROUP eMaster )
{
	m_pActor->clearBodyFlag( NX_BF_KINEMATIC );
	m_pActor->setLinearDamping( 1 );
	m_pActor->setAngularDamping( 5 );
	m_pActor->setGlobalPosition( vPos );
	m_pActor->setLinearVelocity( vDir * 200.f );
	m_pActor->setAngularVelocity( NxVec3( 90.f, 0.f, 180.f ) );
	m_eMasterGroup = eMaster;
}

void CBanana::Frozen( void )
{
	m_pActor->raiseBodyFlag( NX_BF_KINEMATIC );
	m_pActor->setGlobalPosition( NxVec3( 0.f, 0.f, -1000.f ) );
	m_eMasterGroup = COL_DYNAMIC;
}

DWORD CBanana::Release( void )
{
	PhysXManager::getInstance().getSDK()->releaseCCDSkeleton( *( m_pActor->getShapes()[ 0 ]->getCCDSkeleton() ) );
	delete this;

	return 0;
}
