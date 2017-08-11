#include "stdafx.h"
#include "Trail.h"
#include "Texture.h"
#include "MeshMgr.h"
#include "Function.h"
#include "TextureMgr.h"
#include "Mesh.h"
#include "EffectMgr.h"
#include "Mathematics.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "Define.h"
#include "RenderState.h"
#include "Renderer.h"

CTrail::CTrail()
	: CEffectObject()
	, m_pShader( nullptr )
	, m_pConstantBuffer( nullptr )
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
	m_vVelocity.w = CEffectMgr::EFFECT_TRAIL;
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

	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_Trail" );
	CreateConstantBuffer( pDevice );

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

HRESULT CTrail::CreateConstantBuffer( ID3D11Device* pDevice )
{
	::Safe_Release( m_pConstantBuffer );

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( CB_PARTICLE ) * TRAIL_LENGTH;
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, nullptr, &m_pConstantBuffer );

	return S_OK;
}

void CTrail::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	memcpy_s( MappedSubresource.pData, sizeof( CB_PARTICLE ) * TRAIL_LENGTH, m_tParticleInfo, sizeof( CB_PARTICLE ) * TRAIL_LENGTH );

	pContext->Unmap( m_pConstantBuffer, 0 );
	pContext->VSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
	pContext->GSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
	pContext->PSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
}

int CTrail::Update( const float& fTimeDelta )
{
	// if( m_vOption.y > 0 )
	//	m_vOption.y -= fTimeDelta * 2.f;

	return 0;
}

void CTrail::Render( ID3D11DeviceContext* pContext )
{
	if( m_bActive )
	{
		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		SetConstantBuffer( pContext );
		m_pShader->Render( pContext );
		
		CRenderState::Set_Rasterize( pContext, CRenderState::RS_NO_CULL );
		
		pContext->Draw( TRAIL_LENGTH, 0 );
		
		if( CRenderer::m_bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
		else CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );
	}
}

DWORD CTrail::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		::Safe_Release( m_pShader );
		::Safe_Release( m_pConstantBuffer );
		delete this;
	}

	return 0;
}
