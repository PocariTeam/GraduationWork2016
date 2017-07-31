#include "stdafx.h"
#include "ParticleObject.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"

CParticleObject::CParticleObject()
	: CGameObject()
{
}

CParticleObject::~CParticleObject()
{
}

HRESULT CParticleObject::Initialize( ID3D11Device* pDevice )
{
	m_vOption.x = 0.5f;		// Ä«Å÷ ¾ÈÇÑ´Ù
	m_vOption.w = 0.1f;

	return S_OK;
}

CParticleObject* CParticleObject::Create( ID3D11Device* pDevice )
{
	CParticleObject* pParticle = new CParticleObject;

	if( FAILED( pParticle->Initialize( pDevice ) ) )
	{
		pParticle->Release();
		pParticle = nullptr;
	}
	return pParticle;
}

XMFLOAT4X4* CParticleObject::GetWorld()
{
	return &m_mtxWorld;
}

void CParticleObject::Render( ID3D11DeviceContext* pContext )
{
	pContext->Draw( 6, 0 );
}

DWORD CParticleObject::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		delete this;
	}

	return 0;
}
