#include "stdafx.h"
#include "ParticleMgr.h"
#include "ShaderMgr.h"
#include "ParticleObject.h"

CParticleMgr*	CSingleton<CParticleMgr>::m_pInstance;

HRESULT CParticleMgr::Initialize( ID3D11Device* pDevice )
{
	// m_pParticleShaderArr[ PARTICLE_TRAIL ] = CShaderMgr::GetInstance()->Clone( "Shader_Trail" );
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

void CParticleMgr::Add( PARTICLE_TYPE eType, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vVelocity/* = XMFLOAT3{ 0.f, 0.f, 0.f }*/ )
{
	// 파티클 오브젝트 생성
	// 해당되는 타입의 파티클 셰이더에 푸쉬
}

void CParticleMgr::Render( ID3D11DeviceContext* pContext )
{
	/*for( DWORD i = 0; i < ( DWORD )PARTICLE_END; ++i )
		m_pParticleShaderArr[ i ]->Render( pContext );*/
}
