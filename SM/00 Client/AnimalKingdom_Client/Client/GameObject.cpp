#include "stdafx.h"
#include "GameObject.h"
#include "Function.h"
#include "Define.h"

CGameObject::CGameObject()
	: CBase()
	, m_pActor( nullptr )
{
	XMStoreFloat4x4( &m_mtxWorld, XMMatrixIdentity() );
}

int	CGameObject::Update( const float& fTimeDelta )
{
	return 0;
}

DWORD	CGameObject::Release()
{
	CBase::Release();

	m_pActor = nullptr;

	return 0;
}