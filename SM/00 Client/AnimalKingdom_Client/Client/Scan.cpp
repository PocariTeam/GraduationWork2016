#include "stdafx.h"
#include "Scan.h"
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

CScan::CScan()
	: CEffectObject()
	, m_pShader( nullptr )
	, m_pOwner( nullptr )
{
}

CScan::~CScan()
{
}

HRESULT CScan::Initialize( ID3D11Device* pDevice, CPlayer* pOwner )
{
	m_vOption.x = 0.5f;		// Ä«Å÷ ¾ÈÇÑ´Ù
	if( nullptr == pOwner ) return E_FAIL;
	m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Noise" );
	m_pOwner = pOwner;
	m_pShader = CShaderMgr::GetInstance()->Clone( "Shader_Noise" );

	return S_OK;
}

CScan* CScan::Create( ID3D11Device* pDevice, CPlayer* pOwner )
{
	CScan* pScan = new CScan;

	if( FAILED( pScan->Initialize( pDevice, pOwner ) ) )
	{
		pScan->Release();
		pScan = nullptr;
	}
	return pScan;
}

XMFLOAT4X4* CScan::GetWorld()
{
	return &m_mtxWorld;
}

int CScan::Update( const float& fTimeDelta )
{
	BOOL bActive{ m_pOwner->GetSkillOn() };

	if( bActive )
	{
		m_vOption.w += fTimeDelta;
	}
	else m_vOption.w = 0.f;

	return 0;
}

void CScan::Render( ID3D11DeviceContext* pContext )
{
	if( m_pOwner->GetSkillOn() )
	{
		if( m_pOwner->GetID() != CJungle::m_iFocus ) return;

		CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_STENCIL_COMPARE );

		m_pShader->SetConstantBuffer( pContext, m_mtxWorld, m_vOption );
		m_pShader->Render( pContext );
		m_pTexture->Render( pContext );
		pContext->Draw( 6, 0 );

		CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );
	}
}

DWORD CScan::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		::Safe_Release( m_pShader );
		delete this;
	}

	return 0;
}
