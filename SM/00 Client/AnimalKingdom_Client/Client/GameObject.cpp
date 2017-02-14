#include "stdafx.h"
#include "GameObject.h"
#include "Function.h"

CGameObject::CGameObject()
	: CBase()
	, m_pConstantBufferWorld( nullptr )
	, m_pConstantBufferMaterial( nullptr )
	, m_pActor( nullptr )
{
}

int	CGameObject::Update( const float& fTimeDelta )
{
	return 0;
}

DWORD	CGameObject::Release()
{
	CBase::Release();

	m_pActor = nullptr;
	::Safe_Release( m_pConstantBufferWorld );
	::Safe_Release( m_pConstantBufferMaterial );

	return 0;
}