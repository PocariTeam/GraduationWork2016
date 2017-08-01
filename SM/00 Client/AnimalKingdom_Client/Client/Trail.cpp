#include "stdafx.h"
#include "Trail.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"
#include "ParticleMgr.h"

CTrail::CTrail()
	: CParticleObject()
{
}

CTrail::~CTrail()
{
}

HRESULT CTrail::Initialize( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos )
{
	m_mtxWorld = mtxWorld;
	m_vOption.x = 0.5f;		// Ä«Å÷ ¾ÈÇÑ´Ù
	m_vOption.y = fAge;
	m_vOption.z = vSize.x;
	m_vOption.w = vSize.y;
	m_vVelocity.x = vPos.x;
	m_vVelocity.y = vPos.y;
	m_vVelocity.z = vPos.z;
	m_vVelocity.w = CParticleMgr::PARTICLE_TRAIL;

	return S_OK;
}

CTrail* CTrail::Create( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos )
{
	CTrail* pTrail = new CTrail;

	if( FAILED( pTrail->Initialize( pDevice, mtxWorld, vSize, fAge, vPos ) ) )
	{
		pTrail->Release();
		pTrail = nullptr;
	}
	return pTrail;
}

XMFLOAT4X4* CTrail::GetWorld()
{
	return &m_mtxWorld;
}

int CTrail::Update( const float& fTimeDelta )
{
	m_vOption.y -= fTimeDelta * 2.f;

	if( m_vOption.y < 0 )
		return 1;

	return 0;
}

void CTrail::Render( ID3D11DeviceContext* pContext )
{
	pContext->DrawInstanced( 6, m_iInstanceCnt, 0, 0 );
}

DWORD CTrail::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		delete this;
	}

	return 0;
}
