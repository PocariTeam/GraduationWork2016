#include "stdafx.h"
#include "ParticleMgr.h"
#include "ShaderMgr.h"
#include "ParticleObject.h"
#include "Trail.h"
#include "Renderer.h"

CParticleMgr*	CSingleton<CParticleMgr>::m_pInstance;

HRESULT CParticleMgr::Initialize( ID3D11Device* pDevice )
{
	m_pDevice = pDevice;

	return S_OK;
}

DWORD CParticleMgr::Release( void )
{
	delete this;

	return 0;
}

CParticleObject* CParticleMgr::Add( PARTICLE_TYPE eType, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, XMFLOAT3& vVelocity/* = XMFLOAT3{ 0.f, 0.f, 0.f }*/ )
{
	CParticleObject* pParticle{ nullptr };
	// 파티클 오브젝트 생성
	switch( eType )
	{
	case PARTICLE_TRAIL:
		pParticle = CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity );
		CRenderer::GetInstance()->Add_RenderGroup_Alpha( pParticle );
		break;
	default:
		break;
	}
		return pParticle;
}