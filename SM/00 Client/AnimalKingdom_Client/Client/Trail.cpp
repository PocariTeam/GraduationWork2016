#include "stdafx.h"
#include "Trail.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"
#include "ParticleMgr.h"
#include "Mathematics.h"

CTrail::CTrail()
	: CParticleObject()
{
}

CTrail::~CTrail()
{
}

HRESULT CTrail::Initialize( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos )
{
	m_vOption.x = 0.5f;		// Ä«Å÷ ¾ÈÇÑ´Ù
	m_vOption.y = fAge;
	m_vOption.z = vSize.x;
	m_vOption.w = vSize.y;
	m_vVelocity.x = vPos.x;
	m_vVelocity.y = vPos.y;
	m_vVelocity.z = vPos.z;
	m_vVelocity.w = CParticleMgr::PARTICLE_TRAIL;
	m_iTrailCnt = TRAIL_LENGTH;

	CB_PARTICLE tIniData{};
	tIniData.m_mtxWorld = m_mtxWorld = mtxWorld;
	tIniData.m_vOption = m_vOption;
	tIniData.m_vVelocity = m_vVelocity;

	for( int i = 0; i < TRAIL_LENGTH; ++i )
	{
		tIniData.m_vOption.y -= 1.f / TRAIL_LENGTH;
		m_tParticleInfo[ i ] = tIniData;
	}

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

void CTrail::UpdateParticleValue( const XMFLOAT4X4& mtxWorld )
{
	for( int i = TRAIL_LENGTH - 1; i > 0; --i )
		m_tParticleInfo[ i ].m_mtxWorld = m_tParticleInfo[ i - 1 ].m_mtxWorld;
	m_tParticleInfo[ 0 ].m_mtxWorld = mtxWorld;
	/*for( int i = 1; i < 6; ++i )
	{
		m_tParticleInfo[ i * 5 + 1 ].m_mtxWorld = CMathematics::LerpXMFloat4x4( &m_tParticleInfo[ i * 5 ].m_mtxWorld, &m_tParticleInfo[ i * 5 + 4 ].m_mtxWorld, 0.25f );
		m_tParticleInfo[ i * 5 + 2 ].m_mtxWorld = CMathematics::LerpXMFloat4x4( &m_tParticleInfo[ i * 5 ].m_mtxWorld, &m_tParticleInfo[ i * 5 + 4 ].m_mtxWorld, 0.5f );
		m_tParticleInfo[ i * 5 + 3 ].m_mtxWorld = CMathematics::LerpXMFloat4x4( &m_tParticleInfo[ i * 5 ].m_mtxWorld, &m_tParticleInfo[ i * 5 + 4 ].m_mtxWorld, 0.75f );
	}*/

	for( int i = 1; i < 15; ++i )
	{
		m_tParticleInfo[ i * 2 + 1 ].m_mtxWorld = CMathematics::LerpXMFloat4x4( &m_tParticleInfo[ i * 2 ].m_mtxWorld, &m_tParticleInfo[ i * 2 + 2 ].m_mtxWorld, 0.5f );
	}
}

void CTrail::SetParticleValue( const XMFLOAT4X4& mtxWorld )
{
	for( int i = 0; i < TRAIL_LENGTH; ++i )
		m_tParticleInfo[ i ].m_mtxWorld = mtxWorld;
}

XMFLOAT4X4* CTrail::GetWorld()
{
	return &m_mtxWorld;
}

LPVOID CTrail::GetParticleData()
{
	return m_tParticleInfo;
}

int CTrail::Update( const float& fTimeDelta )
{
	// if( m_vOption.y > 0 )
	//	m_vOption.y -= fTimeDelta * 2.f;

	return 0;
}

void CTrail::Render( ID3D11DeviceContext* pContext )
{
	if( m_bActive ) pContext->Draw( TRAIL_LENGTH, 0 );
}

DWORD CTrail::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		delete this;
	}

	return 0;
}
