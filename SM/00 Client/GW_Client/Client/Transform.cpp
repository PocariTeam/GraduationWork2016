#include "stdafx.h"
#include "Transform.h"

CTransform::CTransform()
	: m_vPos( 0.f, 0.f, 0.f )
	, m_vRotate( 0.f, 0.f, 0.f )
	, m_vScale( 1.f, 1.f, 1.f )
	, m_vDir( 0.f, 0.f, 1.f )
{
	XMStoreFloat4x4A( &m_mtxWorld, XMMatrixIdentity() );
}

CTransform::~CTransform( void )
{

}

void CTransform::Update( void )
{
	XMMATRIX	mtxScale, mtxRotX, mtxRotY, mtxRotZ, mtxTrans;

	if( m_vRotate.y >= XM_2PI )
		m_vRotate.y -= XM_2PI;
	else if( m_vRotate.y <= 0.f )
		m_vRotate.y += XM_2PI;

	mtxRotX = XMMatrixRotationX( m_vRotate.x );
	mtxRotY = XMMatrixRotationY( m_vRotate.y );
	mtxRotZ = XMMatrixRotationZ( m_vRotate.z );

	mtxTrans = XMMatrixTranslation( m_vPos.x, m_vPos.y, m_vPos.z );

	if( m_vScale.x != 1.f
		|| m_vScale.y != 1.f
		|| m_vScale.z != 1.f )
	{
		mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
		XMStoreFloat4x4A( &m_mtxWorld, mtxScale * mtxRotX * mtxRotY * mtxRotZ * mtxTrans );
	}

	else
		XMStoreFloat4x4A( &m_mtxWorld, mtxRotX * mtxRotY * mtxRotZ * mtxTrans );
}

CTransform* CTransform::Create( void )
{
	CTransform*		pTransform = new CTransform();

	return pTransform;
}

DWORD CTransform::Release( void )
{
	DWORD dwRefCnt = CComponent::Release();

	if( 0 == dwRefCnt )
		delete this;

	return dwRefCnt;
}