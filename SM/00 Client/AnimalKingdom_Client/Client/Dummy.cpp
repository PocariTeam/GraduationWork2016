#include "stdafx.h"
#include "Dummy.h"
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
#include "Player.h"
#include "Jungle.h"

CDummy::CDummy()
	: CEffectObject()
	, m_pShader( nullptr )
	, m_pOwner( nullptr )
{
}

CDummy::~CDummy()
{
}

HRESULT CDummy::Initialize( ID3D11Device* pDevice, CPlayer* pOwner )
{
	m_vOption.x = 0.5f;		// Ä«Å÷ ¾ÈÇÑ´Ù
	m_vOption.w = 1.f;

	if( nullptr == pOwner ) return E_FAIL;

	m_pOwner = pOwner;
	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_AnimateMesh_Alpha" );

	return S_OK;
}

CDummy* CDummy::Create( ID3D11Device* pDevice, CPlayer* pOwner )
{
	CDummy* pTrail = new CDummy;

	if( FAILED( pTrail->Initialize( pDevice, pOwner ) ) )
	{
		pTrail->Release();
		pTrail = nullptr;
	}
	return pTrail;
}

XMFLOAT4X4* CDummy::GetWorld()
{
	return &m_mtxWorld;
}

int CDummy::Update( const float& fTimeDelta )
{
	BOOL bActive{ m_pOwner->GetSkillOn() };

	if( bActive ) {
		if( m_vOption.w > 0.3f )
			m_vOption.w -= fTimeDelta;
	}
	else m_vOption.w = 1.f;

	return 0;
}

void CDummy::Render( ID3D11DeviceContext* pContext )
{
	if( m_pOwner->GetSkillOn() )
	{
		if( m_pOwner->GetID() != CJungle::m_iFocus ) return;
		m_pShader->SetConstantBuffer( pContext, *m_pOwner->GetWorld(), m_vOption );
		m_pShader->Render( pContext );
		m_pOwner->Render_Dummy( pContext );
	}
}

DWORD CDummy::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		::Safe_Release( m_pShader );
		delete this;
	}

	return 0;
}
