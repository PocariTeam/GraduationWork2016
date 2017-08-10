#include "stdafx.h"
#include "ParticleObject.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"

CParticleObject::CParticleObject()
	: CGameObject()
	, m_bActive( false )
{
}

CParticleObject::~CParticleObject()
{
}

XMFLOAT4X4* CParticleObject::GetWorld()
{
	return &m_mtxWorld;
}

int CParticleObject::Update( const float& fTimeDelta )
{
	return 0;
}

DWORD CParticleObject::Release( void )
{
	return CGameObject::Release();
}
