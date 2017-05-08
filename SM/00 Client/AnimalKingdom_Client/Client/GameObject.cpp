#include "stdafx.h"
#include "GameObject.h"
#include "Function.h"
#include "Define.h"
#include <NxSimpleTypes.h>
#include "NxActor.h"
#include "Texture.h"
#include "Mesh.h"

CGameObject::CGameObject()
	: CBase()
	, m_pMesh( nullptr )
	, m_pTexture( nullptr )
	, m_pActor( nullptr )
	, m_vDir( 0.f, 0.f, 0.f )
	, m_vOption( 0.f, 0.f, 0.f, 1.f )
{
	memset( &m_mtxWorld, 0, sizeof( XMFLOAT4X4 ) );
}

int	CGameObject::Update( const float& fTimeDelta )
{
	/*NxF32 mtxWorld[ 16 ]{};
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &m_mtxWorld, mtxWorld, sizeof( XMFLOAT4X4 ) );*/

	return 0;
}

DWORD	CGameObject::Release()
{
	if( 0 == CBase::Release() )
	{
		::Safe_Release( m_pTexture );
		::Safe_Release( m_pMesh );
		m_pActor = nullptr;
	}


	return CBase::Release();
}

void CGameObject::SetDir( NxVec3 vDir )
{
	m_vDir = vDir;
}

void CGameObject::SetOption( int iKey, float fValue )
{
	switch( iKey )
	{
	case 0 :
		m_vOption.x = fValue;
		break;
	case 1:
		m_vOption.y = fValue;
		break;
	case 2:
		m_vOption.z = fValue;
		break;
	case 3:
		m_vOption.w = fValue;
		break;
	}
}

XMFLOAT4X4* CGameObject::GetWorld()
{
	return &m_mtxWorld;
}

NxActor* CGameObject::GetActor()
{
	return m_pActor;
}

DirectX::XMFLOAT4 CGameObject::GetOption()
{
	return m_vOption;
}

CTexture* CGameObject::GetTexture()
{
	return m_pTexture;
}
