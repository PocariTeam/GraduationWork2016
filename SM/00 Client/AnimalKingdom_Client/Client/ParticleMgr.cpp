#include "stdafx.h"
#include "ParticleMgr.h"
#include "ShaderMgr.h"
#include "ParticleObject.h"
#include "Trail.h"

CParticleMgr*	CSingleton<CParticleMgr>::m_pInstance;

HRESULT CParticleMgr::Initialize( ID3D11Device* pDevice )
{
	m_pDevice = pDevice;
	m_pParticleShaderArr[ PARTICLE_TRAIL ] = CShaderMgr::GetInstance()->Clone( "Shader_Trail" );
	// m_pParticleShaderArr[ PARTICLE_FIRE ]  = CShaderMgr::GetInstance()->Clone( "Shader_Trail" );

	return S_OK;
}

DWORD CParticleMgr::Release( void )
{
	for( DWORD i = 0; i < ( DWORD )PARTICLE_END; ++i )
		::Safe_Release( m_pParticleShaderArr[ i ] );

	delete this;
	return 0;
}

void CParticleMgr::Add( PARTICLE_TYPE eType, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, XMFLOAT3& vVelocity/* = XMFLOAT3{ 0.f, 0.f, 0.f }*/ )
{
	// 파티클 오브젝트 생성
	switch( eType )
	{
	case PARTICLE_TRAIL:
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		vVelocity.x += 0.1f;
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		vVelocity.x -= 0.2f;
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		vVelocity.x += 0.1f;
		vVelocity.y += 0.1f;
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		vVelocity.y -= 0.2f;
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		vVelocity.y += 0.1f;
		vVelocity.z += 0.1f;
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		vVelocity.z -= 0.2f;
		m_pParticleShaderArr[ PARTICLE_TRAIL ]->Add_RenderObject( CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity ) );
		break;
	default:
		return ;
	}
}

void CParticleMgr::Update( const float& fTimeDelta )
{
	for( DWORD i = 0; i < ( DWORD )PARTICLE_END; ++i )
		m_pParticleShaderArr[ i ]->Update( fTimeDelta );
}

void CParticleMgr::Render( ID3D11DeviceContext* pContext )
{
	for( DWORD i = 0; i < ( DWORD )PARTICLE_END; ++i )
		m_pParticleShaderArr[ i ]->Render( pContext );
}
