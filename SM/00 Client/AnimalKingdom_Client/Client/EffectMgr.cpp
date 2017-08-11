#include "stdafx.h"
#include "EffectMgr.h"
#include "ShaderMgr.h"
#include "EffectObject.h"
#include "Trail.h"
#include "Renderer.h"

CEffectMgr*	CSingleton<CEffectMgr>::m_pInstance;

HRESULT CEffectMgr::Initialize( ID3D11Device* pDevice )
{
	m_pDevice = pDevice;

	return S_OK;
}

DWORD CEffectMgr::Release( void )
{
	delete this;

	return 0;
}

CEffectObject* CEffectMgr::Add( EFFECT_TYPE eType, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, XMFLOAT3& vVelocity/* = XMFLOAT3{ 0.f, 0.f, 0.f }*/ )
{
	CEffectObject* pParticle{ nullptr };
	// 파티클 오브젝트 생성
	switch( eType )
	{
	case EFFECT_TRAIL:
		pParticle = CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity );
		CRenderer::GetInstance()->Add_RenderGroup_Alpha( pParticle );
		break;
	default:
		break;
	}
		return pParticle;
}