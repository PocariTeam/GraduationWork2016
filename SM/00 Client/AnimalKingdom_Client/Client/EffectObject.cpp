#include "stdafx.h"
#include "EffectObject.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"

CEffectObject::CEffectObject()
	: CGameObject()
	, m_bActive( false )
{
}

CEffectObject::~CEffectObject()
{
}

XMFLOAT4X4* CEffectObject::GetWorld()
{
	return &m_mtxWorld;
}

int CEffectObject::Update( const float& fTimeDelta )
{
	return 0;
}

DWORD CEffectObject::Release( void )
{
	return CGameObject::Release();
}
