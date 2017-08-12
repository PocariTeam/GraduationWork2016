#include "stdafx.h"
#include "EffectMgr.h"
#include "ShaderMgr.h"
#include "EffectObject.h"
#include "Trail.h"
#include "Renderer.h"
#include "Dummy.h"

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
	CEffectObject* pEffectObject{ nullptr };
	// 오브젝트 생성
	switch( eType )
	{
	case EFFECT_TRAIL:
		pEffectObject = CTrail::Create( m_pDevice, mtxWorld, vSize, fAge, vVelocity );
		CRenderer::GetInstance()->Add_RenderGroup_Alpha( pEffectObject );
		break;
	default:
		break;
	}
	return pEffectObject;
}

CEffectObject* CEffectMgr::Add( EFFECT_TYPE eType, CPlayer* pOwner )
{
	CEffectObject* pEffectObject{ nullptr };
	// 오브젝트 생성
	switch( eType )
	{
	case EFFECT_DUMMY:
		pEffectObject = CDummy::Create( m_pDevice, pOwner );
		CRenderer::GetInstance()->Add_RenderGroup_Alpha( pEffectObject );
		break;
	default:
		break;
	}
	return pEffectObject;
}
